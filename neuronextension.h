#ifndef NEURONEXTENSION_H
#define NEURONEXTENSION_H

#include <QObject>
#include <QHash>
#include <QTimer>
#include <QtSerialBus>
#include <QUuid>

class NeuronExtension : public QObject
{
    Q_OBJECT
public:

    enum ExtensionTypes {
        xS10,
        xS20,
        xS30,
        xS40,
        xS50
    };

    explicit NeuronExtension(ExtensionTypes extensionType, QModbusRtuSerialMaster *modbusInterface, int slaveAddress, QObject *parent = nullptr);
    ~NeuronExtension();

    bool init();
    QString type();
    int slaveAddress();
    void setSlaveAddress(int slaveAddress);

    QList<QString> digitalInputs();
    QList<QString> digitalOutputs();
    QList<QString> analogInputs();
    QList<QString> analogOutputs();
    QList<QString> userLEDs();

    QUuid setDigitalOutput(const QString &circuit, bool value);
    bool getDigitalOutput(const QString &circuit);
    bool getDigitalInput(const QString &circuit);

    QUuid setAnalogOutput(const QString &circuit, double value);
    bool getAnalogOutput(const QString &circuit);
    bool getAnalogInput(const QString &circuit);

    bool getAllDigitalOutputs();
    bool getAllDigitalInputs();
    bool getAllAnalogOutputs();
    bool getAllAnalogInputs();

    QUuid setUserLED(const QString &circuit, bool value);
    bool getUserLED(const QString &circuit);
private:

    QTimer *m_inputPollingTimer = nullptr;
    QTimer *m_outputPollingTimer = nullptr;

    QHash<QString, int> m_modbusDigitalOutputRegisters;
    QHash<QString, int> m_modbusDigitalInputRegisters;
    QHash<QString, int> m_modbusAnalogInputRegisters;
    QHash<QString, int> m_modbusAnalogOutputRegisters;
    QHash<QString, int> m_modbusUserLEDRegisters;
    QList<QPair<QUuid, QModbusDataUnit>> m_writeRequestQueue;

    QModbusRtuSerialMaster *m_modbusInterface = nullptr;
    int m_slaveAddress = 0;
    ExtensionTypes m_extensionType = ExtensionTypes::xS10;
    QHash<int, uint16_t> m_previousModbusRegisterValue;

    bool loadModbusMap();
    bool modbusWriteRequest(QUuid requestId, QModbusDataUnit request);

signals:
    void requestExecuted(QUuid requestId, bool success);
    void requestError(QUuid requestId, const QString &error);
    void digitalInputStatusChanged(QString &circuit, bool value);
    void digitalOutputStatusChanged(QString &circuit, bool value);

    void analogInputStatusChanged(QString &circuit, double value);
    void analogOutputStatusChanged(QString &circuit, double value);

    void userLEDStatusChanged(QString &circuit, bool value);

    void connectionStateChanged(bool state);

private slots:
    void onOutputPollingTimer();
    void onInputPollingTimer();

    void onFinished();
};

#endif // NEURONEXTENSION_H
