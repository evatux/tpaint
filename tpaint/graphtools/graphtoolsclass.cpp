#include "graphtoolsclass.h"

#include <QLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QButtonGroup>
#include <QGroupBox>
#include <QMouseEvent>

GraphToolsClass::GraphToolsClass(QWidget *parent) :
    QWidget(parent)
{
    this->createButtons();
    this->createAdvancedTools();

    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->addWidget(brushButton, 0, 0, Qt::AlignTop);
    gridLayout->addWidget(eraserButton, 0, 1, Qt::AlignTop);
    gridLayout->addWidget(shotButton, 1, 0, Qt::AlignTop);

    gridLayout->setSpacing(2);
    gridLayout->setMargin(2);

    QGroupBox *mainGroupBox = new QGroupBox(tr("Tools"), this);
    mainGroupBox->setLayout(gridLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->setSpacing(0);
    mainLayout->setMargin(2);
    mainLayout->addWidget(mainGroupBox);
    mainLayout->addStretch();
    mainLayout->addWidget(brushAdvancedTool);
    mainLayout->addWidget(eraserAdvancedTool);
    setLayout(mainLayout);
}

void GraphToolsClass::initGraphToolsClass() {
    emit thicknessPenChanged(definedThicknessPen[predefinedNumber/2]);
    emit thicknessEraserChanged(definedThicknessEraser[predefinedNumber/2]);
    emit toolChanged(GraphWidget::Pen);
}

void GraphToolsClass::advancedSlot(const ShapeWidget::ShapeType &tp, const int &sz) {
    if (tp == ShapeWidget::Circle) emit thicknessPenChanged(sz);
    else if (tp == ShapeWidget::Square) emit thicknessEraserChanged(sz);
}

void GraphToolsClass::createButtons() {
    brushButton = new QPushButton();
    brushButton->setIcon(QIcon(":/brush.png"));
    brushButton->setCheckable(true);
    brushButton->setChecked(true);
    brushButton->setFocusPolicy(Qt::NoFocus);
    connect(brushButton, SIGNAL(clicked()), this, SLOT(brushToolClicked()));

    eraserButton = new QPushButton();
    eraserButton->setIcon(QIcon(":/eraser.png"));
    eraserButton->setCheckable(true);
    eraserButton->setFocusPolicy(Qt::NoFocus);
    connect(eraserButton, SIGNAL(clicked()), this, SLOT(eraserToolClicked()));

    shotButton = new QPushButton();
    shotButton->setIcon(QIcon(":/camera.png"));
    shotButton->setCheckable(false);
    shotButton->setFocusPolicy(Qt::NoFocus);
    connect(shotButton, SIGNAL(clicked()), this, SLOT(shotToolClicked()));

    QButtonGroup *buttonGroup = new QButtonGroup();
    buttonGroup->addButton(brushButton);
    buttonGroup->addButton(eraserButton);
    buttonGroup->addButton(shotButton);
}

void GraphToolsClass::createAdvancedTools() {
    brushAdvancedTool = new AdvancedToolOptionsClass(GraphWidget::Pen);
    connect(brushAdvancedTool, SIGNAL(mouseClicked(ShapeWidget::ShapeType,int)), this, SLOT(advancedSlot(ShapeWidget::ShapeType,int)));
    eraserAdvancedTool = new AdvancedToolOptionsClass(GraphWidget::Eraser);
    connect(eraserAdvancedTool, SIGNAL(mouseClicked(ShapeWidget::ShapeType,int)), this, SLOT(advancedSlot(ShapeWidget::ShapeType,int)));
}

void GraphToolsClass::brushToolClicked() {
    emit toolChanged(GraphWidget::Pen);
    brushAdvancedTool->show();
    eraserAdvancedTool->hide();
}

void GraphToolsClass::eraserToolClicked() {
    emit toolChanged(GraphWidget::Eraser);
    brushAdvancedTool->hide();
    eraserAdvancedTool->show();
}

void GraphToolsClass::shotToolClicked() {
    emit shotSignal();
}

ShapeWidget::ShapeWidget(ShapeType tp, int sz, bool __checked, QWidget *parent) : QWidget(parent)
{
    if (__checked) checked = 2;
    else checked = 0;
    this->tp = tp;
    this->sz = sz;
    setFixedSize(sz, sz);
}

void ShapeWidget::changeState(bool final) {
    if (final && (checked == 1)) checked = 2;
    else if (checked == 2) {
        checked = 0;
        update();
    }
}

void ShapeWidget::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setPen(QPen(Qt::color1, 0));

    if (tp == Circle) {
        painter.drawEllipse(0, 0, sz-1, sz-1);
    } else
    if (tp == Square) {
        painter.drawRect(0, 0, sz-1, sz-1);
    }

    if (checked >= 1) {
        painter.setPen(QPen(Qt::blue, 0));
        if (tp == Circle) painter.drawEllipse(0, 0, sz-1, sz-1);
        if (tp == Square) painter.drawRect(0, 0, sz-1, sz-1);
    }
}

void ShapeWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) emit mouseClicked(tp, sz);
    checked = 1;
    update();
    event->ignore();
}

AdvancedToolOptionsClass::AdvancedToolOptionsClass(GraphWidget::ToolType tool, QWidget *parent) :
        QFrame(parent), currentShape(predefinedNumber/2)
{
    this->setLineWidth(1);
    this->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    this->tool = tool;

    customThicknessCheckBox = new QCheckBox("");
    customThicknessCheckBox->setChecked(false);
    customThicknessSpinBox = new QSpinBox();
    customThicknessSpinBox->setRange(2,31);
    customThicknessSpinBox->setValue(10);
    connect(customThicknessCheckBox, SIGNAL(clicked()), this, SLOT(customShapeActivated()));
    connect(customThicknessSpinBox, SIGNAL(valueChanged(int)), this, SLOT(customShapeActivated()));

    QHBoxLayout *customLayout = new QHBoxLayout;
    customLayout->setSpacing(0);
    customLayout->addWidget(customThicknessCheckBox);
    customLayout->addWidget(customThicknessSpinBox);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(2, 4, 2, 4);
    mainLayout->setSpacing(4);
    mainLayout->addLayout(customLayout);

    shWidget = new ShapeWidget* [predefinedNumber];
    if (tool == GraphWidget::Pen) {
        for(int i = predefinedNumber-1; i >= 0; i--) {
            if (i == currentShape) shWidget[i] = new ShapeWidget(ShapeWidget::Circle, definedThicknessPen[i], true);
            else shWidget[i] = new ShapeWidget(ShapeWidget::Circle, definedThicknessPen[i]);
            mainLayout->addWidget(shWidget[i], 0, Qt::AlignCenter);
            connect(shWidget[i], SIGNAL(mouseClicked(ShapeWidget::ShapeType,int)), this, SLOT(shapeClicked()));
            connect(shWidget[i], SIGNAL(mouseClicked(ShapeWidget::ShapeType,int)), this, SIGNAL(mouseClicked(ShapeWidget::ShapeType,int)));
        }
    } else
    if (tool == GraphWidget::Eraser) {
        for(int i = predefinedNumber-1; i >= 0; i--) {
            if (i == currentShape) shWidget[i] = new ShapeWidget(ShapeWidget::Square, definedThicknessEraser[i], true);
            else shWidget[i] = new ShapeWidget(ShapeWidget::Square, definedThicknessEraser[i]);
            mainLayout->addWidget(shWidget[i], 0, Qt::AlignCenter);
            connect(shWidget[i], SIGNAL(mouseClicked(ShapeWidget::ShapeType,int)), this, SLOT(shapeClicked()));
            connect(shWidget[i], SIGNAL(mouseClicked(ShapeWidget::ShapeType,int)), this, SIGNAL(mouseClicked(ShapeWidget::ShapeType,int)));
        }
        hide();
    }
}

void AdvancedToolOptionsClass::mousePressEvent(QMouseEvent*) {
    int profitShape = -1;
    for (int i = 0; i < predefinedNumber; i++)
        if (shWidget[i]->getChecked() == 1) profitShape = i;
    if (profitShape == -1) return;

    for (int i = 0; i < predefinedNumber; i++)
        shWidget[i]->changeState();

    if (profitShape != -1) {
        shWidget[profitShape]->changeState(true);
        currentShape = profitShape;
    }
}

void AdvancedToolOptionsClass::shapeClicked() {
    if (customThicknessCheckBox->checkState() != Qt::Unchecked) customThicknessCheckBox->setChecked(false);
}

void AdvancedToolOptionsClass::customShapeActivated() {
    if (customThicknessCheckBox->checkState() == Qt::Checked) {
        if (this->tool == GraphWidget::Pen) emit mouseClicked(ShapeWidget::Circle, customThicknessSpinBox->value());
        else if (this->tool == GraphWidget::Eraser) emit mouseClicked(ShapeWidget::Square, customThicknessSpinBox->value());
    } else {
        if (this->tool == GraphWidget::Pen) emit mouseClicked(ShapeWidget::Circle, definedThicknessPen[currentShape]);
        else if (this->tool == GraphWidget::Eraser) emit mouseClicked(ShapeWidget::Square, definedThicknessEraser[currentShape]);
    }
}
