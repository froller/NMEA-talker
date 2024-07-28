#ifndef PLUGINSETTIME_H
#define PLUGINSETTIME_H

#include "plugin-base.h"

namespace Ui {
class PluginSetTime;
}

class PluginSetTime : public PluginBase
{
    Q_OBJECT

public:
    explicit PluginSetTime(QWidget *parent = nullptr);
    virtual ~PluginSetTime();

private:
    Ui::PluginSetTime *ui;

public slots:
    virtual void onRequestMessage() override;
    virtual void onTick() override;

private slots:
    void on_checkBox_checkStateChanged(const Qt::CheckState &arg1);
};

#endif // PLUGINSETTIME_H
