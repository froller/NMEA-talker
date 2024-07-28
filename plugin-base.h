#ifndef PLUGINBASE_H
#define PLUGINBASE_H

#include <QWidget>
#include "string.h"

class PluginBase : public QWidget
{
    Q_OBJECT

public:
    PluginBase(QWidget *parent = nullptr)
        : QWidget(parent)
    { 1; };
    virtual ~PluginBase() = default;

    static uint8_t NMEAChecksum(const char* sentence)
    {
        const size_t len = strlen(sentence);
        uint8_t checksum = 0;
        for (int i = 1; i < len && sentence[i] != '*'; ++i)
            checksum ^= sentence[i];
        return checksum;
    }

public slots:
    virtual void onRequestMessage() { qDebug() << "Message requested"; };
    virtual void onTick() { };

signals:
    void message(const QString &message);

};

#endif // PLUGINBASE_H
