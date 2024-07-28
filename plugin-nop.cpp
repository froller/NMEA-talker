#include "plugin-nop.h"
#include "ui_plugin-nop.h"

PluginNop::PluginNop(QWidget *parent)
    : PluginBase(parent)
    , ui(new Ui::PluginNop)
{
    ui->setupUi(this);
}

PluginNop::~PluginNop()
{
    delete ui;
}

void PluginNop::onRequestMessage()
{
    PluginBase::onRequestMessage();
    char sentence[16] = "$PIZMNOP";
    snprintf(&sentence[8], 7, "*%02X", NMEAChecksum(sentence));
    emit message(sentence);
}
