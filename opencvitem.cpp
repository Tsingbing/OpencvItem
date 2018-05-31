#include "opencvitem.h"

OpencvItem::OpencvItem(QQuickItem *parent):
    QQuickItem(parent)
{
    // By default, QQuickItem does not draw anything. If you subclass
    // QQuickItem to create a visual item, you will need to uncomment the
    // following line and re-implement updatePaintNode()

    // setFlag(ItemHasContents, true);

    setFlag(Flag::ItemHasContents,true);
    thr = new CameraThread(this);
    qRegisterMetaType<QImage>("QImage&");
    connect(thr,SIGNAL(sigImage(QImage&)),this,SLOT(setCurFrame(QImage&)));
    connect(thr,SIGNAL(sigError(QString)),this,SLOT(slotError(QString)));
}

OpencvItem::~OpencvItem()
{
    thr->requestInterruption();
        thr->wait();
}

QSGNode* OpencvItem::updatePaintNode(QSGNode *node, UpdatePaintNodeData *)
{
    QSGSimpleTextureNode *texture = static_cast<QSGSimpleTextureNode *>(node);
    if (!texture) {
        texture = new QSGSimpleTextureNode();
    }

    if(_img.isNull()){
        _img = QImage(boundingRect().size().toSize(), QImage::Format_RGB888);
    }

    QSGTexture *sgTexture = window()->createTextureFromImage(_img.scaled(boundingRect().size().toSize()));
    if(sgTexture){
        QSGTexture *tt = texture->texture();
        if(tt){
            tt->deleteLater();
        }
        texture->setRect(boundingRect());
        texture->setTexture(sgTexture);
    }
    return texture;
}

bool OpencvItem::running() const
{
    return _running;
}

void OpencvItem::setRunning(bool flag)
{
    _running = flag;
    if(_running)
        thr->start();
    else{
        thr->requestInterruption();
        thr->wait();
    }
    emit runngingChanged();
}

void OpencvItem::setCurFrame(QImage &imgFrom)
{
    _img = imgFrom;
    update();
    emit curFrameChanged();
}

QImage OpencvItem::curFrame() const
{
    return _img;
}

void OpencvItem::slotError(QString str)
{
    _error = str;
    thr->quit();
    thr->wait();
    emit errorComing();
}

QString OpencvItem::error() const
{
    return _error;
}
