#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <limits>

#include <get_input.hpp>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

struct Point {
    int x, y;

    void move(int dx, int dy) { x += dx; y += dy; }
    bool is_adjacent(const Point& pt);
    int distance(const Point& pt);
};

bool Point::is_adjacent(const Point& pt)
{
    return distance(pt) == 1;
}

int Point::distance(const Point& pt)
{
    return std::abs(x - pt.x) + std::abs(y - pt.y);
}

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

    //void map_distances(const Point& pt);
private:
    //bool map_adjacents(const Point& pt, int n);
};

Dist_map::Dist_map(const std::vector<std::string>& battle_map)
{
    dist_map.resize(battle_map.size());

    for (auto& vi : dist_map)
        vi.resize(battle_map.front().size());

    for (size_t i = 0; i < battle_map.size(); ++i)
        for (size_t j = 0; j < battle_map[i].size(); ++j)
            dist_map[i][j] = battle_map[i][j] == '.'
                                ? std::numeric_limits<int>::max()
                                : -1;
}

/*
void Dist_map::map_distances(const Point& pt)
{
    dist_map[pt.y][pt.x] = 0;

    map_adjacents(pt, 1);
}

bool Dist_map::map_adjacents(const Point& pt, int n)
{
    int node = dist_map[pt.y][pt.x];

    if (0 < node && n < node) {
        dist_map[p.y][p.x] = n++;
        map_adjacents(Point{p.y-1,p.x}, n);     // up
        map_adjacents(Point{p.y,p.x-1}, n);     // left
        map_adjacents(Point{p.y,p.x+1}, n);     // right
        map_adjacents(Point{p.y+1,p.x}, n);     // down
    }
}
*/

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

class Unit;

class Battle {
private:
    int rounds = 0;
    std::vector<std::string> battle_map;
    std::vector<Unit> units;
public:
    explicit Battle(std::vector<std::string> input);

    int get_score() const;
    Dist_map get_dist_map(const Point& origin) const;

    void run_simulation();
    std::vector<Unit*> get_targets(const char type);
private:
    bool process_turn();
};

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

class Unit {
private:
    char token;
    Point location;
    int hp = 200;
    int atk_pwr = 3;
    Battle* pb;
public:
    Unit(char tt, int xx, int yy, Battle* pbattle)
        : token{tt}, location{xx, yy}, pb{pbattle} { }

    Point get_loc()  const { return location; }
    char  get_tok()  const { return token; }
    int   get_hp()   const { return hp; }
    bool  is_alive() const { return hp > 0 ? true : false; }

    bool take_turn();
    void attack(Unit& u) const { u.take_damage(atk_pwr); }
    void take_damage(int damage) { hp -= damage; }
private:
    std::vector<Unit*>::iterator check_adj(std::vector<Unit*>& targets) const;
};

bool Unit::take_turn()
{
    auto targets = pb->get_targets(token);
    if (targets.empty())
        return false;                               // end of battle!

    auto it_adj = check_adj(targets);               // attack if already adj
    if (it_adj != std::end(targets))
        attack(**it_adj);

    auto dist_map = pb->get_dist_map(get_loc());

    return false;
}

std::vector<Unit*>::iterator Unit::check_adj(std::vector<Unit*>& targets) const
{
    std::sort(std::begin(targets), std::end(targets),
            [](const auto pa, const auto pb) {
                return pa->get_loc() < pb->get_loc();
            });

    return std::find_if(std::begin(targets), std::end(targets),
            [this](const auto p) {
                return this->get_loc().is_adjacent(p->get_loc());
            });
}

bool operator<(const Unit& a, const Unit& b)
{
    return a.get_loc() < b.get_loc();
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

Battle::Battle(std::vector<std::string> input)
    : battle_map{std::move(input)}
{
    for (size_t y = 0; y < battle_map.size(); ++y)
        for (size_t x = 0; x < battle_map[y].size(); ++x) {
            switch (battle_map[y][x]) {
            case 'E':
                units.emplace_back(Unit{'E', int(x), int(y), this});
                break;
            case 'G':
                units.emplace_back(Unit{'G', int(x), int(y), this});
                break;
            default:
                // do nothing for #'s and .'s
                break;
            }
        }
}

int Battle::get_score() const
{
    return rounds * std::accumulate(std::begin(units), std::end(units), 0,
                [](auto sum, auto& u) { return sum + u.get_hp(); });
}

void Battle::run_simulation()
{
    // run sim
    while (process_turn())
        ++rounds;

    // remove deceased units
    units.erase(std::remove_if(std::begin(units), std::end(units),
                [](auto& u) { return !u.is_alive(); }),
            std::end(units));
}

Dist_map Battle::get_dist_map(const Point& origin) const
{
    auto dist_map = Dist_map{battle_map};

    //dist_map.map_distances(origin);

    return dist_map;
}

std::vector<Unit*> Battle::get_targets(const char type)
{
    std::vector<Unit*> targets;

    for (auto& unit : units)
        if (unit.get_tok() != type && unit.is_alive())
            targets.push_back(&unit);

    return targets;
}

bool Battle::process_turn()
{
    std::sort(std::begin(units), std::end(units));

    for (auto& unit : units)
        if (unit.is_alive() && !unit.take_turn())   // false take_turn ends
            return false;                           // battle

    return true;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

int main(int argc, char* argv[])
{
    std::cout << "AoC 2018 Day 15 - Beverage Bandits\n";

    auto input = utils::get_input_lines(argc, argv, "15");

    auto battle = Battle{input};
    battle.run_simulation();

    auto part1 = battle.get_score();
    std::cout << "Part 1: " << part1 << '\n';
}
