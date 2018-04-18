
#include <memory>
#include <string>
#include <vector>
#include "utilities.h"
#include <opencv2/opencv.hpp>

using namespace cv;
class ImageReader{
 public:
 ImageReader(const std::string& str): path(str){
    image = imread(path, CV_LOAD_IMAGE_GRAYSCALE);
    threshold(image, image_bw, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
  }
  ~ImageReader(){}

  void findStaves();
  void display(){
    namedWindow("Display Image", WINDOW_AUTOSIZE);
    imshow("Display Image", image);
    waitKey(0);
  }

 private:
  std::string path;
  Mat image;
  Mat image_bw;
  std::vector<Staff> staves;
};
