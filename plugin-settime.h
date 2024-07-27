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
    virtual void onTabShow(const QString &tabName) override;

};

#endif // PLUGINSETTIME_H
