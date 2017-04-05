#include "tempMatchPupilDetect.h"
#include "opticalFlowRoiDetect.h"
#include "motionToColor.h"
#include "imgDiffProcess.h"
#include "programMode.h"

using namespace std;
using namespace cv;

VideoWriter outPutMask,outPutContour,outPutProccessed;

int main() {


    int videoSource=0;//0:read video file ; 1: open webcam

    long totalFrameNumber;
    cv::VideoCapture capture;
    long frameToStart = 10;
    long frameIndex=frameToStart;
    cv::Point2d center = cv::Point2d(0, 0);
    cv::Mat frame,preFrame;
    cv::Rect roi;
    int key;
    int flag;
    double scale =0.5;
    Mat imgDiffAcumu;
    int patchThre = 0;
    int pupilMissCount =0;
    Mat templateImg;
    int pupilSizeMin = 1;
    int pupilSizeMax = 10000;

    if(!videoSource){
        capture.open("../3.avi");
        totalFrameNumber = capture.get(CV_CAP_PROP_FRAME_COUNT);
        capture.set( CV_CAP_PROP_POS_FRAMES,frameToStart);
        cout<<"start frame:"<<frameToStart<<"  to read"<<endl;}
    else{
        capture.open(0);
        totalFrameNumber = 10000;}
    if(!capture.isOpened())
        cout<<"fail to open video source!"<<endl;
    double rate = capture.get(CV_CAP_PROP_FPS);
    cout<<"fps:"<<rate<<endl;

//reading one frame for the preFrame for calculate optical flow
    if(!capture.read(frame)){
        cout<<"reading frame failure"<<endl;
        return -1;}
    frameIndex++;
    roi = cv::Rect(0, 0, 0, 0);
    templateImg=imread("../pupilTemplate.jpg");

    if(debugTag){
        outPutMask.open("./mask.avi" , CV_FOURCC('D', 'I', 'V', 'X'), 30,Size(200,200), false);
        outPutContour.open("./contour.avi" , CV_FOURCC('D', 'I', 'V', 'X'), 30,Size(200,200), false);
        outPutProccessed.open("./proccessed.avi",CV_FOURCC('D', 'I', 'V', 'X'), 30,Size(frame.cols,frame.rows), true);
        if (!outPutMask.isOpened()||!outPutContour.isOpened()||!outPutProccessed.isOpened()){
            cout  << "Could not open the output video for write: " <<  endl;
            return -1;
        }
    }

//start the detect task loop
    while (key!=27 && key!= 1048603 && frameIndex < totalFrameNumber) {       //Esc key value in win is 27, and in linux is 1048603
        double t = (double)cvGetTickCount();  //calculate the cost time

        std::swap(preFrame, frame);
        if(!capture.read(frame)){
            cout<<"reading frame failure"<<endl;
            return -1;
        }
        std::cout<<"totalFrameNumber:"<<totalFrameNumber<<"   frameIndex:"<<frameIndex<<endl;

        monoPupilDetect(frame,templateImg,roi,center,flag,patchThre,pupilMissCount,pupilSizeMin,pupilSizeMax);
//        opticalFlowPupilDetect(preFrame,frame,scale,roi,center,flag);
//        imgDiffProcess(preFrame,frame,imgDiffAcumu,scale,roi);

        frameIndex++;
        key = waitKey(1);
        cout<<key<<endl;
//按下按键后会停留在当前帧，等待下一次按键
        if( key > 0 && key!=1048603 && key!=27){
            key = waitKey(0);}
        cout<<key<<endl;

        t = (double)cvGetTickCount() - t;
        std::cout << "mainloop  cost time: " << t / ((double)cvGetTickFrequency()*1000.) <<std::endl;

    }

    if(debugTag){
        outPutMask.release();
        outPutContour.release();
        outPutProccessed.release();}
    capture.release();
    waitKey(0);

}


