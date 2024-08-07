#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"

#include <QtSerialPort/QSerialPort>

PreferencesDialog::PreferencesDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::PreferencesDialog)
{
    ui->setupUi(this);
}

PreferencesDialog::~PreferencesDialog()
{
    delete ui;
}

Ui::PreferencesDialog *PreferencesDialog::getUi()
{
    return ui;
}
