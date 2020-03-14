#include <set>
#include <string_view>
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
            if(!contains('\0'))
                add_child('\0');
            return;
        }
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
        file << "return {true, i-1};\n";
        return;
    }

    file << "switch(str[i++]) {\n";
    for(const auto &child : tree.children) {
        if(child->letter != '\0') {
            file << "case '" << child->letter << "':\n";
            print_tree(*child, file);
            file << "break;\n";
        } else {
            file << "case '\\0':\n";
            file << "return {true, i};\n";
        }
    }
    file << "default:\n"
         << "return {false, 0};\n"
         << "}\n";
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

    std::ofstream file("cpp-matcher.cpp");
    file << "std::pair<bool,int> match(const char *str)\n"
         << "{\n"
         << "int i = 0;\n";
    print_tree(t, file);
    file << "}\n";

    return 0;
}
