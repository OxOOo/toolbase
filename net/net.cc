#include "net/net.h"

#include "absl/strings/str_format.h"
#include "utils/status_macros.h"

namespace net {

absl::StatusOr<std::string> SocketAddr::ip() const {
    // buf size should be large than INET6_ADDRSTRLEN = 48
    char buf[128];
    memset(buf, 0, sizeof(buf));

    if (addr_.ss_family == AF_INET) {
        const struct sockaddr_in *addr_v4 = (struct sockaddr_in *)(&addr_);
        if (inet_ntop(AF_INET, &addr_v4->sin_addr, buf, sizeof(buf) - 1) ==
            NULL) {
            return absl::InternalError(strerror(errno));
        } else {
            return buf;
        }
    } else if (addr_.ss_family == AF_INET6) {
        const struct sockaddr_in6 *addr_v6 = (struct sockaddr_in6 *)(&addr_);
        if (inet_ntop(AF_INET6, &addr_v6->sin6_addr, buf, sizeof(buf) - 1) ==
            NULL) {
            return absl::InternalError(strerror(errno));
        } else {
            return buf;
        }
    } else if (addr_.ss_family == AF_UNIX) {
        const struct sockaddr_un *addr_un = (struct sockaddr_un *)(&addr_);
        return addr_un->sun_path;
    } else {
        return absl::UnimplementedError(absl::StrFormat(
            "Cannot get ip for ss_family = %d", addr_.ss_family));
    }
}
absl::StatusOr<uint16_t> SocketAddr::port() const {
    if (addr_.ss_family == AF_INET) {
        const struct sockaddr_in *addr_v4 = (struct sockaddr_in *)(&addr_);
        return ntohs(addr_v4->sin_port);
    } else if (addr_.ss_family == AF_INET6) {
        const struct sockaddr_in6 *addr_v6 = (struct sockaddr_in6 *)(&addr_);
        return ntohs(addr_v6->sin6_port);
    } else if (addr_.ss_family == AF_UNIX) {
        return absl::UnimplementedError("Cannot get port for AF_UNIX family");
    } else {
        return absl::UnimplementedError(absl::StrFormat(
            "Cannot get port for ss_family = %d", addr_.ss_family));
    }
}
absl::StatusOr<std::string> SocketAddr::ToString() const {
    if (addr_.ss_family == AF_INET) {
        ASSIGN_OR_RETURN(auto ip, ip());
        ASSIGN_OR_RETURN(auto port, port());
        return absl::StrCat(ip, ":", port);
    } else if (addr_.ss_family == AF_INET6) {
        ASSIGN_OR_RETURN(auto ip, ip());
        ASSIGN_OR_RETURN(auto port, port());
        return absl::StrFormat("[%s]:%d", ip, port);
    } else if (addr_.ss_family == AF_UNIX) {
        return ip();
    } else {
        return absl::UnimplementedError(absl::StrFormat(
            "Cannot parse to string for ss_family = %d", addr_.ss_family));
    }
}

absl::StatusOr<SocketAddr> SocketAddr::NewIPv4(absl::string_view ip,
                                               uint16_t port) {
    struct sockaddr_storage addr;
    memset(&addr, 0, sizeof(addr));

    struct sockaddr_in *addr_v4 = (struct sockaddr_in *)&addr;
    addr_v4->sin_family = AF_INET;
    addr_v4->sin_port = htons(port);

    std::string ip_str = std::string(ip);
    int ret = inet_pton(AF_INET, ip_str.c_str(), &addr_v4->sin_addr);
    if (ret == 1) {
        return SocketAddr(addr);
    } else if (ret == 0) {
        return absl::InvalidArgumentError(
            absl::StrFormat("Cannot parse ipv4 from `%s`", ip));
    } else {
        return absl::InternalError(strerror(errno));
    }
}
absl::StatusOr<SocketAddr> SocketAddr::NewIPv6(absl::string_view ip,
                                               uint16_t port) {
    struct sockaddr_storage addr;
    memset(&addr, 0, sizeof(addr));

    struct sockaddr_in6 *addr_v6 = (struct sockaddr_in6 *)&addr;
    addr_v6->sin6_family = AF_INET6;
    addr_v6->sin6_port = htons(port);

    std::string ip_str = std::string(ip);
    int ret = inet_pton(AF_INET6, ip_str.c_str(), &addr_v6->sin6_addr);
    if (ret == 1) {
        return SocketAddr(addr);
    } else if (ret == 0) {
        return absl::InvalidArgumentError(
            absl::StrFormat("Cannot parse ipv6 from `%s`", ip));
    } else {
        return absl::InternalError(strerror(errno));
    }
}
absl::StatusOr<SocketAddr> SocketAddr::NewUnix(absl::string_view path) {
    struct sockaddr_storage addr;
    memset(&addr, 0, sizeof(addr));

    struct sockaddr_un *addr_un = (struct sockaddr_un *)&addr;
    addr_un->sun_family = AF_UNIX;
    if (path.length() >= sizeof(addr_un->sun_path)) {
        return absl::InvalidArgumentError(
            absl::StrFormat("Path `%s` is too long for unix sockt", path));
    }
    memcpy(addr_un->sun_path, path.data(), path.length());

    return SocketAddr(addr);
}

absl::StatusOr<std::unique_ptr<NetSocket>> Socket(int domain, int type,
                                                  int protocol) {
    int fd = socket(domain, type, protocol);
    if (fd < 0) {
        return absl::InternalError(strerror(errno));
    }
    return std::unique_ptr<NetSocket>(new NetSocket(fd));
}

absl::Status NetSocket::Bind(const SocketAddr &addr) {
    int ret = bind(fd_, addr.addr(), addr.len());
    if (ret != 0) {
        return absl::InternalError(strerror(errno));
    }
    bound_addr_ = addr;
    return absl::OkStatus();
}

absl::Status NetSocket::Listen(int backlog) {
    int ret = listen(fd_, backlog);
    if (ret != 0) {
        return absl::InternalError(strerror(errno));
    }
    return absl::OkStatus();
}

absl::StatusOr<std::unique_ptr<NetSocket>> NetSocket::Accept() {
    struct sockaddr_storage addr;
    socklen_t len = bound_addr_.len();
    int fd = accept(fd_, (struct sockaddr *)&addr, &len);
    if (fd < 0) {
        return absl::InternalError(strerror(errno));
    }
    auto socket = std::unique_ptr<NetSocket>(new NetSocket(fd));
    socket->SetRemoteAddr(SocketAddr(addr));
    socket->SetLocalAddr(bound_addr_);
    return std::move(socket);
}

absl::Status NetSocket::Connect(const SocketAddr &addr) {
    int ret = connect(fd_, addr.addr(), addr.len());
    if (ret != 0) {
        return absl::InternalError(strerror(errno));
    }
    return absl::OkStatus();
}

absl::StatusOr<size_t> NetSocket::Send(absl::string_view data, int flags) {
    return Send((const uint8_t *)data.data(), data.length(), flags);
}
absl::StatusOr<size_t> NetSocket::Send(const uint8_t *data, size_t count,
                                       int flags) {
    if (count <= 0) {
        return absl::InvalidArgumentError(
            absl::StrFormat("`count` = %d which should > 0", count));
    }

    ssize_t ret = send(fd_, data, count, flags);
    if (ret < 0) {
        return absl::InternalError(strerror(errno));
    }
    return ret;
}

absl::StatusOr<std::string> NetSocket::Recv(size_t count, int flags) {
    std::string out;
    RETURN_IF_ERROR(RecvTo(out, count, flags));
    return out;
}
absl::Status NetSocket::RecvTo(std::string &out, size_t count, int flags) {
    if (count <= 0) {
        return absl::InvalidArgumentError(
            absl::StrFormat("`count` = %d which should > 0", count));
    }

    out.resize(count);
    ssize_t ret = recv(fd_, out.data(), count, flags);

    if (ret == 0) {
        out = std::string();
        return absl::OkStatus();
    }
    if (ret < 0) {
        return absl::InternalError(strerror(errno));
    }
    out.resize(ret);

    return absl::OkStatus();
}

absl::StatusOr<size_t> NetSocket::SendTo(absl::string_view data, int flags,
                                         const SocketAddr *dest_addr) {
    return SendTo((const uint8_t *)data.data(), data.length(), flags,
                  dest_addr);
}
absl::StatusOr<size_t> NetSocket::SendTo(const uint8_t *data, size_t count,
                                         int flags,
                                         const SocketAddr *dest_addr) {
    if (count <= 0) {
        return absl::InvalidArgumentError(
            absl::StrFormat("`count` = %d which should > 0", count));
    }

    ssize_t ret = -1;
    if (dest_addr == NULL || dest_addr == nullptr) {
        ret = sendto(fd_, data, count, flags, NULL, 0);
    } else {
        ret = sendto(fd_, data, count, flags, dest_addr->addr(),
                     dest_addr->len());
    }
    if (ret < 0) {
        return absl::InternalError(strerror(errno));
    }
    return ret;
}

absl::StatusOr<std::string> NetSocket::RecvFrom(size_t count, int flags,
                                                SocketAddr *src_addr) {
    std::string out;
    RETURN_IF_ERROR(RecvFromTo(out, count, flags, src_addr));
    return out;
}
absl::Status NetSocket::RecvFromTo(std::string &out, size_t count, int flags,
                                   SocketAddr *src_addr) {
    if (count <= 0) {
        return absl::InvalidArgumentError(
            absl::StrFormat("`count` = %d which should > 0", count));
    }

    out.resize(count);
    ssize_t ret = -1;
    if (src_addr == NULL || src_addr == nullptr) {
        ret = recvfrom(fd_, out.data(), count, flags, NULL, NULL);
    } else {
        socklen_t len = src_addr->storage_len();
        ret = recvfrom(fd_, out.data(), count, flags, src_addr->mutable_addr(),
                       &len);
    }

    if (ret == 0) {
        out = std::string();
        return absl::OkStatus();
    }
    if (ret < 0) {
        return absl::InternalError(strerror(errno));
    }
    out.resize(ret);

    return absl::OkStatus();
}

}  // namespace net
