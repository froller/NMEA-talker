#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QVBoxLayout>
#include <QPushButton>

#include "plugin-base.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    timer.setParent(this);
    timer.setSingleShot(false);
    timer.setInterval(1000);    // 1 sec
    timer.start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_log_item_push(const QString &s)
{
    ui->logTextEdit->appendPlainText(s);
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
