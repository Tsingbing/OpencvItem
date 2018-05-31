#pragma once
#include <opencv.hpp>
#define THRES 2.5e+07

class MatchTemp
{
public:
    MatchTemp(cv::Mat &srcfrom,cv::Mat &tempfrom);
	~MatchTemp();
    cv::Rect match(int method);
    void showMatch(cv::String &winname);
	float result();

private:
    cv::Mat src, temp;
    cv::Rect mathRect;
	double matchRes;
};

