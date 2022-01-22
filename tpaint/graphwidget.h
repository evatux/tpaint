#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QGraphicsScene>

class QLabel;
class OptionsDialog;

class GraphWidget : public QWidget {
    Q_OBJECT

public:
    GraphWidget(OptionsDialog* __optionDialog, QWidget* parent = 0, int __width = 760, int __height = 480);
    QImage getImage();

    enum ToolType {Pen, Eraser};

signals:
    void imageModified(const QByteArray& arr);

public slots:
    void setImage(QImage);
    void clearImage(bool emitcall = 1);
    void imageUpdate(const QByteArray& arr);
    void loadFromFile(const QString& filename);
    void saveToFile(const QString& filename);
    void sendWholeImage();
    void setTool(const GraphWidget::ToolType& tool);
    void setForegroundColor(const QColor& color) { foregroundColor = color; }
    void setBackgroundColor(const QColor& color) { backgroundColor = color; }
    void setThicknessPen(const int& thick) { thicknessPen = thick; setTool(Pen); }
    void setThicknessEraser(const int& thick) { thicknessEraser = thick; setTool(Eraser); }
    void makeScreenShot();

protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *);

    QRect mouseMove(QPainter &painter, const ToolType& tool, const QColor& color, const int& thick, const QPoint &oldPos, const QPoint &newPos, bool emitcall = 1);

private:
    void setupPainter(QPainter &painter);

    void sendDrawSignal(const QColor& color, const QPoint &oldPos, const QPoint &newPos);
    void sendEraserSignal(const QPoint &oldPos, const QPoint &newPos);
    void sendClearSignal();

private:
    OptionsDialog*  optionDialog;

    int widthField, heightField;

    QImage boardImage;

    ToolType currentToolType;

    QColor foregroundColor;
    QColor backgroundColor;
    int thicknessPen;
    int thicknessEraser;

    QPoint lastPos;
};

#endif // GRAPHWIDGET_H
