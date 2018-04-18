
#include <memory>
#include <iostream>
#include "imageio.h"

int main(){
  ImageReader reader("/Users/liangchen/Documents/Projects/NoteMatcher/data/中国音乐学院考级/中国音乐学院考级__10级__1.练习曲.3.侏儒之舞.李斯特曲.pets1.png");
  reader.findStaves();
}
