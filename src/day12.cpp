#include <iostream>
#include <vector>
#include <algorithm>
#include <regex>
#include <sstream>

#include <get_input.hpp>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

struct Growth_condition {
    explicit Growth_condition(const std::string& s);
    std::string pattern;
    bool result;
};

Growth_condition::Growth_condition(const std::string& s)
{
    std::istringstream iss {s};
    iss >> pattern;

    std::string throwaway;
    iss >> throwaway;

    char r;
    iss >> r;
    result = r == '#' ? true : false;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

class Plant_plan {
public:
    Plant_plan(std::string init, std::vector<Growth_condition> vcond)
        : state{std::move(init)}, conditions{std::move(vcond)} { }

    int sum_plant_pots() const;

    void advance_year(int n) { while (--n) advance_year(); }
private:
    void advance_year();

    int zero_offset = 0;
    std::string state;
    std::vector<Growth_condition> conditions;
};

void Plant_plan::advance_year()
{
    // pad state if needed
    auto it = std::find(std::begin(state), std::end(state), '#');
    if (std::distance(std::begin(state), it) < 5) {
        state.insert(0, 5, '.');
        zero_offset += 5;
    }

    auto rit = std::find(std::rbegin(state), std::rend(state), '#');
    if (std::distance(rit, std::rbegin(state)) < 5)
        state.append(5, '.');

    std::string next_year {state.size(), '.'};
    for (size_t i = 0; i < state.size() - 5; ++i) {

        auto cit = std::find_if(std::begin(conditions), std::end(conditions),
                [this, i](const auto& cnd) {
                    return cnd.pattern == this->state.substr(i, 5);
                });

        if (cit->result)
            next_year[i + 2] = '#';
    }
    state = next_year;
}

int Plant_plan::sum_plant_pots() const
{
    int total = 0;
    for (size_t i = 0; i < state.size(); ++i)
        if (state[i] == '#')
            total += i - zero_offset;
    return total;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

auto parse_input(const std::vector<std::string>& input)
{
    const std::regex state_pat {R"(^initial state: ([#.]+)$)"};
    const std::regex growth_pat {R"(^([#.]{5} => (#|\.)$))"};
    std::smatch match;

    auto it = std::begin(input);

    std::string state;
    if (std::regex_match(*it++, match, state_pat)) 
        state = match[1];
    else
        std::cout << "Could not read initial state\n";

    std::vector<Growth_condition> vcond;
    while (++it != std::end(input)) {
        if (std::regex_match(*it, match, growth_pat))
            vcond.emplace_back(Growth_condition{match[1].str()});
        else
            std::cout << "Could not read growth pattern\n";
    }

    return Plant_plan{state, vcond};
}

int main(int argc, char* argv[])
{
    std::cout << "AoC 2018 Day 12 - Subterranean Sustainability\n";

    auto input = utils::get_input_lines(argc, argv, "12");
    Plant_plan plan = parse_input(input);
    plan.advance_year(20);
    auto part1 = plan.sum_plant_pots();
    std::cout << "Part 1: " << part1 << '\n';
}

