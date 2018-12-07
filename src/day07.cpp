#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <regex>

#include <get_input.hpp>

struct Step {
    char do_this = '\0';
    char before = '\0';
};

std::ostream& operator<<(std::ostream& os, const Step& s)
{
    return os << "Do " << s.do_this << " before " << s.before;
}

auto parse_steps(const std::vector<std::string>& input)
{
    std::vector<Step> vsteps (input.size());

    std::transform(std::begin(input), std::end(input), std::begin(vsteps),
            [](auto line) {
                static const std::regex pat_1 {R"(^Step\s(\w))"};
                static const std::regex pat_2 {R"((\w) can begin\.\s?$)"};
                Step step;
                std::smatch m_1;
                std::regex_search(line, m_1, pat_1);
                step.do_this = m_1[1].str()[0];
                std::smatch m_2;
                std::regex_search(line, m_2, pat_2);
                step.before = m_2[1].str()[0];

                return step;
            });

    return vsteps;
}

auto get_step_order(const std::vector<Step>& steps)
{
    std::vector<char> done;
    std::vector<char> ready;
    std::vector<char> pool;
    for (char c = 'A'; c <= 'Z'; ++c)
        pool.push_back(c);

    while (pool.size()) {
        ready.resize(pool.size());
        std::copy(std::begin(pool), std::end(pool), std::begin(ready));
        pool.clear();

        for (const auto& step : steps) {
            auto dit = std::find(std::begin(done), std::end(done),
                                 step.do_this);     // check if do_this is done
            if (dit == std::end(done)) {            // not done
                auto rit = std::find(std::begin(ready), std::end(ready),
                                     step.before);
                if (rit != std::end(ready)) {
                    ready.erase(rit);
                    pool.push_back(step.before);
                }
            }
        }
        std::sort(std::rbegin(ready), std::rend(ready));
        done.push_back(ready.back());
        ready.pop_back();
        for (const auto c : ready)
            pool.push_back(c);
        ready.clear();
    }
    return std::string {std::begin(done), std::end(done)};
    /*
    std::string done;
    std::string ready;
    std::string pool {"ABCDEF"};//GHIJKLMNOPQRSTUVWXYZ"};

    while (pool.size()) {
        pool.copy(ready);
        pool.clear();

        for (const auto& step : steps) {
            auto dpos = done.find(step.do_this);
            if (dpos == std::string::npos) {
                auto rpos = ready.find(step.before);
                if (rpos != std::string::npos) {
                    ready.erase(rpos);
                    pool + step.before;
                }
            }
        }
        std::sort(std::begin(ready), std::end(ready));
        done.append(ready);
        std::cout << "Done: " << done << '\n';
        std::cout << "Ready: " << ready << '\n';
        std::cout << "Pool: " << pool << '\n';
    }
    return done;
    */
}

int main()
{
    std::cout << "AoC 2018 Day 7 - The Sum of Its Parts\n";

    auto input = utils::get_input_lines();
    auto steps = parse_steps(input);

    auto part1 = get_step_order(steps);
    std::cout << "Part 1: " << part1 << '\n';
}
