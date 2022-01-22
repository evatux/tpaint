#include <QDebug>
#include <QEvent>
#include <QLabel>
#include <QMouseEvent>
#include <QBuffer>
#include <QBitmap>
#include <QCursor>
#include <QDateTime>
#include "mainform.h"
#include "graphwidget.h"
#include "optionsdialog.h"

GraphWidget::GraphWidget(OptionsDialog* __optionDialog, QWidget *parent, int __width, int __height) :
        QWidget(parent), optionDialog(__optionDialog),
        widthField(__width), heightField(__height),
        boardImage(widthField, heightField, QImage::Format_RGB32),
        lastPos(-1, -1)
{
    setAttribute(Qt::WA_StaticContents);
    setAttribute(Qt::WA_OpaquePaintEvent); // WA_NoBackground

    boardImage.fill(qRgb(255, 255, 255));

    setMinimumSize(widthField, heightField);
    setMaximumSize(widthField, heightField);

    foregroundColor = Qt::black;
    backgroundColor = Qt::white,

    thicknessPen = 20;
    thicknessEraser = 10;

    setTool(Pen);
}

QImage GraphWidget::getImage() {
    QImage image(boardImage);
    return image;
}

void GraphWidget::setImage(QImage image) {
    boardImage = image;
    update();
}

void GraphWidget::clearImage(bool emitcall) {
    boardImage.fill(qRgb(255, 255, 255));
    update();

    if (emitcall) sendClearSignal();
}

void GraphWidget::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.drawImage(QPoint(0, 0), boardImage);
    painter.setPen(QPen(Qt::black));
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(QRect(0, 0, width() - 1, height() - 1));
}

void GraphWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->buttons() & (Qt::LeftButton | Qt::RightButton)) {
        QPainter painter(&boardImage);
        setupPainter(painter);
        QRect rect;
        if (currentToolType == Pen) {
            if (event->buttons() & Qt::LeftButton) rect = mouseMove(painter, currentToolType, foregroundColor, thicknessPen, event->pos(), event->pos());
            if (event->buttons() & Qt::RightButton) rect = mouseMove(painter, currentToolType, backgroundColor, thicknessPen, event->pos(), event->pos());
        } else
        if (currentToolType == Eraser && (event->buttons() & Qt::LeftButton)) rect = mouseMove(painter, currentToolType, backgroundColor, thicknessEraser, event->pos(), event->pos());
        update(rect);
    }
    lastPos = event->pos();
}

void GraphWidget::mouseMoveEvent(QMouseEvent *event)
{
    if ((event->buttons() & (Qt::LeftButton | Qt::RightButton)) && lastPos != QPoint(-1, -1)) {
        QPainter painter(&boardImage);
        setupPainter(painter);
        QRect rect;
        if (currentToolType == Pen) {
            if (event->buttons() & Qt::LeftButton) rect = mouseMove(painter, currentToolType, foregroundColor, thicknessPen, lastPos, event->pos());
            if (event->buttons() & Qt::RightButton) rect = mouseMove(painter, currentToolType, backgroundColor, thicknessPen, lastPos, event->pos());
        } else
        if (currentToolType == Eraser) {
            if (event->buttons() & Qt::LeftButton) rect = mouseMove(painter, currentToolType, backgroundColor, thicknessEraser, lastPos, event->pos());
        }
        update(rect);
    }
    lastPos = event->pos();
}

void GraphWidget::mouseReleaseEvent(QMouseEvent *)
{
    lastPos = QPoint(-1, -1);
}

void GraphWidget::setupPainter(QPainter &painter)
{
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QPen(foregroundColor, thicknessPen, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
}

QRect GraphWidget::mouseMove(QPainter &painter, const ToolType& tool, const QColor& color, const int& thick, const QPoint &oldPos, const QPoint &newPos, bool emitcall)
{
    painter.save();

    int rad = thick;
    QRect boundingRect = QRect(oldPos, newPos).normalized().adjusted(-rad, -rad, +rad, +rad);

    if (tool == Pen) painter.setPen(QPen(color, thick, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    else if (tool == Eraser) painter.setPen(QPen(color, thick, Qt::SolidLine, Qt::SquareCap, Qt::BevelJoin));

    if (oldPos == newPos) painter.drawPoint(newPos);
    else painter.drawLine(oldPos, newPos);

    if (emitcall) {
        if (tool == Pen) sendDrawSignal(color, oldPos, newPos);
        else if (tool == Eraser) sendEraserSignal(oldPos, newPos);
    }

    painter.restore();
    return boundingRect;
}

void GraphWidget::sendDrawSignal(const QColor& color, const QPoint &oldPos, const QPoint &newPos) {
    QByteArray  arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_7);
    out << QString("DRAW") << color << thicknessPen << oldPos << newPos;

    emit imageModified(arrBlock);
}

void GraphWidget::sendEraserSignal(const QPoint &oldPos, const QPoint &newPos) {
    QByteArray  arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_7);
    out << QString("ERASER") << backgroundColor << thicknessEraser << oldPos << newPos;

    emit imageModified(arrBlock);
}

void GraphWidget::sendClearSignal() {
    QByteArray  arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_7);
    out << QString("CLEAR");

    emit imageModified(arrBlock);
}

void GraphWidget::imageUpdate(const QByteArray& arr) {
    QByteArray arrBlock(arr);
    QDataStream in(&arrBlock, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_4_7);
    QString protoStr;
    in >> protoStr;

    if (protoStr == "DRAW") {
        QColor color;
        int thickness;
        QPoint oldPos;
        QPoint newPos;
        in >> color >> thickness >> oldPos >> newPos;
        QPainter painter(&boardImage);
        setupPainter(painter);

        const QRect rect = mouseMove(painter, Pen, color, thickness, oldPos, newPos, false);
        update(rect);
    } else
    if (protoStr == "ERASER") {
        QColor color;
        int thickness;
        QPoint oldPos;
        QPoint newPos;
        in >> color >> thickness >> oldPos >> newPos;
        QPainter painter(&boardImage);
        setupPainter(painter);

        const QRect rect = mouseMove(painter, Eraser, color, thickness, oldPos, newPos, false);
        update(rect);
    } else
    if (protoStr == "CLEAR") {
        this->clearImage(false);
    } else
    if (protoStr == "IMAGE") {
        QByteArray ba;
        in >> ba;
        boardImage.loadFromData(qUncompress(ba));
        update();
    } else {
        qDebug() << "unknown protoStr: '" + protoStr + "'. maybe the version of Tpaint is out of date. Please update it";
    }
}

void GraphWidget::loadFromFile(const QString &filename) {
    if (boardImage.load(filename)) update();
    else qDebug("image cannot be loaded!");
}

void GraphWidget::saveToFile(const QString &filename) {
    if (!boardImage.save(filename)) qDebug("image cannot be saved!");
}

void GraphWidget::sendWholeImage() {
    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);
    boardImage.save(&buffer, "PNG");

    QByteArray  arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_7);
    out << QString("IMAGE") << qCompress(ba);

    emit imageModified(arrBlock);
}

void GraphWidget::setTool(const GraphWidget::ToolType& tool) {
    currentToolType = tool;
    if (currentToolType == Pen) {
        QBitmap penPix(32, 32);
        QBitmap maskPix(32, 32);
        penPix.clear();
        maskPix.clear();
        QPainter painter(&penPix);
        painter.setPen(QPen(Qt::color1, thicknessPen, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter.drawPoint(15,15);
        painter.setPen(QPen(Qt::color0, thicknessPen-2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter.drawPoint(15,15);
        painter.end();

        painter.begin(&maskPix);
        painter.fillRect(0,0,32,32, Qt::color0);
        painter.setPen(QPen(Qt::color1, thicknessPen, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter.drawPoint(15,15);
        painter.setPen(QPen(Qt::color0, thicknessPen-2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter.drawPoint(15,15);

        QCursor penCursor(penPix, maskPix);
        setCursor(penCursor);
    } else
    if (currentToolType == Eraser) {
        QPixmap eraserPix(thicknessEraser+1, thicknessEraser+1); // Danger!!!
        QPainter painter(&eraserPix);
        QPolygon polygon;
        painter.setBrush(QBrush(Qt::white, Qt::SolidPattern));
        painter.drawRect(0, 0, thicknessEraser, thicknessEraser);
        painter.drawPolygon(polygon);
        QCursor eraserCursor(eraserPix);
        setCursor(eraserCursor);
    }
}

void GraphWidget::makeScreenShot() {
    QString str = optionDialog->getScreenShotDir() + QDir::separator()
                  + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + ".png";
    qDebug((QString("Screenshot is made: ") + str).toLatin1());
    boardImage.save(str);
}
