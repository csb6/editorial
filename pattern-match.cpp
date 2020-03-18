#include <fstream>
#include "syntax-highlight.h"
#include "pattern-match.h"
#include <algorithm>

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


// Used to construct the declaration of matching functions (e.g. match_cpp())
constexpr const char *ReturnType = "std::tuple<bool,Color,std::size_t> ";
constexpr const char *Args = "(Screen &window, int col, int row)";

void write_header(const std::string &path, const char *func_name)
{
    std::ofstream header_file(path + ".h");
    std::string header_title(path);
    std::transform(header_title.begin(), header_title.end(),
                   header_title.begin(), ::toupper);
    header_file << "#ifndef " << header_title << "_H\n"
                << "#define " << header_title << "_H\n"
                << "#include <tuple>\n"
                << "#include \"screen.h\"\n\n"
                << ReturnType << func_name << Args << ";\n"
                << "#endif" << std::endl;
}

void write_source(const std::string &path, const char *func_name,
                  const Tree &tree)
{
    std::ofstream src_file(path + ".cpp");
    src_file << "#include \"" << path << ".h\"\n"
             << "#include \"syntax-highlight.h\"\n\n"
             << ReturnType << func_name << Args << '\n'
             << "{\n"
             << "int curr_col = col;\n"
             << "Color curr_color;\n"
             << "const int window_width = window.width();\n";
    print_tree(tree, src_file);
    src_file << "}" << std::endl;
}
