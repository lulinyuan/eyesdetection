#ifndef OPTICALFLOWROIDETECT_H
#define OPTICALFLOWROIDETECT_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <opencv/cv.hpp>
#include <opencv/cxcore.hpp>

#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/video/tracking.hpp"
#include <vector>

#include "motionToColor.h"

int opticalFlowPupilDetect(Mat& preFrame,
                           Mat& frame,
                           double& scale,
                           cv::Rect& roi,
                           cv::Point2d& pupilCenter,
                           int& flag
                           );


#endif // OPTICALFLOWROIDETECT_H
