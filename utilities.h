
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
  void setTopy(int y) {topy = y;}
  void setGap(int g) {gap = g;}
  void addBar(const Bar& b) {bars.push_back(b);}

 private:
  int topy;
  int gap;
  std::vector<Bar> bars;
};

class Rect{
 public:
 Rect(int x, int y, int w, int h): topx(x), topy(y), width(w), height(h){}
  ~Rect(){}

  int getTopx() const{return topx;}
  int getTopy() const{return topy;}
  int getWidth() const{return width;}
  int getHeight() const{return height;}

 private:
  int topx;
  int topy;
  int width;
  int height;
};
