#include <iostream>
#include <fstream>
#include <vector>
#include <numeric>
#include <set>

std::vector<int> get_input(const std::string& fname)
{
    std::vector<int> vi;

    std::ifstream ifs {fname};

    int val;
    while (ifs >> val)
        vi.push_back(val);

    return vi;
}

int find_duplicate(const std::vector<int>& vi)
{
    std::set<int> si;

    int freq = 0;
    si.insert(freq);

    while (true)
        for (const auto val : vi) {
            freq += val;
            auto it = si.find(freq);
            if (it != si.end())
                return *it;
            si.insert(freq);
        }
}

int main()
{
    auto vi = get_input("./input01.txt");

    auto part1 = std::accumulate(std::begin(vi), std::end(vi), 0);

    std::cout << "Part 1: " << part1 << '\n';

    auto part2 = find_duplicate(vi);

    std::cout << "Part 2: " << part2 << '\n';

}
