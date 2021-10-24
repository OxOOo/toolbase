#ifndef TOOLBASE_FILE_EPOLL_H_
#define TOOLBASE_FILE_EPOLL_H_

#include <sys/epoll.h>

#include <map>
#include <memory>
#include <vector>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/time/time.h"
#include "file/file.h"

namespace file {

struct EPollEvent {
    File* file;
    uint32_t events;
    void* ptr;
};

// Represents an epoll instance.
// Note: EPoll does not own the File*
// Common events: EPOLLIN, EPOLLOUT, EPOLLERR
// This class is not thread-safe
class EPoll {
   public:
    EPoll(int efd) : efd_(efd) {}
    ~EPoll() {
        if (efd_ > 0) {
            auto status = Close();
            if (!status.ok()) {
                LOG(ERROR) << status;
            }
        }
    }

    int efd() const { return efd_; }

    static absl::StatusOr<std::unique_ptr<EPoll>> Create();

    absl::Status Close();

    // Adds a file to epoll, listen the `events`, the `ptr` will be returned in
    // Wait function if the `file` is triggered.
    absl::Status Add(File* file, uint32_t events, void* ptr = nullptr);

    absl::Status Modify(File* file, uint32_t events, void* ptr = nullptr);

    absl::Status Delete(File* file);

    // Waits epoll, returns triggered events (up to `maxevents` events).
    // `timeout` = nullptr means blocks indefinitely.
    absl::StatusOr<std::vector<EPollEvent>> Wait(int maxevents,
                                                 absl::Duration* timeout);

   private:
    int efd_;
    std::map<int, File*> fd2file_;
    std::map<int, void*> fd2ptr_;
};

}  // namespace file

#endif  // TOOLBASE_FILE_EPOLL_H_