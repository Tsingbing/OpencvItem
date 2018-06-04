#ifndef CAMERATHREAD_H
#define CAMERATHREAD_H
#include <QThread>
#include <QImage>
#include <opencv.hpp>
//#include <opencv2/opencv.hpp>
#include "./dehaze/dehaze.h"

#define VIDEOWIDTH  1920   //摄像头视频帧宽度
#define VIDEOHEIGHT 1080   //摄像头视频帖高度
#define BOXWIDTH  400     //银行卡扫描区宽度
#define BOXHEIGHT 250     //银行卡扫描区高度
#define VIDEOLINEWIDTH 2  //扫描区外围线度宽度
#define VIDEOLABELVER 20  //扫描区外围线条垂直高度
#define TEMPLATIMG "temp.jpg"

class CameraThread : public QThread
{
    Q_OBJECT
public:
    CameraThread(QObject *parent=0);
    CameraThread(const CameraThread &) = delete;
    QRect cardRect();

signals:
    void sigImage(QImage&);
	void sigImage1(QImage&);
    void sigError(QString);

protected:
    void run() override;

private:
    cv::Rect rect;
	Dehaze dehaze;
    QImage toQImage(cv::Mat &);

public:
	int ShowFlag;
};

#endif // CAMERATHREAD_H
