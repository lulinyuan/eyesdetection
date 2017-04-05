#ifndef PUPILDETECT_H
#define PUPILDETECT_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <opencv/cv.hpp>
#include <opencv/cxcore.hpp>

#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/video/tracking.hpp"
#include <vector>
#include "imgDiffProcess.h"
#include "tempMatchPupilDetect.h"

using namespace cv;


int pupilDetec(Mat& preFrame,
               Mat& frame,
               double& scale,
               Mat& imgDiffAcumu,
               Rect& roi,
               int& roiValidation,
               int& pupilValidation,
               int& pupilMissCount,
               Point2i& pupilCenter
               );



#endif // PUPILDETECT_H
