#include "net/net.h"

#include "gtest/gtest.h"
#include "utils/testing.h"

namespace net {
namespace {

using ::utils::testing::IsOkAndHolds;
using ::utils::testing::StatusIs;

TEST(SocketAddr, IPv4) {
    auto addr = SocketAddr::NewIPv4("192.168.1.1", 1234);
    EXPECT_OK(addr);
    EXPECT_THAT(addr->ip(), IsOkAndHolds("192.168.1.1"));
    EXPECT_THAT(addr->port(), IsOkAndHolds(1234));
    EXPECT_THAT(addr->ToString(), IsOkAndHolds("192.168.1.1:1234"));

    EXPECT_THAT(SocketAddr::NewIPv4("192", 1234),
                StatusIs(absl::StatusCode::kInvalidArgument));
}

TEST(SocketAddr, IPv6) {
    auto addr = SocketAddr::NewIPv6("1050:0:0:0:5:600:300c:326b", 1234);
    EXPECT_OK(addr);
    EXPECT_THAT(addr->ip(), IsOkAndHolds("1050::5:600:300c:326b"));
    EXPECT_THAT(addr->port(), IsOkAndHolds(1234));
    EXPECT_THAT(addr->ToString(), IsOkAndHolds("[1050::5:600:300c:326b]:1234"));

    EXPECT_THAT(SocketAddr::NewIPv6("192", 1234),
                StatusIs(absl::StatusCode::kInvalidArgument));
}

TEST(Socket, TestTCPOpen) { EXPECT_OK(Socket(AF_INET, SOCK_STREAM, 0)); }

TEST(Socket, TestUDPOpen) { EXPECT_OK(Socket(AF_INET6, SOCK_DGRAM, 0)); }

TEST(Socket, TestAutoClose) {
    int fd = -1;
    {
        auto socket = Socket(AF_INET, SOCK_STREAM, 0);
        EXPECT_OK(socket);

        fd = (*socket)->fd();
        EXPECT_EQ(accept(fd, NULL, NULL), -1);
        EXPECT_EQ(errno, EINVAL);
    }
    EXPECT_EQ(accept(fd, NULL, NULL), -1);
    EXPECT_EQ(errno, EBADF);
}

TEST(Socket, TestTCP) {
    auto server = Socket(AF_INET, SOCK_STREAM, 0);
    EXPECT_OK(server);

    EXPECT_OK((*server)->SetReuseAddr());
    EXPECT_OK((*server)->Bind(*SocketAddr::NewIPv4("127.0.0.1", 62781)));
    EXPECT_OK((*server)->Listen(10));

    auto client = Socket(AF_INET, SOCK_STREAM, 0);
    EXPECT_OK(client);
    EXPECT_OK((*client)->Connect(*SocketAddr::NewIPv4("127.0.0.1", 62781)));

    auto socket = (*server)->Accept();
    EXPECT_OK(socket);
    EXPECT_THAT((*socket)->remote_addr().ip(), IsOkAndHolds("127.0.0.1"));
    EXPECT_THAT((*socket)->local_addr().ToString(),
                IsOkAndHolds("127.0.0.1:62781"));

    EXPECT_THAT((*socket)->Send("Hello World", 0), IsOkAndHolds(11));
    EXPECT_THAT((*client)->Recv(1024, 0), IsOkAndHolds("Hello World"));
}

TEST(Socket, TestUDP) {
    auto server = Socket(AF_INET, SOCK_DGRAM, 0);
    EXPECT_OK(server);

    EXPECT_OK((*server)->Bind(*SocketAddr::NewIPv4("127.0.0.1", 62781)));

    auto client = Socket(AF_INET, SOCK_DGRAM, 0);
    EXPECT_OK(client);

    auto to_addr = *SocketAddr::NewIPv4("127.0.0.1", 62781);
    EXPECT_THAT((*client)->SendTo("Hello World", 0, &to_addr),
                IsOkAndHolds(11));

    SocketAddr src_addr;
    EXPECT_THAT((*server)->RecvFrom(11, 0, &src_addr),
                IsOkAndHolds("Hello World"));
    EXPECT_THAT(src_addr.ip(), IsOkAndHolds("127.0.0.1"));
}

}  // namespace
}  // namespace net
