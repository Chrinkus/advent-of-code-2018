#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>

#include <get_input.hpp>

class No_spring{};

struct Point {
    int x, y;
};

struct Vein {
    Point start;
    Point end;
    Vein(int x1, int y1, int x2, int y2)
        : start{x1, y1}, end{x2, y2} { }

    int get_min_x() const { return std::min(start.x, end.x); }
    int get_min_y() const { return std::min(start.y, end.y); }
    int get_max_x() const { return std::max(start.x, end.x); }
    int get_max_y() const { return std::max(start.y, end.y); }
};

auto parse_input(const std::vector<std::string>& input)
{
    std::vector<Vein> vv;

    for (const auto& s : input) {
        int x1, y1, x2, y2;
        std::istringstream iss { s };
        char ch;
        iss >> ch;
        if (ch == 'x') {
            iss >> ch >> x1 >> ch;
            x2 = x1;
            iss >> ch >> ch >> y1 >> ch >> ch >> y2;
        } else if (ch == 'y') {
            iss >> ch >> y1 >> ch;
            y2 = y1;
            iss >> ch >> ch >> x1 >> ch >> ch >> x2;
        } else {
            std::cerr << "Bad input read: " << ch << '\n';
        }
        vv.emplace_back(Vein{x1, y1, x2, y2});
    }
    return vv;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

class Ground_slice {
private:
    int min_x, min_y, max_x, max_y; 
    std::vector<std::string> slice;
public:
    explicit Ground_slice(const std::vector<Vein>& vd);

    void print_slice() const;
private:
    int shift_x(int x) const { return x - min_x; }
    int shift_y(int y) const { return y - min_y; }
};

Ground_slice::Ground_slice(const std::vector<Vein>& vd)
{
    min_x = std::min_element(std::begin(vd), std::end(vd),
            [](const auto& a, const auto& b) {
                return a.get_min_x() < b.get_min_x();
            })->get_min_x();
    min_y = std::min_element(std::begin(vd), std::end(vd),
            [](const auto& a, const auto& b) {
                return a.get_min_y() < b.get_min_y();
            })->get_min_y();
    max_x = std::max_element(std::begin(vd), std::end(vd),
            [](const auto& a, const auto& b) {
                return a.get_max_x() < b.get_max_x();
            })->get_max_x();
    max_y = std::max_element(std::begin(vd), std::end(vd),
            [](const auto& a, const auto& b) {
                return a.get_max_y() < b.get_max_y();
            })->get_max_y();

    if (500 < min_x || max_x < 500)
        throw No_spring{};

    slice.resize(max_y + 1 - min_y);
    for (auto& row : slice) {
        row.resize(max_x + 1 - min_x);
        std::fill(std::begin(row), std::end(row), '.');     // fill w/sand
    }

    slice[0][shift_x(500)] = '+';                           // place spring

    for (const auto& v : vd) {
        for (int y = v.start.y; y <= v.end.y; ++y)
            for (int x = v.start.x; x <= v.end.x; ++x)
                slice[shift_y(y)][shift_x(x)] = '#';
    }
}

void Ground_slice::print_slice() const
{
    for (const auto row : slice) {
        for (const auto c : row)
            std::cout << c;
        std::cout << '\n';
    }
}

void Ground_slice::fall_like_water()
{
    int level = 1;
    int x = shift_x(500);

    while (0 < level && level < slice.size()) {
        char* pc = &slice[level][x];
        switch (*pc) {
        case '.':
            // fall
            break;
        case '#':
            // fill above
            break;
        case '~':
            // do nothing?
            break;
        default:
            break;
        }
    }
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

int main(int argc, char* argv[])
{
    std::cout << "AoC 2018 Day 17 - Resevoir Research\n";

    auto input = utils::get_input_lines(argc, argv, "17");

    auto vein_data = parse_input(input);
    Ground_slice ground {vein_data};
    ground.print_slice();
}
