#include "gflags/gflags.h"
#include "glog/logging.h"

DECLARE_bool(logtostderr);

int main(int argc, char* argv[]) {
    // Initialize Google’s logging library.
    FLAGS_logtostderr = true;
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    google::InitGoogleLogging(argv[0]);

    // ...
    LOG(INFO) << "Hello GLOG";

    return 0;
}
