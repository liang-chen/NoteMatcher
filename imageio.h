
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <utility>
#include <cmath>
#include <fstream>
#include <opencv2/opencv.hpp>
#include "utilities.h"

class ImageReader{
 public:
 ImageReader(const std::string& p, const std::string& f): path(p), filename(f){
    std::string full_path = path + "/" + filename;
    image = cv::imread(full_path, CV_LOAD_IMAGE_GRAYSCALE);
    cv::threshold(image, image_bw, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
  }
  ~ImageReader(){}

  void findStaves();
  void findBars();
  void saveMeasures() const;
  void displayStaves() const;
  void display() const{
    cv::namedWindow("Display Image", cv::WINDOW_AUTOSIZE);
    cv::imshow("Display Image", image);
    cv::waitKey(0);
  }

 private:
  std::string path;
  std::string filename;
  cv::Mat image;
  cv::Mat image_bw;
  std::vector<Staff> staves;
  std::vector<Rect> measures;
};
