/*
 * doc_s.cpp
 *
 *  Created on: Aug 1, 2020
 *      Author: vishal gupta
 */

#include <iostream>
#include <string>
#include <vector>
#include <experimental/filesystem>

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

void drawRectAndCornerCircle();
void scanAndDetectVertices(string filename);
bool isInside(int circle_x, int circle_y,
                   int rad, int x, int y);
Point2f final_points[4];
float width = 0;
float hieght = 0;
float aspect_ratio =0;
Point2f dts[4];

enum index_l {
  top_left =0,
  top_right =1,
  bottom_right =2,
  bottom_left =3,
};

Mat input_image,img1_clone;
vector <bool> v_changed = {false, false, false, false};
bool prev_event = false;
vector<string> filename;
int list_it=0;

int main () {
  std::string path = TEST_DATA_PATH;
  for (const auto & entry : std::experimental::filesystem::directory_iterator(path)) {
    filename.push_back(entry.path());
  }

  cv::namedWindow("Doc_Scanner",WINDOW_NORMAL);
  cv::setMouseCallback("Doc_Scanner",[](int event, int x, int y, int flags, void* userdata) {
  if (event == cv::MouseEventTypes::EVENT_LBUTTONDOWN) {
    for (int i =0; i<4; i++) {
      bool result = isInside(final_points[i].x,final_points[i].y,10,x,y);
      if (result) {
        v_changed[i]= true;
        prev_event = true;
      }
    }
  } else if((event ==  EVENT_MOUSEMOVE && (prev_event == true))) {
    for (int i =0; i<4; i++) {
      if (v_changed[i]==true) {
        final_points[i] = Point(x,y);
      }
    }
    drawRectAndCornerCircle();
  } else if(event == cv::MouseEventTypes::EVENT_LBUTTONUP) {
    prev_event = false;
    for (int i =0; i<4; i++) {
      v_changed[i] = false;
    }
  } else if (event == cv::MouseEventTypes::EVENT_LBUTTONDBLCLK) {
    if (list_it<filename.size()) {
      scanAndDetectVertices(filename[list_it++]);
    }
  }
  }
  );

  int ch=-1;
  while (1) {
    if (ch == 'q') {
      break;
    } else if(ch == 'o') {
      vector<Point2f> pts_src {final_points[index_l::top_left], final_points[index_l::top_right],
      final_points[index_l::bottom_right], final_points[index_l::bottom_left]};

      vector<Point2f>  pts_dst {Point2f(0,0), Point2f(900,0),
      Point2f(900,1200),Point2f(0,1200)};

      Mat h = findHomography(pts_src,pts_dst);
      Mat im_out;
      warpPerspective(input_image, im_out, h,cv::Size(900,1200));
      cv::imwrite("output.jpg",im_out);
      imshow("output",im_out);
    } else if(ch=='r') {
      imshow("Doc_Scanner",img1_clone);
    }
    ch = cv::waitKey();
  }
  return 0;
}

void scanAndDetectVertices(string filename) {
  input_image = imread(filename);
  imshow("Doc_Scanner",input_image);
  img1_clone =input_image.clone();

  Mat pyr,timg,edges,gray0(img1_clone.size(), CV_8U);
  // down-scale and upscale the image to filter out the noise
  pyrDown(img1_clone, pyr, Size(img1_clone.cols/2, img1_clone.rows/2));
  pyrUp(pyr, timg, img1_clone.size());

  // find squares in every color plane of the image
  for ( int c = 0; c < 3; c++ ) {
    int ch[] = {c, 0};
    mixChannels(timg, gray0, ch, 1);
    
    Canny(gray0, edges, 0, 50,3);
    dilate(edges, edges, UMat(), Point(-1,-1));

    vector<vector<cv::Point> > contours;
    cv::findContours(edges, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    vector<double> countourArc;
    for (int i=0; i<contours.size();i++) {
      countourArc.push_back(cv::arcLength(contours[i],true));
    }
    auto itr = std::max_element(countourArc.begin(),countourArc.end());
    int coi = itr - countourArc.begin();

    vector<Vec2f> op;
    double epsilon = 0.02*arcLength(contours[coi], true);
    approxPolyDP(contours[coi],op,epsilon,true);

    if (((op.size()==4) && isContourConvex(op))) {
      final_points[index_l::top_left]     = op[1];
      final_points[index_l::top_right]    = op[0];
      final_points[index_l::bottom_left]  = op[2];
      final_points[index_l::bottom_right] = op[3];

      width = std::sqrt(std::pow((final_points[index_l::top_right].x -final_points[index_l::top_left].x),2) +
			std::pow((final_points[index_l::top_right].y -final_points[index_l::top_left].y),2));

      hieght = std::sqrt(std::pow((final_points[index_l::top_right].x -final_points[index_l::bottom_right].x),2) +
				std::pow((final_points[index_l::bottom_right].y -final_points[index_l::bottom_right].y),2));

      aspect_ratio = hieght/width;
      break;
    } else {
      final_points[index_l::top_left]     = Point2f(0,0);
      final_points[index_l::top_right]    = Point2f(img1_clone.size().width,0);
      final_points[index_l::bottom_left]  = Point2f(0,img1_clone.size().height);
      final_points[index_l::bottom_right] = Point2f(img1_clone.size().width,img1_clone.size().height);
    }
  }
  drawRectAndCornerCircle();
}

void drawRectAndCornerCircle() {
  input_image.copyTo(img1_clone);
  for (int i =0; i<4;i++) {
    cv::circle(img1_clone,Point{final_points[i]},10,Scalar(255,0,0),-1);
  }

  cv::line(img1_clone, final_points[index_l::top_left], final_points[index_l::top_right], cv::Scalar(255,0,0),3);
  cv::line(img1_clone, final_points[index_l::top_right], final_points[index_l::bottom_right], cv::Scalar(255,0,0),3);
  cv::line(img1_clone, final_points[index_l::bottom_right], final_points[index_l::bottom_left], cv::Scalar(255,0,0),3);
  cv::line(img1_clone, final_points[index_l::bottom_left], final_points[index_l::top_left], cv::Scalar(255,0,0),3);
  imshow("Doc_Scanner", img1_clone);
}

bool isInside(int circle_x, int circle_y,
              int rad, int x, int y) {
  if ((x - circle_x) * (x - circle_x) +
      (y - circle_y) * (y - circle_y) <= rad * rad)
    return true;
  else
    return false;
}
