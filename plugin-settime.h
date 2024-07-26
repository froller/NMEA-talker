#ifndef PLUGINSETTIME_H
#define PLUGINSETTIME_H

#include <QWidget>

namespace Ui {
class PluginSetTime;
}

class PluginSetTime : public QWidget
{
    Q_OBJECT

public:
    explicit PluginSetTime(QWidget *parent = nullptr);
    ~PluginSetTime();

private:
    Ui::PluginSetTime *ui;
};

#endif // PLUGINSETTIME_H
