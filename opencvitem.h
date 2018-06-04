#ifndef OPENCVITEM_H
#define OPENCVITEM_H

#include <QQuickItem>
#include <QImage>
#include "cameraThread/camerathread.h"
#include <QSGSimpleTextureNode>
#include <QQuickWindow>
#include <QSGTexture>
class OpencvItem : public QQuickItem
{
    Q_OBJECT
    Q_DISABLE_COPY(OpencvItem)
    Q_PROPERTY(bool running READ running WRITE setRunning NOTIFY runngingChanged FINAL) //控制是否运行
    Q_PROPERTY(QImage curFrame READ curFrame WRITE setCurFrame NOTIFY curFrameChanged FINAL)//当前帧
    Q_PROPERTY(QString error READ error() NOTIFY errorComing) //错误信息
	Q_PROPERTY(int imageFlag READ imageFlag WRITE setImageFlag NOTIFY imageFlagChanged FINAL) 
	Q_PROPERTY(int showFlag READ showFlag WRITE setShowFlag NOTIFY showFlagChanged FINAL)

public:
    OpencvItem(QQuickItem *parent = nullptr);
    ~OpencvItem();
    bool running() const;
    void setRunning(bool);
    QImage curFrame() const;
	//QImage curFrame1() const;
    QString error() const;
	int imageFlag() const;
	int showFlag() const;
	void setImageFlag(int flag);
	void setShowFlag(int flag);

public slots:
    void setCurFrame(QImage&);
    void setCurFrame1(QImage&);

protected:
    QSGNode* updatePaintNode(QSGNode *, UpdatePaintNodeData *) override;

private:
    bool _running;
    QImage _img;
	QImage _img1;
    QString _error;
    CameraThread *thr;
	QSGNode *nodeP;
	int _imageFlag;
	int _showFlag;

private slots:
    void slotError(QString);

signals:
    void runngingChanged();
    void curFrameChanged();
    void errorComing();
	void imageFlagChanged();
	void showFlagChanged();
};

#endif // OPENCVITEM_H
