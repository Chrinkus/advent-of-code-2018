#include <iostream>
#include <string>
#include <cctype>
#include <algorithm>

#include <get_input.hpp>

bool are_opposite(char a, char b)
{
    if (isupper(a) && tolower(a) == b)
        return true;
    else if (islower(a) && toupper(a) == b)
        return true;
    else
        return false;
}

int react_polymer(std::string s)
{
    auto it = std::begin(s);
    auto sentry = std::end(s);

    while (it != sentry) {
        auto nit = it + 1;
        if (nit != sentry && are_opposite(it[0], nit[0])) {
            it = s.erase(it, nit + 1);
            sentry = std::end(s);

            if (it != std::begin(s))
                --it;
        } else {
            ++it;
        }
    }

    return s.size();
}

void remove_all_of_char(std::string& s, char c)
{
    auto it = std::begin(s);
    auto sentry = std::end(s);
    while (it != sentry) {
        if (it[0] == c) {
            it = s.erase(it);
            sentry = std::end(s);
        } else {
            ++it;
        }
    }
}

std::string remove_lower_upper(std::string s, char c)
    // always given lowercase chars
{
    remove_all_of_char(s, c);
    remove_all_of_char(s, toupper(c));
    return s;
}

int get_shortest_length(std::string s)
{
    std::vector<char> alphabet;
    for (char c = 'a'; c <= 'z'; ++c)
        alphabet.push_back(c);

    std::vector<std::string> reduced;   // 26 versions of reduced input
    for (const auto c : alphabet)
        reduced.emplace_back(remove_lower_upper(s, c));

    std::vector<int> lengths;           // sizes of reduced & reacted input
    for (auto s : reduced)
        lengths.push_back(react_polymer(s));

    auto it_min = std::min_element(std::begin(lengths), std::end(lengths));
    return *it_min;
}

int main()
{
    std::cout << "AoC 2018 Day 5 - Alchemical Reduction\n";

    auto input = utils::get_input_string();

    auto part1 = react_polymer(input);
    std::cout << "Part 1: " << part1 << '\n';

    auto part2 = get_shortest_length(input);
    std::cout << "Part 2: " << part2 << '\n';
}
