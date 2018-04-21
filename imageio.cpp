#include "imageio.h"

namespace{
  std::vector<float> sortedArrayCluster(const std::vector<int>& sorted){
    float left = *sorted.begin();
    float right = *sorted.end();
    int iterMax = 10;

    for(int i = 0; i < iterMax; i++){
      int numLeft = 0;
      int numRight = 0;
      float sumLeft = 0;
      float sumRight = 0;
      for(const auto& el: sorted){
        if(fabs(left - el) < fabs(right - el)){
          numLeft++;
          sumLeft+=el;
        }
        else{
          numRight++;
          sumRight+=el;
        }
      }

      left = numLeft == 0 ? 0 : sumLeft/numLeft;
      right = numRight == 0 ? 0 : sumRight/numRight;
      //std::cout << "mean" << (sumLeft+sumRight)/(numLeft+numRight) << std::endl;
    }

    std::vector<float> results;
    results.push_back(left);
    results.push_back(right);

    return results;
  }

  float evaluateStaffScore(const std::vector<float>& hist, int y, int g, float bthresh, float wthresh){
    if(y+4*g >= hist.size()) return -1;
    float score = 0;
    for(int i = 0; i < 5; i++){
      score += hist[y+i*g] - bthresh;
    }
    for(int i = 0; i < 4; i++){
      score += wthresh - hist[y+i*g+g/2];
    }
    return score;
  }

  void setRGB(cv::Mat& rgbImage, int y, int x, float r, float g, float b){
    rgbImage.at<Vec3b>(y, x) = Vec3b(b, g, r);
  }

  void drawLine(cv::Mat& rgbImage, int y){
    for(int x = 0; x < rgbImage.cols; x++){
      setRGB(rgbImage, y, x, 255, 0, 0);
    }
  }

  void drawStaff(cv::Mat& rgbImage, int y, int g){
    for(int i = 0; i < 5; i++){
      drawLine(rgbImage, y+i*g);
    }
  }
}

void ImageReader::findStaves(){
  //std::cout << image_bw.rows << " " << image_bw.cols << std::endl;
  std::vector<float> hist(image_bw.rows, 0);
  for(int i = 0; i < image_bw.rows; i++){
    float curr = 0;
    for(int j = 0; j < image_bw.cols; j++){
      curr += 1 - image_bw.at<uchar>(i, j)/255.0;
    }
    hist[i] = curr;
  }

  //sort(hist.begin(), hist.end());
  float bthresh = 0.8*image_bw.cols;
  float wthresh = 0.5*image_bw.cols;

  int minGap = 9;//6;
  int maxGap = 9;//10;

  //0: no staff; 1: having staff
  std::vector<std::vector<float>> scores(hist.size(), std::vector<float>(2, -1));
  std::vector<std::vector<std::pair<int, int>>> pred(hist.size(), std::vector<std::pair<int,int>>(2, std::make_pair(-1,-1)));

  scores[0][0] = 0;
  int opt_g = minGap;

  for(int g = minGap; g <= maxGap; g++){
    for(int i = 0; i < hist.size(); i++){
      scores[i][1] += evaluateStaffScore(hist, i, g, bthresh, wthresh);
      if(i + 5*g < hist.size() && scores[i+5*g][0] < scores[i][1]){
        scores[i+5*g][0] = scores[i][1];
        pred[i+5*g][0] = std::make_pair(1, i);
      }

      if(i+1 < hist.size() && scores[i+1][0] < scores[i][0]){
        scores[i+1][0] = scores[i][0];
        pred[i+1][0] = std::make_pair(0, i);
      }

      if(i+1 < hist.size() && scores[i+1][1] < scores[i][0]){
        scores[i+1][1] = scores[i][0];
        pred[i+1][1] = std::make_pair(0, i);
      }
    }
    opt_g = g;
  }

  int curr_y = hist.size()-1;
  int staff_id = 0;
  while(curr_y != -1){
    if(staff_id == 1){
      Staff stf(curr_y, opt_g);
      staves.push_back(stf);
    }
    auto prev_state = pred[curr_y][staff_id];
    curr_y = prev_state.second;
    staff_id = prev_state.first;
  }

  std::reverse(staves.begin(), staves.end());
}

void ImageReader::displayStaves(){
  cv::Mat imageDisplay;
  cv::cvtColor(image, imageDisplay, cv::COLOR_GRAY2BGR);
  for(auto staff: staves){
    //std::cout << staff.getTopy() << staff.getGap() << std::endl;
    drawStaff(imageDisplay, staff.getTopy(), staff.getGap());
  }

  cv::namedWindow("Display Image", cv::WINDOW_AUTOSIZE);
  cv::imshow("Display Image", imageDisplay);
  cv::waitKey(0);
}
