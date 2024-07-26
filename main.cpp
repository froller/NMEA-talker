#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

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

    QWidget *pluginSetTime = new PluginSetTime();
    w.add_constructor_tab(pluginSetTime, "Set time");

    return a.exec();
}
