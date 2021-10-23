"""Link third party libraries."""

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:utils.bzl", "maybe")

def repo_leveldb():
    """
    Link leveldb library.
    """

    repo_crc32c()
    repo_snappy()

    maybe(
        http_archive,
        name = "com_google_leveldb",
        build_file = "//third_party/leveldb:leveldb.BUILD",
        sha256 = "a6135d362ebb25ce705f12866361d864450023adb48bb14e5b234c76dea4f2a5",
        strip_prefix = "leveldb-c5d5174a66f02e66d8e30c21ff4761214d8e4d6d",
        urls = [
            "https://github.com/google/leveldb/archive/c5d5174a66f02e66d8e30c21ff4761214d8e4d6d.zip",
        ],
    )

def repo_crc32c():
    """
    Link crc32c library.
    """

    maybe(
        http_archive,
        name = "com_google_crc32c",  # 1.1.2
        build_file = "//third_party/crc32c:crc32c.BUILD",
        sha256 = "a40244a21b9ea50808b214e127e53500f3ef50defca2db2f7125cf95008431bd",
        strip_prefix = "crc32c-02e65f4fd3065d27b2e29324800ca6d04df16126",
        urls = [
            "https://github.com/google/crc32c/archive/02e65f4fd3065d27b2e29324800ca6d04df16126.zip",
        ],
    )

def repo_snappy():
    """
    Link snappy library.
    """

    maybe(
        http_archive,
        name = "com_google_snappy",  # 1.1.9
        build_file = "//third_party/snappy:snappy.BUILD",
        sha256 = "f5c27f3b7099e4cb43b17f21ac78eaf009c7a7c22d38b33b9cbf679afbffc58d",
        strip_prefix = "snappy-2b63814b15a2aaae54b7943f0cd935892fae628f",
        urls = [
            "https://github.com/google/snappy/archive/2b63814b15a2aaae54b7943f0cd935892fae628f.zip",
        ],
    )

def repo_json():
    """
    Link json library.
    """

    maybe(
        http_archive,
        name = "nlohmann_json",  # 1.1.9
        build_file = "//third_party/json:json.BUILD",
        sha256 = "a5af2225a81b5e8842ee6a95715674e734b91b1d8d23a964dc23d7eac338ca60",
        strip_prefix = "json-f5b3fb326c1a651cd3e62201b0e7074cf987f748",
        urls = [
            "https://github.com/nlohmann/json/archive/f5b3fb326c1a651cd3e62201b0e7074cf987f748.zip",
        ],
    )
