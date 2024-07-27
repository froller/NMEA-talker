#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QPlainTextEdit>

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

private slots:
    void on_action_quit_triggered();

    void on_constructorTabWidget_currentChanged(int index);

signals:
    void tabShow(const QString &tabName);

private:
    Ui::MainWindow *ui;

};
#endif // MAINWINDOW_H
