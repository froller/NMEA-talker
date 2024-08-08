#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QPlainTextEdit>
#include <QTimer>
#include <QSerialPort>

#include "plugin-base.h"
#include "preferencesdialog.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
class PreferencesDialog;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void addPlugin(PluginBase *plugin, const QString &tabName);

public:
    void on_log_item_push(const QString &s);

public slots:
    void onMessage(const QString &s);
    void onReadyRead();

private slots:
    void on_action_save_as_triggered();
    void on_action_quit_triggered();

    void on_action_preferences_triggered();

    void on_connectButton_clicked();

private:
    Ui::MainWindow *ui;
    QTimer timer;
    PreferencesDialog preferencesDialog;
    QString settingsFileName;
    QSerialPort *tty;

private:
    void setPreferencesDialogValues(const QSettings &settings);
    bool connectDevice();
    void disconnectDevice();

};
#endif // MAINWINDOW_H
