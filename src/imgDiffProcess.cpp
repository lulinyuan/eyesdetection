#include "imgDiffProcess.h"

using namespace cv;
using namespace std;

int imgDiffProcess(Mat& preFrame,
                   Mat& frame,
                   Mat& imgDiffAcumu,
                   double& scale,
                   cv::Rect& roi
                   ){


    double t = (double)cvGetTickCount();
    vector<vector<cv::Point>> contours;
    double maxArea = 0;
    vector<cv::Point> maxContour;

    Mat roiedImg;



    Mat frameScaled,preFrameScaled,motion;

    resize(frame, frameScaled,Size(),scale, scale,INTER_LINEAR);
    resize(preFrame,preFrameScaled,Size(),scale,scale,INTER_LINEAR);
    if(frameScaled.channels()>1)
        cv::cvtColor(frameScaled,frameScaled,cv::COLOR_BGR2GRAY);
    if(preFrameScaled.channels()>1)
        cv::cvtColor(preFrameScaled,preFrameScaled,cv::COLOR_BGR2GRAY);

    cv::absdiff(frameScaled, preFrameScaled, motion);

    if(0==imgDiffAcumu.size().width || 0==imgDiffAcumu.size().height )
    {
        motion.copyTo(imgDiffAcumu);
        cout<< "diff img not stored"<<endl;
    }
    else
    {
        imgDiffAcumu = max(1*imgDiffAcumu,motion);

    }
    namedWindow("diff img");
    imshow("diff img",motion);


    cv::threshold(imgDiffAcumu, motion, 0, 255, THRESH_OTSU);
//    cv::erode(motion, motion, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5,5)));

    morphologyEx( motion,motion,MORPH_OPEN,
                  cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3,3)),
                  Point(-1,-1),1,
                  BORDER_CONSTANT,
                  morphologyDefaultBorderValue());

    namedWindow("threshold acumu diff img");
    imshow("threshold acumu diff img",motion);



    cv::findContours(motion,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);


    for(size_t i = 0; i < contours.size(); i++)
    {
        double area = cv::contourArea(contours[i]);
        if (area > maxArea)
        {
            maxArea = area;
            maxContour = contours[i];
        }
    }
    roi = cv::boundingRect(maxContour);
    cv::rectangle(motion, roi, cv::Scalar(255));


    namedWindow("roi img");
    imshow("roi img",motion);


    roi.x*=1/scale;
    roi.y*=1/scale;
    roi.width *=1/scale;
    roi.height*=1/scale;
    cv::rectangle(preFrame, roi, cv::Scalar(255));
    namedWindow("roi marked img");
    imshow("roi marked img",preFrame);


    preFrame(roi).copyTo(roiedImg);
    cv::cvtColor(roiedImg,roiedImg,cv::COLOR_BGR2GRAY);
    namedWindow("roied img");
    imshow("roied img",roiedImg);

    t = (double)cvGetTickCount() - t;
    cout << "img diff cost time: " << t / ((double)cvGetTickFrequency()*1000.) << endl;

    return 0;

}

