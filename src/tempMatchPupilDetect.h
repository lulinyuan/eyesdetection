#ifndef PUPIL_DETECT_H
#define PUPIL_DETECT_H
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <opencv/cv.hpp>
#include <opencv/cxcore.hpp>

#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/video/tracking.hpp"
#include <vector>
\

using namespace std;
using namespace cv;

int monoPupilDetect(const cv::Mat& eye,
                    const cv::Mat& templateImg,
                    cv::Rect& roi,
                    cv::Point2d& pupilCenter,
                    int& matchFlag,
                    int& patchThre,
                    int& pupilMissCount,
                    int& pupilSizeMin,
                    int& pupilSizeMax
                    );

extern VideoWriter outPutMask,outPutContour,outPutProccessed;


#endif // PUPIL_DETECT_H
