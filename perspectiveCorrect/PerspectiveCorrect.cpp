#include "PerspectiveCorrect.h"
#include <QDebug>
using namespace cv;

PerspectiveCorrect::PerspectiveCorrect(cv::Mat const & srcfrom):
	srcColor(srcfrom.clone())
{
    //如果不是灰度图像进行转化
    if(srcColor.channels() != 1)
        cvtColor(srcColor, srcGray, CV_BGR2GRAY);
    else
        srcGray = srcColor;
}

PerspectiveCorrect::~PerspectiveCorrect()
{
}

bool PerspectiveCorrect::correct(Mat &outPut)
{
    //降噪并求边缘
    blur(srcGray,srcGray,Size(3,3));
    Canny(srcGray, srcGray, 40, 120,3);
    imshow("srcGray",srcGray);
    //求轮廓
    std::vector<std::vector<Point>> contours;
    findContours(srcGray, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);
    //求出目标图像（矫正后）的最大面积
    Mat res = Mat::zeros(srcGray.size(), CV_8UC1);
    std::vector<Point> edge;
    edge.push_back(Point(res.cols, 0));
    edge.push_back(Point(res.cols, res.rows));
    edge.push_back(Point(0, res.rows));
    edge.push_back(Point(0, 0));
    double maxArea = contourArea(edge);
    std::cout << res.rows << " " << res.cols << " " << "Max area:" << maxArea << std::endl;
    //通过轮廓面积比例找到银行卡轮廓，并将它画到一个新的同等大小的图像中
    std::vector<Point> curContour;
    std::vector<std::vector<Point>>::iterator ite;
    for (ite=contours.begin();ite != contours.end();ite++) {
        curContour = *ite;
        double area = contourArea(curContour);
        if (area / maxArea > AREARRATION) {
            drawContours(res, contours, std::distance(contours.begin(), ite), Scalar(255, 255, 255));
            std::cout << area << std::endl;
            break;
        }
    }

    if(ite == contours.end()){
        qDebug() << "can't get contour";
        return false;
    }

    imshow("res", res);
    //对银行卡轮廓进行直线检测
    std::vector<Vec4i> lines;
    HoughLinesP(res, lines, 1, CV_PI / 180, 80, res.rows * 0.4, 15);

    Mat resL=Mat::zeros(srcGray.size(), CV_8UC1);
    //对直线筛选，在同一边近似重合的直线保留一条
    qDebug() << "first lines size:" << lines.size();
    for (std::vector<Vec4i>::iterator ite = lines.begin();ite != lines.end();) {
        Vec4i l = *ite;
        line(resL, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255, 255, 255));
        for (std::vector<Vec4i>::iterator itee = ite + 1;itee != lines.end();) {
            Vec4i ll = *itee;
            std::vector<Point2f> points;
            points.push_back(Point(l[0],l[1]));
            points.push_back(Point(l[2],l[3]));
            points.push_back(Point(ll[0],ll[1]));
            points.push_back(Point(ll[2],ll[3]));
            //当四个顶点（两条直线围成的面积小于50（以像素为单位）时，认为重合）
            double area = contourArea(points);
            qDebug() << "area:" << area << endl;
            if(area < 300){
                itee = lines.erase(itee);
            }else{
                itee ++;
            }
        }
        ite ++;
    }
    imshow("resl", resL);

    if(lines.size() != 4){
        qDebug() << "lines not equal 4" << lines.size();
        return false;
    }

    Mat resC = Mat::zeros(srcGray.size(),CV_8UC1);
    std::vector<Point2f> corners;
    for(int i=0;i<lines.size();i++){
        Vec4i l = lines.at(i);
        line(resC, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255, 255, 255));
        for(int j = i + 1 ;j<lines.size();j++){
            Point2f pt = computeIntersect(lines[i],lines[j]);
            if(pt.x > 0 && pt.x <= resC.cols && pt.y > 0 && pt.y <= resC.rows){
                corners.push_back(pt);
            }
        }
    }
    imshow("corL",resC);

    if(corners.size() !=4 ){
        qDebug() << "points not equal 4" << corners.size();
        return false;
    }
    //用多边形逼进4个点,判断是否是矩形
    std::vector<Point2f> approx;
    approxPolyDP(Mat(corners),approx,arcLength(Mat(corners),true)*0.02,true);
    if(approx.size() !=4){
        qDebug() << "The object is not quadrilateral";
        return false;
    }
    std::cout << "corners number" << corners.size();
    //对四个顶点排序
    sortCorners(corners);
    Mat dst = Mat::zeros(Size(DSTIMGX,DSTIMGY),CV_8UC3);
    std::vector<Point2f> dst_pt;
    dst_pt.push_back(Point2f(0, 0));
    dst_pt.push_back(Point2f(dst.cols, 0));
    dst_pt.push_back(Point2f(dst.cols, dst.rows));
    dst_pt.push_back(Point2f(0, dst.rows));
    //进行透视变换
    Mat transmtx = getPerspectiveTransform(corners, dst_pt);
    warpPerspective(srcColor, dst, transmtx, dst.size());
    cvtColor(dst, dst, CV_BGR2GRAY);
    imshow("srcColor", srcColor);
    imshow("dst", dst);

    getAccount(dst);
    outPut = dst;
    return true;
}

Mat PerspectiveCorrect::getAccount(Mat &input)
{
    Mat colorDst;
    cvtColor(input,colorDst,CV_GRAY2BGR);
    std::vector<Vec4i> cardLines;
//    blur(input,input,Size(3,3));
//    Canny(input,input,50,150,3);
    ImageSharp(input,input,400);
    threshold(input,input,0,255,CV_THRESH_BINARY_INV);
    HoughLinesP(input,cardLines,1,CV_PI / 180,40,input.cols * 0.5,10);
    for(std::vector<Vec4i>::iterator ite = cardLines.begin();ite != cardLines.end();ite++){
        Vec4i l = *ite;
        line(colorDst,Point(l[0],l[1]),Point(l[2],l[3]),Scalar(0,255,0));
    }
    imshow("lineDst",colorDst);
    imshow("input",input);
    return Mat();
}

Point2f PerspectiveCorrect::computeIntersect(cv::Vec4i a, cv::Vec4i b)
{
	int x1 = a[0], y1 = a[1], x2 = a[2], y2 = a[3];
	int x3 = b[0], y3 = b[1], x4 = b[2], y4 = b[3];

	if (float d = ((float)(x1 - x2) * (y3 - y4)) - ((y1 - y2) * (x3 - x4)))
	{
		cv::Point2f pt;
		pt.x = ((x1*y2 - y1*x2) * (x3 - x4) - (x1 - x2) * (x3*y4 - y3*x4)) / d;
		pt.y = ((x1*y2 - y1*x2) * (y3 - y4) - (y1 - y2) * (x3*y4 - y3*x4)) / d;
		return pt;
	}
	else
		return cv::Point2f(-1, -1);
}

void PerspectiveCorrect::sortCorners(std::vector<cv::Point2f>& corners)
{
	Point2f center;
	int x = 0, y = 0;
	for (int i = 0;i < corners.size();i++) {
		x += corners.at(i).x;
		y += corners.at(i).y;
	}

	x /= corners.size();
	y /= corners.size();
	center.x = x;
	center.y = y;

	std::vector<cv::Point2f> top, bot;

	for (int i = 0; i < corners.size(); i++)
	{
		if (corners[i].y < center.y)
			top.push_back(corners[i]);
		else
			bot.push_back(corners[i]);
	}

	cv::Point2f tl = top[0].x > top[1].x ? top[1] : top[0];
	cv::Point2f tr = top[0].x > top[1].x ? top[0] : top[1];
	cv::Point2f bl = bot[0].x > bot[1].x ? bot[1] : bot[0];
	cv::Point2f br = bot[0].x > bot[1].x ? bot[0] : bot[1];

	corners.clear();
	corners.push_back(tl);
	corners.push_back(tr);
	corners.push_back(br);
	corners.push_back(bl);
}

void PerspectiveCorrect::ImageSharp(Mat &src, Mat &dst, int nAmount)
{
    double sigma = 3;
    int threshold = 1;
    float amount = nAmount / 100.0f;

    Mat imgBlurred;
    GaussianBlur(src, imgBlurred, Size(), sigma, sigma);

    Mat lowContrastMask = abs(src - imgBlurred)<threshold;
    dst = src*(1 + amount) + imgBlurred*(-amount);
    src.copyTo(dst, lowContrastMask);
}
