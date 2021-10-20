#include <iostream>
#include <string>

#include "snappy.h"

int main() {
    std::string data = "hello world data";

    std::string compressed;
    snappy::Compress(data.data(), data.length(), &compressed);
    std::cout << "compressed length = " << compressed.length() << std::endl;

    size_t uncompressed_length;
    std::cout << "get length : "
              << snappy::GetUncompressedLength(compressed.data(),
                                               compressed.length(),
                                               &uncompressed_length)
              << std::endl;
    std::cout << "length = " << uncompressed_length << std::endl;

    std::string uncompressed;
    snappy::Uncompress(compressed.data(), compressed.length(), &uncompressed);
    std::cout << "uncompressed = " << uncompressed << std::endl;

    return 0;
}
