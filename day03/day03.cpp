#include <iostream>
#include <vector>
#include <regex>
#include <algorithm>
#include <array>

#include <get_input.hpp>

constexpr size_t FAB_WIDTH = 1000;

using Fabric = std::array<std::array<int,FAB_WIDTH>,FAB_WIDTH>;

struct Claim {
    int id = 0;
    int x = 0;
    int y = 0;
    int w = 0;
    int h = 0;
};

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

auto parse_claims(const std::vector<std::string>& vs)
{
    std::vector<Claim> vc;
    vc.reserve(vs.size());

    const std::regex claim_pat {R"(^#(\d+)\s@\s(\d+),(\d+):\s(\d+)x(\d+))"};
    std::smatch matches;

    std::for_each(std::begin(vs), std::end(vs),
            [&](const std::string& line) {
                std::regex_match(line, matches, claim_pat);
                auto it = ++matches.begin();
                Claim claim {};
                claim.id = std::stoi(*it++);
                claim.x = std::stoi(*it++);
                claim.y = std::stoi(*it++);
                claim.w = std::stoi(*it++);
                claim.h = std::stoi(*it++);
                vc.push_back(claim);
            });

    return vc;
}

auto map_claims(const std::vector<Claim>& vc)
{
    Fabric fabric;
    /*
    std::for_each(std::begin(fabric), std::end(fabric),
            [](auto vec) { vec.fill(0); });
    */
    for (size_t i = 0; i < FAB_WIDTH; ++i)
        for (size_t j = 0; j < FAB_WIDTH; ++j)
            fabric[i][j] = 0;

    std::for_each(std::begin(vc), std::end(vc),
            [&fabric](const Claim& c) {
                for (int i = c.y; i < c.y + c.h; ++i)
                    for (int j = c.x; j < c.x + c.w; ++j)
                        ++(fabric[i][j]);
            });

    return fabric;
}

int count_overlaps(const Fabric& aai)
{
    int count = 0;
    /*
    for (const auto& ai : aai)
        for (const auto i : ai)
            if (i > 1)
                ++count;
    */
    for (size_t i = 0; i < FAB_WIDTH; ++i)
        for (size_t j = 0; j < FAB_WIDTH; ++j)
            if (aai[i][j] > 1) {
                ++count;
            }
    return count;
}

int find_intact_id(const std::vector<Claim>& vc, const Fabric& fab)
{
    for (const auto& c : vc) {
        bool intact = true;
        for (int i = c.y; intact && i < c.y + c.h; ++i)
            for (int j = c.x; intact && j < c.x + c.w; ++j)
                if (fab[i][j] != 1)
                    intact = false;
        if (intact)
            return c.id;
    }

    return 0;       // no swaths are intact
}

auto count_overlapping_claims(const std::vector<std::string>& vs)
{
    auto vc = parse_claims(vs);
    auto fabric = map_claims(vc);
    auto overlaps = count_overlaps(fabric);
    auto intact_id = find_intact_id(vc, fabric);
    return std::make_pair(overlaps, intact_id);
}

int main()
{
    auto input = utils::get_input_lines();

    auto parts = count_overlapping_claims(input);
    std::cout << "Part 1: " << parts.first << '\n';
    std::cout << "Part 2: " << parts.second << '\n';
}
