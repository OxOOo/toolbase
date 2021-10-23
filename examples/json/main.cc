#include <iostream>

#include "nlohmann/json.hpp"

using nlohmann::json;

int main() {
    json j = {{"pi", 3.141},
              {"happy", true},
              {"name", "Niels"},
              {"nothing", nullptr},
              {"answer", {{"everything", 42}}},
              {"list", {1, 0, 2}},
              {"object", {{"currency", "USD"}, {"value", 42.99}}}};

    std::cout << j.dump() << std::endl;

    json j2 = json::parse(j.dump());
    std::cout << j2 << std::endl;

    return 0;
}
