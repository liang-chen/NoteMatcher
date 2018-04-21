
#include <memory>
#include <iostream>
#include "imageio.h"

int main(){
  ImageReader reader("/Users/liangchen/Documents/Projects/NoteMatcher/data/中国音乐学院考级/中国音乐学院考级__9级__3.自选曲目.1.帕斯比利舞曲.德彪西曲.pets7.png");
  reader.findStaves();
  reader.displayStaves();
}
