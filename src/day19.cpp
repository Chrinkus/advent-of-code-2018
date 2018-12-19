#include <iostream>
#include <vector>
#include <algorithm>
#include <array>
#include <sstream>
#include <unordered_map>

#include <get_input.hpp>

using Registers = std::array<int,6>;
using Execution = std::function<void(const Instruction&,Registers&)>;

class Operations {
private:
    std::unordered_map<std::string,Execution> ops;
    std::unordered_map<std::string,std::vector<int>> op_tally;
    std::unordered_map<int,std::string> op_map;
public:
    Operations();

    int count_possible(const Sample_op& s) const;
    void map_ops(const std::vector<Sample_op>& vso);
    void print_op_tally() const;
    void execute(const Instruction& ins, Registers& r) const;
private:
    void reduce_op_tally(int key);
};

Operations::Operations()
{
    ops.emplace("addr", [](const Instruction& ins, Registers& r) {
                r[ins[2]]  = r[ins[0]] + r[ins[1]];
            });
    ops.emplace("addi", [](const Instruction& ins, Registers& r) {
                r[ins[2]]  = r[ins[0]] + ins[1];
            });
    ops.emplace("mulr", [](const Instruction& ins, Registers& r) {
                r[ins[2]]  = r[ins[0]] * r[ins[1]];
            });
    ops.emplace("muli", [](const Instruction& ins, Registers& r) {
                r[ins[2]]  = r[ins[0]] * ins[1];
            });
    ops.emplace("banr", [](const Instruction& ins, Registers& r) {
                r[ins[2]]  = r[ins[0]] & r[ins[1]];
            });
    ops.emplace("bani", [](const Instruction& ins, Registers& r) {
                r[ins[2]]  = r[ins[0]] & ins[1];
            });
    ops.emplace("borr", [](const Instruction& ins, Registers& r) {
                r[ins[2]]  = r[ins[0]] | r[ins[1]];
            });
    ops.emplace("bori", [](const Instruction& ins, Registers& r) {
                r[ins[2]]  = r[ins[0]] | ins[1];
            });
    ops.emplace("setr", [](const Instruction& ins, Registers& r) {
                r[ins[2]]  = r[ins[0]];
            });
    ops.emplace("seti", [](const Instruction& ins, Registers& r) {
                r[ins[2]]  = ins[0];
            });
    ops.emplace("gtir", [](const Instruction& ins, Registers& r) {
                r[ins[2]] = ins[0] > r[ins[1]] ? 1 : 0;
            });
    ops.emplace("gtri", [](const Instruction& ins, Registers& r) {
                r[ins[2]] = r[ins[0]] > ins[1] ? 1 : 0;
            });
    ops.emplace("gtrr", [](const Instruction& ins, Registers& r) {
                r[ins[2]] = r[ins[0]] > r[ins[1]] ? 1 : 0;
            });
    ops.emplace("eqir", [](const Instruction& ins, Registers& r) {
                r[ins[2]] = ins[0] == r[ins[1]] ? 1 : 0;
            });
    ops.emplace("eqri", [](const Instruction& ins, Registers& r) {
                r[ins[2]] = r[ins[0]] == ins[1] ? 1 : 0;
            });
    ops.emplace("eqrr", [](const Instruction& ins, Registers& r) {
                r[ins[2]] = r[ins[0]] == r[ins[1]] ? 1 : 0;
            });
}

int Operations::count_possible(const Sample_op& s) const
{
    auto t = std::count_if(std::begin(ops), std::end(ops),
            [&s](const auto& op) {
                auto after = op.second(s.ins, s.before);
                return after == s.after;
            });

    return t;
}

void Operations::map_ops(const std::vector<Sample_op>& vso)
{
    for (const auto& op : ops) {
        std::vector<int> codes;
        for (const auto& so : vso) {
            auto code = so.ins[0];
            auto it = std::find(std::begin(codes), std::end(codes), code);

            if (op.second(so.ins, so.before) == so.after) {
                if (it == std::end(codes))
                    codes.push_back(code);
            } else {
                if (it != std::end(codes))
                    codes.erase(it);
            }
        }
        op_tally[op.first] = std::move(codes);
    }

    auto key = std::find_if(std::begin(op_tally), std::end(op_tally),
            [](auto& op) { return op.second.size() == 1; })->second.front();

    reduce_op_tally(key);

    for (const auto& op : op_tally)
        op_map[op.second.front()] = op.first;
}

void Operations::reduce_op_tally(int key)
{
    for (auto& op : op_tally) {
        auto it = std::find(std::begin(op.second), std::end(op.second), key);
        if (op.second.size() > 1 && it != std::end(op.second)) {
            op.second.erase(it);
            if (op.second.size() == 1)
                reduce_op_tally(op.second.front());
        }
    }
}

void Operations::print_op_tally() const
{
    for (const auto& op : op_tally) {
        std::cout << op.first << '\t';
        for (const auto& i : op.second)
            std::cout << i << ' ';
        std::cout << '\n';
    }
}

void Operations::execute(const Instruction& ins, Registers& r) const
{
    ops.at(ins.name)(ins.fields, r);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
struct Instruction {
    std::string name;
    std::array<int,3> fields;
};

std::istream& operator>>(std::istream& is, Instruction& ins)
{
    is >> ins.name;
    for (auto& f : fields)
        is >> f;
    return is;
}

int run_program(const std::vector<std::string>& input)
{
    std::istringstream iss {input};

    std::string ip;
    int ipr;
    iss >> ip >> ipr;

    int iprv = 0;
    Registers regs;
    for (Instruction ins; iss >> ins; ) {
        ops.execute(ins, regs);
    }

    return registers[0];
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

int main(int argc, char* argv[])
{
    std::cout << "AoC 2018 Day 19 - Go With The Flow\n";

    auto input = utils::get_input_string(argc, argv, "19");
}
