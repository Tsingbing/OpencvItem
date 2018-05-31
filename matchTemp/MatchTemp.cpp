#include "MatchTemp.h"
using namespace cv;
using namespace std;

MatchTemp::MatchTemp(Mat & srcfrom, Mat & tempfrom)
{
	if (srcfrom.empty() || tempfrom.empty()) {
        cout << "bad image src or templat " << __LINE__  << __func__ << endl;
		return;
	}

	src = srcfrom;
	temp = tempfrom;
	mathRect.x = -1;
	mathRect.y = -1;
	mathRect.width = 0;
	mathRect.height = 0;
	matchRes = 0.0;
}

MatchTemp::~MatchTemp()
{
}

Rect MatchTemp::match(int method)
{
	Mat result(src.cols - temp.cols + 1, src.rows - temp.rows + 1, CV_32FC1);

	matchTemplate(src, temp, result, method);
//	normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat());

	double minVal, maxVal;
	Point minLoc, maxLoc;
	Point matchLoc;

	minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());

	if (method == CV_TM_SQDIFF || method == CV_TM_SQDIFF_NORMED) {
		matchLoc = minLoc;
		matchRes = minVal;

        if (matchRes > THRES || matchLoc.x <= src.cols/2 || matchLoc.y <= src.rows/2)
			return mathRect;
	}
	else {
		matchLoc = maxLoc;
		matchRes = maxVal;
	}

    mathRect = Rect(matchLoc.x, matchLoc.y, temp.cols, temp.rows);

	return mathRect;
}

void MatchTemp::showMatch(String &winname)
{
	Mat display;
	display = src;
	if (src.channels() == IMREAD_GRAYSCALE) {
		cvtColor(src, display, CV_GRAY2BGR);
	}

	if (mathRect.width != 0 || mathRect.height != 0)
		rectangle(display, mathRect, Scalar(0, 255, 0, 0));

	imshow(winname.c_str(), display);
}

float MatchTemp::result()
{
	return matchRes;
}
