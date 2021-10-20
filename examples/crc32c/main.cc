#include <iostream>

#include "absl/strings/string_view.h"
#include "crc32c/crc32c.h"

int main() {
    const std::uint8_t buffer[] = {0, 0, 0, 0};
    std::uint32_t result;

    // Process a raw buffer.
    result = crc32c::Crc32c(buffer, 4);
    std::cout << result << " = " << 1214729159 << std::endl;

    // Process a std::string.
    std::string string;
    string.resize(4);
    result = crc32c::Crc32c(string);
    std::cout << result << " = " << 1214729159 << std::endl;

    // If you have C++17 support, process a std::string_view.
    absl::string_view string_view(string);
    result = crc32c::Crc32c(string_view.data(), string_view.length());
    std::cout << result << " = " << 1214729159 << std::endl;

    return 0;
}
