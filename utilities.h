
#include <vector>

class Bar{
 public:
 Bar(int x, int y, int l): topx(x), topy(y), length(l){}
  ~Bar(){}

  int getTopx() const{return topx;}
  int getTopy() const{return topy;}
  int getLength() const{return length;}

 private:
  int topx;
  int topy;
  int length;
};

class Staff{
 public:
 Staff(int y, int g): topy(y), gap(g){}
  ~Staff(){}

  std::vector<Bar> getBars() const{return bars;}
  int getTopy() const{return topy;}
  int getGap() const{return gap;}

 private:
  int topy;
  int gap;
  std::vector<Bar> bars;
};
