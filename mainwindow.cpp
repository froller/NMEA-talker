#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QSaveFile>
#include <QSettings>
#include <QMessageBox>
#include <QSerialPortInfo>

#include "plugin-base.h"
#include "ui_preferencesdialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , preferencesDialog()
    , settingsFileName(QApplication::applicationDirPath() + "/NMEA-talker.ini")
    , tty(nullptr)
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
    if (tty)
        disconnectDevice();
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
        {
            int e = errno;
            qErrnoWarning(e, "Failed to save log file");
            QMessageBox errorMessage;
            errorMessage.setIcon(QMessageBox::Critical);
            errorMessage.setText(tr("Error saving log file"));
            errorMessage.setInformativeText(strerror(e));
            errorMessage.exec();
        }
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
    QObject::connect(sendButton, SIGNAL(clicked()), plugin, SLOT(onRequestMessage()));
    QObject::connect(&timer, SIGNAL(timeout()), plugin, SLOT(onTick()));
    QObject::connect(plugin, SIGNAL(message(QString)), this, SLOT(onMessage(QString)));

    vBoxLayout->addWidget(sendButton);

    ui->constructorTabWidget->addTab(tabWidget, tabName);
}

void MainWindow::onMessage(const QString &s)
{
    tty->write(s.toStdString().c_str());
    ui->logTextEdit->moveCursor(QTextCursor::End, QTextCursor::KeepAnchor);
    ui->logTextEdit->insertPlainText(s + "\n");
    ui->logTextEdit->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
}

void MainWindow::onReadyRead()
{
    const QByteArray &data = tty->readAll();
    ui->logTextEdit->moveCursor(QTextCursor::End, QTextCursor::KeepAnchor);
    ui->logTextEdit->insertPlainText(data.constData());
    ui->logTextEdit->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
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
    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();

    Ui::PreferencesDialog *dialogUi = preferencesDialog.getUi();
    dialogUi->deviceComboBox->clear();
    for (const QSerialPortInfo &p : ports)
        dialogUi->deviceComboBox->addItem(p.portName(), p.portName());

    dialogUi->deviceComboBox->setCurrentText(s.value("connectivity/device").toString());
    dialogUi->baudrateComboBox->setCurrentText(s.value("connectivity/baudrate").toString());
    dialogUi->databitsComboBox->setCurrentIndex(8 - s.value("connectivity/databits").toUInt());
    dialogUi->parityComboBox->setCurrentIndex(s.value("connectivity/parity").toUInt());
    dialogUi->stopbitsComboBox->setCurrentIndex(s.value("connectivity/stopbits").toUInt() - 1);
    dialogUi->flowControlComboBox->setCurrentIndex(s.value("connectivity/flowcontrol").toUInt());
}

void MainWindow::on_connectButton_clicked()
{
    if (tty)
    {
        ui->statusbar->showMessage(QString("Disconnected from ") + tty->portName(), 5000);
        disconnectDevice();
        ui->connectButton->setText("Connect");
        ui->logTextEdit->setEnabled(false);
    }
    else if (connectDevice())
    {
        ui->connectButton->setText("Disconnect");
        ui->statusbar->showMessage(QString("Connected to ") + tty->portName(), 0);
        ui->logTextEdit->setEnabled(true);
    }
    else
    {
        ui->connectButton->setText("Connect");
        ui->statusbar->showMessage("Connection failed", 5000);
        ui->logTextEdit->setEnabled(false);
    }
}

bool MainWindow::connectDevice()
{
    QSettings settings(settingsFileName, QSettings::IniFormat);

    tty = new QSerialPort(settings.value("connectivity/device").toString());
    if (!tty)
    {
        qCritical() << "Failed to open" << settings.value("connectivity/device").toString().toUtf8();
        return false;
    }

    tty->setBaudRate(settings.value("connectivity/baudrate").toUInt(), QSerialPort::AllDirections);
    tty->setDataBits(QSerialPort::DataBits(settings.value("connectivity/databits").toUInt()));
    switch (settings.value("connectivity/parity").toUInt())
    {
    case 1:
        tty->setParity(QSerialPort::Parity::OddParity);
        break;
    case 2:
        tty->setParity(QSerialPort::Parity::EvenParity);
        break;
    case 0:
    default:
        tty->setParity(QSerialPort::Parity::NoParity);
    }
    tty->setStopBits(QSerialPort::StopBits(settings.value("connectivity/stopbits").toUInt()));
    tty->setFlowControl(QSerialPort::FlowControl(settings.value("connectivity/flowcontrol").toUInt()));
    if (!tty->open(QSerialPort::ReadWrite))
    {
        qCritical() << "Failed to open" << settings.value("connectivity/device").toString().toUtf8() << ":" << tty->error();
        delete tty;
        tty = nullptr;
        return false;
    }

    QObject::connect(tty, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    qInfo() << "Connected to" << tty->portName();
    return true;
}

void MainWindow::disconnectDevice()
{
    qInfo() << "Disconnected from" << tty->portName();
    if (tty->isOpen())
        tty->close();
    QObject::disconnect(tty, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    delete tty;
    tty = nullptr;
}

