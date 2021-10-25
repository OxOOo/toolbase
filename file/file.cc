#include "file/file.h"

#include <fcntl.h>
#include <unistd.h>

#include <string>
#include <vector>

#include "absl/strings/str_format.h"
#include "utils/status_macros.h"

namespace file {

absl::StatusOr<std::unique_ptr<File>> Open(absl::string_view path, int flags) {
    std::string path_str = std::string(path);

    int fd = open(path_str.c_str(), flags);
    if (fd < 0) {
        return absl::InternalError(strerror(errno));
    }

    return std::unique_ptr<File>(new File(fd));
}

absl::StatusOr<std::unique_ptr<File>> Open(absl::string_view path, int flags,
                                           mode_t mode) {
    std::string path_str = std::string(path);

    int fd = open(path_str.c_str(), flags, mode);
    if (fd < 0) {
        return absl::InternalError(strerror(errno));
    }

    return std::unique_ptr<File>(new File(fd));
}

absl::Status File::Close() {
    if (close(fd_) == 0) {
        fd_ = -1;
        return absl::OkStatus();
    }
    return absl::InternalError(strerror(errno));
}

absl::Status File::Sync() {
    if (fsync(fd_) == 0) {
        return absl::OkStatus();
    }
    return absl::InternalError(strerror(errno));
}

absl::StatusOr<std::string> File::Read(size_t count) {
    std::string out;
    RETURN_IF_ERROR(ReadTo(out, count));
    return out;
}

absl::Status File::ReadTo(std::string& out, size_t count) {
    if (count <= 0) {
        return absl::InvalidArgumentError(
            absl::StrFormat("`count` = %d which should > 0", count));
    }

    std::vector<uint8_t> buf(count);
    ssize_t ret = read(fd_, buf.data(), count);

    if (ret == 0) {
        out = std::string();
        return absl::OkStatus();
    }
    if (ret < 0) {
        return absl::InternalError(strerror(errno));
    }

    out = std::string((const char*)buf.data(), ret);
    return absl::OkStatus();
}

absl::StatusOr<std::string> File::PRead(size_t count, off_t offset) {
    std::string out;
    RETURN_IF_ERROR(PReadTo(out, count, offset));
    return out;
}

absl::Status File::PReadTo(std::string& out, size_t count, off_t offset) {
    if (count <= 0) {
        return absl::InvalidArgumentError(
            absl::StrFormat("`count` = %d which should > 0", count));
    }
    if (offset < 0) {
        return absl::InvalidArgumentError(
            absl::StrFormat("`offset` = %d which should >= 0", offset));
    }

    std::vector<uint8_t> buf(count);
    ssize_t ret = pread(fd_, buf.data(), count, offset);

    if (ret == 0) {
        out = std::string();
        return absl::OkStatus();
    }
    if (ret < 0) {
        return absl::InternalError(strerror(errno));
    }

    out = std::string((const char*)buf.data(), ret);
    return absl::OkStatus();
}

absl::StatusOr<size_t> File::Write(absl::string_view data) {
    return Write((const uint8_t*)data.data(), data.length());
}

absl::StatusOr<size_t> File::Write(const uint8_t* data, size_t count) {
    if (count <= 0) {
        return absl::InvalidArgumentError(
            absl::StrFormat("`count` = %d which should > 0", count));
    }

    ssize_t ret = write(fd_, data, count);
    if (ret < 0) {
        return absl::InternalError(strerror(errno));
    }
    return ret;
}

absl::StatusOr<size_t> File::PWrite(absl::string_view data, off_t offset) {
    return PWrite((const uint8_t*)data.data(), data.length(), offset);
}

absl::StatusOr<size_t> File::PWrite(const uint8_t* data, size_t count,
                                    off_t offset) {
    if (count <= 0) {
        return absl::InvalidArgumentError(
            absl::StrFormat("`count` = %d which should > 0", count));
    }
    if (offset < 0) {
        return absl::InvalidArgumentError(
            absl::StrFormat("`offset` = %d which should >= 0", offset));
    }

    ssize_t ret = pwrite(fd_, data, count, offset);
    if (ret < 0) {
        return absl::InternalError(strerror(errno));
    }
    return ret;
}

absl::Status File::WriteAll(absl::string_view data) {
    return WriteAll((const uint8_t*)data.data(), data.length());
}
absl::Status File::WriteAll(const uint8_t* data, size_t count) {
    size_t pos = 0;
    while (pos < count) {
        ASSIGN_OR_RETURN(size_t written, Write(data + pos, count - pos));
        if (written == 0) {
            return absl::InternalError("Write 0 bytes");
        }
        pos += written;
    }
    return absl::OkStatus();
}

absl::StatusOr<off_t> File::LSeek(off_t offset, int whence) {
    off_t ret = lseek(fd_, offset, whence);
    if (ret < 0) {
        return absl::InternalError(strerror(errno));
    }
    return ret;
}

}  // namespace file
