#include "file/filesystem.h"

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include "absl/strings/str_format.h"
#include "file/file.h"
#include "file/path.h"
#include "utils/status_macros.h"

namespace file {
namespace {

constexpr absl::string_view kPathSelf = ".";
constexpr absl::string_view kPathParent = "..";

}  // namespace

absl::StatusOr<PathStat> Stat(absl::string_view path) {
    std::string path_str = std::string(path);
    struct stat s;
    if (stat(path_str.c_str(), &s) == 0) {
        return PathStat(s);
    } else {
        return absl::InternalError(strerror(errno));
    }
}

absl::StatusOr<PathStat> LStat(absl::string_view path) {
    std::string path_str = std::string(path);
    struct stat s;
    if (lstat(path_str.c_str(), &s) == 0) {
        return PathStat(s);
    } else {
        return absl::InternalError(strerror(errno));
    }
}

absl::StatusOr<bool> Exists(absl::string_view path) {
    std::string path_str = std::string(path);
    if (access(path_str.c_str(), F_OK) == 0) {
        return true;
    } else if (errno == ENOENT) {
        return false;
    } else {
        return absl::InternalError(strerror(errno));
    }
}

absl::Status CreateFile(absl::string_view path, mode_t mode) {
    std::string path_str = std::string(path);
    int fd = open(path_str.c_str(), O_CREAT | O_EXCL | O_WRONLY, mode);
    if (fd < 0) {
        return absl::InternalError(strerror(errno));
    }
    if (close(fd) != 0) {
        return absl::InternalError(strerror(errno));
    }
    return absl::OkStatus();
}

absl::Status Chmod(absl::string_view path, mode_t mode) {
    std::string path_str = std::string(path);
    if (chmod(path_str.c_str(), mode) == 0) {
        return absl::OkStatus();
    } else {
        return absl::InternalError(strerror(errno));
    }
}

absl::Status Unlink(absl::string_view path) {
    std::string path_str = std::string(path);
    if (unlink(path_str.c_str()) == 0) {
        return absl::OkStatus();
    } else {
        return absl::InternalError(strerror(errno));
    }
}

absl::Status Rmdir(absl::string_view path) {
    std::string path_str = std::string(path);
    if (rmdir(path_str.c_str()) == 0) {
        return absl::OkStatus();
    } else {
        return absl::InternalError(strerror(errno));
    }
}

absl::Status Mkdir(absl::string_view path, mode_t mode) {
    std::string path_str = std::string(path);
    if (mkdir(path_str.c_str(), mode) == 0) {
        return absl::OkStatus();
    } else {
        return absl::InternalError(strerror(errno));
    }
}

absl::Status Rename(absl::string_view oldpath, absl::string_view newpath) {
    std::string oldpath_str = std::string(oldpath);
    std::string newpath_str = std::string(newpath);
    if (rename(oldpath_str.c_str(), newpath_str.c_str()) == 0) {
        return absl::OkStatus();
    } else {
        return absl::InternalError(strerror(errno));
    }
}

absl::StatusOr<std::vector<std::string>> ListDirectory(absl::string_view path) {
    std::string path_str = std::string(path);
    DIR* d = opendir(path_str.c_str());
    if (d == NULL) {
        return absl::InternalError(strerror(errno));
    }
    std::vector<std::string> entries;
    while (true) {
        errno = 0;
        struct dirent* dir = readdir(d);
        if (dir == NULL) {
            if (errno != 0) {
                return absl::InternalError(strerror(errno));
            }
            break;
        }
        if (dir->d_name != kPathSelf && dir->d_name != kPathParent) {
            entries.push_back(dir->d_name);
        }
    }
    if (closedir(d) != 0) {
        return absl::InternalError(strerror(errno));
    }
    return entries;
}

absl::Status RmTree(absl::string_view path) {
    ASSIGN_OR_RETURN(bool path_exists, Exists(path));
    if (!path_exists) {
        return absl::OkStatus();
    }
    ASSIGN_OR_RETURN(auto entries, ListDirectory(path));
    for (auto entry : entries) {
        std::string subpath = PathJoin(path, entry);
        ASSIGN_OR_RETURN(auto stat, Stat(subpath));
        if (stat.IsDirectory()) {
            RETURN_IF_ERROR(RmTree(subpath));
        } else {
            RETURN_IF_ERROR(Unlink(subpath));
        }
    }
    RETURN_IF_ERROR(Rmdir(path));

    return absl::OkStatus();
}

absl::StatusOr<std::string> GetContents(absl::string_view path) {
    std::string out;
    RETURN_IF_ERROR(GetContents(out, path));
    return out;
}
absl::Status GetContents(std::string& out, absl::string_view path) {
    ASSIGN_OR_RETURN(auto file, File::Open(path, O_RDONLY));
    ASSIGN_OR_RETURN(auto stat, Stat(path));
    return file->ReadTo(out, stat.size());
}

absl::Status PutContents(absl::string_view data, absl::string_view path) {
    return PutContents((const uint8_t*)data.data(), data.length(), path);
}
absl::Status PutContents(const uint8_t* data, size_t count,
                         absl::string_view path) {
    if (count <= 0) {
        return absl::InvalidArgumentError(
            absl::StrFormat("`count` = %d which should > 0"));
    }
    ASSIGN_OR_RETURN(auto file, File::Open(path, O_CREAT | O_WRONLY, 0644));
    while (count > 0) {
        errno = 0;
        ASSIGN_OR_RETURN(size_t size, file->Write(data, count));
        if (size == 0) {
            return absl::InternalError(absl::StrFormat(
                "Put contents error: wrote only %d bytes, errno = %s", size,
                strerror(errno)));
        }
        count -= size;
        data += size;
    }
    return absl::OkStatus();
}

}  // namespace file
