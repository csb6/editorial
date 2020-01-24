#include <iostream>
#define TRUE true

enum class {
    One, Two, Three
};

class A {
private:
  int c = 0;
public:
  A();
  ~A();
  A(const A &other) = default;
};


int main(int argc, char **argv)
{
  bool foo = false;
  bool not_foo = true;
  int a = 78;
  A* bar = new A();
  const char letter = 'o';
  if(a < 80) {
    std::cerr << "Error: a < 80\n";
    return 1;
  } else {
    for(int i = 0; i < 34; ++i) {
      std::cout << "for else if return bool int\n";
    }
  }

  switch(a) {
    case 'f': 
      break;
    default:
      std::cout << "Not f\n";
  }

  delete bar;
  return 0;
}