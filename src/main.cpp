#include "color.hpp"
#include <iostream>

const int IMAGE_WIDTH = 256;
const int IMAGE_HEIGHT = 256;

int main() {
  std::cout << "P3\n" << IMAGE_WIDTH << " " << IMAGE_HEIGHT << "\n255\n";
  for (int i = 0; i < IMAGE_HEIGHT; i++) {
    std::clog << "\rScanlines Remaining: " << IMAGE_HEIGHT - i << std::flush;
    for (int j = 0; j < IMAGE_WIDTH; j++) {
      write_color(std::cout, color3(double(i) / (IMAGE_HEIGHT - 1),
                                       double(j) / (IMAGE_WIDTH - 1), 0.0f));
    }
  }
  std::clog << "\rDone                      \n";
}
