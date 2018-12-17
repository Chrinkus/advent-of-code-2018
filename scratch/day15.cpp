#include <iostream>
#include <vector>
#include <algorithm>

#include <get_input.hpp>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

struct Point {
    unsigned int x, y;

    void move(int dx, int dy) { x += dx; y += dy; }
};

bool operator<(const Point& a, const Point& b)
{
    return a.y < b.y || (a.y == b.y && a.x < b.x);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

class Dist_map {
private:
    std::vector<std::vector<int>> dist_map;
public:
    Dist_map(const std::vector<std::string>& battle_map);

    void map_distances(const Point& pt);
private:
    void map_adjacents(const Point& pt, int n);
};

Dist_map::Dist_map(const std::vector<std::string>& battle_map)
{
    dist_map.resize(battle_map.size());

    for (auto& vi : dist_map)
        vi.resize(battle_map.front().size());

    for (size_t i = 0; i < battle_map.size(); ++i)
        for (size_t j = 0; j < battle_map[i].size(); ++j)
            dist_map[i][j] = battle_map[i][j] == '.' ? INT_MAX : -1;
}

void Dist_map::map_distances(const Point& pt)
{
    dist_map[pt.y][pt.x] = 0;
}

void Dist_map::map_adjacents(const Point& p, int n)
{
    int node = dist_map[p.y][p.x];

    if (node > 0 && node > n) {
        dist_map[p.y][p.x] = n++;
        map_adjacents(Point{p.y-1,p.x}, n);     // up
        map_adjacents(Point{p.y,p.x-1}, n);     // left
        map_adjacents(Point{p.y,p.x+1}, n);     // right
        map_adjacents(Point{p.y+1,p.x}, n);     // down
    }
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

class Unit {
private:
    const char token;
    Point location;
    int hp = 200;
    int atk = 3;
public:
    Unit(char tt, unsigned int xx, unsigned int yy)
        : token{tt}, loc{xx, yy} { }

    Point get_loc()  const { return location; }
    char  get_tok()  const { return token; }
    int   get_hp()   const { return hp; }
    bool  is_alive() const { return hp > 0 ? true : false; }

    bool take_turn(Battle& battle);
};

bool Unit::take_turn(Battle& battle)
{
    auto targets = battle.get_targets(token);       // need
    if (targets.empty())
        return false;                               // end of battle

    auto int_map = battle.get_int_map();
}

bool operator<(const Unit& a, const Unit& b)
{
    return a.get_loc() < b.get_loc();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

/*
class Elf : public Unit {
public:
    Elf(char tt, unsigned int xx, unsigned int yy)
        : Unit{tt, xx, yy} { }
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

class Goblin : public Unit {
public:
    Goblin(char tt, unsigned int xx, unsigned int yy)
        : Unit{tt, xx, yy} { }
};
*/

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

/*
struct Open_square {
    Point loc;
    int dist;
};

bool operator<(const Open_square& a, const Open_square& b)
{
    return a.dist < b.dist && a.loc < b.loc;
}

struct Target {
    Unit* u;
    std::vector<Open_square> open_squares;
};
*/

class Battle {
private:
    int turns = 0;                          // completed turns
    std::vector<std::string> battle_map;
    std::vector<Unit> units;
public:
    explicit Battle(std::vector<std::string> input);

    int run_simulation();
    std::vector<int> get_int_map() const;
    //std::vector<Target> get_targets(const Unit& u);
private:
    bool process_turn();
    //std::vector<Point> get_open_squares(const Point& p) const;
};

Battle::Battle(std::vector<std::string> input)
    : battle_map{std::move{input}}
{
    for (size_t y = 0; y < battle_map.size(); ++y)
        for (size_t x = 0; x < battle_map[y].size(); ++x) {
            switch (battle_map[y][x]) {
            case 'E':
                units.emplace_back(Unit{'E', x, y});
                break;
            case 'G':
                units.emplace_back(Unit{'G', x, y});
                break;
            default:
                // do nothing for #'s and .'s
                break;
            }
        }
}

int Battle::run_simulation()
{
    // run sim
    while (process_turn())
        ++turns;

    // return result
    units.erase(std::remove_if(std::begin(units), std::end(units),
                [](auto& u) { return !u.is_alive(); }),
            std::end(units));

    return turns * std::accumulate(std::begin(units), std::end(units), 0
                [](auto sum, auto& u) { return sum + u.get_hp(); });
}

std::vector<int> Battle::get_int_map() const
{
    std::vector<std::vector<int>> int_map;
    int_map.resize(battle_map.size());

    for (auto& vi : int_map)
        vi.resize(battle_map.front().size());

    for (size_t i = 0; i < battle_map.size(); ++i)
        for (size_t j = 0; j < battle_map[i].size(); ++j)
            int_map[i][j] = battle_map[i][j] == '.' ? INT_MAX : -1;

    return int_map;
}

std::vector<Unit*> Battle::get_targets(const Unit& u)
{
    std::vector<Unit*> targets;

    for (auto& unit : units)
        if (unit.get_tok() != u.get_tok() && unit.is_alive())
            targets.push_back(&unit);

    return targets;
}

/*
std::vector<Point> Battle::get_open_squares(const Point& p)
{
    std::vector<Point> vpoints;

    if (battle_map[p.y-1][p.x] == '.')
        vpoints.emplace_back(Point{p.x, p.y-1});
    if (battle_map[p.y][p.x-1] == '.')
        vpoints.emplace_back(Point{p.x-1, p.y});
    if (battle_map[p.y][p.x+1] == '.')
        vpoints.emplace_back(Point{p.x+1, p.y});
    if (battle_map[p.y+1][p.x] == '.')
        vpoints.emplace_back(Point{p.x, p.y+1});

    return vpoints;
}
*/

bool Battle::process_turn()
    // return false from unit.take_turn to signify zero targets left
    // - don't increment turn and tally up the result
{
    std::sort(std::begin(units), std::end(units));

    for (auto& unit : units)
        if (unit.is_alive() && !unit.take_turn(*this))
            return false;

    return true;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

int main(int argc, char* argv[])
{
    std::cout << "AoC 2018 Day 15 - ????\n";

    auto input = utils::get_input_lines(argc, argv, "15");

    auto battle = Battle{input};

    auto part1 = battle.run_simulation();
    std::cout << "Part 1: " << part1 << '\n';
}
