#include <iostream>
#include <sstream>
#include <vector>

namespace utils {

auto get_input_lines(std::istream& is = std::cin)
{
    std::vector<std::string> vs;
    std::string line;
    while (std::getline(is, line))
        vs.push_back(std::move(line));

    return vs;
}

template<typename T>
auto get_input_values(std::istream& is = std::cin)
{
    std::vector<T> vt;
    for (T val; is >> val; )
        vt.push_back(val);

    return vt;
}

template<typename T>
auto get_input_value_matrix(std::istream& is = std::cin, size_t s_hint = 100)
{
    std::vector<std::vector<T>> vvt;
    vvt.reserve(s_hint);                // reallocations could get heavy

    for (std::string line; std::getline(is, line); ) {
        std::vector<T> vt;

        std::istringstream iss {line};
        for (T val; iss >> val; )
            vt.push_back(val);

        vvt.push_back(vt);
    }
    return vvt;
}

}   // namespace utils
