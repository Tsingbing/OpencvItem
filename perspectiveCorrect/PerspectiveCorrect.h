#pragma once
#include <opencv.hpp>
#define AREARRATION 0.5
#define DSTIMGX 350
#define DSTIMGY 220
/*
@param srcfrom 需要灰度图像
*/
class PerspectiveCorrect
{
public:
	PerspectiveCorrect(cv::Mat const & srcfrom);
	~PerspectiveCorrect();
    bool correct(cv::Mat &outPut);

private:
    cv::Mat srcColor,srcGray;

	cv::Point2f computeIntersect(cv::Vec4i a, cv::Vec4i b);
	void sortCorners(std::vector<cv::Point2f>&corners);
    cv::Mat getAccount(cv::Mat &input);
    void ImageSharp(cv::Mat &src,cv::Mat &dst,int nAmount = 200);
};

