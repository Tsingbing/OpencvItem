#include "camerathread.h"
#include "../matchTemp/MatchTemp.h"
#include "../perspectiveCorrect/PerspectiveCorrect.h"
#include <QDebug>
using namespace cv;

CameraThread::CameraThread(QObject *parent):
    QThread(parent)
{

}

void CameraThread::run()
{
    assert(VIDEOWIDTH > BOXWIDTH && VIDEOHEIGHT > BOXHEIGHT);

    VideoCapture capture(0);

    if(!capture.isOpened()){
        emit sigError(QStringLiteral("打开摄像头失败,请检查是否安装摄像头设备"));
        return;
    }

    capture.set(CV_CAP_PROP_FRAME_WIDTH,VIDEOWIDTH);
    capture.set(CV_CAP_PROP_FRAME_HEIGHT,VIDEOHEIGHT);

    double frameWidth = capture.get(CV_CAP_PROP_FRAME_WIDTH);
    double frameHight = capture.get(CV_CAP_PROP_FRAME_HEIGHT);

    int boxX = (frameWidth - BOXWIDTH)/2;
    int boxY = (frameHight - BOXHEIGHT)/2;

    rect = Rect(boxX,boxY,BOXWIDTH,BOXHEIGHT);
    //创建掩码矩阵
    Mat frame,alphaROI,RGBFrame;
    //在opencv中定位文本坐标
//    String text(FRAMETEXT);
//    int fontScale = 1,thick=2,baseLine=0;
//    Size textSize = getTextSize(text,FONT_HERSHEY_PLAIN,fontScale,thick,&baseLine);
//    Point org((frameWidth- textSize.width)/2,(frameHight-textSize.height)/2);

    while(!isInterruptionRequested()){
        capture >> frame;
        //方法一：设置银行卡矩形框以外的亮度
//        alphaROI = frame(Rect(0,0,frameWidth,boxY));
//        alphaROI.convertTo(alphaROI,-1,0.6,0);
//        alphaROI = frame(Rect(0,boxY,boxX,BOXHEIGHT));
//        alphaROI.convertTo(alphaROI,-1,0.6,0);
//        alphaROI = frame(Rect(boxX + BOXWIDTH,boxY,boxX,BOXHEIGHT));
//        alphaROI.convertTo(alphaROI,-1,0.6,0);
//        alphaROI = frame(Rect(0,boxY + BOXHEIGHT,frameWidth,boxY));
//        alphaROI.convertTo(alphaROI,-1,0.6,0);
        //方法二：
        //frame.convertTo(frame,-1,0.5,0);
        //alphaROI = frame(rect);
        //alphaROI.convertTo(alphaROI,-1,2,0);

        line(frame,Point(boxX - VIDEOLINEWIDTH,boxY),Point(boxX - VIDEOLINEWIDTH,boxY + VIDEOLABELVER),Scalar(0,255,0),VIDEOLINEWIDTH);
        line(frame,Point(boxX - VIDEOLINEWIDTH,boxY - VIDEOLINEWIDTH),Point(boxX + BOXWIDTH,boxY - VIDEOLINEWIDTH),Scalar(0,255,0),VIDEOLINEWIDTH);
        line(frame,Point(boxX + BOXWIDTH + 1,boxY),Point(boxX + BOXWIDTH + 1,boxY + VIDEOLABELVER),Scalar(0,255,0),VIDEOLINEWIDTH);
        line(frame,Point(boxX - VIDEOLINEWIDTH,boxY  + BOXHEIGHT - VIDEOLABELVER),Point(boxX - VIDEOLINEWIDTH,boxY + BOXHEIGHT),Scalar(0,255,0),VIDEOLINEWIDTH);
        line(frame,Point(boxX - VIDEOLINEWIDTH,boxY  + BOXHEIGHT + 1),Point(boxX + BOXWIDTH,boxY  + BOXHEIGHT + 1),Scalar(0,255,0),VIDEOLINEWIDTH);
        line(frame,Point(boxX + BOXWIDTH + 1,boxY + BOXHEIGHT - VIDEOLABELVER),Point(boxX + BOXWIDTH + 1,boxY + BOXHEIGHT),Scalar(0,255,0),VIDEOLINEWIDTH);
        //在opencv中添加文本
//        putText(frame,text,org,FONT_HERSHEY_PLAIN,fontScale,Scalar(0,255,0),thick);
        //画出文本所在的矩形框
//        rectangle(frame, org + Point(0, baseLine),
//                  org + Point(textSize.width, -textSize.height),
//                  Scalar(0,255,0));
        //画出文本除去字体粗细后的下基准线
//        line(frame, org + Point(0, thick),
//             org + Point(textSize.width, thick),
//             Scalar(0, 0, 255));
        //转换格式，并发给GUI显示
        emit sigImage(toQImage(frame));

        waitKey(30);

        //MatchTemp matchBank(alphaROI,imread(TEMPLATIMG));
        //Rect m_rect = matchBank.match(CV_TM_SQDIFF);
        //if(m_rect.width == 0){
        //    qDebug() << QStringLiteral("请插入银行卡");
        //    continue;
        //}

        //qDebug() << QStringLiteral("已检测到银行卡") << m_rect.x << m_rect.y;
        //PerspectiveCorrect perspective(alphaROI);
        //perspective.correct(alphaROI);
        emit sigImage(toQImage(frame));
    }
}

QImage CameraThread::toQImage(Mat & srcFrom)
{
    Mat rgbFrame;
    cvtColor(srcFrom,rgbFrame,CV_BGR2RGB);
    QImage img((const uchar*)rgbFrame.data,rgbFrame.cols,rgbFrame.rows,rgbFrame.step,QImage::Format_RGB888);
    img.bits(); //深拷贝，避免线程退出内存错误
    return img;
}
