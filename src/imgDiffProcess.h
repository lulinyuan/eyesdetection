#ifndef IMGDIFFPROCESS_H
#define IMGDIFFPROCESS_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <opencv/cv.hpp>
#include <opencv/cxcore.hpp>

#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/video/tracking.hpp"
#include <vector>
using namespace cv;

int imgDiffProcess(Mat& preFrame,
                   Mat& frame,
                   Mat& imgDiffAcumu,
                   double& scale,
                   Rect& roi
                   );
#endif // IMGDIFFPROCESS_H
