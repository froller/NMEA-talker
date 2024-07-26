#include "plugin-settime.h"
#include "ui_plugin-settime.h"

PluginSetTime::PluginSetTime(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PluginSetTime)
{
    ui->setupUi(this);
}

PluginSetTime::~PluginSetTime()
{
    delete ui;
}

void PluginSetTime::onRequestMessage()
{
    qDebug() << "Message requested";
}
