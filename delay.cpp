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
    while (true) {
        clog << "输出" << endl;
        delay(2000);
        clog << "输出" << endl;
    }

}

