#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <unordered_map>
#include <regex>
#include <memory>

#include <get_input.hpp>

struct Guard {
    int id = 0;
    int total_sleep = 0;
    int sleepiest_min = -1;
    std::vector<std::pair<int,int>> asleep_awake;
    std::vector<int> mins;

    explicit Guard(int i)
        : id{i}, mins{std::vector<int>(60)}
    {
        std::fill(std::begin(mins), std::end(mins), 0);
    }

    void print_mins() const;

    void add_sleep_record(int sl, int aw);
    void calc_total_sleep();
    void map_minutes();
    void set_sleepiest_min();
};

void Guard::print_mins() const
{
    std::cout << "ID# " << id << ' ';
    for (const auto m : mins)
        std::cout << m << ' ';
    std::cout << '\n';
}

void Guard::add_sleep_record(int sl, int aw)
{
    asleep_awake.emplace_back(std::make_pair(sl, aw));
}

void Guard::calc_total_sleep()
{
    total_sleep = std::accumulate(std::begin(asleep_awake),
                                  std::end(asleep_awake),
                                  0,
                                  [](int sum, auto aa) {
                                      return sum + aa.second - aa.first;
                                  });
}

void Guard::map_minutes()
{
    for (const auto& aa : asleep_awake) 
        for (int i = aa.first; i < aa.second; ++i)
            ++(mins[i]);
}

void Guard::set_sleepiest_min()
{
    auto it = std::max_element(std::begin(mins), std::end(mins));
    sleepiest_min = std::distance(std::begin(mins), it);
}

std::ostream& operator<<(std::ostream& os, const Guard& g)
{
    return os << "Id#" << g.id << " total sleep: " << g.total_sleep
              << " sleepy minute: " << g.sleepiest_min;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

using Guard_sleep_map = std::unordered_map<int, std::unique_ptr<Guard>>;

auto retrieve_guard(int key, Guard_sleep_map& gsm)
{
    auto& pg = gsm[key];
    if (pg)
        return pg.get();
    pg = std::make_unique<Guard>(key);
    return pg.get();
}

void calc_sleep_totals(const Guard_sleep_map& gsm)
{
    for (auto& g : gsm)
        g.second->calc_total_sleep();
}

void map_all_mins(const Guard_sleep_map& gsm)
{
    std::for_each(std::begin(gsm), std::end(gsm),
            [](auto& gp) { gp.second->map_minutes(); });
}

void set_all_sleepy_mins(const Guard_sleep_map& gsm)
{
    std::for_each(std::begin(gsm), std::end(gsm),
            [](auto& gp) { gp.second->set_sleepiest_min(); });
}

auto map_sleep_data(const std::vector<std::string>& vs)
{
    Guard_sleep_map gsm;

    static const std::regex id_pat {R"(Guard\s#(\d+))"};
    static const std::regex ts_pat {R"(:(\d{2}))"};
    std::smatch matches;
    
    for (auto it = std::begin(vs); it != std::end(vs); ) {
        int id, sleep, wake;
        std::regex_search(*it, matches, id_pat);
        id = std::stoi(matches[1]);
        auto pg = retrieve_guard(id, gsm);

        ++it;
        while (it != std::end(vs) && !std::regex_search(*it, matches, id_pat)) {
            std::regex_search(*it++, matches, ts_pat);
            sleep = std::stoi(matches[1]);
            std::regex_search(*it++, matches, ts_pat);
            wake = std::stoi(matches[1]);
            pg->add_sleep_record(sleep, wake);
        }
    }
    calc_sleep_totals(gsm);
    map_all_mins(gsm);
    set_all_sleepy_mins(gsm);
    return gsm;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

auto max_sleeper(const Guard_sleep_map& gsm)
{
    auto it = std::max_element(std::begin(gsm), std::end(gsm),
                               [](auto& ga, auto& gb) {
                                   return ga.second->total_sleep <
                                          gb.second->total_sleep;
                               });
    return *(it->second);
}

struct Sleeper {
    Sleeper(int i, int m, int s) : id{i}, minute{m}, sleep{s} { }
    int id = 0;
    int minute = 0;
    int sleep = 0;
};

auto find_most_reg_sleeper(const Guard_sleep_map& gsm)
{
    std::vector<Sleeper> vs;
    for (const auto& gp : gsm) {
        auto& p = gp.second;
        auto it = std::max_element(std::begin(p->mins), std::end(p->mins));
        auto minute = std::distance(std::begin(p->mins), it);
        vs.emplace_back(Sleeper{ p->id, static_cast<int>(minute), *it });
    }

    auto it = std::max_element(std::begin(vs), std::end(vs),
            [](auto& a, auto& b) {
                return a.sleep < b.sleep;
            });

    return it->id * it->minute;
}

int main()
{
    std::cout << "AoC 2018 Day 4 - Repose Record\n";

    auto input = utils::get_input_lines();
    std::sort(std::begin(input), std::end(input));

    auto gsm = map_sleep_data(input);

    auto worst_guard = max_sleeper(gsm);

    auto part1 = worst_guard.id * worst_guard.sleepiest_min;
    std::cout << "Part 1: " << part1 << '\n';

    auto part2 = find_most_reg_sleeper(gsm);
    std::cout << "Part 2: " << part2 << '\n';
}
