#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include <QSettings>

#include "mainform.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("TPaint");
    app.setOrganizationName("HNTeam");

    QSettings settings;
    settings.beginGroup("/Settings");
    QString lang = settings.value("/language", "undef").toString();
    if (lang == "undef") {
        lang = QLocale::system().name().left(2);
        qDebug(lang.toLatin1());
        settings.setValue("/language", lang);
    }
    settings.endGroup();

    QTranslator translator;
    translator.load("../i18n/tpaint_" + lang);
    app.installTranslator(&translator);

    MainForm mform;
    mform.show();

    return app.exec();
}
