#ifndef GRAPHTOOLSCLASS_H
#define GRAPHTOOLSCLASS_H

#include <QWidget>
#include <QSpinBox>
#include <QCheckBox>
#include <QFrame>

#include "graphwidget.h"

QT_BEGIN_NAMESPACE
class QPushButton;
QT_END_NAMESPACE

class ShapeWidget : public QWidget
{
    Q_OBJECT
public:
    enum ShapeType {Circle, Square};

    ShapeWidget(ShapeType tp, int sz, bool __checked = false, QWidget *parent = 0);

    void setChecked(int checked) { this->checked = checked; }
    void changeState(bool final = false);
    int getChecked() { return checked; }

protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);

signals:
    void mouseClicked(const ShapeWidget::ShapeType& tp, const int& sz);

private:
    void clearShape();

private:
    ShapeType tp;
    int sz;
    int checked; // 0 - unchecked, 1 - newly checked, 2 - prev. checked
};

class AdvancedToolOptionsClass : public QFrame
{
    Q_OBJECT
public:
    AdvancedToolOptionsClass(GraphWidget::ToolType tool, QWidget *parent = 0);

protected:
    void mousePressEvent(QMouseEvent *event);

signals:
    void mouseClicked(const ShapeWidget::ShapeType& tp, const int& sz);

private slots:
    void shapeClicked();
    void customShapeActivated();

private:
    GraphWidget::ToolType tool;
    int         currentShape;
    QCheckBox   *customThicknessCheckBox;
    QSpinBox    *customThicknessSpinBox;
    ShapeWidget** shWidget;
};

class GraphToolsClass : public QWidget
{
    Q_OBJECT
public:
    GraphToolsClass(QWidget *parent = 0);

    void initGraphToolsClass();

signals:
    void toolChanged(const GraphWidget::ToolType& tool);
    void thicknessPenChanged(const int& sz);
    void thicknessEraserChanged(const int& sz);
    void shotSignal();

private slots:
    void advancedSlot(const ShapeWidget::ShapeType& tp, const int& sz);

private slots:
    void brushToolClicked();
    void eraserToolClicked();
    void shotToolClicked();

private:
    void createButtons();
    void createAdvancedTools();

private:
    QPushButton     *brushButton;
    QPushButton     *eraserButton;
    QPushButton     *shotButton;

    AdvancedToolOptionsClass *brushAdvancedTool;
    AdvancedToolOptionsClass *eraserAdvancedTool;
};

const int predefinedNumber = 5;
const int definedThicknessPen[predefinedNumber] = {3, 5, 10, 15, 20};
const int definedThicknessEraser[predefinedNumber] = {6, 10, 15, 20, 25};

#endif // GRAPHTOOLSCLASS_H
