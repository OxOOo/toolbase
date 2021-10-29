#ifndef TOOLBASE_FILE_NONBLOCKING_H_
#define TOOLBASE_FILE_NONBLOCKING_H_

#include <memory>
#include <string>

#include "absl/status/statusor.h"
#include "absl/strings/string_view.h"
#include "file/file.h"

namespace file {

// A class to handle nonblocking IO operations.
// For write(some_data):
//  1. Copy some_data to the memory buffer
//  2. Try to write data from memory buffer
// For read:
//  1. Try to read data from `file` to the memory buffer
//  2. Returns a string_view for user to read data
//  3. Use comsume read bytes to free read memory buffer
// Example non-blocking write:
//  io->AppendWrite(...);
//  while(io->HasDataToWrite()) {
//      io->TryWriteOnce();
//  }
// Example non-blocking read:
//  io->TryReadOnce(1024);
//  if (io->HasDataToRead()) {
//      data = io->DataToRead();
//      io->ConsumeReadData(data.length());
//  }
class NonblockingIO {
   public:
    explicit NonblockingIO(std::unique_ptr<File> file)
        : file_(std::move(file)) {}

    static absl::StatusOr<std::unique_ptr<NonblockingIO>> Create(
        std::unique_ptr<File> file);

    const File* file() const { return file_.get(); }
    File* file() { return file_.get(); }

    // Appends data to write buffer, this call will not perform write.
    void AppendWriteData(absl::string_view data);

    // Performs a write, returns the number of written bytes.
    // Returns 0 means need wait.
    absl::StatusOr<size_t> TryWriteOnce();

    bool HasDataToWrite() { return !write_buf_.empty(); }

    // Performs a read, returns the number of read bytes.
    // `count` means the max number of bytes read in this call.
    // Returns 0 means need wait.
    absl::StatusOr<size_t> TryReadOnce(size_t count);

    bool HasDataToRead() { return !read_buf_.empty(); }

    // Returns a view of read buffer, the view will be valid until next call of
    // `TryReadOnce` or `ConsumeReadData`.
    absl::string_view DataToRead() { return read_buf_; }

    void ConsumeReadData(size_t bytes);

   private:
    std::unique_ptr<File> file_;
    std::string write_buf_;
    std::string read_buf_;
};

}  // namespace file

#endif  // TOOLBASE_FILE_NONBLOCKING_H_
