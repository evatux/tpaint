#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <QDialog>
#include <QDir>
#include <QSettings>

QT_BEGIN_NAMESPACE
class QLabel;
class QLineEdit;
class QPushButton;
class QComboBox;
QT_END_NAMESPACE

class OptionsDialog : public QDialog
{
    Q_OBJECT
public:
    OptionsDialog(QWidget *parent = 0);

public:
    QString getScreenShotDir() const { return screenShotDir.path(); }
    void setScreenShotDir(const QDir& dir) { screenShotDir = dir; }

    QString getNickName() const { return nickName; }
    void setNickName(const QString& nick) {nickName = nick; }

public slots:
    void saveSettings();
    void loadSettings();

protected:
    void closeEvent(QCloseEvent* event);

private slots:
    void nickNameChanged(const QString& nick);
    void screenShotDirChanged(const QString& spath);
    void screenShotDirButtonClicked();
    void languageChanged(const QString& lang);

private:
    QDir    screenShotDir;
    QString nickName;
    QString languageFull;

    QSettings settings;

private:
    QLabel      *nickLabel;
    QLineEdit   *nickEdit;

    QLabel      *screenShotDirLabel;
    QLineEdit   *screenShotDirEdit;
    QPushButton *screenShotDirButton;

    QLabel      *languageLabel;
    QComboBox   *languageBox;

    QPushButton *closeButton;
};

#endif // OPTIONSDIALOG_H
