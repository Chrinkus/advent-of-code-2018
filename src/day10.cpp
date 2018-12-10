#include <iostream>
#include <algorithm>
#include <vector>
#include <regex>
#include <fstream>

#include <get_input.hpp>

struct Light {
    int x, y, dx, dy;
};

auto parse_lights(const std::vector<std::string>& input)
{
    std::vector<Light> vlights;

    static const std::regex pat {
        R"(^position=<\s?(-?\d+),\s+(-?\d+).*<\s?(-?\d+),\s+(-?\d+)>$)"
    };

    for (const auto& line : input) {
        std::smatch matches;
        std::regex_match(line, matches, pat);
        if (!matches.empty()) {
            auto it = ++std::begin(matches);
            int x, y, dx, dy;
            x  = std::stoi(*it++);
            y  = std::stoi(*it++);
            dx = std::stoi(*it++);
            dy = std::stoi(*it++);
            vlights.emplace_back(Light{x, y, dx, dy});
        } else {
            std::cout << "Problem: No match! Check your foolish regex..\n";
        }
    }

    return vlights;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

class Grid {
public:
    Grid(const std::vector<Light>& vlights);

    void print_sky() const;
private:

    int shift_x(int x) const { return x + std::abs(min_x); }
    int shift_y(int y) const { return y + std::abs(min_y); }

    int min_x, min_y, max_x, max_y;
    std::vector<std::vector<char>> grid;
};

Grid::Grid(const std::vector<Light>& vlights)
{
    min_x = std::min_element(std::begin(vlights), std::end(vlights),
            [](const Light& a, const Light& b) { return a.x < b.x; })->x;
    min_y = std::min_element(std::begin(vlights), std::end(vlights),
            [](const Light& a, const Light& b) { return a.y < b.y; })->y;
    max_x = std::max_element(std::begin(vlights), std::end(vlights),
            [](const Light& a, const Light& b) { return a.x < b.x; })->x;
    max_y = std::max_element(std::begin(vlights), std::end(vlights),
            [](const Light& a, const Light& b) { return a.y < b.y; })->y;

    std::cout << min_x << ' ' << min_y << ' ' << max_x << ' ' << max_y << '\n';
    grid.resize(max_y + 1 - min_y);
    for (auto& vc : grid) {
        vc.resize(max_x + 1 - min_x);
        std::fill(std::begin(vc), std::end(vc), '.');
    }

    for (const auto& l : vlights) {
        grid[shift_y(l.y)][shift_x(l.x)] = '#';
    }
}

void Grid::print_sky() const
{
    for (const auto& vl : grid) {
        for (const auto l : vl)
            std::cout << l;
        std::cout << '\n';
    }
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

struct Min_max {
    int get_width() const { return std::abs(min_x) + std::abs(max_x); }
    int get_height() const { return std::abs(min_y) + std::abs(max_y); }
    int min_x, min_y, max_x, max_y;
};

bool operator==(const Min_max& a, const Min_max& b)
{
    return a.min_x == b.min_x &&
           a.min_y == b.min_y &&
           a.max_x == b.max_x &&
           a.max_y == b.max_y;
}

bool operator!=(const Min_max& a, const Min_max& b)
{
    return !(a == b);
}

bool operator<(const Min_max& a, const Min_max& b)
{
    return a.get_width() < b.get_width() &&
           a.get_height() < b.get_height();
}

class Star_pattern {
public:
    Star_pattern(const std::vector<std::string>& input)
        : vlights{parse_lights(input)} { mm = get_min_max(); }

    int time_till_smallest();

    std::vector<Light> get_vlights() const { return vlights; }
    void advance_time();
    void advance_time(int t);

private:
    Min_max get_min_max() const;
    bool set_min_max_if_smaller();
    void rewind_time();

    Min_max mm;
    std::vector<Light> vlights;
};

int Star_pattern::time_till_smallest()
{
    int time = 0;
    do {
        advance_time();
        ++time;
    } while (set_min_max_if_smaller());
    rewind_time();
    return time;
}

Min_max Star_pattern::get_min_max() const
{
    Min_max temp;

    temp.min_x = std::min_element(std::begin(vlights), std::end(vlights),
            [](const Light& a, const Light& b) { return a.x < b.x; })->x;
    temp.min_y = std::min_element(std::begin(vlights), std::end(vlights),
            [](const Light& a, const Light& b) { return a.y < b.y; })->y;
    temp.max_x = std::max_element(std::begin(vlights), std::end(vlights),
            [](const Light& a, const Light& b) { return a.x < b.x; })->x;
    temp.max_y = std::max_element(std::begin(vlights), std::end(vlights),
            [](const Light& a, const Light& b) { return a.y < b.y; })->y;

    return temp;
}

bool Star_pattern::set_min_max_if_smaller()
{
    auto temp = get_min_max();
    if (temp < mm) {
        mm = temp;
        return true;
    }
    return false;
}

void Star_pattern::advance_time()
{
    for (auto& l : vlights) {
        l.x += l.dx;
        l.y += l.dy;
    }
}

void Star_pattern::advance_time(int t)
{
    while (--t)
        advance_time();
}

void Star_pattern::rewind_time()
{
    for (auto& l : vlights) {
        l.x -= l.dx;
        l.y -= l.dy;
    }
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

int main()
{
    std::cout << "AoC 2018 Day 10 - The Stars Align\n";
    std::ifstream ifs {"../inputs/input10.txt"};

    auto input = utils::get_input_lines(ifs);
    Star_pattern sp {input};
    //auto time_s = sp.time_till_smallest();
    sp.advance_time(10781);
    for (std::string msg; std::cin >> msg; ) {
        Grid grid {sp.get_vlights()};
        grid.print_sky();
        sp.advance_time();
    }
    /*
    Grid grid {5, input};
    grid.run();
    */
}
/*
std::string get_fname(int argc, char* argv[], const std::string& day)
{
    if (argc == 1)
        return "../inputs/input" + day + ".txt";

    std::vector<std::string> varg {argv, argv + argc};

    if (argc == 2 && varg[1] == "-t")
        return "../inputs/test-input" + day + ".txt";

    return varg[1];
}
*/
