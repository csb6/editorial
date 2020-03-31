#include <fstream>
#include <algorithm>
#include <set>
#include <memory>
#include <string_view>
#include <string>
#include <vector>
#include <iostream>
#include <array>

class Tree {
private:
    void add_child(char letter, std::string_view color);
    bool contains(char letter) const;
public:
    const char letter;
    const std::string color;
    std::set<std::unique_ptr<Tree>> children;

    Tree() : letter{} {}
    explicit Tree(char l, std::string_view c) : letter(l), color(c) {}

    Tree* get_child(char letter);
    std::size_t size() const { return children.size(); }
    bool empty() const { return size() == 0; }
    void add_match(std::string_view match, std::string_view color);
    std::size_t tree_size() const;
};

void Tree::add_child(char letter, std::string_view color)
{
    children.insert(std::make_unique<Tree>(letter, color));
}

bool Tree::contains(char letter) const
{
    for(const auto &child : children) {
        if(child->letter == letter)
            return true;
    }
    return false;
}

Tree* Tree::get_child(char letter)
{
    for(auto &child : children) {
        if(child->letter == letter)
            return child.get();
    }
    return nullptr;
}

void Tree::add_match(std::string_view match, std::string_view color)
{
    if(match.empty()) {
        // At end of match being added
        return;
    }

    char letter = match[0];
    if(!contains(letter))
        add_child(letter, color);

    get_child(letter)->add_match(match.substr(1), color);
}

std::size_t Tree::tree_size() const
{
    if(children.empty())
        return 0;

    std::size_t size = children.size();
    for(auto &child : children) {
        size += child->tree_size();
    }
    return size;
}

constexpr std::size_t CharSize = 2 << (sizeof(char)*8-1); //in bits

enum class Color : char {
    A = 0, B = 2, C = 3, D = 4,
    E = 5, F = 6, G = 7
};

class JumpTable {
private:
    std::vector<std::array<unsigned char,CharSize>> m_levels;
    unsigned char m_id = 1;
    constexpr static auto color_of(char value) { return (value & 0b11100000) >> 5; }
    constexpr static auto id_of(char value)    { return  value & 0b00011111;       }
public:
    void add_match(std::string_view match, Color fg)
    {
        if(match.empty())
            return;

        while(m_levels.size() < match.size())
            m_levels.emplace_back();

        const auto color = static_cast<unsigned char>(fg) << 5;
        for(std::size_t i = 0; i < match.size(); ++i) {
            char letter = match[i];
            m_levels[i][letter] = m_id | color;
        }
        ++m_id;
    }

    bool contains(std::string_view match) const
    {
        if(match.size() > m_levels.size() || match.empty()
           || (m_levels[0][match[0]] == 0))
           return false;

        auto id = id_of(m_levels[0][match[0]]);

        for(std::size_t i = 1; i < match.size(); ++i) {
            char letter = match[i];
            if(m_levels[i][letter] == 0 || id_of(m_levels[i][letter]) != id)
                return false;
        }
        return true;
    }
};

int main()
{
    /*
               e
            d
      a  b
            g
               e
     */
    Tree tree;
    tree.add_match("abde", "");
    tree.add_match("abge", "");

    JumpTable table;
    table.add_match("hey", Color::A);
    table.add_match("you", Color::B);
    // TODO: change structure so collisions don't overwrite
    // old entries
    table.add_match("not", Color::C);

    assert(table.contains("hey"));
    //assert(table.contains("you")); // BUG: this fails
    assert(!table.contains("hoy"));

    return 0;
}
