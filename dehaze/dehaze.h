#pragma once
#include "mat.h"
#include "matrix.h"
#include "convConst.h"
#include <ximgproc.hpp>  
#include <opencv2/opencv.hpp>
using namespace std;

class Dehaze {
public:
	Dehaze();
	~Dehaze();
	void Dehaze::convolution(vector<cv::Mat> &out, const vector<cv::Mat>& src, int sc, float weight[], float biases[], int c, int p, int f, int b);
	void Dehaze::colorClahe(cv::Mat &src);
	void Dehaze::pic_enhacement(cv::Mat &src, cv::Mat &dst);
	void Dehaze::fusion(cv::Mat &src, cv::Mat &dst);
	void Dehaze::whitebalance(cv::Mat &src);
	cv::Mat Dehaze::LaplacianBlend(const cv::Mat &left, const cv::Mat &right, const cv::Mat &mask);
	void Dehaze::demo(cv::Mat &src, cv::Mat &dst);
	const int r0 = 50;
	const float eps = 0.001f;
	float *weights_conv1;
	float *weights_conv3;
	float *weights_conv5;
	float *weights_conv7;
	float *weights_ip;
	float *biases_conv1;
	float *biases_conv3;
	float *biases_conv5;
	float *biases_conv7;
	float *biases_ip;
};

//class LaplacianBlending {
//private:
//	Mat left;
//	Mat right;
//	Mat blendMask;
//
//	//Laplacian Pyramids
//	vector<Mat> leftLapPyr, rightLapPyr, resultLapPyr;
//	Mat leftHighestLevel, rightHighestLevel, resultHighestLevel;
//	//mask为三通道方便矩阵相乘
//	vector<Mat> maskGaussianPyramid;
//
//	int levels;
//
//	void buildPyramids()
//	{
//		buildLaplacianPyramid(left, leftLapPyr, leftHighestLevel);
//		buildLaplacianPyramid(right, rightLapPyr, rightHighestLevel);
//		buildGaussianPyramid();
//	}
//
//	void buildGaussianPyramid()
//	{
//		//金字塔内容为每一层的掩模
//		assert(leftLapPyr.size() > 0);
//
//		maskGaussianPyramid.clear();
//		Mat currentImg;
//		cvtColor(blendMask, currentImg, CV_GRAY2BGR);
//		//保存mask金字塔的每一层图像
//		maskGaussianPyramid.push_back(currentImg); //0-level
//
//		currentImg = blendMask;
//		for (int l = 1; l < levels + 1; l++) {
//			Mat _down;
//			if (leftLapPyr.size() > l)
//				pyrDown(currentImg, _down, leftLapPyr[l].size());
//			else
//				pyrDown(currentImg, _down, leftHighestLevel.size()); //lowest level
//
//			Mat down;
//			cvtColor(_down, down, CV_GRAY2BGR);
//			//add color blend mask into mask Pyramid
//			maskGaussianPyramid.push_back(down);
//			currentImg = _down;
//		}
//	}
//
//	void buildLaplacianPyramid(const Mat& img, vector<Mat>& lapPyr, Mat& HighestLevel)
//	{
//		lapPyr.clear();
//		Mat currentImg = img;
//		for (int l = 0; l < levels; l++) {
//			Mat down, up;
//			pyrDown(currentImg, down);
//			pyrUp(down, up, currentImg.size());
//			Mat lap = currentImg - up;
//			lapPyr.push_back(lap);
//			currentImg = down;
//		}
//		currentImg.copyTo(HighestLevel);
//	}
//
//	Mat reconstructImgFromLapPyramid()
//	{
//		//将左右laplacian图像拼成的resultLapPyr金字塔中每一层
//		//从上到下插值放大并与残差相加，即得blend图像结果
//		Mat currentImg = resultHighestLevel;
//		for (int l = levels - 1; l >= 0; l--)
//		{
//			Mat up;
//			pyrUp(currentImg, up, resultLapPyr[l].size());
//			currentImg = up + resultLapPyr[l];
//		}
//		return currentImg;
//	}
//
//	void blendLapPyrs()
//	{
//		//获得每层金字塔中直接用左右两图Laplacian变换拼成的图像resultLapPyr
//		resultHighestLevel = leftHighestLevel.mul(maskGaussianPyramid.back()) +
//			rightHighestLevel.mul(Scalar(1.0, 1.0, 1.0) - maskGaussianPyramid.back());
//		for (int l = 0; l < levels; l++)
//		{
//			Mat A = leftLapPyr[l].mul(maskGaussianPyramid[l]);
//			Mat antiMask = Scalar(1.0, 1.0, 1.0) - maskGaussianPyramid[l];
//			Mat B = rightLapPyr[l].mul(antiMask);
//			Mat blendedLevel = A + B;
//
//			resultLapPyr.push_back(blendedLevel);
//		}
//	}
//
//public:
//	LaplacianBlending(const Mat& _left, const Mat& _right, const Mat& _blendMask, int _levels) ://construct function, used in LaplacianBlending lb(l,r,m,4);
//		left(_left), right(_right), blendMask(_blendMask), levels(_levels)
//	{
//		assert(_left.size() == _right.size());
//		assert(_left.size() == _blendMask.size());
//		//创建拉普拉斯金字塔和高斯金字塔
//		buildPyramids();
//		//每层金字塔图像合并为一个
//		blendLapPyrs();
//	};
//
//	Mat blend()
//	{
//		//重建拉普拉斯金字塔
//		return reconstructImgFromLapPyramid();
//	}
//};
//


