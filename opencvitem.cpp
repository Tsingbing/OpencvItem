#include "opencvitem.h"
#include <QDebug>
OpencvItem::OpencvItem(QQuickItem *parent):
    QQuickItem(parent)
	, _imageFlag(0)
{
    // By default, QQuickItem does not draw anything. If you subclass
    // QQuickItem to create a visual item, you will need to uncomment the
    // following line and re-implement updatePaintNode()

    // setFlag(ItemHasContents, true);

    setFlag(Flag::ItemHasContents,true);
    thr = new CameraThread(this);
    qRegisterMetaType<QImage>("QImage&");
	connect(thr, SIGNAL(sigImage(QImage&)), this, SLOT(setCurFrame(QImage&)));
	connect(thr,SIGNAL(sigImage1(QImage&)),this,SLOT(setCurFrame1(QImage&)));
    connect(thr,SIGNAL(sigError(QString)),this,SLOT(slotError(QString)));
}

OpencvItem::~OpencvItem()
{
    thr->requestInterruption();
        thr->wait();
}

void OpencvItem::setCurFrame1(QImage &image)
{
	_img1 = image;
}

QSGNode* OpencvItem::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *)
{	
	//if (!oldNode) {
	//		nodeP = new QSGNode;
	//	}
	//	else {
	//		nodeP = static_cast<QSGNode *>(oldNode);
	//	}
	//if (_imageFlag == 2)
	//{
	//	thr->ShowFlag = 2;
	//	nodeP->removeAllChildNodes();
	//	/*   QSGSimpleTextureNode *texture = static_cast<QSGSimpleTextureNode *>(node);
	//	if (!texture) {
	//	texture = new QSGSimpleTextureNode();
	//	}*/

	//	QSGSimpleTextureNode *texture = 0;
	//	if (!texture) {
	//		texture = new QSGSimpleTextureNode();
	//	}
	//	if (_img.isNull()) {
	//		_img = QImage(boundingRect().size().toSize(), QImage::Format_RGB888);
	//	}

	//	QSGTexture *sgTexture = window()->createTextureFromImage(_img.scaled(boundingRect().size().toSize()));
	//	if (sgTexture) {
	//		QSGTexture *tt = texture->texture();
	//		if (tt) {
	//			tt->deleteLater();
	//		}
	//		texture->setRect(boundingRect());
	//		texture->setTexture(sgTexture);
	//	}
	//	nodeP->appendChildNode(texture);

	//	//////////////////////////////////////////////////
	//	if (_imageFlag == 2)
	//	{
	//		//QSGSimpleTextureNode *texture1 = 0;
	//		//if (!texture1) {
	//		//	texture1 = new QSGSimpleTextureNode();
	//		//}
	//		//if (_img1.isNull()) {
	//		//_img1 = QImage(boundingRect().size().toSize(), QImage::Format_RGB888);
	//		//}

	//		//QSGTexture *sgTexture1 = window()->createTextureFromImage(_img1.scaled(boundingRect().size().toSize()));
	//		//if (sgTexture1) {
	//		//	QSGTexture *tt1 = texture1->texture();
	//		//	if (tt1) {
	//		//		tt1->deleteLater();
	//		//	}
	//		//	texture1->setRect(500, 500, 500, 500);
	//		//	texture1->setTexture(sgTexture1);
	//		//}
	//		////nodeP->markDirty(QSGNode::DirtyGeometry);
	//		//nodeP->appendChildNode(texture1);
	//	}

	//	///////////////////////////////////

	//	qDebug() << nodeP->childCount() << endl;
	//	return nodeP;
	//}
	//if (_imageFlag == 0)
	//{
	//	thr->ShowFlag = 0;
	//	nodeP->removeAllChildNodes();
	//	QSGSimpleTextureNode *texture = static_cast<QSGSimpleTextureNode *>(oldNode);
	//	if (!texture) {
	//		texture = new QSGSimpleTextureNode();
	//	}

	//	if (_img.isNull()) {
	//		_img = QImage(boundingRect().size().toSize(), QImage::Format_RGB888);
	//	}

	//	QSGTexture *sgTexture = window()->createTextureFromImage(_img.scaled(boundingRect().size().toSize()));
	//	if (sgTexture) {
	//		QSGTexture *tt = texture->texture();
	//		if (tt) {
	//			tt->deleteLater();
	//		}
	//		texture->setRect(boundingRect());
	//		texture->setTexture(sgTexture);
	//	}
	//	return texture;
	//}

	//return texture;

	if (_imageFlag == 2)
	{
		thr->ShowFlag = 2;
	}
	if (_imageFlag == 0)
	{
		thr->ShowFlag = 0;
	}

	QSGSimpleTextureNode *texture = static_cast<QSGSimpleTextureNode *>(oldNode);
	if (!texture) {
		texture = new QSGSimpleTextureNode();
	}

	if (_img.isNull()) {
		_img = QImage(boundingRect().size().toSize(), QImage::Format_RGB888);
	}

	QSGTexture *sgTexture = window()->createTextureFromImage(_img.scaled(boundingRect().size().toSize()));
	if (sgTexture) {
		QSGTexture *tt = texture->texture();
		if (tt) {
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

int OpencvItem::imageFlag() const
{
	return _imageFlag;
}

int OpencvItem::showFlag() const
{
	return _showFlag;
}

void OpencvItem::setImageFlag(int flag)
{
	_imageFlag = flag;
	
	emit imageFlagChanged();
}

void OpencvItem::setShowFlag(int flag)
{
	_showFlag = flag;
	emit imageFlagChanged();
}
