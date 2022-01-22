#ifndef BRUSHCLASS_H
#define BRUSHCLASS_H

#include <QStringList>
#include <QPainter>

QStringList brushes();

QRect mousePress(const QString &brush, QPainter &painter, const QPoint &pos);
QRect mouseMove(const QString &brush, QPainter &painter, const QPoint &oldPos, const QPoint &newPos);
QRect mouseRelease(const QString &brush, QPainter &painter, const QPoint &pos);

#endif // BRUSHCLASS_H
