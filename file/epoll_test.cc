#include "file/epoll.h"

#include "gtest/gtest.h"
#include "utils/testing.h"

namespace file {
namespace {

using ::utils::testing::IsOkAndHolds;
using ::utils::testing::StatusIs;

TEST(EPoll, Create) { EXPECT_OK(EPoll::Create()); }

TEST(EPoll, AutoClose) {
    int fd = -1;
    {
        auto epoll = EPoll::Create();
        EXPECT_OK(epoll);

        fd = (*epoll)->efd();
    }
    EXPECT_EQ(close(fd), -1);
    EXPECT_EQ(errno, EBADF);
}

TEST(EPoll, EPoll) {
    int pipefd1[2];
    int pipefd2[2];
    ASSERT_EQ(pipe(pipefd1), 0);
    ASSERT_EQ(pipe(pipefd2), 0);

    auto read1 = std::unique_ptr<File>(new File(pipefd1[0]));
    auto write1 = std::unique_ptr<File>(new File(pipefd1[1]));
    auto read2 = std::unique_ptr<File>(new File(pipefd2[0]));
    auto write2 = std::unique_ptr<File>(new File(pipefd2[1]));

    auto epoll = *EPoll::Create();

    EXPECT_OK(epoll->Add(read1.get(), EPOLLIN, (void*)1));
    EXPECT_OK(epoll->Add(read2.get(), EPOLLIN, (void*)2));

    auto nowait = absl::Milliseconds(0);
    {
        auto events = epoll->Wait(1024, &nowait);
        EXPECT_OK(events);
        EXPECT_TRUE(events->empty());
    }

    EXPECT_OK(write1->Write("hello world"));
    {
        auto events = epoll->Wait(1024, &nowait);
        EXPECT_OK(events);
        EXPECT_EQ(events->size(), 1);
        EXPECT_EQ(events->at(0).ptr, (void*)1);
        EXPECT_EQ(events->at(0).events, EPOLLIN);
    }

    EXPECT_OK(write2->Write("hello world"));
    {
        auto events = epoll->Wait(1024, &nowait);
        EXPECT_OK(events);
        EXPECT_EQ(events->size(), 2);
        EXPECT_EQ(events->at(0).ptr, (void*)1);
        EXPECT_EQ(events->at(0).events, EPOLLIN);
        EXPECT_EQ(events->at(1).ptr, (void*)2);
        EXPECT_EQ(events->at(1).events, EPOLLIN);
    }

    EXPECT_THAT(read1->Read(1024), IsOkAndHolds("hello world"));
    {
        auto events = epoll->Wait(1024, &nowait);
        EXPECT_OK(events);
        EXPECT_EQ(events->size(), 1);
        EXPECT_EQ(events->at(0).ptr, (void*)2);
        EXPECT_EQ(events->at(0).events, EPOLLIN);
    }

    EXPECT_OK(epoll->Modify(read2.get(), EPOLLOUT));
    {
        auto events = epoll->Wait(1024, &nowait);
        EXPECT_OK(events);
        EXPECT_EQ(events->size(), 0);
    }

    EXPECT_OK(write1->Write("hello world"));
    {
        auto events = epoll->Wait(1024, &nowait);
        EXPECT_OK(events);
        EXPECT_EQ(events->size(), 1);
        EXPECT_EQ(events->at(0).ptr, (void*)1);
        EXPECT_EQ(events->at(0).events, EPOLLIN);
    }

    EXPECT_OK(epoll->Delete(read1.get()));
    {
        auto events = epoll->Wait(1024, &nowait);
        EXPECT_OK(events);
        EXPECT_EQ(events->size(), 0);
    }
}

}  // namespace
}  // namespace file
