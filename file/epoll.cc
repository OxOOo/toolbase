#include "file/epoll.h"

#include "absl/strings/str_format.h"

namespace file {

absl::StatusOr<std::unique_ptr<EPoll>> EPoll::Create() {
    int efd = epoll_create1(0);
    if (efd < 0) {
        return absl::InternalError(strerror(errno));
    }

    return std::unique_ptr<EPoll>(new EPoll(efd));
}

absl::Status EPoll::Close() {
    if (close(efd_) == 0) {
        efd_ = -1;
        return absl::OkStatus();
    }
    return absl::InternalError(strerror(errno));
}

absl::Status EPoll::Add(File* file, uint32_t events, void* ptr) {
    if (fd2file_.find(file->fd()) != fd2file_.end()) {
        return absl::InvalidArgumentError(absl::StrFormat(
            "File(fd = %d) has already been added to epoll", file->fd()));
    }
    struct epoll_event event;
    event.events = events;
    event.data.fd = file->fd();
    int ret = epoll_ctl(efd_, EPOLL_CTL_ADD, file->fd(), &event);
    if (ret != 0) {
        return absl::InternalError(strerror(errno));
    }

    fd2file_[file->fd()] = file;
    fd2ptr_[file->fd()] = ptr;

    return absl::OkStatus();
}

absl::Status EPoll::Modify(File* file, uint32_t events, void* ptr) {
    if (fd2file_.find(file->fd()) == fd2file_.end()) {
        return absl::InvalidArgumentError(absl::StrFormat(
            "File(fd = %d) has not already been added to epoll", file->fd()));
    }
    struct epoll_event event;
    event.events = events;
    event.data.fd = file->fd();
    int ret = epoll_ctl(efd_, EPOLL_CTL_MOD, file->fd(), &event);
    if (ret != 0) {
        return absl::InternalError(strerror(errno));
    }

    fd2file_[file->fd()] = file;
    fd2ptr_[file->fd()] = ptr;

    return absl::OkStatus();
}

absl::Status EPoll::AddOrModify(File* file, uint32_t events, void* ptr) {
    if (fd2file_.find(file->fd()) == fd2file_.end()) {
        return Add(file, events, ptr);
    } else {
        return Modify(file, events, ptr);
    }
}

absl::Status EPoll::Delete(File* file) {
    if (fd2file_.find(file->fd()) == fd2file_.end()) {
        return absl::InvalidArgumentError(absl::StrFormat(
            "File(fd = %d) has not already been added to epoll", file->fd()));
    }
    int ret = epoll_ctl(efd_, EPOLL_CTL_DEL, file->fd(), NULL);
    if (ret != 0) {
        return absl::InternalError(strerror(errno));
    }
    fd2file_.erase(file->fd());
    fd2ptr_.erase(file->fd());

    return absl::OkStatus();
}

absl::Status EPoll::DeleteIfExists(File* file) {
    if (fd2file_.find(file->fd()) != fd2file_.end()) {
        return Delete(file);
    }
    return absl::OkStatus();
}

absl::StatusOr<std::vector<EPollEvent>> EPoll::Wait(
    int maxevents, const absl::Duration* timeout) {
    if (maxevents <= 0) {
        return absl::InvalidArgumentError(absl::StrFormat(
            "`maxevents` = %d should be greater than zero", maxevents));
    }
    std::vector<struct epoll_event> events(maxevents);
    int timeout_ms = -1;
    if (timeout != nullptr) {
        timeout_ms = absl::ToInt64Milliseconds(*timeout);
    }

    int ret = epoll_wait(efd_, events.data(), maxevents, timeout_ms);
    if (ret < 0) {
        return absl::InternalError(strerror(errno));
    }

    std::vector<EPollEvent> return_events;
    for (int i = 0; i < ret; i++) {
        int fd = events[i].data.fd;
        return_events.push_back({.file = fd2file_.at(fd),
                                 .events = events[i].events,
                                 .ptr = fd2ptr_.at(fd)});
    }

    return return_events;
}

}  // namespace file