#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>

#include <get_input.hpp>

int main(int argc, char* argv[])
{
    std::cout << "AoC 2018 Day 22 - Mode Maze\n";

    auto input = utils::get_input_string(argc, argv, "22");
    std::cout << input.size() << '\n';
}
