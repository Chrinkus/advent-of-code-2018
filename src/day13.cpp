#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>

#include <get_input.hpp>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

class Off_tracks {};

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

enum class Turn { left, straight, right };

Turn operator++(Turn& t)
{
    t = (t == Turn::right) ? Turn::left : Turn(int(t) + 1);
    return t;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

enum class Orientation { up = '^', right = '>', down = 'v', left = '<',
                         crash = 'x' };

Orientation operator++(Orientation& o)
{
    switch (o) {
    case Orientation::up:
        o = Orientation::right;
        break;
    case Orientation::right:
        o = Orientation::down;
        break;
    case Orientation::down:
        o = Orientation::left;
        break;
    case Orientation::left:
        o = Orientation::up;
        break;
    case Orientation::crash:
        break;
    }
    return o;
}

Orientation operator--(Orientation& o)
{
    switch (o) {
    case Orientation::up:
        o = Orientation::left;
        break;
    case Orientation::right:
        o = Orientation::up;
        break;
    case Orientation::down:
        o = Orientation::right;
        break;
    case Orientation::left:
        o = Orientation::down;
        break;
    case Orientation::crash:
        break;
    }
    return o;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

class Cart {
public:
    Cart(int x, int y, Orientation o) : xx{x}, yy{y}, ff{o}
    {
        switch (ff) {
        case Orientation::up:
        case Orientation::down:
            current = '|';
            break;
        case Orientation::left:
        case Orientation::right:
            current = '-';
            break;
        default:
            // will never get here..
            break;
        }
    }

    int x() const { return xx; }
    int y() const { return yy; }
    Orientation facing() const { return ff; }

    char turn();
    void advance();
    void bend(char ahead);
    void crash() { ff = Orientation::crash; }
    void set_curr(char curr) { current = curr; }
    char get_curr() const { return current; }
private:
    int xx, yy;
    Orientation ff;
    char current = '\0';
    Turn next_turn = Turn::left;
};

char Cart::turn()
{
    char next_token = '\0';

    switch (next_turn) {
    case Turn::left:      next_token = char(--ff);    break;
    case Turn::straight:  next_token = char(ff);      break;
    case Turn::right:     next_token = char(++ff);    break;
    default:                                            break; }

    ++next_turn;
    return next_token;
}

void Cart::advance()
{
    switch (ff) {
    case Orientation::up:     --yy;    break;
    case Orientation::right:  ++xx;    break;
    case Orientation::down:   ++yy;    break;
    case Orientation::left:   --xx;    break;
    default:                            break; }
}

void Cart::bend(char ahead)
{
    switch (ff) {
    case Orientation::up:
        ahead == '/' ? ++ff : --ff;
        break;
    case Orientation::right:
        ahead == '/' ? --ff : ++ff;
        break;
    case Orientation::down:
        ahead == '/' ? ++ff : --ff;
        break;
    case Orientation::left:
        ahead == '/' ? --ff : ++ff;
        break;
    default:
        break;
    }
}

bool operator<(const Cart& a, const Cart& b)
{
    return a.y() <= b.y() && a.x() < b.x();
}

std::ostream& operator<<(std::ostream& os, const Cart& c)
{
    return os << char(c.facing()) << ' ' << c.x() << ',' << c.y();
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

auto map_carts(const std::vector<std::string>& trax)
{
    std::vector<Cart> carts;
    const std::string cart_symbols {"^v<>"};

    for (size_t i = 0; i < trax.size(); ++i)
        for (size_t j = 0; j < trax[i].size(); ++j)
            if (cart_symbols.find(trax[i][j]) != std::string::npos)
                carts.emplace_back(Cart{j, i, Orientation(trax[i][j])});

    for (const auto& cart : carts)
        std::cout << cart << '\n';

    return carts;
}

bool advance_tick(std::vector<std::string>& trax, std::vector<Cart>& carts)
{
    std::sort(std::begin(carts), std::end(carts));

    for (auto& cart : carts) {
        //std::cout << "Ticking.. " << cart << '\n';
        char look_ahead = '\0';
        switch (cart.facing()) {
        case Orientation::up:
            look_ahead = trax[cart.y()-1][cart.x()];
            break;
        case Orientation::right:
            look_ahead = trax[cart.y()][cart.x()+1];
            break;
        case Orientation::down:
            look_ahead = trax[cart.y()+1][cart.x()];
            break;
        case Orientation::left:
            look_ahead = trax[cart.y()][cart.x()-1];
            break;
        default:
            break;
        }

        switch (look_ahead) {
        case '-':
        case '|':
            trax[cart.y()][cart.x()] = cart.get_curr();
            cart.set_curr(look_ahead);
            cart.advance();
            trax[cart.y()][cart.x()] = char(cart.facing());
            break;
        case '/':
        case '\\':
            trax[cart.y()][cart.x()] = cart.get_curr();
            cart.set_curr(look_ahead);
            cart.advance();
            trax[cart.y()][cart.x()] = char(cart.facing());
            cart.bend(look_ahead);
            break;
        case '+':
            trax[cart.y()][cart.x()] = cart.get_curr();
            cart.set_curr(look_ahead);
            cart.advance();
            trax[cart.y()][cart.x()] = char(cart.facing());
            cart.turn();
            break;
        case '^':
        case 'v':
        case '<':
        case '>':
            trax[cart.y()][cart.x()] = cart.get_curr();
            cart.advance();
            trax[cart.y()][cart.x()] = 'X';
            cart.crash();
            return true;
        default:
            std::cerr << "Somehow got off the tracks with: "
                      << cart << '\n';
            throw Off_tracks{};
            break;
        }
    }

    return false;
}

std::string find_crash(const std::vector<Cart>& carts)
{
    auto it = std::find_if(std::begin(carts), std::end(carts),
            [](const auto& cart) {
                return cart.facing() == Orientation::crash;
            });
    std::stringstream ss;
    ss << it->x() << ',' << it->y();
    return ss.str();
}

int main(int argc, char* argv[])
{
    std::cout << "AoC 2018 Day 13 - Mine Cart Madness\n";

    auto trax = utils::get_input_lines(argc, argv, "13");
    auto cartz = map_carts(trax);
    while (!advance_tick(trax, cartz))
        ;
    auto part1 = find_crash(cartz);
    std::cout << "Part 1: " << part1 << '\n';
    /*
    for (const auto& t : trax)
        std::cout << t << '\n';
    */
}
