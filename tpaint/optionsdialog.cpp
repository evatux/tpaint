#include "optionsdialog.h"

#include <QSettings>
#include <QHostInfo>
#include <QCoreApplication>
#include <QFileDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QLayout>
#include <QCloseEvent>

OptionsDialog::OptionsDialog(QWidget *parent) :
    QDialog(parent)
{
    this->loadSettings();

    setWindowTitle(tr("Options"));
    setWindowFlags(Qt::Dialog);
    setWindowModality(Qt::ApplicationModal);

    nickLabel = new QLabel(tr("Nick: "));
    nickEdit = new QLineEdit(nickName);
    nickLabel->setBuddy(nickEdit);
    connect(nickEdit, SIGNAL(textChanged(QString)), this, SLOT(nickNameChanged(QString)));

    screenShotDirLabel = new QLabel(tr("Screenshot directory: "));
    screenShotDirEdit = new QLineEdit(screenShotDir.path());
    screenShotDirButton = new QPushButton("...");
    screenShotDirLabel->setBuddy(screenShotDirEdit);
    connect(screenShotDirButton, SIGNAL(clicked()), this, SLOT(screenShotDirButtonClicked()));
    connect(screenShotDirEdit, SIGNAL(textChanged(QString)), this, SLOT(screenShotDirChanged(QString)));

    languageLabel = new QLabel(tr("<html>Language <sup>(restart is needed)</sup>:</html>"));
    languageBox = new QComboBox;
    languageBox->addItem("English");
    languageBox->addItem("Russian");
    languageBox->setCurrentIndex(languageBox->findText(languageFull));
    connect(languageBox, SIGNAL(activated(QString)), this, SLOT(languageChanged(QString)));

    closeButton = new QPushButton(tr("Close"));
    connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));

    QPushButton *saveButton = new QPushButton(tr("Save"));
    connect(saveButton, SIGNAL(clicked()), this, SLOT(saveSettings()));

    QHBoxLayout *nickLayout = new QHBoxLayout;
    nickLayout->addWidget(nickLabel);
    nickLayout->addWidget(nickEdit);

    QHBoxLayout *screenShotLayout = new QHBoxLayout;
    screenShotLayout->addWidget(screenShotDirLabel);
    screenShotLayout->addWidget(screenShotDirEdit);
    screenShotLayout->addWidget(screenShotDirButton);

    QHBoxLayout *languageLayout = new QHBoxLayout;
    languageLayout->addWidget(languageLabel);
    languageLayout->addWidget(languageBox);

    QHBoxLayout *closeLayout = new QHBoxLayout;
    closeLayout->addStretch();
    closeLayout->addWidget(saveButton);
    closeLayout->addWidget(closeButton);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(nickLayout);
    mainLayout->addLayout(screenShotLayout);
    mainLayout->addLayout(languageLayout);
    mainLayout->addLayout(closeLayout);
}

void OptionsDialog::saveSettings() {
    settings.beginGroup("/Settings");

    settings.setValue("/screenShotDir", screenShotDir.path());
    settings.setValue("/nickName", nickName);
    if (languageBox->currentText() == "English")
        settings.setValue("/language", "en");
    else if (languageBox->currentText() == "Russian")
        settings.setValue("/language", "ru");

    if (parent()) {
        QWidget *pw = qobject_cast<QWidget*>(parent());

        settings.setValue("/width", pw->width());
        settings.setValue("/height", pw->height());
        settings.setValue("/pos_x", pw->x());
        settings.setValue("/pos_y", pw->y());

//      settings.setValue("pos", pw->pos());   ????
//      settings.setValue("size", pw->size()); ????
    }

    settings.endGroup();
}

void OptionsDialog::loadSettings() {
    settings.beginGroup("/Settings");

    screenShotDir = QDir(settings.value("/screenShotDir", QCoreApplication::applicationDirPath() + QDir::separator() + QString("tpaint")).toString());
    nickName = settings.value("/nickName", QHostInfo::localHostName()).toString();

    QString lang = settings.value("/language").toString();
    if (lang == "en") languageFull = "English";
    else if (lang == "ru") languageFull = "Russian";
    else {
        settings.setValue("/language", "en");
        lang = "en";
    }

    if (parent()) {
        QWidget *pw = qobject_cast<QWidget*>(parent());
        int w = settings.value("/width", pw->width()).toInt();
        int h = settings.value("/height", pw->height()).toInt();
        int _x = settings.value("/pos_x", 0).toInt();
        int _y = settings.value("/pos_y", 0).toInt();

        pw->move(_x, _y);
        pw->resize(w, h);
    }

    settings.endGroup();
}

void OptionsDialog::closeEvent(QCloseEvent *event) {
    this->saveSettings();
    event->accept();
}

void OptionsDialog::nickNameChanged(const QString &nick) {
    nickName = nick;
}

void OptionsDialog::screenShotDirChanged(const QString &spath) {
    screenShotDir = QDir(spath);
}

void OptionsDialog::screenShotDirButtonClicked() {
    QString spath = QFileDialog::getExistingDirectory(this, tr("Select Directory for tpaint screenshots"),
                        screenShotDir.path(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    screenShotDirEdit->setText(spath);
}

void OptionsDialog::languageChanged(const QString&) {
    //  Dinamic translation
}
