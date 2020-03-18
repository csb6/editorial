#ifndef PATTERN_MATCH_CODE_GEN_H
#define PATTERN_MATCH_CODE_GEN_H
#include <set>
#include <memory>
#include <string_view>
#include <string>

class Tree {
private:
    void add_child(char letter, std::string_view color);
    bool contains(char letter) const;
public:
    const char letter;
    const std::string color;
    std::set<std::unique_ptr<Tree>> children;

    explicit Tree(char l, std::string_view c) : letter(l), color(c) {}

    Tree* get_child(char letter);
    std::size_t size() const { return children.size(); }
    bool empty() const { return size() == 0; }
    void add_match(std::string_view match, std::string_view color);
};

void write_header(const std::string &path, const char *declaration);
void write_source(const std::string &path, const char *declaration,
                  const Tree &tree);
#endif
