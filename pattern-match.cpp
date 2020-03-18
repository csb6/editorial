#include <set>
#include <string_view>
#include <string>
#include <memory>
#include <fstream>
#include "syntax-highlight.h"

class Tree {
public:
    const char letter;
    const std::string color;
    std::set<std::unique_ptr<Tree>> children;
    explicit Tree(char l, std::string_view c) : letter(l), color(c) {}
    void add_child(char letter, std::string_view color)
    {
        children.insert(std::make_unique<Tree>(letter, color));
    }

    bool contains(char letter) const
    {
        for(const auto &child : children) {
            if(child->letter == letter)
                return true;
        }
        return false;
    }

    Tree* get_child(char letter)
    {
        for(auto &child : children) {
            if(child->letter == letter)
                return child.get();
        }
        return nullptr;
    }

    std::size_t size() const { return children.size(); }

    bool empty() const { return size() == 0; }

    void add_match(std::string_view match, std::string_view color)
    {
        if(match.empty()) {
            // At end of match being added
            return;
        }
        assert(!std::any_of(match.begin(), match.end(),
                            [](auto e) { return std::isspace(e); })
               && "Error: match can't contain a space character");

        char letter = match[0];
        if(!contains(letter))
            add_child(letter, color);

        get_child(letter)->add_match(match.substr(1), color);
    }
};


static void print_tree(const Tree &tree, std::ofstream &file)
{
    if(tree.empty()) {
        // If user's string ends at this node, they have a match
        file << "return {true, curr_color, curr_col - col};\n";
        return;
    }

    file << "if(col >= window_width) return {false, Color::Default, 0};\n"
         << "switch(window.get(curr_col++, row)) {\n";
    for(const auto &child : tree.children) {
        file << "case '" << child->letter << "':\n"
             << "curr_color = " << child->color << ";\n";
        print_tree(*child, file);
        file << "break;\n";
    }
    file << "default:\n"
         << "return {false, Color::Default, 0};\n"
         << "}\n";
}

void write_header(const std::string &path, const char *declaration)
{
    std::ofstream header_file(path + ".h");
    header_file << "#ifndef CPP_MATCHER_H\n"
                << "#define CPP_MATCHER_H\n"
                << "#include <tuple>\n"
                << "#include \"screen.h\"\n\n"
                << "class Screen;\n"
                << declaration << ";\n"
                << "#endif" << std::endl;
}

void write_source(const std::string &path, const char *declaration,
                  const Tree &tree)
{
    std::ofstream src_file(path + ".cpp");
    src_file << "#include \"" << path << ".h\"\n"
             << "#include \"syntax-highlight.h\"\n\n"
             << declaration << '\n'
             << "{\n"
             << "int curr_col = col;\n"
             << "Color curr_color;\n"
             << "const int window_width = window.width();\n";
    print_tree(tree, src_file);
    src_file << "}" << std::endl;
}


int main()
{
    Tree t('\0', "");
    t.add_match("#include", "PreprocessorColor");
    t.add_match("#define", "PreprocessorColor");
    t.add_match("#ifndef", "PreprocessorColor");
    t.add_match("#ifdef", "PreprocessorColor");
    t.add_match("#endif", "PreprocessorColor");
    t.add_match("#include", "PreprocessorColor");
    t.add_match("#define", "PreprocessorColor");
    t.add_match("#ifndef", "PreprocessorColor");
    t.add_match("#ifdef", "PreprocessorColor");
    t.add_match("#endif", "PreprocessorColor");
    t.add_match("+", "KeywordColor");
    t.add_match("-", "KeywordColor");
    t.add_match("*", "KeywordColor");
    t.add_match("/", "KeywordColor");
    t.add_match("=", "KeywordColor");
    t.add_match("!", "KeywordColor");
    t.add_match("<", "KeywordColor");
    t.add_match(">", "KeywordColor");
    t.add_match("&", "KeywordColor");
    t.add_match("|", "KeywordColor");
    t.add_match("^", "KeywordColor");
    t.add_match("~", "KeywordColor");
    t.add_match("auto", "KeywordColor");
    t.add_match("bool", "TypeColor");
    t.add_match("break", "KeywordColor");
    t.add_match("char", "TypeColor");
    t.add_match("case", "KeywordColor");
    t.add_match("constexpr", "KeywordColor");
    t.add_match("const", "KeywordColor");
    t.add_match("continue", "KeywordColor");
    t.add_match("class", "KeywordColor");
    t.add_match("catch", "KeywordColor");
    t.add_match("default", "KeywordColor");
    t.add_match("delete", "KeywordColor");
    t.add_match("do", "KeywordColor");
    t.add_match("else", "KeywordColor");
    t.add_match("enum", "KeywordColor");
    t.add_match("for", "KeywordColor");
    t.add_match("false", "KeywordColor");
    t.add_match("int", "TypeColor");
    t.add_match("if", "KeywordColor");
    t.add_match("new", "KeywordColor");
    t.add_match("public:", "KeywordColor");
    t.add_match("private:", "KeywordColor");
    t.add_match("return", "KeywordColor");
    t.add_match("switch", "KeywordColor");
    t.add_match("true", "KeywordColor");
    t.add_match("try", "KeywordColor");
    t.add_match("unsigned", "TypeColor");
    t.add_match("while", "KeywordColor");

    const char *declaration = "std::tuple<bool,Color,std::size_t> match(Screen &window, int col, int row)";
    const std::string path("cpp-matcher");

    write_header(path, declaration);
    write_source(path, declaration, t);

    return 0;
}
