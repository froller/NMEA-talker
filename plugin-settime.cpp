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

void PluginSetTime::onRequestMessage()
{
    PluginBase::onRequestMessage();
    const size_t buffer_len = 48;
    char sentence[buffer_len] = { 0 }; // $PIZMST,HHMMSS.SS,dd,mm,yyyy,ZZ,zz[*CC]
    size_t len = snprintf(sentence, buffer_len, "$PIZMST,%02u%02u%02u,%02u,%02u,%04u,,",
                                                         ui->dateTimeEdit->time().hour(),
                                                             ui->dateTimeEdit->time().minute(),
                                                                 ui->dateTimeEdit->time().second(),
                                                                      ui->dateTimeEdit->date().day(),
                                                                           ui->dateTimeEdit->date().month(),
                                                                                ui->dateTimeEdit->date().year()
                          );
    snprintf(sentence + len, buffer_len - len - 1, "*%02X", NMEAChecksum(sentence));
    emit message(sentence);
}

void PluginSetTime::onTick()
{
    PluginBase::onTick();
    if (ui->checkBox->isChecked())
        ui->dateTimeEdit->setDateTime(QDateTime::currentDateTime());
}

void PluginSetTime::on_checkBox_checkStateChanged(const Qt::CheckState &arg1)
{
    ui->dateTimeEdit->setEnabled(arg1 == Qt::CheckState::Unchecked);
}

