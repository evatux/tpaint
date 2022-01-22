#ifndef LOGDIALOG_H
#define LOGDIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
class QLabel;
class QTextEdit;
class QPushButton;
QT_END_NAMESPACE

class LogDialog : public QDialog
{
    Q_OBJECT
public:
    LogDialog(QWidget *parrent = 0);

public slots:
    void debugMessageReceived(const QString& str);

protected:
    void showEvent(QShowEvent *event);
    void closeEvent(QCloseEvent *event);

private:
    QRect       dialogGeometryRect;

    QTextEdit   *debugTextEdit;
    QPushButton *closeButton;
};

#endif // LOGDIALOG_H
