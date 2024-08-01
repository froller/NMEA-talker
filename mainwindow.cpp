#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QSaveFile>
#include <QSettings>

#include "plugin-base.h"
#include "ui_preferencesdialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , preferencesDialog()
    , settingsFileName(QApplication::applicationDirPath() + "/NMEA-talker.ini")
{
    ui->setupUi(this);

    timer.setParent(this);
    timer.setSingleShot(false);
    timer.setInterval(1000);    // 1 sec
    timer.start();

    QSettings settings(settingsFileName, QSettings::IniFormat);
    setPreferencesDialogValues(settings);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_log_item_push(const QString &s)
{
    ui->logTextEdit->appendPlainText(s);
}

void MainWindow::on_action_save_as_triggered()
{
    QFileDialog saveLogAsDialog(this, "Save log as...");
    saveLogAsDialog.setAcceptMode(QFileDialog::AcceptSave);
    saveLogAsDialog.setNameFilters(QStringList() << "Log files (*.log)" << "Text files (*.txt)" << "All files (*.*)");
    if (saveLogAsDialog.exec())
    {
        QSaveFile logFile(saveLogAsDialog.selectedFiles().at(0));
        if (logFile.open(QSaveFile::WriteOnly | QSaveFile::Truncate | QSaveFile::Text))
        {
            logFile.write(ui->logTextEdit->toPlainText().toLatin1());
            logFile.commit();
        }
        else
            qErrnoWarning("Failed to open log file");
    }
}

void MainWindow::on_action_quit_triggered()
{
    close();
}

void MainWindow::addPlugin(PluginBase *plugin, const QString &tabName)
{
    QWidget *tabWidget = new QWidget(ui->constructorTabWidget);
    QVBoxLayout *vBoxLayout = new QVBoxLayout(tabWidget);

    plugin->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    vBoxLayout->addWidget(plugin);

    char buttonName[16];
    snprintf(buttonName, 16, "sendButton%02u", ui->constructorTabWidget->children().length());

    QPushButton *sendButton = new QPushButton("Transmit");
    sendButton->setObjectName(buttonName);
    QObject::connect(sendButton, &QPushButton::clicked, plugin, &PluginBase::onRequestMessage);
    QObject::connect(&timer, &QTimer::timeout,          plugin, &PluginBase::onTick);
    QObject::connect(plugin, &PluginBase::message,      this, &MainWindow::onMessage);
    QObject::connect(plugin, &PluginBase::message,      this->ui->logTextEdit, &QPlainTextEdit::appendPlainText);

    vBoxLayout->addWidget(sendButton);

    ui->constructorTabWidget->addTab(tabWidget, tabName);
}

void MainWindow::onMessage(const QString &s)
{
    qDebug() << s;
}

void MainWindow::on_action_preferences_triggered()
{
    QSettings settings(settingsFileName, QSettings::IniFormat);

    if (preferencesDialog.exec())
    {
        Ui::PreferencesDialog *dialogUi = preferencesDialog.getUi();
        settings.beginGroup("connectivity");
        settings.setValue("device", dialogUi->deviceComboBox->currentText());
        settings.setValue("baudrate", dialogUi->baudrateComboBox->currentText().toUInt());
        settings.setValue("databits", 8 - dialogUi->databitsComboBox->currentIndex());
        settings.setValue("parity", dialogUi->parityComboBox->currentIndex());
        settings.setValue("stopbits", 1 + dialogUi->stopbitsComboBox->currentIndex());
        settings.setValue("flowcontrol", dialogUi->flowControlComboBox->currentIndex());
        settings.endGroup();
    }
    else
    {
        setPreferencesDialogValues(settings);
    }
}

void MainWindow::setPreferencesDialogValues(const QSettings &s)
{
    Ui::PreferencesDialog *dialogUi = preferencesDialog.getUi();
    dialogUi->deviceComboBox->setCurrentText(s.value("connectivity/device").toString());
    dialogUi->baudrateComboBox->setCurrentText(s.value("connectivity/baudrate").toString());
    dialogUi->databitsComboBox->setCurrentIndex(8 - s.value("connectivity/databits").toUInt());
    dialogUi->parityComboBox->setCurrentIndex(s.value("connectivity/parity").toUInt());
    dialogUi->stopbitsComboBox->setCurrentIndex(s.value("connectivity/stopbits").toUInt() - 1);
    dialogUi->flowControlComboBox->setCurrentIndex(s.value("connectivity/flowcontrol").toUInt());
}
