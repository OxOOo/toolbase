#ifndef TOOLBASE_FILE_FILE_H_
#define TOOLBASE_FILE_FILE_H_

#include <memory>
#include <string>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "glog/logging.h"

namespace file {

class File {
   public:
    explicit File(int fd) : fd(fd) {}
    ~File() {
        if (fd > 0) {
            auto status = Close();
            if (!status.ok()) {
                LOG(ERROR) << status;
            }
        }
    }

    absl::Status Close();

    absl::Status Sync();

    absl::StatusOr<std::string> Read(size_t count);
    absl::Status ReadTo(std::string& out, size_t count);
    absl::StatusOr<std::string> PRead(size_t count, off_t offset);
    absl::Status PReadTo(std::string& out, size_t count, off_t offset);

    absl::StatusOr<size_t> Write(absl::string_view data);
    absl::StatusOr<size_t> Write(const uint8_t* data, size_t count);
    absl::StatusOr<size_t> PWrite(absl::string_view data, off_t offset);
    absl::StatusOr<size_t> PWrite(const uint8_t* data, size_t count,
                                  off_t offset);

    absl::StatusOr<off_t> LSeek(off_t offset, int whence);

   private:
    int fd;
};

absl::StatusOr<std::unique_ptr<File>> Open(absl::string_view path, int flags);

absl::StatusOr<std::unique_ptr<File>> Open(absl::string_view path, int flags,
                                           mode_t mode);

}  // namespace file

#endif  // TOOLBASE_FILE_FILE_H_