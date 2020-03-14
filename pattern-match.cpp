#include <set>
#include <string_view>
#include <memory>
#include <iostream>

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

    Tree* get(char letter)
    {
        for(auto &child : children) {
            if(child->letter == letter)
                return child.get();
        }
        return nullptr;
    }

    std::size_t size() const { return children.size(); }
    bool empty() const { return size() == 0; }
};


void add_word(Tree *tree, std::string_view word)
{
    if(word.empty() || tree == nullptr)
        return;
    char letter = word[0];
    if(!tree->contains(letter))
        tree->add_child(letter);

    add_word(tree->get(letter), word.substr(1));
}

void print_tree(const Tree &tree)
{
    if(tree.empty()) {
        std::cout << "return true;\n";
        return;
    }

    std::cout << "switch(a) {\n";
    for(const auto &child : tree.children) {
        std::cout << "case '" << child->letter << "':\n";
        print_tree(*child);
        std::cout << "break;\n";
    }
    std::cout << "}\n";
}


int main()
{   
    Tree t('\0');
    add_word(&t, "hello");
    add_word(&t, "hi");
    print_tree(t);
    
    return 0;
}
