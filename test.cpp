#include <iostream>
#ifdef __APPLE__
#define TRUE true
#ifndef __APPLE__
#define TRUE false
#endif

enum class Numbers {
    One, Two, Three
};

/**
 * This is a multi-line comment.
 * It can span multiple lines.
 */

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
  bool not_foo = !foo;
  auto a = 78;
  try {
    A* bar = new A();
  } catch(const std::exception &e) {
    do {
      std::cout << "Exception occurred\n" << std::endl;
    } while(false);
  }
  const char letter = 'o';
  if(a < 80) {
    int w = 5 - 6;
    w /= 1;
    w *= 56;
    std::cerr << "Error: a < 80\n";
    return 1;
  } else {
    for(int i = 0; i < 34; ++i) {
      std::cout << "for else if return bool int\n";
      continue;
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