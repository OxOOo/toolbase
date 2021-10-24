// TCP server example:
//  ASSIGN_OR_RETURN(auto server, Socket(AF_INET, SOCK_STREAM, 0));
//  RETURN_IF_ERROR(server->SetReuseAddr());
//  RETURN_IF_ERROR(server->Bind(*SocketAddr::NewIPv4(...)));
//  RETURN_IF_ERROR(server->Listen(1024));
//
//  ASSIGN_OR_RETURN(auto socket, server->Accept());
//  RETURN_IF_ERROR(socket->Write("Hello World"));
//
// TCP client example:
//  ASSIGN_OR_RETURN(auto client, Socket(AF_INET, SOCK_STREAM, 0));
//  RETURN_IF_ERROR(client->Connect(*SocketAddr::NewIPv4(...)));
//  ASSIGN_OR_RETURN(auto data, client->Read(1024));
//
// UDP server example:
//  ASSIGN_OR_RETURN(auto server, Socket(AF_INET, SOCK_DGRAM, 0));
//  RETURN_IF_ERROR(server->Bind(*SocketAddr::NewIPv4(...)));
//  ASSIGN_OR_RETURN(auto data, server->RecvFrom(1024, 0, &src_addr));
//
// UDP client example:
//  ASSIGN_OR_RETURN(auto client, Socket(AF_INET, SOCK_DGRAM, 0));
//  RETURN_IF_ERROR(client->SendTo("Hello World", 0, &to_addr));

#ifndef TOOLBASE_NET_NET_H_
#define TOOLBASE_NET_NET_H_

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <memory>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/string_view.h"
#include "file/file.h"

namespace net {

class SocketAddr {
   public:
    SocketAddr() {
        memset(&addr_, 0, sizeof(addr_));
        addr_.ss_family = AF_UNSPEC;
    }
    explicit SocketAddr(struct sockaddr_storage addr) : addr_(addr) {}

    bool IsValid() const { return addr_.ss_family != AF_UNSPEC; }

    absl::StatusOr<std::string> ip() const;
    absl::StatusOr<uint16_t> port() const;
    absl::StatusOr<std::string> ToString() const;

    const struct sockaddr* addr() const {
        return (const struct sockaddr*)&addr_;
    }
    struct sockaddr* mutable_addr() {
        return (struct sockaddr*)&addr_;
    }
    socklen_t len() const {
        if (addr_.ss_family == AF_INET) {
            return sizeof(struct sockaddr_in);
        } else if (addr_.ss_family == AF_INET6) {
            return sizeof(struct sockaddr_in6);
        } else if (addr_.ss_family == AF_UNIX) {
            return sizeof(struct sockaddr_un);
        } else {
            return sizeof(addr_);
        }
    }
    socklen_t storage_len() const { return sizeof(addr_); }

    static absl::StatusOr<SocketAddr> NewIPv4(absl::string_view ip,
                                              uint16_t port);
    static absl::StatusOr<SocketAddr> NewIPv6(absl::string_view ip,
                                              uint16_t port);
    static absl::StatusOr<SocketAddr> NewUnix(absl::string_view path);

   private:
    struct sockaddr_storage addr_;
};

class NetSocket : public file::File {
   public:
    explicit NetSocket(int fd) : file::File(fd) {}

    const SocketAddr& remote_addr() const { return remote_addr_; }
    const SocketAddr& local_addr() const { return local_addr_; }
    void SetRemoteAddr(const SocketAddr& addr) { remote_addr_ = addr; }
    void SetLocalAddr(const SocketAddr& addr) { local_addr_ = addr; }

    // Sets SO_REUSEADDR, this usually should be done before bind TCP address.
    absl::Status SetReuseAddr() {
        return SetSockOpt<int>(SOL_SOCKET, SO_REUSEADDR, 1);
    }

    absl::Status Bind(const SocketAddr& addr);

    // Masks the socket as a passive socket.
    // `backlog` argument defines the maximum length to which the queue of
    // pending connections
    absl::Status Listen(int backlog);

    // Accepts an new TCP socket.
    // Will set `remote_addr` as the incomming socket's addr.
    // Will set `local_addr` as bound addr.
    absl::StatusOr<std::unique_ptr<NetSocket>> Accept();

    absl::Status Connect(const SocketAddr& addr);

    absl::StatusOr<size_t> Send(absl::string_view data, int flags);
    absl::StatusOr<size_t> Send(const uint8_t* data, size_t count, int flags);

    absl::StatusOr<std::string> Recv(size_t count, int flags);
    absl::Status RecvTo(std::string& out, size_t count, int flags);

    // Sends data to a specific addr, `dest_addr` can be NULL or nullptr.
    absl::StatusOr<size_t> SendTo(absl::string_view data, int flags,
                                  const SocketAddr* dest_addr);
    absl::StatusOr<size_t> SendTo(const uint8_t* data, size_t count, int flags,
                                  const SocketAddr* dest_addr);

    // Receives message from a specific addr, `src_addr` can be NULL or nullptr.
    absl::StatusOr<std::string> RecvFrom(size_t count, int flags,
                                         SocketAddr* src_addr);
    absl::Status RecvFromTo(std::string& out, size_t count, int flags,
                            SocketAddr* src_addr);

    template <class T>
    absl::StatusOr<T> GetSockOpt(int level, int optname) {
        T optval;
        socklen_t len = sizeof(optval);
        if (getsockopt(fd_, level, optname, &optval, &len) == 0) {
            return optval;
        }
        return absl::InternalError(strerror(errno));
    }

    template <class T>
    absl::Status SetSockOpt(int level, int optname, T optval) {
        if (setsockopt(fd_, level, optname, &optval, sizeof(optval)) == 0) {
            return absl::OkStatus();
        }
        return absl::InternalError(strerror(errno));
    }

   protected:
    SocketAddr bound_addr_;
    SocketAddr remote_addr_;
    SocketAddr local_addr_;
};

// Creates a socket.
// Common used `domain`:
//  1. AF_UNIX for UNIX file
//  2. AF_INET for IPv4
//  3. AF_INET6 for IPv6
// Common used `type`:
//  1. SOCK_STREAM for TCP
//  2. SOCK_DGRAM for UDP
//  3. SOCK_NONBLOCK for setting NONBLOCK
//  4. SOCK_CLOEXEC for close fd when `exec`
// `protocol` can be set as 0.
// Examples:
//  IPv4 TCP socket: Socket(AF_INET, SOCK_STREAM, 0)
//  Nonblock IPv6 UDP socket: Socket(AF_INET6, SOCK_DGRAM, 0)
absl::StatusOr<std::unique_ptr<NetSocket>> Socket(int domain, int type,
                                                  int protocol);

}  // namespace net

#endif  // TOOLBASE_NET_NET_H_