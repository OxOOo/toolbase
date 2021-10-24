#ifndef TOOLBASE_FILE_FILE_H_
#define TOOLBASE_FILE_FILE_H_

#include <fcntl.h>
#include <unistd.h>

#include <memory>
#include <string>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "glog/logging.h"

namespace file {

// The file class.
// For simply read/write file, you can use
// `file::GetContents`/`file::PutContents` instead.
class File {
   public:
    explicit File(int fd) : fd_(fd) {}
    virtual ~File() {
        if (fd_ > 0) {
            auto status = Close();
            if (!status.ok()) {
                LOG(ERROR) << status;
            }
        }
    }

    int fd() const { return fd_; }

    absl::Status Close();

    absl::Status Sync();

    // Reads string from file.
    // Empty string means eof.
    absl::StatusOr<std::string> Read(size_t count);
    absl::Status ReadTo(std::string& out, size_t count);
    absl::StatusOr<std::string> PRead(size_t count, off_t offset);
    absl::Status PReadTo(std::string& out, size_t count, off_t offset);

    absl::StatusOr<size_t> Write(absl::string_view data);
    absl::StatusOr<size_t> Write(const uint8_t* data, size_t count);
    absl::StatusOr<size_t> PWrite(absl::string_view data, off_t offset);
    absl::StatusOr<size_t> PWrite(const uint8_t* data, size_t count,
                                  off_t offset);

    // Seek
    // Possible whence: SEEK_SET, SEEK_CUR, SEEK_END
    // Returns the resulting offset location
    absl::StatusOr<off_t> LSeek(off_t offset, int whence);

   protected:
    int fd_;
};

// Opens a file.
// Common flags: O_APPEND, O_CLOEXEC, O_CREAT, O_NONBLOCK, O_WRONLY, O_RDONLY
// Examples:
//  Open("/file", O_RDONLY)
//  Open("/file", O_WRONLY | O_CREAT, 0644)
absl::StatusOr<std::unique_ptr<File>> Open(absl::string_view path, int flags);
absl::StatusOr<std::unique_ptr<File>> Open(absl::string_view path, int flags,
                                           mode_t mode);

}  // namespace file

#endif  // TOOLBASE_FILE_FILE_H_