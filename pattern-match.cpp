#include <iostream>
#include <fstream>
#include <array>
#include <map>
#include <string>
#include <string_view>

struct Tree {
    std::map<char, Tree*> children;
    void add(std::string_view text)
    {
        if(text.empty())
            return;
        else if(!isSet) {
            value = text[0];
            text = text.substr(1);
            isSet = true;
        }
        if(text.empty())
            return;

         if(children.count(text[0]) != 0) {
            return children[text[0]]->add(text.substr(1));
         } else {
             auto *newChild = new Tree();
             newChild->value = text[0];
             newChild->isSet = true;
             children[text[0]] = 
             return false;
         }
    }

    bool match(std::string_view text) {
        if(text.empty() && isSet)
            return false;
        else if(text.empty())
            return true;

        if(children.count(text[0]) != 0) {
            return children[text[0]]->match(text.substr(1));
        } else
            return false;
        
    }
};

int main()
{
    const std::array<std::string, 2> targets{"#include", "auto"};
    std::ofstream output("match.cpp");
    output << "#include <string_view>\n\n"
           << "bool match(std::string_view text)\n"
           << "{\n";

    Tree tree;

    //tree.add("#include");
    //tree.add("#ifdef");
    
    
    return 0;
}
