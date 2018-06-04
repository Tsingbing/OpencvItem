#include "dehaze.h"

using namespace std;

class LaplacianBlending {
private:
	cv::Mat left;
	cv::Mat right;
	cv::Mat blendMask;

	//Laplacian Pyramids
	vector<cv::Mat> leftLapPyr, rightLapPyr, resultLapPyr;
	cv::Mat leftHighestLevel, rightHighestLevel, resultHighestLevel;
	//mask为三通道方便矩阵相乘
	vector<cv::Mat> maskGaussianPyramid;

	int levels;

	void buildPyramids()
	{
		buildLaplacianPyramid(left, leftLapPyr, leftHighestLevel);
		buildLaplacianPyramid(right, rightLapPyr, rightHighestLevel);
		buildGaussianPyramid();
	}

	void buildGaussianPyramid()
	{
		//金字塔内容为每一层的掩模
		assert(leftLapPyr.size() > 0);

		maskGaussianPyramid.clear();
		cv::Mat currentImg;
		cvtColor(blendMask, currentImg, CV_GRAY2BGR);
		//保存mask金字塔的每一层图像
		maskGaussianPyramid.push_back(currentImg); //0-level

		currentImg = blendMask;
		for (int l = 1; l < levels + 1; l++) {
			cv::Mat _down;
			if (leftLapPyr.size() > l)
				pyrDown(currentImg, _down, leftLapPyr[l].size());
			else
				pyrDown(currentImg, _down, leftHighestLevel.size()); //lowest level

			cv::Mat down;
			cvtColor(_down, down, CV_GRAY2BGR);
			//add color blend mask into mask Pyramid
			maskGaussianPyramid.push_back(down);
			currentImg = _down;
		}
	}

	void buildLaplacianPyramid(const cv::Mat& img, vector<cv::Mat>& lapPyr, cv::Mat& HighestLevel)
	{
		lapPyr.clear();
		cv::Mat currentImg = img;
		for (int l = 0; l < levels; l++) {
			cv::Mat down, up;
			pyrDown(currentImg, down);
			pyrUp(down, up, currentImg.size());
			cv::Mat lap = currentImg - up;
			lapPyr.push_back(lap);
			currentImg = down;
		}
		currentImg.copyTo(HighestLevel);
	}

	cv::Mat reconstructImgFromLapPyramid()
	{
		//将左右laplacian图像拼成的resultLapPyr金字塔中每一层
		//从上到下插值放大并与残差相加，即得blend图像结果
		cv::Mat currentImg = resultHighestLevel;
		for (int l = levels - 1; l >= 0; l--)
		{
			cv::Mat up;
			pyrUp(currentImg, up, resultLapPyr[l].size());
			currentImg = up + resultLapPyr[l];
		}
		return currentImg;
	}

	void blendLapPyrs()
	{
		//获得每层金字塔中直接用左右两图Laplacian变换拼成的图像resultLapPyr
		resultHighestLevel = leftHighestLevel.mul(maskGaussianPyramid.back()) +
			rightHighestLevel.mul(cv::Scalar(1.0, 1.0, 1.0) - maskGaussianPyramid.back());
		for (int l = 0; l < levels; l++)
		{
			cv::Mat A = leftLapPyr[l].mul(maskGaussianPyramid[l]);
			cv::Mat antiMask = cv::Scalar(1.0, 1.0, 1.0) - maskGaussianPyramid[l];
			cv::Mat B = rightLapPyr[l].mul(antiMask);
			cv::Mat blendedLevel = A + B;

			resultLapPyr.push_back(blendedLevel);
		}
	}

public:
	LaplacianBlending(const cv::Mat& _left, const cv::Mat& _right, const cv::Mat& _blendMask, int _levels) ://construct function, used in LaplacianBlending lb(l,r,m,4);
		left(_left), right(_right), blendMask(_blendMask), levels(_levels)
	{
		assert(_left.size() == _right.size());
		assert(_left.size() == _blendMask.size());
		//创建拉普拉斯金字塔和高斯金字塔
		buildPyramids();
		//每层金字塔图像合并为一个
		blendLapPyrs();
	};

	cv::Mat blend()
	{
		//重建拉普拉斯金字塔
		return reconstructImgFromLapPyramid();
	}
};



Dehaze::Dehaze() {
	MATFile *pmatFile = NULL;
	mxArray *pReadArray = NULL;
	mxArray *pWriteArray = NULL;
	const char **dir;
	int  ndir;        //矩阵的数目
	const char *name;       //矩阵的名称
	const char *file;   //文件路径
					   // 读取.mat文件（mat文件名为"a.mat"，其中包含"m"名称的矩阵）  
	float *data;
	file = "./data/dehaze.mat";
	pmatFile = matOpen(file, "r");
	if (pmatFile == NULL) {
		printf("Error opening file %s\n", file);
	}
	dir = (const char **)matGetDir(pmatFile, &ndir);
	if (dir == NULL) {
		printf("Error reading directory of file %s\n", file);
	}
	else {
		printf("Directory of %s:\n", file);
		for (int i = 0; i < ndir; i++)
			printf("%s\n", dir[i]);
	}
	this->weights_conv3 = (float*)malloc(sizeof(float)*4*9*16);
	this->biases_conv3 = (float*)malloc(sizeof(float)*16);
	this->weights_conv5 = (float*)malloc(sizeof(float) * 4 * 25 * 16);
	this->biases_conv5 = (float*)malloc(sizeof(float) * 16);
	this->weights_conv7 = (float*)malloc(sizeof(float) * 4 * 49 * 16);
	this->biases_conv7 = (float*)malloc(sizeof(float) *  16);
	this->weights_ip = (float*)malloc(sizeof(float) * 48 * 36);
	this->biases_ip = (float*)malloc(sizeof(float));
	this->weights_conv1 = (float*)malloc(sizeof(float) * 3 * 25 * 16);
	this->biases_conv1 = (float*)malloc(sizeof(float) * 16);

	cout << "dirnum" << ndir << endl;
	int dim[10][3] = {
		16,9,4,
		16,1,1,
		16,25,4,
		16,1,1,
		16,49,4,
		16,1,1,
		1,36,48,
		1,1,1,
		16,25,3,
		16,1,1
	};
	
	for (int dir_num = 0; dir_num < ndir; dir_num++) {
		pReadArray = matGetVariable(pmatFile, dir[dir_num]);
		size_t M = mxGetM(pReadArray);  //矩阵行数
		size_t N = mxGetN(pReadArray);  //矩阵列数
		cout << M << "--" << N << endl;
		data = (float*)mxGetData(pReadArray);
		for (int k = 0; k < dim[dir_num][0]; k++) {
			for (int j = 0; j < dim[dir_num][1]; j++) {
				for (int i = 0; i < dim[dir_num][2]; i++) {
					if (dir_num == 0)
						this->weights_conv3[i * 16 * 9 + j * 16 + k] = data[j * 4 + i + k * 9 * 4];
					if (dir_num == 1)
						this->biases_conv3[ k] = data[k ];
					if (dir_num == 2)
						this->weights_conv5[i * 16 * 25 + j * 16 + k] = data[j * 4 + i + k * 25 * 4];
					if (dir_num == 3)
						this->biases_conv5[k] = data[k];
					if (dir_num == 4)
						this->weights_conv7[i * 16 * 49 + j * 16 + k] = data[j * 4 + i + k * 49 * 4];
					if (dir_num == 5)
						this->biases_conv7[k] = data[k];
					if (dir_num == 6)
						this->weights_ip[i * 36 + j] = data[j * 48 + i];
					if (dir_num == 7)
						this->biases_ip[0] = *data;
					if (dir_num == 8)
						this->weights_conv1[i * 16 * 25 + j * 16 + k] = data[j * 3 + i + k * 25 * 3];
					if (dir_num == 9)
						this->biases_conv1[k] = data[k];
				}
			}
		}
	}
	//获取数据时是以列获取的
	mxFree(dir);
	matClose(pmatFile);
	cout << "dehaze init" << endl;
}
Dehaze::~Dehaze() {
		cout << "dehaze free" << endl;
}
void  Dehaze:: convolution(vector<cv::Mat> &out,const vector<cv::Mat> &src,int sc, float weight[],float biases[],int c,int p,int f,int b) {
	int q = (int)sqrt(p);
	int hei = src[0].cols;
	int wid = src[0].rows;
	for (int i = 0; i < f; i++) {
		cv::Mat t = cv::Mat::zeros(src[0].size(), CV_32F);
		out.push_back(t);
	}
	if (b != f)
		cout << "basis not match" << endl;
	cv::Mat valueo = cv::Mat::zeros(src[0].size(), CV_32F);
	float *valuep = (float*)malloc(sizeof(float)*p);
	for (int i = 0; i < f; i++) {
		for (int j = 0; j < c; j++) {
			for (int k = 0; k < p; k++) {
				valuep[k] = weight[j*p*f + k*f + i];
			}
			cv::Mat kern = cv::Mat(q, q, CV_32F, valuep);
			kern = kern.t();
			filter2D(src[j], valueo, src[j].depth(), kern);
			out[i] = out[i] +valueo;
		}
		out[i] += biases[i];
	}
	free(valuep);
}
void Dehaze ::pic_enhacement(cv::Mat &src , cv::Mat &dst) {
	//Mat src = imread("./canyon.png");
	vector<cv::Mat>color_trans_1, color_trans_2;
	vector<cv::Mat> out,in;
	cv::Mat gray_I,src_;
	cv::Mat mergeout;

	split(src, color_trans_1);
	color_trans_2.push_back(color_trans_1[2]);
	color_trans_2.push_back(color_trans_1[1]);
	color_trans_2.push_back(color_trans_1[0]);
	merge(color_trans_2, src);
	color_trans_1.clear();
	color_trans_2.clear();

	src.convertTo(src, CV_32FC3, 1 / 255.0);
	cvtColor(src, gray_I, CV_RGB2GRAY);
	src_ = src.clone();
	split(src, color_trans_1);
	color_trans_1[0] = color_trans_1[0] - 0.5;
	color_trans_1[1] = color_trans_1[1] - 0.5;
	color_trans_1[2] = color_trans_1[2] - 0.5;
	color_trans_2.push_back(color_trans_1[0]);
	color_trans_2.push_back(color_trans_1[1]);
	color_trans_2.push_back(color_trans_1[2]);
	merge(color_trans_2, src);
	color_trans_1.clear();
	color_trans_2.clear();

	split(src, in);
	this->convolution(out, in, src.channels(), this->weights_conv1, this->biases_conv1, 3, 25, 16, 16);

	merge(out, mergeout);
	in.clear();
	out.clear();

	mergeout = mergeout.reshape(1, src.cols*src.rows);
	cv::Mat mergemaxrow = cv::Mat::zeros(src.size(), CV_32FC(4));

	float* imgdata = (float *)mergeout.data;
	float* merdata = (float *)mergemaxrow.data;
	for (int j = 0; j < src.cols*src.rows; j++) {
		for (int i = 0; i < 4; i++) {
			float maxt = -1000000;
			for (int c = 0; c < 4; c++) {
				float temp = imgdata[j * 16 + i * 4 + c];
				if (temp > maxt)
					maxt = temp;
			}
			merdata[4 * j + i] = maxt;
		}
	}

	cv::Mat F1 = mergemaxrow;
	cv::Mat F2;
	vector<cv::Mat> F2_sub;
	for (int i = 0; i < 3; i++) {
		split(F1, in);
		vector<cv::Mat>outs;
		if (i == 0)this->convolution(outs, in, 4, this->weights_conv3, this->biases_conv3, 4, 9, 16, 16);
		if (i == 1)this->convolution(outs, in, 4, this->weights_conv5, this->biases_conv5, 4, 25, 16, 16);
		if (i == 2)this->convolution(outs, in, 4, this->weights_conv7, this->biases_conv7, 4, 49, 16, 16);
		for (int i = 0; i < outs.size(); i++)
			F2_sub.push_back(outs[i]);
		outs.clear();
		in.clear();
	}

	merge(F2_sub, F2);

	cv::Mat F3;
	split(F2, out);
	cv::Mat F3_ = cv::Mat::zeros(F2.size(), CV_32F);
	cv::Mat F3__ = cv::Mat::zeros(F2.size(), CV_32FC(48));
	split(F3__, color_trans_1);
	for (int i = 0; i < out.size(); i++) {
		convMax((float*)(out[i].data), (float*)(F3_.data), F2.rows, F2.cols, 1, 3);
		convMax((float*)(F3_.data), (float*)(color_trans_1[i].data), F2.rows, F2.cols, 1, 3);
	}
	merge(color_trans_1, F3);
	color_trans_1.clear();
	out.clear();

	split(F3, in);
	vector<cv::Mat>outs;
	this->convolution(out, in, 48, this->weights_ip, this->biases_ip, 48, 36, 1, 1);
	cv::Mat F4;
	F4 = out[0];
	out.clear();
	in.clear();
	F4 = min(max(F4, 0), 1);

	vector<float>sortdata;
	for (int i = 0; i < F4.rows; i++) {
		for (int j = 0; j < F4.cols; j++) {
			sortdata.push_back(F4.at<float>(i, j));
		}
	}
	sort(sortdata.begin(), sortdata.end());
	int idx = round(0.01*F4.cols*F4.rows);
	float thresh_val = sortdata[idx];
	cv::Mat id_set = cv::Mat::zeros(F4.size(), CV_8U);
	float max_p_val = 0;
	for (int y = 0; y < F4.rows; y++) {
		for (int x = 0; x < F4.cols; x++) {
			if (F4.at <float>(y, x) < thresh_val) {
				id_set.at<uchar>(y, x) = 255;
				if (gray_I.at<float>(y, x) > max_p_val)
					max_p_val = gray_I.at<float>(y, x);
			}
		}
	}
	for (int y = 0; y < F4.rows; y++) {
		for (int x = 0; x < F4.cols; x++) {
			if (id_set.at<uchar>(y, x) == 255) {
				if (gray_I.at<float>(y, x) < max_p_val)
					id_set.at<uchar>(y, x) = 0;
			}
		}
	}
	cv::Mat A = cv::Mat::zeros(cv::Size(1, 1), CV_32FC3);
	for (int i = 0; i < 3; i++) {
		float temp = 0;
		int num = 0;
		for (int y = 0; y < F4.rows; y++) {
			for (int x = 0; x < F4.cols; x++) {
				if (id_set.at<uchar>(y, x) == 255) {
					temp += src_.at<cv::Vec3f>(y, x)[i];
					num++;
				}
			}
		}
		if (num != 0)
			A.at<cv::Vec3f>(0, 0)[i] = temp / num;
		else
			A.at<cv::Vec3f>(0, 0)[i] = 0.7;
		cout << A.at<cv::Vec3f>(0, 0)[i] << "--";
	}
	cout << endl;
	cv::ximgproc::guidedFilter(gray_I, F4, F4, this->r0, this->eps);
	vector<cv::Mat> J_, F4_;
	split(src_, J_);
	for (int i = 0; i < J_.size(); i++) {
		J_[i] = J_[i] - A.at<cv::Vec3f>(0, 0)[i];
		J_[i] = J_[i] / F4;
		J_[i] = J_[i] + A.at<cv::Vec3f>(0, 0)[i];
	}
	cv::Mat J;
	merge(J_, J);

	color_trans_1.clear();
	color_trans_2.clear();
	split(J, color_trans_1);
	color_trans_2.push_back(color_trans_1[2]);
	color_trans_2.push_back(color_trans_1[1]);
	color_trans_2.push_back(color_trans_1[0]);
	merge(color_trans_2, J);
	dst = J;

}
void Dehaze::fusion(cv::Mat &src, cv::Mat&dst){
	cv::Mat src1 = src.clone();
	cv::Mat src2 = src.clone();
	cv::Mat dst1, dst2;
	this->whitebalance(src1);
	bilateralFilter(src1, dst1, 3, 3 * 2, 9);
	dst2 = dst1.clone();

	this->colorClahe(dst2);

	cv::Mat img8UL = dst1;
	cv::Mat img8UR = dst2;

	int imgH = img8UL.rows;
	int imgW = img8UL.cols;

	cv::Mat img32fL, img32fR;
	img8UL.convertTo(img32fL, CV_32F);
	img8UR.convertTo(img32fR, CV_32F);

	cv::Mat mask = cv::Mat::zeros(imgH, imgW, CV_32FC1);
	float poiTrackBar = 95;
	mask(cv::Range::all(), cv::Range::all()) = poiTrackBar / 100.0;

	cv::Mat blendImg = LaplacianBlend(img32fL, img32fR, mask);
	blendImg.convertTo(blendImg, CV_8UC3);


	dst =  blendImg;
}
cv::Mat Dehaze::LaplacianBlend(const cv::Mat &left, const cv::Mat &right, const cv::Mat &mask)
{
	LaplacianBlending laplaceBlend(left, right, mask, 10);
	return laplaceBlend.blend();
}
void Dehaze:: whitebalance(cv::Mat &src) {
	vector<cv::Mat> BGR;
	split(src, BGR);
	double R, G, B;
	B = cv::mean(BGR[0])[0];
	G = cv::mean(BGR[1])[0];
	R = cv::mean(BGR[2])[0];
	double KR, KG, KB;
	KB = (R + G + B) / (3 * B);
	KG = (R + G + B) / (3 * G);
	KR = (R + G + B) / (3 * R);
	BGR[0] = BGR[0] * KB;
	BGR[1] = BGR[1] * KG;
	BGR[2] = BGR[2] * KR;
	merge(BGR, src);
}
void Dehaze:: colorClahe(cv::Mat &src) {
	vector<cv::Mat> BGR;
	cv::split(src, BGR);
	cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();
	clahe->setClipLimit(9);
	clahe->setTilesGridSize(cv::Size(2, 2));
	clahe->apply(BGR[0], BGR[0]);
	clahe->apply(BGR[1], BGR[1]);
	clahe->apply(BGR[2], BGR[2]);
	merge(BGR, src);
}
void Dehaze::demo(cv::Mat &src, cv::Mat &dst) {
	cv::Mat dst1;
	this->pic_enhacement(src, dst1);
	//imshow("dehaze", dst1);
	//dst1.convertTo(dst1, CV_8UC3, 255);
	//this->fusion(dst1, dst1);
	dst = dst1;
}
