package(default_visibility = ["//visibility:public"])

proto_library(
    name = "hello_world_proto",
    srcs = ["hello_world.proto"],
)

cc_proto_library(
    name = "hello_world_cc_proto",
    deps = [":hello_world_proto"],
)
