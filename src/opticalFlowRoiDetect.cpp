#include"opticalFlowRoiDetect.h"

using namespace std;
using namespace cv;

int opticalFlowPupilDetect(Mat& preFrame,
                           Mat& frame,
                           double& scale,
                           cv::Rect& roi,
                           cv::Point2d& pupilCenter,
                           int& flag
                           )
{
    Mat flow,motion2color,motion2gray;
    Mat frameScaled,preFrameScaled;


    double t = (double)cvGetTickCount();

    resize(frame, frameScaled,Size(),scale, scale,INTER_LINEAR);
    resize(preFrame,preFrameScaled,Size(),scale,scale,INTER_LINEAR);
    if(frameScaled.channels()>1)
        cv::cvtColor(frameScaled,frameScaled,cv::COLOR_BGR2GRAY);
    if(preFrameScaled.channels()>1)
        cv::cvtColor(preFrameScaled,preFrameScaled,cv::COLOR_BGR2GRAY);

    namedWindow("flow-color");
    namedWindow("flow-gray");
    calcOpticalFlowFarneback(preFrameScaled, frameScaled, flow, 0.5, 3, 15, 3, 5, 1.2, 0);
    motionToColor(flow, motion2color);
    imshow("flow-color", motion2color);
    motionToGray(flow, motion2gray);
    threshold(motion2gray,motion2gray,255,255,THRESH_OTSU);
    imshow("flow-gray", motion2gray);

    t = (double)cvGetTickCount() - t;

    cout << "optical flow cost time: " << t / ((double)cvGetTickFrequency()*1000.) << endl;

    return 0;

}

