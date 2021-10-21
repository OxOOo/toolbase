package(default_visibility = ["//visibility:public"])

cc_library(
    name = "testing",
    hdrs = ["testing.h"],
    deps = [
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_googletest//:gtest",
    ],
)

cc_test(
    name = "testing_test",
    srcs = ["testing_test.cc"],
    deps = [
        ":testing",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "status_macros",
    hdrs = ["status_macros.h"],
    deps = [
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
    ],
)

cc_test(
    name = "status_macros_test",
    srcs = ["status_macros_test.cc"],
    deps = [
        ":status_macros",
        "@com_google_glog//:glog",
        "@com_google_googletest//:gtest",
    ],
)
