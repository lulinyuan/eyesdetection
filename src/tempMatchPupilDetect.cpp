#include "tempMatchPupilDetect.h"
#include "programMode.h"

//Mat obj is the pupil template to match over the eye image

//monoPupilDetect
using namespace cv;
using namespace std;

int monoPupilDetect(const cv::Mat& eye,
                    const cv::Mat& templateImg,
                    cv::Rect& roi,
                    cv::Point2d& pupilCenter,
                    int& matchFlag,
                    int& patchThre,
                    int& pupilMissCount,
                    int& pupilSizeMin,
                    int& pupilSizeMax
                    )
{
    int method = cv::TemplateMatchModes::TM_CCOEFF_NORMED;  //template match method setting
//if the roi rectangle is not valid ,return
    if (roi.width==0||roi.height==0){
        cout<<"roi initializing error"<<endl;
        roi.x =0;
        roi.y =0;
        roi.width=eye.cols;
        roi.height=eye.rows;
        return -1;}
    double t = (double)cvGetTickCount();  //calculate the cost time

    cv::Mat roiImg;                       //roied img from the original eye img
    eye(roi).copyTo(roiImg);
    cv::Mat obj;
    templateImg.copyTo(obj);              //read the template to match in the souce img ,of square size
    Mat showEye;                          //show the origial img with a matched rectangle on it without changing the source img
    eye.copyTo(showEye);                  //copy the source img to the showEye

    cv::Mat matchResult,scaledObj;        //template matching feature map , and the scaled template
    double minVal, maxVal;                //the max and min matching value in the feature map
    cv::Point minLoc, maxLoc;             //the position of the worst matching and the best matching
    cv::Mat matchPatch;                   //the best match patch in the roi img
    cv::Rect matchRect;                   //the outer rectangle of the match patch img
    double pupilSizeRatioRef2Roi=1;       //the ratio of the matching template size to the roi img
    double pupilCheckValue;               // check the area perimeter ratio of the match patch
    cv::Point pupilCenterInMatchPatch;
    cv::Moments pupilCenterMoment;        //get the pupil center cordinates in the match patch

    int shortLen = (roiImg.size().width<roiImg.size().height)?roiImg.size().width:roiImg.size().height;   //find the shorter edge of the roi rectangle
    if(roiImg.channels()>1)                                          //format the roiImg and the template img obj to gray 1channel
        cv::cvtColor(roiImg,roiImg,cv::COLOR_BGR2GRAY);
    if(obj.channels()>1)
        cv::cvtColor(obj,obj,cv::COLOR_BGR2GRAY);

    pupilSizeRatioRef2Roi = (2*pupilSizeMax>shortLen)? 1:(2*pupilSizeMax/shortLen);


//scale the obj template to the proper size in the ROI image of eye image
    while(1){

        pupilSizeRatioRef2Roi*=0.75;                              //scaling the template obj from 0.95 size of roiImg till a size of 10 pixel,0.95 times step by step to match the pupil best
        cv::resize(obj,scaledObj,
                   cv::Size(round(pupilSizeRatioRef2Roi/2.0*shortLen)*2+1,round(pupilSizeRatioRef2Roi/2.0*shortLen)*2+1),
                   0,0,cv:: INTER_LINEAR);
        if(scaledObj.size().width<2*pupilSizeMin || scaledObj.size().height<2*pupilSizeMin )
        {
            std::cout<<"zoom out template enough and still no match"<<std::endl;
            matchFlag = 0;
            pupilMissCount++;
            if(pupilMissCount>30){                                //if the pupil was not found for 30 frames ,set the search area the whole original img and the path threshold history
                pupilMissCount=0;
                patchThre=0;
                roi = cv::Rect(0, 0, eye.cols, eye.rows);
                pupilSizeMin=15;
                pupilSizeMax=10000;
                }
            cout<<"pupil miss count :"<<pupilMissCount<<endl;
            cout<<"pupil size max:"<<pupilSizeMax<<"    pupil size min:"<<pupilSizeMin<<endl;
            cout<<"roi width,height:"<<roi.width<<","<<roi.height<<endl;
            return -1;
        }

        std::vector<std::vector<cv::Point>> contours;                   //the all contours(external,without hole in one contour) in the binarized img
        double maxArea = 0;                                            //the area of the connect-component with the max area in the best match patch
        std::vector<cv::Point> maxContour;                             //the contour of the max area connect-component
        int morphOpenSize;                                             //open morph operation in the best match patch to avoid noise and eyelash
        cv::matchTemplate(roiImg,scaledObj,matchResult,method);        //match the scaled template in the roi image

//find out the position of the most matching patch
        cv::minMaxLoc(matchResult, &minVal, &maxVal, &minLoc, &maxLoc);           //find the location of the best match patch
        matchRect.x=maxLoc.x;
        matchRect.y=maxLoc.y;
        matchRect.width=scaledObj.size().width;
        matchRect.height=scaledObj.size().height;
        roiImg(matchRect).copyTo(matchPatch); //clone the best match patch to a new img:matchPatch
        if(!patchThre){
            cv::threshold(matchPatch, matchPatch, 0, 255, cv::THRESH_OTSU);      //binarize the best match patch, assign 1 to pupil area element and 0 to the other
            matchPatch=255 - matchPatch;}
        else{
            cv::threshold(matchPatch, matchPatch, patchThre, 255, cv::THRESH_BINARY_INV);
        }
        morphOpenSize = (scaledObj.size().width/10 < 1) ?  1 : scaledObj.size().width/10;      // limit the morphOpenSize as 1/5 size of the pupil
        morphologyEx( matchPatch,matchPatch,MORPH_OPEN,
                      cv::getStructuringElement(cv::MORPH_RECT, cv::Size(morphOpenSize,morphOpenSize)),
                      Point(-1,-1),1,
                      BORDER_CONSTANT,
                      morphologyDefaultBorderValue());
        cout<<"morph open size: "<<morphOpenSize<<endl;

//find out the maxArea contour
        cv::findContours(matchPatch.clone(),contours,CV_RETR_EXTERNAL,CHAIN_APPROX_NONE);        //find the contours in the binarized best match

        for(size_t i = 0; i < contours.size(); i++)                                      //search for the max area contour in the match patch
        {
            double area = cv::contourArea(contours[i]);
            if (area > maxArea)
            {
                maxArea = area;
                maxContour = contours[i];

            }
        }
        if(maxArea < matchPatch.rows*matchPatch.cols*0.6*3.14/16)   //if the ratio of the max area of a connected component
            continue;                                               //to the area of whole match patch is less than a limit,match failing


        Rect convexRect = boundingRect(maxContour);
        if( convexRect.x<0.1*matchPatch.cols ||                      //if the max area of black blob is to closer to the
                convexRect.y< 0.1*matchPatch.rows  ||                //edge of the match patch ,match failing
                convexRect.x+convexRect.width >0.9*matchPatch.cols  ||
                convexRect.y+convexRect.height > 0.9*matchPatch.rows )
            continue;

        std::vector<cv::Point> maxContourHull;
        convexHull(maxContour,  maxContourHull, false,true);        // get the convex hull of the max area of black blob in the
        double hullArea = contourArea(maxContourHull);              // match patch ,and check if the hull is almost an ellipse
        double hullArcLength = arcLength(maxContourHull,true);      // if not so similar to , match failing


        pupilCheckValue = hullArea*4*3.14/(hullArcLength*hullArcLength);
        cout << "circle check:" <<pupilCheckValue<<endl;
        if(pupilCheckValue < 0.8){
            continue;
        }
        else{
            matchFlag = 1;                                                  //calculate the pupil center in the match patch
            pupilCenterMoment = moments(maxContourHull);
            pupilCenterInMatchPatch.x =pupilCenterMoment.m10/pupilCenterMoment.m00 ;
            pupilCenterInMatchPatch.y = pupilCenterMoment.m01/pupilCenterMoment.m00;

            pupilSizeMin = 0.2*matchPatch.rows;
            pupilSizeMax = 1.5*matchPatch.rows;
            pupilMissCount=0;

//set the match patch threshold to the average value  of the balck blob of the max area in the match patch ,additional plus a offset 10
            Scalar meanValInPupilBlob;
            std::vector<vector<cv::Point>> maskContour(1);
            maskContour[0] = maxContour;
            Mat mask(matchRect.height, matchRect.width,CV_8UC1,Scalar(0));
            drawContours(mask, maskContour, -1, Scalar(255), CV_FILLED);

            meanValInPupilBlob = mean(roiImg(matchRect),mask);
            cout<<"threshold"<<meanValInPupilBlob<<endl;
            patchThre = meanValInPupilBlob.val[0] +20;
            pupilCenter.x = roi.x + matchRect.x + pupilCenterInMatchPatch.x;
            pupilCenter.y = roi.y + matchRect.y + pupilCenterInMatchPatch.y;

            if(roi.width==eye.cols && roi.height==eye.rows){
                roi.x=(matchRect.x - 0.5*matchRect.width)<0 ? 0 : (matchRect.x - 0.5*matchRect.width);
                roi.y=(matchRect.y - 0.5*matchRect.height)<0 ? 0 : (matchRect.y - 0.5*matchRect.height) ;
                roi.width=(matchRect.x+1.5*matchRect.width)>eye.cols ? (eye.cols-roi.x) : matchRect.x+1.5*matchRect.width-roi.x ;
                roi.height=(matchRect.y+1.5*matchRect.height)>eye.rows ? (eye.rows-roi.y) : matchRect.y+1.5*matchRect.height-roi.y;
            }
            else{
                int tempX=(roi.x+matchRect.x - 0.5*matchRect.width)<0 ?
                            0 :((matchRect.x - 0.5*matchRect.width)<0 ?
                                    (roi.x+matchRect.x - 0.5*matchRect.width): roi.x);
                int tempY=(roi.y+matchRect.y - 0.5*matchRect.height)<0 ?
                            0 :((matchRect.y - 0.5*matchRect.height)<0 ?
                                    (roi.y+matchRect.y - 0.5*matchRect.height): roi.y);
                int tempWidth=(roi.x+matchRect.x+1.5*matchRect.width)> eye.cols ?
                            (eye.cols-tempX):((matchRect.x+1.5*matchRect.width)>roi.width ?
                                                  (roi.x+matchRect.x+1.5*matchRect.width-tempX):
                                                  (roi.x+roi.width-tempX));
                int tempHeight=(roi.y+matchRect.y+1.5*matchRect.height)> eye.rows ?
                            (eye.rows-tempY):((matchRect.y+1.5*matchRect.height)>roi.height ?
                                                  (roi.y+matchRect.y+1.5*matchRect.height-tempY):
                                                  (roi.y+roi.height-tempY));
                roi.x=tempX;
                roi.y=tempY;
                roi.width=tempWidth;
                roi.height=tempHeight;

            }

            if(debugTag){
//show the contours in the match patch
                namedWindow("match patch contours");
                Mat matchContourImage = Mat ::zeros(matchPatch.size(),CV_8U);
                drawContours(matchContourImage, contours, -1, Scalar(255, 0, 255));
                imshow("match patch contours",matchContourImage);
//show the pupil convex contour in the best match patch
                namedWindow("pupil covex contour");
                std::vector<vector<cv::Point>> hull(1);
                hull[0]= maxContourHull ;
                Mat contourResultImage = Mat ::zeros(matchPatch.size(),CV_8U);
                drawContours(contourResultImage, hull, -1, Scalar(255, 0, 255));
                imshow("pupil covex contour",contourResultImage);
//write convex contour to avi
                Mat tempContour(200,200,CV_8UC1,Scalar(0));
                resize(contourResultImage,tempContour,Size(tempContour.cols,tempContour.rows),0,0,INTER_LINEAR);
                outPutContour<<tempContour;
                namedWindow("convex contour");
                imshow("convex contour",tempContour);

//shoe the pupil masked img in the match patch
                namedWindow("pupil masked Img without hole in");
                imshow("pupil masked Img without hole in",mask);
//write to avi
                Mat tempMask(200,200,CV_8UC1,Scalar(0));
                resize(mask,tempMask,Size(tempMask.cols,tempMask.rows),0,0,INTER_LINEAR);
                namedWindow("resize mask");
                imshow("resize mask",tempMask);
                outPutMask<<tempMask;
            }

            break;
        }

    }

    if(debugTag){
//show the original img
        namedWindow("original img");
        cv::imshow("original img",eye);
//show the scaled template
        namedWindow("scaled template");
        cv::imshow("scaled template", scaledObj);
//show the match patch after morph open op
        namedWindow("morph open binarized match patch");
        cv::imshow("morph open binarized match patch",matchPatch);
//show the pupil detection result with rectangle roi and circl pupil center ,and write to avi
        cv::rectangle(showEye,cv::Rect(roi.x,roi.y,roi.width,roi.height),cv::Scalar(0,0,255), 2, 8, 0);
        circle(showEye,pupilCenter,5,Scalar(0,255,0));
        namedWindow("roiMarked original img");
        cv::imshow("roiMarked original img",showEye);
        outPutProccessed<<showEye;
    }
    std::cout<< " x:"<< pupilCenter.x <<" y:"<<pupilCenter.y<<std::endl;
    t = (double)cvGetTickCount() - t;
    std::cout << "template match cost time: " << t / ((double)cvGetTickFrequency()*1000.) <<std::endl;
}
