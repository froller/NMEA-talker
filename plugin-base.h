#ifndef PLUGINBASE_H
#define PLUGINBASE_H

#include <QWidget>

class PluginBase : public QWidget
{
    Q_OBJECT

public:
    PluginBase(QWidget *parent = nullptr)
        : QWidget(parent)
    { 1; };
    virtual ~PluginBase() = default;

public slots:
    virtual void onRequestMessage() { qDebug() << "Message requested"; };

signals:
    void messageSent(const QString &message);

};

#endif // PLUGINBASE_H
