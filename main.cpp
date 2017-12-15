#include <cstdlib>
#include <iostream>
#include <vector>

#include "GPIOlib.h"

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

#define PI 3.1415926
#define INTERVAL 3
#define PARALLEL 5
#define SPEED 9

//Uncomment this line at run-time to skip GUI rendering
//#define _DEBUG

using namespace cv;
using namespace std;
using namespace GPIO;

const string CAM_PATH="/dev/video0";
const string MAIN_WINDOW_NAME="Processed Image";
const string CANNY_WINDOW_NAME="Canny";

const int CANNY_LOWER_BOUND=50;
const int CANNY_UPPER_BOUND=250;
const int HOUGH_THRESHOLD=150;

int main()
{
    init();

    int turn = 0;

	VideoCapture capture(CAM_PATH,CV_LOAD_IMAGE_GRAYSCALE);
	//If this fails, try to open as a video camera, through the use of an integer param
	if (!capture.isOpened())
	{
		capture.open(atoi(CAM_PATH.c_str()));
	}

	double dWidth=capture.get(CV_CAP_PROP_FRAME_WIDTH);			//the width of frames of the video
	double dHeight=capture.get(CV_CAP_PROP_FRAME_HEIGHT);		//the height of frames of the video

	Mat image;

    //过重点后的循环计数
//    for(int i=0;i<3;i++) {
        int cnt = 0;

        bool start = true;
        bool hasLine = true;

        bool seeLine = false;

        bool stop = false;

        turnTo(0);
        controlLeft(FORWARD, SPEED);
        controlRight(FORWARD, SPEED);
        start = true;

    int countTurn = 0;

    //转完后延迟计数
    int after_turn_cnt = 0;

        while (cnt < 1) {

            capture >> image;


            if (image.empty()) {
                cout << "empty";
                break;
            }

            //Set the ROI for the image
            Rect roi(0, image.rows / 3, image.cols, image.rows / 3);
            Mat imgROI = image(roi);

            //Canny algorithm
            Mat contours;
            Canny(imgROI, contours, CANNY_LOWER_BOUND, CANNY_UPPER_BOUND);
#ifdef _DEBUG
            imshow(CANNY_WINDOW_NAME,contours);
#endif

            vector<Vec2f> lines;
            HoughLines(contours, lines, 1, PI / 180, 130);
            Mat result(imgROI.size(), CV_8U, Scalar(255));
            imgROI.copyTo(result);
            clog << lines.size() << endl;


            float maxRad = -2 * PI;
            float minRad = 2 * PI;
            //Draw the lines and judge the slope
            int max_xs[2];
            int min_xs[2];
            int count = 0;

            int parallelLines = 0;

            //水平线的最大纵坐标
            int maxRow = 0;
            int minRow = imgROI.rows;

            for (vector<Vec2f>::const_iterator it = lines.begin(); it != lines.end(); ++it) {
                float rho = (*it)[0];            //First element is distance rho
                float theta = (*it)[1];        //Second element is angle theta09
                //Filter to remove vertical and horizontal lines,
                //and atan(0.09) equals about 5 degrees.

                if (theta > 1.45 && theta < 1.65) {
                    parallelLines++;
                    if (maxRow < rho) {
                        maxRow = rho;
                    }
                    if (minRow > rho) {
                        minRow = rho;
                    }
                }


                if ((theta > 0.09 && theta < 1.28) || (theta > 1.82 && theta < 3.05)) {
                    count++;
                    if (theta > maxRad) {
                        maxRad = theta;
                        max_xs[0] = rho / cos(theta);
                        max_xs[1] = (rho - result.rows * sin(theta)) / cos(theta);
                    }
                    if (theta < minRad) {
                        minRad = theta;
                        min_xs[0] = rho / cos(theta);
                        min_xs[1] = (rho - result.rows * sin(theta)) / cos(theta);
                    }

#ifdef _DEBUG
                    //point of intersection of the line with first row
    //				Point pt1(rho/cos(theta),0);
    //				//point of intersection of the line with last row
    //				Point pt2((rho-result.rows*sin(theta))/cos(theta),result.rows);
                    //Draw a line
                    line(result,pt1,pt2,Scalar(0,255,255),3,CV_AA);
                    clog<<"pt:"<<pt1<<" "<<pt2<<"\n";
                    clog<<"Line: ("<<rho<<","<<theta/PI*180<<")\n";
#endif
                }

#ifdef _DEBUG
                //			clog<<"Line: ("<<rho<<","<<theta<<")\n";
#endif
            }


            if (minRow != imgROI.rows && !maxRow == 0) {
                clog << "dif:" << maxRow - minRow << "\n";
            }

            if (maxRow - minRow > 40 && maxRow - minRow<65) {
                clog << "我看到了终点线" << "\n";
//                seeLine = true;
                stop = true;

            }

            if (stop && after_turn_cnt==0) {
//                cnt++;
                if(turn == 2){
                    delay(500);
                    stopRight();
                    stopLeft();
                    break;
                }

                turnTo(0);
                controlLeft(BACKWARD,20);
                controlRight(BACKWARD, 20);
                delay(400);

                stopRight();
                stopLeft();

                turnTo(0);
                delay(10);

                controlLeft(BACKWARD, 20);
                controlRight(FORWARD, 20);
                delay(1000);

                stopRight();
                stopLeft();
                delay(10);
                countTurn++;

                controlLeft(FORWARD, SPEED);
                controlRight(FORWARD, SPEED);

                after_turn_cnt++;
                turn++;

            }

            if(stop && after_turn_cnt>0){
                if(after_turn_cnt>=10){
                    stop = false;
                    after_turn_cnt = 0;
                } else {
                    after_turn_cnt++;
                }
            }

//
//            if (seeLine && maxRow > imgROI.rows - 30) {
//                clog << "我要停了\n";
//                stop = true;
//            }

            if (count >= 1) {
                clog << "min:" << minRad << " max:" << maxRad << "\n";

#ifdef _DEBUG
                //			clog<<"left:"<<thetas[0]/PI*180<<" right:"<<thetas[thetas.size()-1]/PI*180;
                            stringstream overlayedText;
                            overlayedText<<"Lines: "<<lines.size();
                            putText(result,overlayedText.str(),Point(10,result.rows-10),2,0.8,Scalar(0,0,255),0);
                            imshow(MAIN_WINDOW_NAME,result);
#endif

                if (minRad > PI / 2 && minRad < PI) {

                    turnTo(-10);

                    delay(20);
                    clog << "oneside:turn to left\n";
                } else if (maxRad < PI / 2 && minRad > 0) {
                    turnTo(15);

                    delay(20);
                    clog << "oneside:turn to right\n";
                } else {
                }
            }//若没有检测到边
            else {
                turnTo(0);
                delay(20);

            }

        }


//        stopRight();
//        stopLeft();
//
//        turnTo(0);
//        delay(10);
//
//        controlLeft(BACKWARD, 20);
//        controlRight(FORWARD, 20);
//        delay(1000);
//
//        stopRight();
//        stopLeft();
//        delay(10);



//    }

    init();

    return 0;

}

