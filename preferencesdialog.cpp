#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"

PreferencesDialog::PreferencesDialog(QSettings &s, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::PreferencesDialog)
{
    ui->setupUi(this);
}

PreferencesDialog::~PreferencesDialog()
{
    delete ui;
}

void PreferencesDialog::on_buttonBox_accepted()
{
    settings.setValue("device", ui->deviceComboBox->currentText());
    settings.setValue("baudrate", ui->baudrateComboBox->currentText());
    settings.setValue("databits", 8 - ui->databitsComboBox->currentIndex());
    settings.setValue("parity", ui->parityComboBox->currentIndex());
    settings.setValue("stopbits", 1 + ui->stopbitsComboBox->currentIndex());
    settings.sync();
}

