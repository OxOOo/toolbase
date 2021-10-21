#include "utils/status_macros.h"

#include <memory>

#include "gflags/gflags.h"
#include "glog/logging.h"
#include "gtest/gtest.h"

DECLARE_bool(logtostderr);

namespace {

absl::Status test_ASSIGN_OR_RETURN() {
    {
        int x = 1;
        ASSIGN_OR_RETURN(int y, absl::StatusOr<int>(x));
        EXPECT_EQ(x, 1);
        EXPECT_EQ(y, 1);
    }

    {
        std::string x = "hello world";
        ASSIGN_OR_RETURN(auto y, absl::StatusOr<std::string>(x));
        EXPECT_EQ(x, "hello world");
        EXPECT_EQ(y, "hello world");
    }

    {
        absl::StatusOr<std::string> x = "hello world";
        ASSIGN_OR_RETURN(auto y, x);
        EXPECT_EQ(*x, "hello world");
        EXPECT_EQ(y, "hello world");
    }

    {
        std::string x = "hello world";
        std::string y = std::move(x);
        EXPECT_EQ(x, "");
        EXPECT_EQ(y, "hello world");
    }

    {
        auto x = std::unique_ptr<std::string>(new std::string("hello world"));
        ASSIGN_OR_RETURN(
            auto y, absl::StatusOr<std::unique_ptr<std::string>>(std::move(x)));
        EXPECT_FALSE(x);
        EXPECT_TRUE(y);
    }

    {
        auto x = absl::StatusOr<std::unique_ptr<std::string>>(
            new std::string("hello world"));
        ASSIGN_OR_RETURN(auto y, std::move(x));
        EXPECT_FALSE(*x);
        EXPECT_TRUE(y);
    }

    return absl::OkStatus();
}

}  // namespace

int main(int argc, char* argv[]) {
    FLAGS_logtostderr = true;
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    google::InitGoogleLogging(argv[0]);

    auto status = test_ASSIGN_OR_RETURN();
    if (!status.ok()) {
        LOG(ERROR) << status;
        return 1;
    }
    return 0;
}