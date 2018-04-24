
#include <memory>
#include <iostream>
#include <string>
#include <dirent.h>
#include <utility>
#include <cstddef>
#include "imageio.h"

namespace{
  std::pair<std::string, std::string> splitFileName(const std::string& filename){
    std::size_t found = filename.find_last_of("//");
    return make_pair(filename.substr(0, found), filename.substr(found+1));
  }
}

int main(){
  DIR *dir;
  struct dirent *ent;
  std::string path = "/Users/liangchen/Documents/Projects/NoteMatcher/data/中国音乐学院考级";
  if ((dir = opendir (path.c_str())) != NULL) {
    /* print all the files and directories within directory */
    while ((ent = readdir (dir)) != NULL) {
      std::string filename(ent->d_name);
      if(filename.length() <= 4 || filename.substr(filename.length() - 4) != ".png") continue;
      ImageReader reader(path, filename);
      reader.findStaves();
      reader.findBars();
      reader.saveMeasures();
    }
    closedir (dir);
  }
  /*ImageReader reader("/Users/liangchen/Documents/Projects/NoteMatcher/data/中国音乐学院考级/中国音乐学院考级__9级__3.自选曲目.1.帕斯比利舞曲.德彪西曲.pets7.png");
  reader.findStaves();
  reader.findBars();
  reader.saveMeasures();
  reader.displayStaves();*/
}
