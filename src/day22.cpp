#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <limits>

#include <get_input.hpp>

const int int_max = std::numeric_limits<int>::max();

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

class Point {
private:
    int xx, yy;
public:
    Point(int x, int y) : xx{x}, yy{y} { }

    int x() const { return xx; }
    int y() const { return yy; }

    const std::vector<Point> get_adjacents() const;
    const std::vector<Point> get_diagonals() const;

    void move(int dx, int dy) { xx += dx; yy += dy; }
};

const std::vector<Point> Point::get_adjacents() const
{
    std::vector<Point> vadj;
    vadj.emplace_back(Point{x(), y()-1});
    vadj.emplace_back(Point{x()-1, y()});
    vadj.emplace_back(Point{x()+1, y()});
    vadj.emplace_back(Point{x(), y()+1});
    return vadj;
}

const std::vector<Point> Point::get_diagonals() const
{
    std::vector<Point> vdia;
    vdia.emplace_back(Point{x()-1, y()-1});
    vdia.emplace_back(Point{x()+1, y()-1});
    vdia.emplace_back(Point{x()-1, y()+1});
    vdia.emplace_back(Point{x()+1, y()+1});
    return vdia;
}

const Point create_projection(const Point& a, const Point& b)
{
    return Point{ 2 * b.x() - a.x(), 2 * b.y() - a.y() };
}

bool operator==(const Point& a, const Point& b)
{
    return a.x() == b.x() && a.y() == b.y();
}

bool operator!=(const Point& a, const Point& b)
{
    return (!(a==b));
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

template<typename T>
class Grid {
private:
    std::vector<std::vector<T>> grid;
    const T filler;
public:
    Grid(size_t width, size_t height, T fill_value);

    T& operator()(const Point& p) { return grid[p.y()][p.x()]; }
    T& operator()(size_t x, size_t y) { return grid[y][x]; }

    void print() const;

    const std::vector<std::vector<T>>& get_grid() const { return grid; }

    size_t width() const { return grid.front().size(); }
    size_t height() const { return grid.size(); }

    void resize(int x, int y);
};

template<typename T>
Grid<T>::Grid(size_t width, size_t height, T fill_value)
    : grid{std::vector<std::vector<T>>(height)}, filler{fill_value}
{
    for (auto& vt : grid) {
        vt.resize(width);
        std::fill(std::begin(vt), std::end(vt), filler);
    }
}

template<typename T>
void Grid<T>::print() const
{
    for (const auto& vt : grid) {
        for (const auto t : vt)
            std::cout << t;
        std::cout << '\n';
    }
}

template<typename T>
void Grid<T>::resize(int x, int y)
{
    grid.resize(y);
    for (auto& vt : grid) {
        vt.resize(x);
        std::fill(std::begin(vt), std::end(vt), filler);
    }
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

class Cave {
private:
    Grid<int> verosion;
    Grid<char> vtype;
    Point mouth;
    Point target;
    int depth;
public:
    explicit Cave(const std::string& input);

    int calc_risk() const;

private:
    int calc_geologic_index(const Point& p);
    int calc_erosion_level(const Point& p);

    void fill_verosion();
    void fill_vtype();
};

Cave::Cave(const std::string& input)
    : verosion{Grid<int>(0, 0, 0)}
    , vtype{Grid<char>(0, 0, '\0')}
    , mouth{0, 0}
    , target{0, 0}
{
    std::istringstream iss {input};
    std::string label;
    char ch;
    int x, y;
    iss >> label >> depth >> label >> x >> ch >> y;

    verosion.resize(x + 1, y + 1);
    vtype.resize(x + 1, y + 1);

    target.move(x, y);

    vtype(mouth) = 'M';
    vtype(target) = 'T';

    fill_verosion();
    fill_vtype();
}

int Cave::calc_risk() const
{
    auto grid = vtype.get_grid();

    std::vector<int> row_risks;
    for (const auto& row : grid)
        row_risks.push_back(std::accumulate(std::begin(row), std::end(row), 0,
                    [](int& sum, char c) {
                        switch (c) {
                        case '=':   return sum + 1;
                        case '|':   return sum + 2;
                        default:    return sum;
                        }
                    }));

    return std::accumulate(std::begin(row_risks), std::end(row_risks), 0);
}

int Cave::calc_geologic_index(const Point& p)
{
    if (p == mouth || p == target)
        return 0;
    else if (p.y() == 0)
        return p.x() * 16807;
    else if (p.x() == 0)
        return p.y() * 48271;
    else
        return verosion(Point{p.x()-1,p.y()}) *
               verosion(Point{p.x(),p.y()-1});
}

int Cave::calc_erosion_level(const Point& p)
{
    return (calc_geologic_index(p) + depth) % 20183;
}

void Cave::fill_verosion()
{
    for (int i = 0; i < verosion.height(); ++i)
        for (int j = 0; j < verosion.width(); ++j) {
            Point p {j, i};
            verosion(p) = calc_erosion_level(p);
        }
}

void Cave::fill_vtype()
{
    for (size_t i = 0; i < vtype.height(); ++i)
        for (size_t j = 0; j < vtype.width(); ++j) {
            Point p {j, i};
            switch (verosion(p) % 3) {
            case 0:
                vtype(p) = '.';
                break;
            case 1:
                vtype(p) = '=';
                break;
            case 2:
                vtype(p) = '|';
                break;
            default:
                break;
            }
        }
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

int main(int argc, char* argv[])
{
    std::cout << "AoC 2018 Day 22 - Mode Maze\n";

    auto input = utils::get_input_string(argc, argv, "22");

    Cave cave {input};

    auto part1 = cave.calc_risk();
    std::cout << "Part 1: " << part1 << '\n';
}
