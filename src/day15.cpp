#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <limits>

#include <get_input.hpp>

const int int_max = std::numeric_limits<int>::max();

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

struct Point {
    int x, y;

    std::vector<Point> get_adjacents() const;

    void move_towards(const Point& pt);
    bool is_adjacent(const Point& pt);
    int distance(const Point& pt);
};

std::vector<Point> Point::get_adjacents() const
    // returned in order of up, left, right, down
{
    std::vector<Point> vpoints;

    vpoints.emplace_back(Point{x, y-1});
    vpoints.emplace_back(Point{x-1, y});
    vpoints.emplace_back(Point{x+1, y});
    vpoints.emplace_back(Point{x, y+1});

    return vpoints;
}

void Point::move_towards(const Point& pt)
{
    if (pt.y < y)
        --y;
    else if (pt.x < x)
        --x;
    else if (pt.x > x)
        ++x;
    else
        ++y;
}

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

std::ostream& operator<<(std::ostream& os, const Point& pt)
{
    return os << '(' << pt.x << ", " << pt.y << ')';
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

class Dist_map {
private:
    std::vector<std::vector<int>> dist_map;
public:
    Dist_map(const std::vector<std::string>& battle_map);

    void print_grid() const;
    void map_distances(const Point& pt);

    int operator()(const Point& p) const { return dist_map[p.y][p.x]; }
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
            dist_map[i][j] = battle_map[i][j] == '.' ? int_max : -1;
}

void Dist_map::print_grid() const
{
    for (const auto& row : dist_map) {
        for (const auto i : row)
            std::cout << i << '\t';
        std::cout << '\n';
    }
}

void Dist_map::map_distances(const Point& pt)
{
    dist_map[pt.y][pt.x] = 0;

    map_adjacents(pt, 1);
}

void Dist_map::map_adjacents(const Point& pt, int n)
{
    auto adjacents = pt.get_adjacents();

    std::vector<std::pair<bool,Point>> bool_adjs;
    for (const auto adj : adjacents)
        bool_adjs.emplace_back(std::make_pair(false, std::move(adj)));

    for (auto& adj : bool_adjs) {
        int x = adj.second.x;
        int y = adj.second.y;
        if (0 <= x && x < dist_map.front().size() && 0 <= y && 
                y < dist_map.size()) {      // check if valid point
            int* pi = &dist_map[y][x];      // get current distance value
            if (0 < *pi && n < *pi) {       // if 'open' and greater than n..
                *pi = n;                    // ..set value to n..
                adj.first = true;           // and mark it to be mapped
            }
        }
    }

    for (auto& adj : bool_adjs)
        if (adj.first)
            map_adjacents(adj.second, n + 1);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

class Unit;

class Battle {
private:
    int rounds = 0;
    std::vector<std::string> battle_map;
    std::vector<Unit> units;
public:
    explicit Battle(std::vector<std::string> input);

    void print_grid() const;
    int get_score() const;
    Dist_map get_dist_map(const Point& origin) const;

    void run_simulation();
    std::vector<Unit*> get_targets(const char type);

    void write_move(const Point& from, const Point& to, const char token);
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
    void move(const std::vector<Unit*>& vu, const Dist_map& dm);
    void attack(Unit& u) const { u.take_damage(atk_pwr); }
    void take_damage(int damage) { hp -= damage; }
private:
    std::vector<Unit*>::iterator check_adj(std::vector<Unit*>& targets) const;
    Point get_target_square(const std::vector<Unit*>& tars,
                            const Dist_map& dmap) const;
};

bool Unit::take_turn()
{
    auto targets = pb->get_targets(token);
    if (targets.empty())
        return false;                               // end of battle!

    auto it_adj = check_adj(targets);               // attack if already adj
    if (it_adj != std::end(targets))
        attack(**it_adj);
    else {
        auto dist_map = pb->get_dist_map(get_loc());
        move(targets, dist_map);

        it_adj = check_adj(targets);
        if (it_adj != std::end(targets))
            attack(**it_adj);
    }

    pb->print_grid();                           // TEMP
    return true;
}

void Unit::move(const std::vector<Unit*>& vu, const Dist_map& dmap)
{
    Point temp = location;
    auto goal = get_target_square(vu, dmap);
    //location.move_towards(get_target_square(vu, dmap));
    location.move_towards(goal);
    std::cout << get_tok() << ' ' << temp << " to " << get_loc()
              << " on way to " << goal << '\n';
    pb->write_move(temp, get_loc(), get_tok());
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

Point Unit::get_target_square(const std::vector<Unit*>& tars,
                              const Dist_map& dmap) const
{
    std::vector<Point> all_pts;
    for (const auto pu : tars) {
        auto vtemp = pu->get_loc().get_adjacents();
        std::copy(std::begin(vtemp), std::end(vtemp),
                  std::back_inserter(all_pts));
    }

    all_pts.erase(std::remove_if(std::begin(all_pts), std::end(all_pts),
                [&dmap](const auto& pt) {
                    return dmap(pt) < 0 || dmap(pt) == int_max;
                }), std::end(all_pts));

    auto sm_it = std::min_element(std::begin(all_pts), std::end(all_pts),
            [&dmap](const auto& a, const auto& b) {
                return dmap(a) < dmap(b);
            });

    int smallest = dmap(*sm_it);
    all_pts.erase(std::remove_if(std::begin(all_pts), std::end(all_pts),
                [&dmap, smallest](const auto& pt) {
                    return smallest < dmap(pt);
                }), std::end(all_pts));

    std::sort(std::begin(all_pts), std::end(all_pts));
    
    return all_pts.front();
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

void Battle::print_grid() const
{
    for (const auto& row : battle_map) {
        for (const auto ch : row)
            std::cout << ch;
        std::cout << '\n';
    }
    std::cout << '\n';
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

void Battle::write_move(const Point& from, const Point& to, const char token)
{
    battle_map[from.y][from.x] = '.';
    battle_map[to.y][to.x] = token;
}

Dist_map Battle::get_dist_map(const Point& origin) const
{
    auto dist_map = Dist_map{battle_map};

    dist_map.map_distances(origin);

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
