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

    constructorTabWidget = findChild<QTabWidget *>("constructorTabWidget", Qt::FindChildrenRecursively);
    logTextEdit = findChild<QPlainTextEdit *>("logTextEdit", Qt::FindChildrenRecursively);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_log_item_push(const QString &s)
{
    logTextEdit->appendPlainText(s);
}

void MainWindow::on_action_quit_triggered()
{
    close();
}

void MainWindow::addPlugin(PluginBase *plugin, const QString &tabName)
{
    QWidget *tabWidget = new QWidget(constructorTabWidget);
    QVBoxLayout *vBoxLayout = new QVBoxLayout(tabWidget);

    plugin->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    vBoxLayout->addWidget(plugin);

    char buttonName[16];
    snprintf(buttonName, 16, "sendButton%02u", constructorTabWidget->children().length());

    QPushButton *sendButton = new QPushButton("Transmit");
    sendButton->setObjectName(buttonName);
    QObject::connect(sendButton, &QPushButton::clicked,
                     dynamic_cast<PluginBase*>(plugin), &PluginBase::onRequestMessage);
    vBoxLayout->addWidget(sendButton);

    constructorTabWidget->addTab(tabWidget, tabName);
}
