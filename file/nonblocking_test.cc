#include "file/nonblocking.h"

#include <unistd.h>

#include "gtest/gtest.h"
#include "utils/testing.h"

namespace file {
namespace {

using ::utils::testing::IsOkAndHolds;
using ::utils::testing::StatusIs;

TEST(NonblockingIO, NonblockingIO) {
    int pipefd[2];
    EXPECT_EQ(pipe(pipefd), 0);
    auto read_file = std::unique_ptr<File>(new File(pipefd[0]));
    auto write_file = std::unique_ptr<File>(new File(pipefd[1]));
    auto read_io = NonblockingIO::Create(std::move(read_file));
    auto write_io = NonblockingIO::Create(std::move(write_file));
    EXPECT_OK(read_io);
    EXPECT_OK(write_io);

    EXPECT_THAT((*read_io)->TryReadOnce(1024), IsOkAndHolds(0));

    int message_count = 0;
    while (true) {
        if (!(*write_io)->HasDataToWrite()) {
            (*write_io)->AppendWriteData("hello world");
            message_count++;
        }
        auto write = (*write_io)->TryWriteOnce();
        EXPECT_OK(write);
        if (write.value() == 0) {
            break;
        }
    }
    EXPECT_GE(message_count, 1024);

    while (message_count > 0) {
        if ((*write_io)->HasDataToWrite()) {
            EXPECT_OK((*write_io)->TryWriteOnce());
        }
        EXPECT_OK((*read_io)->TryReadOnce(1024));
        EXPECT_TRUE((*read_io)->HasDataToRead());
        while ((*read_io)->DataToRead().substr(0, 11) == "hello world") {
            (*read_io)->ConsumeReadData(11);
            message_count--;
        }
    }
    EXPECT_EQ(message_count, 0);
    EXPECT_THAT((*read_io)->TryReadOnce(1024), IsOkAndHolds(0));
}

}  // namespace
}  // namespace file
