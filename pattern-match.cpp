#include <set>
#include <string_view>
#include <string>
#include <memory>
#include <fstream>

class Tree {
public:
    const char letter;
    std::set<std::unique_ptr<Tree>> children;
    explicit Tree(char l) : letter(l) {}
    void add_child(char letter)
    {
        children.insert(std::make_unique<Tree>(letter));
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

    void add_match(std::string_view match)
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
            add_child(letter);

        get_child(letter)->add_match(match.substr(1));
    }
};


static void print_tree(const Tree &tree, std::ofstream &file)
{
    if(tree.empty()) {
        // If user's string ends at this node, they have a match
        file << "return {true, curr_col - col};\n";
        return;
    }

    file << "switch(window.get(curr_col++, row)) {\n";
    for(const auto &child : tree.children) {
        file << "case '" << child->letter << "':\n";
        print_tree(*child, file);
        file << "break;\n";
    }
    file << "default:\n"
         << "return {false, 0};\n"
         << "}\n";
}

void write_header(const std::string &path, const char *declaration)
{
    std::ofstream header_file(path + ".h");
    header_file << "#ifndef CPP_MATCHER_H\n"
                << "#define CPP_MATCHER_H\n"
                << "#include <utility>\n\n"
                << "class Screen;\n"
                << declaration << ";\n"
                << "#endif\n";
}

void write_source(const std::string &path, const char *declaration,
                  const Tree &tree)
{
    std::ofstream src_file(path + ".cpp");
    src_file << "#include \"" << path << ".h\"\n"
             << "#include \"screen.h\"\n\n"
             << declaration << '\n'
             << "{\n"
             << "std::size_t curr_col = col;\n";
    print_tree(tree, src_file);
    src_file << "}" << std::endl;
}


int main()
{
    Tree t('\0');
    t.add_match("#include");
    t.add_match("#define");
    t.add_match("#ifndef");
    t.add_match("#ifdef");
    t.add_match("#endif");
    t.add_match("#include");
    t.add_match("#define");
    t.add_match("#ifndef");
    t.add_match("#ifdef");
    t.add_match("#endif");
    t.add_match("+");
    t.add_match("-");
    t.add_match("*");
    t.add_match("/");
    t.add_match("=");
    t.add_match("!");
    t.add_match("<");
    t.add_match(">");
    t.add_match("&");
    t.add_match("|");
    t.add_match("^");
    t.add_match("~");
    t.add_match("auto");
    t.add_match("bool");
    t.add_match("break");
    t.add_match("char");
    t.add_match("case");
    t.add_match("constexpr");
    t.add_match("const");
    t.add_match("continue");
    t.add_match("class");
    t.add_match("catch");
    t.add_match("default");
    t.add_match("delete");
    t.add_match("do");
    t.add_match("else");
    t.add_match("enum");
    t.add_match("for");
    t.add_match("false");
    t.add_match("int");
    t.add_match("if");
    t.add_match("new");
    t.add_match("public:");
    t.add_match("private:");
    t.add_match("return");
    t.add_match("switch");
    t.add_match("true");
    t.add_match("try");
    t.add_match("unsigned");
    t.add_match("while");

    const char *declaration = "std::pair<bool,std::size_t> match(Screen &window, int col, int row)";
    const std::string path("cpp-matcher");

    write_header(path, declaration);
    write_source(path, declaration, t);

    return 0;
}
