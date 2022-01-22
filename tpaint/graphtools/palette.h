#ifndef PALETTE_H
#define PALETTE_H

#include <QWidget>
#include <QPushButton>

class ActiveColorsWidget : public QWidget
{
    Q_OBJECT
public:
    ActiveColorsWidget(QWidget *parent = 0);

protected:
    void paintEvent(QPaintEvent *event);
};

class ColorButton : public QWidget
{
    Q_OBJECT
public:
    ColorButton(QColor color, QWidget *parent = 0);

signals:
    void leftButtonClicked(const QColor&);
    void rightButtonClicked(const QColor&);

protected:
    void mousePressEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);

private:
    QColor color;
};

class Palette : public QWidget
{
    Q_OBJECT
public:
    Palette(QWidget *parent = 0);

    QColor getForegroundColor() const { return foregroundColor; }
    QColor getBackgroundColor() const { return backgroundColor; }

signals:
    void foregroundColorChanged(const QColor&);
    void backgroundColorChanged(const QColor&);

public slots:
    void setForegroundColor(const QColor& color);
    void setBackgroundColor(const QColor& color);

private:
    QColor  foregroundColor;
    QColor  backgroundColor;

    ColorButton *colorList[20];
    ActiveColorsWidget *activeColorsWidget;
};

const QString colorTable[20] = {"#000000", "#808080", "#FF0000", "#FF8000", "#FFFF00",
                                "#00FF00", "#00FFFF", "#0000FF", "#FF00FF", "#80FF80",
                                "#FFFFFF", "#C0C0C0", "#800000", "#804000", "#808000",
                                "#008000", "#008080", "#000080", "#800080", "#FFFF80"};

#endif // PALETTE_H
