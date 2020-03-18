#include "pattern-match.h"
#include <string>

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
    t.add_match("constexpr ", "KeywordColor");
    t.add_match("const ", "KeywordColor");
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

    const char *declaration = "std::tuple<bool,Color,std::size_t> match_cpp(Screen &window, int col, int row)";
    const std::string path("cpp_matcher");

    write_header(path, declaration);
    write_source(path, declaration, t);

    return 0;
}
