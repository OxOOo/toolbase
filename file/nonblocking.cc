#include "file/nonblocking.h"

#include <fcntl.h>

#include <vector>

namespace file {

absl::StatusOr<std::unique_ptr<NonblockingIO>> NonblockingIO::Create(
    std::unique_ptr<File> file) {
    int flags = fcntl(file->fd(), F_GETFL, 0);
    if (flags == -1) {
        return absl::InternalError(strerror(errno));
    }
    flags = flags | O_NONBLOCK;
    int ret = fcntl(file->fd(), F_SETFL, flags);
    if (ret == -1) {
        return absl::InternalError(strerror(errno));
    }
    return std::unique_ptr<NonblockingIO>(new NonblockingIO(std::move(file)));
}

void NonblockingIO::AppendWriteData(absl::string_view data) {
    size_t old_size = write_buf_.size();
    write_buf_.resize(write_buf_.size() + data.size());
    memmove(write_buf_.data() + old_size, data.data(), data.size());
}

absl::StatusOr<size_t> NonblockingIO::TryWriteOnce() {
    if (write_buf_.empty()) {
        return absl::InternalError("No data to write");
    }

    ssize_t ret = write(file_->fd(), write_buf_.data(), write_buf_.size());
    if (ret < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return 0;
        }
        return absl::InternalError(strerror(errno));
    }
    size_t new_size = write_buf_.size() - ret;
    memmove(write_buf_.data(), write_buf_.data() + ret, new_size);
    write_buf_.resize(new_size);
    return ret;
}

absl::StatusOr<size_t> NonblockingIO::TryReadOnce(size_t count) {
    std::vector<uint8_t> buf(count);
    ssize_t ret = read(file_->fd(), buf.data(), count);
    if (ret < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return 0;
        }
        return absl::InternalError(strerror(errno));
    }
    size_t old_size = read_buf_.size();
    size_t new_size = read_buf_.size() + ret;
    read_buf_.resize(new_size);
    memmove(read_buf_.data() + old_size, buf.data(), ret);
    return ret;
}

void NonblockingIO::ConsumeReadData(size_t bytes) {
    if (bytes >= read_buf_.size()) {
        read_buf_.clear();
        return;
    }

    size_t old_size = read_buf_.size();
    size_t new_size = old_size - bytes;
    memmove(read_buf_.data(), read_buf_.data() + bytes, new_size);
    read_buf_.resize(new_size);
}

}  // namespace file