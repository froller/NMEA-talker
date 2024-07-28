#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QPlainTextEdit>
#include <QTimer>

#include "plugin-base.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
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

private slots:
    void on_action_quit_triggered();

private:
    Ui::MainWindow *ui;
    QTimer timer;

};
#endif // MAINWINDOW_H
