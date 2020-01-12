#include <iostream>

int main(int argc)
{
  bool foo = false;
  int a = 78;
  if(a < 80) {
    std::cerr << "Error: a < 80\n";
    return 1;
  } else {
    for(int i = 0; i < 34; ++i) {
      std::cout << "no way\n";
    }
  }

  return 0;
}