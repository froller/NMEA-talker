#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

#include "plugin-nop.h"
#include "plugin-settime.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "NMEA-talker_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    MainWindow w;

    w.show();

    w.addPlugin(new PluginNop(), "No operation");
    w.addPlugin(new PluginSetTime(), "Set time");

    qDebug() << "***** Пыщь!!!!";

    return a.exec();
}
