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
    rgbImage.at<cv::Vec3b>(y, x) = cv::Vec3b(b, g, r);
  }

  void drawHorizLine(cv::Mat& rgbImage, int y){
    for(int x = 0; x < rgbImage.cols; x++){
      setRGB(rgbImage, y, x, 255, 0, 0);
    }
  }

  void drawStaff(cv::Mat& rgbImage, const Staff& staff){
    int y = staff.getTopy();
    int g = staff.getGap();
    for(int i = 0; i < 5; i++){
      drawHorizLine(rgbImage, y+i*g);
    }
  }

  void drawVertLine(cv::Mat& rgbImage, int x, int y1, int y2){
    for(int y = y1; y < y2; y++){
      setRGB(rgbImage, y, x, 255, 0, 0);
    }
  }

  void drawBar(cv::Mat& rgbImage, const Bar& g){
    int x = g.getTopx();
    int y = g.getTopy();
    int l = g.getLength();
    drawVertLine(rgbImage, x, y, y+l);
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

  //assumes the optimal gap is always 9 pixels for now.
  int minGap = 9;//7;
  int maxGap = 9;//11;

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

void ImageReader::findBars(){
  for(int i = 0; i < staves.size()-1; i+= 2){
    int top_y = staves[i].getTopy();
    int g = staves[i+1].getGap();
    int bot_y = staves[i+1].getTopy() + 4*g;

    int prev_bar_x = -1;

    for(int x = 0; x < image_bw.cols; x++){
      float curr = 0;
      for(int y = top_y; y < bot_y; y++){
        curr += 1 - image_bw.at<uchar>(y, x)/255.0;
      }
      if(curr > 0.9 * (bot_y - top_y)){
        Bar top_b(x, top_y, 4*g);
        staves[i].addBar(top_b);
        Bar bot_b(x, bot_y-4*g, 4*g);
        staves[i+1].addBar(bot_b);

        if(prev_bar_x > 0){
          measures.emplace_back(prev_bar_x, top_y, x-prev_bar_x, bot_y-top_y);
        }
        prev_bar_x = x;
      }
    }
  }
}

void ImageReader::saveMeasures() const{
  std::ofstream file;
  file.open(path+"/measures/"+filename+".txt");
  for(auto measure: measures){
    file << measure.getTopx() << " " << measure.getTopy() << " " << measure.getWidth()
         << " " << measure.getHeight() << std::endl;
  }
  file.close();
}

void ImageReader::displayStaves() const{
  cv::Mat imageDisplay;
  cv::cvtColor(image, imageDisplay, cv::COLOR_GRAY2BGR);
  for(const auto& staff: staves){
    //std::cout << staff.getTopy() << staff.getGap() << std::endl;
    drawStaff(imageDisplay, staff);
    for(const auto& bar: staff.getBars()){
      drawBar(imageDisplay, bar);
    }
  }

  cv::namedWindow("Display Image", cv::WINDOW_AUTOSIZE);
  cv::imshow("Display Image", imageDisplay);
  cv::waitKey(0);
}
