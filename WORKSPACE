workspace(name = "toolbase")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# Google Abseil
http_archive(
    name = "com_google_absl",  # 2021-10-08 latest master commit
    sha256 = "2da5202a2ac5daaf368cee1369ce3dca817fffa1b42fbab68cf6dfa23fb93120",
    strip_prefix = "abseil-cpp-59672bec983cb49050363cb47c261a450603409a",
    urls = ["https://github.com/abseil/abseil-cpp/archive/59672bec983cb49050363cb47c261a450603409a.zip"],
)

# GoogleTest/GoogleMock framework. Used by most unit-tests.
http_archive(
    name = "com_google_googletest",  # 2021-07-09T13:28:13Z
    sha256 = "12ef65654dc01ab40f6f33f9d02c04f2097d2cd9fbe48dc6001b29543583b0ad",
    strip_prefix = "googletest-8d51ffdfab10b3fba636ae69bc03da4b54f8c235",
    urls = ["https://github.com/google/googletest/archive/8d51ffdfab10b3fba636ae69bc03da4b54f8c235.zip"],
)

# Google benchmark.
http_archive(
    name = "com_github_google_benchmark",  # 2021-07-01T09:02:54Z
    sha256 = "1cb4b97a90aa1fd9c8e412a6bc29fc13fc140162a4a0db3811af40befd8c9ea5",
    strip_prefix = "benchmark-e451e50e9b8af453f076dec10bd6890847f1624e",
    urls = ["https://github.com/google/benchmark/archive/e451e50e9b8af453f076dec10bd6890847f1624e.zip"],
)

# C++ rules for Bazel.
http_archive(
    name = "rules_cc",  # 2021-06-07T16:41:49Z
    sha256 = "b295cad8c5899e371dde175079c0a2cdc0151f5127acc92366a8c986beb95c76",
    strip_prefix = "rules_cc-daf6ace7cfeacd6a83e9ff2ed659f416537b6c74",
    urls = ["https://github.com/bazelbuild/rules_cc/archive/daf6ace7cfeacd6a83e9ff2ed659f416537b6c74.zip"],
)

# Bazel platform rules.
http_archive(
    name = "platforms",
    sha256 = "b601beaf841244de5c5a50d2b2eddd34839788000fa1be4260ce6603ca0d8eb7",
    strip_prefix = "platforms-98939346da932eef0b54cf808622f5bb0928f00b",
    urls = ["https://github.com/bazelbuild/platforms/archive/98939346da932eef0b54cf808622f5bb0928f00b.zip"],
)

# Protobuf
http_archive(
    name = "rules_proto",
    sha256 = "66bfdf8782796239d3875d37e7de19b1d94301e8972b3cbd2446b332429b4df1",
    strip_prefix = "rules_proto-4.0.0",
    urls = [
        "https://mirror.bazel.build/github.com/bazelbuild/rules_proto/archive/refs/tags/4.0.0.tar.gz",
        "https://github.com/bazelbuild/rules_proto/archive/refs/tags/4.0.0.tar.gz",
    ],
)

load("@rules_proto//proto:repositories.bzl", "rules_proto_dependencies", "rules_proto_toolchains")

rules_proto_dependencies()

rules_proto_toolchains()

# TCMalloc
http_archive(
    name = "com_google_tcmalloc",
    sha256 = "ec064ae7c3aacc0ca838bda774c2a03cbe1ce3330e48a48f75d8dbd4496ffb52",
    strip_prefix = "tcmalloc-9bd7897410d837b98eecfb8bd67d0efb68809f2f",
    urls = [
        "https://github.com/google/tcmalloc/archive/9bd7897410d837b98eecfb8bd67d0efb68809f2f.zip",
    ],
)

# CRC32, Snappy, Leveldb
load("//third_party:repositories.bzl", "repo_crc32c", "repo_snappy", "repo_leveldb")

repo_crc32c()

repo_snappy()

repo_leveldb()
