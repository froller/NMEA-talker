#ifndef PLUGINNOP_H
#define PLUGINNOP_H

#include "plugin-base.h"

namespace Ui {
class PluginNop;
}

class PluginNop : public PluginBase
{
    Q_OBJECT

public:
    explicit PluginNop(QWidget *parent = nullptr);
    virtual ~PluginNop();

private:
    Ui::PluginNop *ui;

public slots:
    virtual void onRequestMessage();

};

#endif // PLUGINNOP_H
