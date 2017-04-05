#ifndef MOTIONTOCOLOR_H
#define MOTIONTOCOLOR_H

#include <iostream>
#include "opencv2/opencv.hpp"
using namespace cv;
using namespace std;

void makecolorwheel(vector<Scalar> &colorwheel);
void motionToColor(Mat flow, Mat &color);
void motionToGray(Mat flow, Mat &color);

#endif // MOTIONTOCOLOR_H
