#include "palette.h"
#include <QLayout>
#include <QGroupBox>
#include <QPainter>
#include <QMouseEvent>

Palette::Palette(QWidget *parent) :
        QWidget(parent),
        foregroundColor(Qt::black),
        backgroundColor(Qt::white)
{
    for (int i = 0; i < 20; i++) {
        colorList[i] = new ColorButton(QColor(colorTable[i]));
        connect(colorList[i], SIGNAL(leftButtonClicked(QColor)), this, SLOT(setForegroundColor(QColor)));
        connect(colorList[i], SIGNAL(rightButtonClicked(QColor)), this, SLOT(setBackgroundColor(QColor)));
    }

    QHBoxLayout *topLayout = new QHBoxLayout;
    for (int i = 0; i < 10; i++)
        topLayout->addWidget(colorList[i]);
    topLayout->setSpacing(2);
    topLayout->setSizeConstraint(QLayout::SetFixedSize);
    QHBoxLayout *bottomLayout = new QHBoxLayout;
    for (int i = 10; i < 20; i++)
        bottomLayout->addWidget(colorList[i]);
    bottomLayout->setSpacing(2);
    bottomLayout->setSizeConstraint(QLayout::SetFixedSize);
    QVBoxLayout *colorLayout = new QVBoxLayout;
    colorLayout->addLayout(topLayout);
    colorLayout->addLayout(bottomLayout);
    colorLayout->setSpacing(2);
    colorLayout->setSizeConstraint(QLayout::SetFixedSize);
    colorLayout->setContentsMargins(2, 2, 2, 2);

    QGroupBox *colorsGroupBox = new QGroupBox;
    colorsGroupBox->setLayout(colorLayout);

    activeColorsWidget = new ActiveColorsWidget(this);

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(activeColorsWidget);
    mainLayout->addSpacing(50);
    mainLayout->addWidget(colorsGroupBox);
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
}

void Palette::setForegroundColor(const QColor &color) {
    foregroundColor = color;
    activeColorsWidget->update();
    emit foregroundColorChanged(color);
}

void Palette::setBackgroundColor(const QColor &color) {
    backgroundColor = color;
    activeColorsWidget->update();
    emit backgroundColorChanged(color);
}

ActiveColorsWidget::ActiveColorsWidget(QWidget *parent) : QWidget(parent)
{
    this->setFixedSize(48, 48);
}

void ActiveColorsWidget::paintEvent(QPaintEvent */*event*/) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QPen(Qt::black));

    QColor f = (qobject_cast<Palette*>(parent()))->getForegroundColor();
    QColor b = (qobject_cast<Palette*>(parent()))->getBackgroundColor();

    painter.setBrush(QBrush(f, Qt::SolidPattern));
    QPolygon polygon;
    polygon << QPoint(0,0) << QPoint(48, 0) << QPoint(0, 48);
    painter.drawPolygon(polygon);

    painter.setBrush(QBrush(b, Qt::SolidPattern));
    polygon.clear();
    polygon << QPoint(48,48) << QPoint(48, 0) << QPoint(0, 48);
    painter.drawPolygon(polygon);
}

ColorButton::ColorButton(QColor __color, QWidget *parent) :
        QWidget(parent), color(__color)
{
    this->setFixedSize(24, 24);
}

void ColorButton::mousePressEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) emit leftButtonClicked(color);
    if (event->buttons() & Qt::RightButton) emit rightButtonClicked(color);
}

void ColorButton::paintEvent(QPaintEvent */*event*/) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QPen(Qt::black));

    painter.setBrush(QBrush(color, Qt::SolidPattern));
    painter.drawRect(0, 0, 24, 24);
}
