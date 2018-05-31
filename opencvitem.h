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
public:
    OpencvItem(QQuickItem *parent = nullptr);
    ~OpencvItem();
    bool running() const;
    void setRunning(bool);
    QImage curFrame() const;
    QString error() const;

public slots:
    void setCurFrame(QImage&);

protected:
    QSGNode* updatePaintNode(QSGNode *, UpdatePaintNodeData *) override;

private:
    bool _running;
    QImage _img;
    QString _error;
    CameraThread *thr;

private slots:
    void slotError(QString);

signals:
    void runngingChanged();
    void curFrameChanged();
    void errorComing();
};

#endif // OPENCVITEM_H
