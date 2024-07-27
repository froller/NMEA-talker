#include "plugin-settime.h"
#include "ui_plugin-settime.h"

PluginSetTime::PluginSetTime(QWidget *parent)
    : PluginBase(parent)
    , ui(new Ui::PluginSetTime)
{
    ui->setupUi(this);
}

PluginSetTime::~PluginSetTime()
{
    delete ui;
}

void PluginSetTime::onTabShow(const QString &tabName)
{
    if (tabName == this->objectName())
        ui->dateTimeEdit->setDateTime(QDateTime());
}

void PluginSetTime::onRequestMessage()
{
    PluginBase::onRequestMessage();
    emit messageSent("+PIZMST");
}
