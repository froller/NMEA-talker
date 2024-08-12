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
#include <QTextDocumentFragment>

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

    // Log context menu
    ui->logTextEdit->addAction(ui->action_copy);
    ui->logTextEdit->addAction(ui->action_select_all);
    ui->logTextEdit->addAction(ui->action_clear);

    // Connecting switching of constructor widgets to MainWindow
    QObject::connect(ui->constructorTabWidget, SIGNAL(currentChanged(int)), this, SLOT(onTabChanged(int)));

    // Starting timer for constructor widgets
    timer.setParent(this);
    timer.setSingleShot(false);
    timer.setInterval(1000);    // 1 sec
    timer.start();

    // Reading settings
    QSettings settings(settingsFileName, QSettings::IniFormat);
    setPreferencesDialogValues(settings);

}

MainWindow::~MainWindow()
{
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
    QLayout *tabLayout = new QVBoxLayout();
    tabLayout->addWidget(plugin);
    QWidget *tabWidget = new QWidget(ui->constructorTabWidget);
    tabWidget->setLayout(tabLayout);
    ui->constructorTabWidget->addTab(tabWidget, tabName);

    QPushButton *sendButton = new QPushButton("Transmit");
    sendButton->setObjectName("transmitButton");
    QObject::connect(&timer, SIGNAL(timeout()), plugin, SLOT(onTick()));
    QObject::connect(plugin, SIGNAL(message(QString)), this, SLOT(onMessage(QString)));

}

void MainWindow::onMessage(const QString &s)
{
    if (!tty.isOpen())
    {
        qWarning() << "Sending through closed port";
        return;
    }
    QString msg(s);
    msg += "\r\n";
    if (tty.write(msg.toStdString().c_str()) < 0)
    {
        qCritical() << "Error sending message to" << tty.portName() << ":" << tty.errorString();
        QMessageBox errorMsg(
            QMessageBox::Icon::Critical,
            "Error",
            "Error sending message to " + tty.portName() + ": " + tty.errorString(),
            QMessageBox::StandardButton::Close,
            this
        );
        errorMsg.exec();
        return;
    }
    const QTextCursor &c = ui->logTextEdit->textCursor();
    ui->logTextEdit->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
    ui->logTextEdit->insertPlainText(msg);
    if (c.selection().isEmpty())
        ui->logTextEdit->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
    else
        ui->logTextEdit->setTextCursor(c);
}

void MainWindow::onReadyRead()
{
    const QByteArray &data = tty.readAll();
    const QTextCursor &c = ui->logTextEdit->textCursor();
    ui->logTextEdit->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
    ui->logTextEdit->insertPlainText(data.constData());
    if (c.selection().isEmpty())
        ui->logTextEdit->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
    else
        ui->logTextEdit->setTextCursor(c);
}

void MainWindow::onTabChanged(int index)
{
    qDebug() << "Tab" << index << "selected";
    for (int i = 0; i < ui->constructorTabWidget->count(); ++i)
    {
        QObject *o = ui->constructorTabWidget->widget(i)->children()[1];
        if (i == index)
            QObject::connect(ui->transmitButton, SIGNAL(clicked()), o, SLOT(onRequestMessage()));
        else
            QObject::disconnect(ui->transmitButton, SIGNAL(clicked()), o, SLOT(onRequestMessage()));
    }
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
    dialogUi->databitsComboBox->setCurrentIndex(8 - (s.value("connectivity/databits").toUInt() ? s.value("connectivity/databits").toUInt() : 8));
    dialogUi->parityComboBox->setCurrentIndex(s.value("connectivity/parity").toUInt());
    dialogUi->stopbitsComboBox->setCurrentIndex((s.value("connectivity/stopbits").toUInt() ? s.value("connectivity/stopbits").toUInt() : 1) - 1);
    dialogUi->flowControlComboBox->setCurrentIndex(s.value("connectivity/flowcontrol").toUInt());
}

void MainWindow::on_connectButton_clicked()
{
    if (tty.isOpen())
    {
        ui->statusbar->showMessage(QString("Disconnected from ") + tty.portName(), 5000);
        disconnectDevice();
        ui->connectButton->setText("Connect");
    }
    else if (connectDevice())
    {
        ui->connectButton->setText("Disconnect");
        ui->statusbar->showMessage(QString("Connected to ") + tty.portName(), 0);
    }
    else
    {
        ui->connectButton->setText("Connect");
        ui->statusbar->showMessage("Connection failed", 5000);
    }
}

bool MainWindow::connectDevice()
{
    QSettings settings(settingsFileName, QSettings::IniFormat);

    tty.setPortName(settings.value("connectivity/device").toString());
    tty.setBaudRate(settings.value("connectivity/baudrate").toUInt(), QSerialPort::AllDirections);
    tty.setDataBits(QSerialPort::DataBits(settings.value("connectivity/databits").toUInt()));
    switch (settings.value("connectivity/parity").toUInt())
    {
    case 1:
        tty.setParity(QSerialPort::Parity::OddParity);
        break;
    case 2:
        tty.setParity(QSerialPort::Parity::EvenParity);
        break;
    case 0:
    default:
        tty.setParity(QSerialPort::Parity::NoParity);
    }
    tty.setStopBits(QSerialPort::StopBits(settings.value("connectivity/stopbits").toUInt()));
    tty.setFlowControl(QSerialPort::FlowControl(settings.value("connectivity/flowcontrol").toUInt()));
    if (!tty.open(QSerialPort::ReadWrite))
    {
        qCritical() << "Failed to open" << tty.portName() << ":" << tty.errorString();
        QMessageBox errorMsg(
            QMessageBox::Icon::Critical,
            "Error",
            "Error opening " + tty.portName() + ": " + tty.errorString(),
            QMessageBox::StandardButton::Close,
            this
        );
        errorMsg.exec();
        return false;
    }

    QObject::connect(&tty, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    ui->transmitButton->setEnabled(true);
    qInfo() << "Connected to" << tty.portName();
    return true;
}

void MainWindow::disconnectDevice()
{
    qInfo() << "Disconnected from" << tty.portName();
    tty.close();
    QObject::disconnect(&tty, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    ui->transmitButton->setEnabled(false);
}

