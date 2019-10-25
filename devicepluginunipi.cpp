/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                         *
 *  Copyright (C) 2019 Bernhard Trinnes <bernhard.trinnes@nymea.io>        *
 *  Copyright (C) 2018 Simon Stürz <simon.stuerz@nymea.io>                 *
 *                                                                         *
 *  This file is part of nymea.                                            *
 *                                                                         *
 *  This library is free software; you can redistribute it and/or          *
 *  modify it under the terms of the GNU Lesser General Public             *
 *  License as published by the Free Software Foundation; either           *
 *  version 2.1 of the License, or (at your option) any later version.     *
 *                                                                         *
 *  This library is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU      *
 *  Lesser General Public License for more details.                        *
 *                                                                         *
 *  You should have received a copy of the GNU Lesser General Public       *
 *  License along with this library; If not, see                           *
 *  <http://www.gnu.org/licenses/>.                                        *
 *                                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "devicepluginunipi.h"
#include "plugininfo.h"
#include "devices/devicemanager.h"
#include "devices/deviceplugin.h"
#include "devices/device.h"

#include <QJsonDocument>
#include <QTimer>
#include <QSerialPort>

DevicePluginUniPi::DevicePluginUniPi()
{
}


void DevicePluginUniPi::init()
{
    connect(this, &DevicePluginUniPi::configValueChanged, this, &DevicePluginUniPi::onPluginConfigurationChanged);
    QLoggingCategory::setFilterRules(QStringLiteral("qt.modbus* = false"));

    m_connectionStateTypeIds.insert(uniPi1DeviceClassId, uniPi1ConnectedStateTypeId);
    m_connectionStateTypeIds.insert(uniPi1LiteDeviceClassId, uniPi1LiteConnectedStateTypeId);
    m_connectionStateTypeIds.insert(neuronS103DeviceClassId, neuronS103ConnectedStateTypeId);
    m_connectionStateTypeIds.insert(neuronM103DeviceClassId, neuronM103ConnectedStateTypeId);
    m_connectionStateTypeIds.insert(neuronM203DeviceClassId, neuronM203ConnectedStateTypeId);
    m_connectionStateTypeIds.insert(neuronM303DeviceClassId, neuronM303ConnectedStateTypeId);
    m_connectionStateTypeIds.insert(neuronM403DeviceClassId, neuronM403ConnectedStateTypeId);
    m_connectionStateTypeIds.insert(neuronM503DeviceClassId, neuronM503ConnectedStateTypeId);
    m_connectionStateTypeIds.insert(neuronL203DeviceClassId, neuronL203ConnectedStateTypeId);
    m_connectionStateTypeIds.insert(neuronL303DeviceClassId, neuronL303ConnectedStateTypeId);
    m_connectionStateTypeIds.insert(neuronL403DeviceClassId, neuronL403ConnectedStateTypeId);
    m_connectionStateTypeIds.insert(neuronL503DeviceClassId, neuronL503ConnectedStateTypeId);
    m_connectionStateTypeIds.insert(neuronL513DeviceClassId, neuronL513ConnectedStateTypeId);
    m_connectionStateTypeIds.insert(neuronXS10DeviceClassId, neuronXS10ConnectedStateTypeId);
    m_connectionStateTypeIds.insert(neuronXS20DeviceClassId, neuronXS20ConnectedStateTypeId);
    m_connectionStateTypeIds.insert(neuronXS30DeviceClassId, neuronXS30ConnectedStateTypeId);
    m_connectionStateTypeIds.insert(neuronXS40DeviceClassId, neuronXS40ConnectedStateTypeId);
    m_connectionStateTypeIds.insert(neuronXS50DeviceClassId, neuronXS50ConnectedStateTypeId);
}

void DevicePluginUniPi::discoverDevices(DeviceDiscoveryInfo *info)
{
    DeviceClassId deviceClassId = info->deviceClassId();

    if (deviceClassId == digitalInputDeviceClassId) {
        foreach(Device *parentDevice, myDevices()) {
            if ((parentDevice->deviceClassId() == uniPi1DeviceClassId) || (parentDevice->deviceClassId() == uniPi1LiteDeviceClassId)) {
                foreach (QString circuit, m_unipi->digitalInputs()) {
                    DeviceDescriptor deviceDescriptor(digitalInputDeviceClassId, QString("Digital input %1").arg(circuit), "UniPi 1", parentDevice->id());
                    foreach(Device *device, myDevices().filterByParentDeviceId(parentDevice->id())) {
                        if (device->paramValue(digitalInputDeviceCircuitParamTypeId) == circuit) {
                            qCDebug(dcUniPi()) << "Found already added Circuit:" << circuit;
                            deviceDescriptor.setDeviceId(device->id());
                            break;
                        }
                    }
                    ParamList params;
                    params.append(Param(digitalInputDeviceCircuitParamTypeId, circuit));
                    deviceDescriptor.setParams(params);
                    info->addDeviceDescriptor(deviceDescriptor);
                }
                break;
            }
        }

        foreach (NeuronExtension *neuronExtension, m_neuronExtensions) {
            DeviceId parentDeviceId = m_neuronExtensions.key(neuronExtension);
            foreach (QString circuit, neuronExtension->digitalInputs()) {
                DeviceDescriptor deviceDescriptor(digitalInputDeviceClassId, QString("Digital input %1").arg(circuit), QString("Neuron extension %1, slave address %2").arg(neuronExtension->type().arg(QString::number(neuronExtension->slaveAddress()))), parentDeviceId);
                foreach(Device *device, myDevices().filterByParentDeviceId(m_neuronExtensions.key(neuronExtension))) {
                    if (device->paramValue(digitalInputDeviceCircuitParamTypeId) == circuit) {
                        qCDebug(dcUniPi()) << "Found already added Circuit:" << circuit;
                        deviceDescriptor.setDeviceId(device->id());
                        break;
                    }
                }
                ParamList params;
                params.append(Param(digitalInputDeviceCircuitParamTypeId, circuit));
                deviceDescriptor.setParams(params);
                info->addDeviceDescriptor(deviceDescriptor);
            }
        }

        foreach (Neuron *neuron, m_neurons) {
            DeviceId parentDeviceId = m_neurons.key(neuron);
            foreach (QString circuit, neuron->digitalInputs()) {
                DeviceDescriptor deviceDescriptor(digitalInputDeviceClassId, QString("Digital input %1").arg(circuit), QString("Neuron %1").arg(neuron->type()), parentDeviceId);
                foreach(Device *device, myDevices().filterByParentDeviceId(m_neurons.key(neuron))) {
                    if (device->paramValue(digitalInputDeviceCircuitParamTypeId) == circuit) {
                        qCDebug(dcUniPi()) << "Found already added Circuit:" << circuit << parentDeviceId;
                        deviceDescriptor.setDeviceId(device->id());
                        break;
                    }
                }
                ParamList params;
                params.append(Param(digitalInputDeviceCircuitParamTypeId, circuit));
                deviceDescriptor.setParams(params);
                info->addDeviceDescriptor(deviceDescriptor);
            }
        }
        return info->finish(Device::DeviceErrorNoError);
    }

    if (deviceClassId == digitalOutputDeviceClassId) {
        foreach(Device *parentDevice, myDevices()) {
            if ((parentDevice->deviceClassId() == uniPi1DeviceClassId) || (parentDevice->deviceClassId() == uniPi1LiteDeviceClassId)) {
                foreach (QString circuit, m_unipi->digitalOutputs()) {
                    DeviceDescriptor deviceDescriptor(digitalOutputDeviceClassId, QString("Digital output %1").arg(circuit), "UniPi 1", parentDevice->id());
                    foreach(Device *device, myDevices().filterByParentDeviceId(parentDevice->id())) {
                        if (device->paramValue(digitalOutputDeviceCircuitParamTypeId) == circuit) {
                            qCDebug(dcUniPi()) << "Found already added Circuit:" << circuit << parentDevice->id();
                            deviceDescriptor.setDeviceId(device->id());
                            break;
                        }
                    }
                    ParamList params;
                    params.append(Param(digitalOutputDeviceCircuitParamTypeId, circuit));
                    deviceDescriptor.setParams(params);
                    info->addDeviceDescriptor(deviceDescriptor);
                }
                break;
            }
        }

        foreach (NeuronExtension *neuronExtension, m_neuronExtensions) {
            DeviceId parentDeviceId = m_neuronExtensions.key(neuronExtension);
            foreach (QString circuit, neuronExtension->digitalOutputs()) {
                DeviceDescriptor deviceDescriptor(digitalOutputDeviceClassId, QString("Digital output %1").arg(circuit), QString("Neuron extension %1, Slave address %2").arg(neuronExtension->type().arg(QString::number(neuronExtension->slaveAddress()))), parentDeviceId);
                foreach(Device *device, myDevices().filterByParentDeviceId(m_neuronExtensions.key(neuronExtension))) {
                    if (device->paramValue(digitalOutputDeviceCircuitParamTypeId) == circuit) {
                        qCDebug(dcUniPi()) << "Found already added Circuit:" << circuit << parentDeviceId;
                        deviceDescriptor.setDeviceId(device->id());
                        break;
                    }
                }
                ParamList params;
                params.append(Param(digitalOutputDeviceCircuitParamTypeId, circuit));
                deviceDescriptor.setParams(params);
                info->addDeviceDescriptor(deviceDescriptor);
            }
        }

        foreach (Neuron *neuron, m_neurons) {
            DeviceId parentDeviceId = m_neurons.key(neuron);
            foreach (QString circuit, neuron->digitalOutputs()) {
                DeviceDescriptor deviceDescriptor(digitalOutputDeviceClassId, QString("Digital output %1").arg(circuit), QString("Neuron %1").arg(neuron->type()), parentDeviceId);
                foreach(Device *device, myDevices().filterByParentDeviceId(m_neurons.key(neuron))) {
                    if (device->paramValue(digitalOutputDeviceCircuitParamTypeId) == circuit) {
                        qCDebug(dcUniPi()) << "Found already added Circuit:" << circuit << parentDeviceId;
                        deviceDescriptor.setDeviceId(device->id());
                        break;
                    }
                }
                ParamList params;
                params.append(Param(digitalOutputDeviceCircuitParamTypeId, circuit));
                deviceDescriptor.setParams(params);
                info->addDeviceDescriptor(deviceDescriptor);
            }
        }
        return info->finish(Device::DeviceErrorNoError);
    }

    if (deviceClassId == analogInputDeviceClassId) {
        foreach(Device *parentDevice, myDevices()) {
            if ((parentDevice->deviceClassId() == uniPi1DeviceClassId) || (parentDevice->deviceClassId() == uniPi1LiteDeviceClassId)) {
                foreach (QString circuit, m_unipi->analogInputs()) {
                    DeviceDescriptor deviceDescriptor(analogInputDeviceClassId, QString("Analog input %1").arg(circuit), "UniPi", parentDevice->id());
                    foreach(Device *device, myDevices().filterByParentDeviceId(parentDevice->id())) {
                        if (device->paramValue(analogInputDeviceCircuitParamTypeId) == circuit) {
                            qCDebug(dcUniPi()) << "Found already added Circuit:" << circuit << parentDevice->id();
                            deviceDescriptor.setDeviceId(device->id());
                            break;
                        }
                    }
                    ParamList params;
                    params.append(Param(analogInputDeviceCircuitParamTypeId, circuit));
                    deviceDescriptor.setParams(params);
                    info->addDeviceDescriptor(deviceDescriptor);
                }
                break;
            }
        }

        foreach (NeuronExtension *neuronExtension, m_neuronExtensions) {
            DeviceId parentDeviceId = m_neuronExtensions.key(neuronExtension);
            foreach (QString circuit, neuronExtension->analogInputs()) {
                DeviceDescriptor deviceDescriptor(analogInputDeviceClassId, QString("Analog input %1").arg(circuit), QString("Neuron extension %1, Slave address %2").arg(neuronExtension->type().arg(QString::number(neuronExtension->slaveAddress()))), parentDeviceId);
                foreach(Device *device, myDevices().filterByParentDeviceId(m_neuronExtensions.key(neuronExtension))) {
                    if (device->paramValue(analogInputDeviceCircuitParamTypeId) == circuit) {
                        qCDebug(dcUniPi()) << "Found already added Circuit:" << circuit << parentDeviceId;
                        deviceDescriptor.setDeviceId(device->id());
                        break;
                    }
                }
                ParamList params;
                params.append(Param(analogInputDeviceCircuitParamTypeId, circuit));
                deviceDescriptor.setParams(params);
                info->addDeviceDescriptor(deviceDescriptor);
            }
        }

        foreach (Neuron *neuron, m_neurons) {
            DeviceId parentDeviceId = m_neurons.key(neuron);
            foreach (QString circuit, neuron->analogInputs()) {
                DeviceDescriptor deviceDescriptor(analogInputDeviceClassId, QString("Analog input %1").arg(circuit), QString("Neuron %1").arg(neuron->type()), parentDeviceId);
                foreach(Device *device, myDevices().filterByParentDeviceId(m_neurons.key(neuron))) {
                    if (device->paramValue(analogInputDeviceCircuitParamTypeId) == circuit) {
                        qCDebug(dcUniPi()) << "Found already added Circuit:" << circuit << parentDeviceId;
                        deviceDescriptor.setDeviceId(device->id());
                        break;
                    }
                }
                ParamList params;
                params.append(Param(analogInputDeviceCircuitParamTypeId, circuit));
                deviceDescriptor.setParams(params);
                info->addDeviceDescriptor(deviceDescriptor);
            }
        }
        return info->finish(Device::DeviceErrorNoError);
    }

    if (deviceClassId == analogOutputDeviceClassId) {
        //TODO add unipi 1 and unipi 1 lite

        foreach (NeuronExtension *neuronExtension, m_neuronExtensions) {
            DeviceId parentDeviceId = m_neuronExtensions.key(neuronExtension);
            foreach (QString circuit, neuronExtension->analogOutputs()) {
                DeviceDescriptor deviceDescriptor(analogOutputDeviceClassId, QString("Analog output %1").arg(circuit), QString("Neuron extension %1, Slave address %2").arg(neuronExtension->type().arg(QString::number(neuronExtension->slaveAddress()))), parentDeviceId);
                foreach(Device *device, myDevices().filterByParentDeviceId(m_neuronExtensions.key(neuronExtension))) {
                    if (device->paramValue(analogOutputDeviceCircuitParamTypeId) == circuit) {
                        qCDebug(dcUniPi()) << "Found already added Circuit:" << circuit << parentDeviceId;
                        deviceDescriptor.setDeviceId(device->id());
                        break;
                    }
                }
                ParamList params;
                params.append(Param(analogOutputDeviceCircuitParamTypeId, circuit));
                deviceDescriptor.setParams(params);
                info->addDeviceDescriptor(deviceDescriptor);
            }
        }

        foreach (Neuron *neuron, m_neurons) {
            DeviceId parentDeviceId = m_neurons.key(neuron);
            foreach (QString circuit, neuron->analogOutputs()) {
                DeviceDescriptor deviceDescriptor(analogOutputDeviceClassId, QString("Analog output %1").arg(circuit), QString("Neuron %1").arg(neuron->type()), parentDeviceId);
                foreach(Device *device, myDevices().filterByParentDeviceId(m_neurons.key(neuron))) {
                    if (device->paramValue(analogOutputDeviceCircuitParamTypeId) == circuit) {
                        qCDebug(dcUniPi()) << "Found already added Circuit:" << circuit << parentDeviceId;
                        deviceDescriptor.setDeviceId(device->id());
                        break;
                    }
                }
                ParamList params;
                params.append(Param(analogOutputDeviceCircuitParamTypeId, circuit));
                deviceDescriptor.setParams(params);
                info->addDeviceDescriptor(deviceDescriptor);
            }
        }
        return info->finish(Device::DeviceErrorNoError);
    }

    if (deviceClassId == userLEDDeviceClassId) {
        QList<DeviceDescriptor> deviceDescriptors;
        foreach (NeuronExtension *neuronExtension, m_neuronExtensions) {
            DeviceId parentDeviceId = m_neuronExtensions.key(neuronExtension);
            foreach (QString circuit, neuronExtension->userLEDs()) {
                DeviceDescriptor deviceDescriptor(userLEDDeviceClassId, QString("User programmable LED %1").arg(circuit), QString("Neuron extension %1, Slave address %2").arg(neuronExtension->type().arg(neuronExtension->slaveAddress())), parentDeviceId);
                foreach(Device *device, myDevices().filterByParentDeviceId(m_neuronExtensions.key(neuronExtension))) {
                    if (device->paramValue(userLEDDeviceCircuitParamTypeId) == circuit) {
                        qCDebug(dcUniPi()) << "Found already added Circuit:" << circuit << parentDeviceId;
                        deviceDescriptor.setDeviceId(device->id());
                        break;
                    }
                }
                ParamList params;
                params.append(Param(userLEDDeviceCircuitParamTypeId, circuit));
                deviceDescriptor.setParams(params);
                info->addDeviceDescriptor(deviceDescriptor);
            }
        }

        foreach (Neuron *neuron, m_neurons) {
            DeviceId parentDeviceId = m_neurons.key(neuron);
            foreach (QString circuit, neuron->userLEDs()) {
                DeviceDescriptor deviceDescriptor(userLEDDeviceClassId, QString("User programmable LED %1").arg(circuit), QString("Neuron %1").arg(neuron->type()), parentDeviceId);
                foreach(Device *device, myDevices().filterByParentDeviceId(m_neurons.key(neuron))) {
                    if (device->paramValue(userLEDDeviceCircuitParamTypeId) == circuit) {
                        qCDebug(dcUniPi()) << "Found already added Circuit:" << circuit << parentDeviceId;
                        deviceDescriptor.setDeviceId(device->id());
                        break;
                    }
                }
                ParamList params;
                params.append(Param(userLEDDeviceCircuitParamTypeId, circuit));
                deviceDescriptor.setParams(params);
                info->addDeviceDescriptor(deviceDescriptor);
            }
        }
        return info->finish(Device::DeviceErrorNoError);
    }
    qCWarning(dcUniPi()) << "Unhandled device class in discoverDevice";
}

void DevicePluginUniPi::setupDevice(DeviceSetupInfo *info)
{
    Device *device = info->device();

    if(device->deviceClassId() == uniPi1DeviceClassId
            || device->deviceClassId() == uniPi1LiteDeviceClassId) {

        if (m_unipi)
            return info->finish(Device::DeviceErrorSetupFailed, QT_TR_NOOP("There is already a UniPi gateway in the system.")); //only one parent device allowed

        if(device->deviceClassId() == uniPi1DeviceClassId) {
            m_unipi = new UniPi(UniPi::UniPiType::UniPi1, this);
        } else if (device->deviceClassId() == uniPi1LiteDeviceClassId) {
            m_unipi = new UniPi(UniPi::UniPiType::UniPi1Lite, this);
        }

        if (!m_unipi->init()) {
            qCWarning(dcUniPi()) << "Could not setup UniPi";
            m_unipi->deleteLater();
            return info->finish(Device::DeviceErrorSetupFailed, QT_TR_NOOP("Error setting up UniPi."));
        }
        connect(m_unipi, &UniPi::digitalInputStatusChanged, this, &DevicePluginUniPi::onUniPiDigitalInputStatusChanged);
        connect(m_unipi, &UniPi::digitalOutputStatusChanged, this, &DevicePluginUniPi::onUniPiDigitalOutputStatusChanged);
        connect(m_unipi, &UniPi::analogInputStatusChanged, this, &DevicePluginUniPi::onUniPiAnalogInputStatusChanged);
        connect(m_unipi, &UniPi::analogOutputStatusChanged, this, &DevicePluginUniPi::onUniPiAnalogOutputStatusChanged);
        device->setStateValue(m_connectionStateTypeIds.value(device->deviceClassId()), true);

        return info->finish(Device::DeviceErrorNoError);
    }

    if(device->deviceClassId() == neuronS103DeviceClassId ||
            device->deviceClassId() == neuronM103DeviceClassId ||
            device->deviceClassId() == neuronM203DeviceClassId ||
            device->deviceClassId() == neuronM303DeviceClassId ||
            device->deviceClassId() == neuronM403DeviceClassId ||
            device->deviceClassId() == neuronL203DeviceClassId ||
            device->deviceClassId() == neuronL303DeviceClassId ||
            device->deviceClassId() == neuronL403DeviceClassId ||
            device->deviceClassId() == neuronL503DeviceClassId ||
            device->deviceClassId() == neuronL513DeviceClassId) {

        if (!neuronDeviceInit())
            return info->finish(Device::DeviceErrorSetupFailed, QT_TR_NOOP("Error setting up Neuron."));

        Neuron *neuron;
        if (device->deviceClassId() == neuronS103DeviceClassId) {
            neuron = new Neuron(Neuron::NeuronTypes::S103, m_modbusTCPMaster, this);
        } else  if (device->deviceClassId() == neuronM103DeviceClassId) {
            neuron = new Neuron(Neuron::NeuronTypes::M103, m_modbusTCPMaster, this);
        } else if (device->deviceClassId() == neuronM203DeviceClassId) {
            neuron = new Neuron(Neuron::NeuronTypes::M203, m_modbusTCPMaster, this);
        } else if (device->deviceClassId() == neuronM303DeviceClassId) {
            neuron = new Neuron(Neuron::NeuronTypes::M303, m_modbusTCPMaster, this);
        } else if (device->deviceClassId() == neuronM403DeviceClassId) {
            neuron = new Neuron(Neuron::NeuronTypes::M403, m_modbusTCPMaster, this);
        } else if (device->deviceClassId() == neuronM503DeviceClassId) {
            neuron = new Neuron(Neuron::NeuronTypes::M503, m_modbusTCPMaster, this);
        } else if (device->deviceClassId() == neuronL203DeviceClassId) {
            neuron = new Neuron(Neuron::NeuronTypes::L203, m_modbusTCPMaster, this);
        } else if (device->deviceClassId() == neuronL303DeviceClassId) {
            neuron = new Neuron(Neuron::NeuronTypes::L303, m_modbusTCPMaster, this);
        } else if (device->deviceClassId() == neuronL403DeviceClassId) {
            neuron = new Neuron(Neuron::NeuronTypes::L403, m_modbusTCPMaster, this);
        } else if (device->deviceClassId() == neuronL503DeviceClassId) {
            neuron = new Neuron(Neuron::NeuronTypes::L503, m_modbusTCPMaster, this);
        } else  if (device->deviceClassId() == neuronL513DeviceClassId) {
            neuron = new Neuron(Neuron::NeuronTypes::L513, m_modbusTCPMaster, this);
        } else {
            neuron = new Neuron(Neuron::NeuronTypes::S103, m_modbusTCPMaster, this);
        }

        if (!neuron->init()) {
            qCWarning(dcUniPi()) << "Could not load the modbus map";
            neuron->deleteLater();
            return info->finish(Device::DeviceErrorSetupFailed, QT_TR_NOOP("Error setting up Neuron device."));
        }
        m_neurons.insert(device->id(), neuron);
        connect(neuron, &Neuron::requestExecuted, this, &DevicePluginUniPi::onRequestExecuted);
        connect(neuron, &Neuron::requestError, this, &DevicePluginUniPi::onRequestError);
        connect(neuron, &Neuron::connectionStateChanged, this, &DevicePluginUniPi::onNeuronConnectionStateChanged);
        connect(neuron, &Neuron::digitalInputStatusChanged, this, &DevicePluginUniPi::onNeuronDigitalInputStatusChanged);
        connect(neuron, &Neuron::digitalOutputStatusChanged, this, &DevicePluginUniPi::onNeuronDigitalOutputStatusChanged);
        connect(neuron, &Neuron::analogInputStatusChanged, this, &DevicePluginUniPi::onNeuronAnalogInputStatusChanged);
        connect(neuron, &Neuron::analogOutputStatusChanged, this, &DevicePluginUniPi::onNeuronAnalogOutputStatusChanged);
        connect(neuron, &Neuron::userLEDStatusChanged, this, &DevicePluginUniPi::onNeuronUserLEDStatusChanged);

        device->setStateValue(m_connectionStateTypeIds.value(device->deviceClassId()), (m_modbusTCPMaster->state() == QModbusDevice::ConnectedState));

        return info->finish(Device::DeviceErrorNoError);
    }

    if(device->deviceClassId() == neuronXS10DeviceClassId ||
            device->deviceClassId() == neuronXS20DeviceClassId ||
            device->deviceClassId() == neuronXS30DeviceClassId ||
            device->deviceClassId() == neuronXS40DeviceClassId ||
            device->deviceClassId() == neuronXS50DeviceClassId) {

        if (!neuronExtensionInterfaceInit())
            return info->finish(Device::DeviceErrorSetupFailed, QT_TR_NOOP("Error setting up Neuron."));

        int slaveAddress = device->paramValue(neuronXS10DeviceSlaveAddressParamTypeId).toInt();
        NeuronExtension *neuronExtension = new NeuronExtension(NeuronExtension::ExtensionTypes::xS10, m_modbusRTUMaster, slaveAddress, this);

        if(device->deviceClassId() == neuronXS10DeviceClassId) {
            neuronExtension = new NeuronExtension(NeuronExtension::ExtensionTypes::xS10, m_modbusRTUMaster, slaveAddress, this);
        } else if (device->deviceClassId() == neuronXS20DeviceClassId) {
            neuronExtension = new NeuronExtension(NeuronExtension::ExtensionTypes::xS20, m_modbusRTUMaster, slaveAddress, this);
        } else if (device->deviceClassId() == neuronXS30DeviceClassId) {
            neuronExtension = new NeuronExtension(NeuronExtension::ExtensionTypes::xS30, m_modbusRTUMaster, slaveAddress, this);
        } else if (device->deviceClassId() == neuronXS40DeviceClassId) {
            neuronExtension = new NeuronExtension(NeuronExtension::ExtensionTypes::xS40, m_modbusRTUMaster, slaveAddress, this);
        } else if (device->deviceClassId() == neuronXS50DeviceClassId) {
            neuronExtension = new NeuronExtension(NeuronExtension::ExtensionTypes::xS50, m_modbusRTUMaster, slaveAddress, this);
        } else {
            neuronExtension = new NeuronExtension(NeuronExtension::ExtensionTypes::xS10, m_modbusRTUMaster, slaveAddress, this);
        }
        if (!neuronExtension->init()) {
            qCWarning(dcUniPi()) << "Could not load the modbus map";
            neuronExtension->deleteLater();
            return info->finish(Device::DeviceErrorSetupFailed, QT_TR_NOOP("Erro loading modbus map."));
        }
        connect(neuronExtension, &NeuronExtension::requestExecuted, this, &DevicePluginUniPi::onRequestExecuted);
        connect(neuronExtension, &NeuronExtension::requestError, this, &DevicePluginUniPi::onRequestError);
        connect(neuronExtension, &NeuronExtension::connectionStateChanged, this, &DevicePluginUniPi::onNeuronExtensionConnectionStateChanged);
        connect(neuronExtension, &NeuronExtension::digitalInputStatusChanged, this, &DevicePluginUniPi::onNeuronExtensionDigitalInputStatusChanged);
        connect(neuronExtension, &NeuronExtension::digitalOutputStatusChanged, this, &DevicePluginUniPi::onNeuronExtensionDigitalOutputStatusChanged);
        connect(neuronExtension, &NeuronExtension::analogInputStatusChanged, this, &DevicePluginUniPi::onNeuronExtensionAnalogInputStatusChanged);
        connect(neuronExtension, &NeuronExtension::analogOutputStatusChanged, this, &DevicePluginUniPi::onNeuronExtensionAnalogOutputStatusChanged);
        connect(neuronExtension, &NeuronExtension::userLEDStatusChanged, this, &DevicePluginUniPi::onNeuronExtensionUserLEDStatusChanged);

        m_neuronExtensions.insert(device->id(), neuronExtension);
        device->setStateValue(m_connectionStateTypeIds.value(device->deviceClassId()), (m_modbusRTUMaster->state() == QModbusDevice::ConnectedState));

        return info->finish(Device::DeviceErrorNoError);
    }

    if (device->deviceClassId() == digitalOutputDeviceClassId) {
        return info->finish(Device::DeviceErrorNoError);
    }

    if (device->deviceClassId() == digitalInputDeviceClassId) {
        return info->finish(Device::DeviceErrorNoError);
    }

    if (device->deviceClassId() == userLEDDeviceClassId) {
        return info->finish(Device::DeviceErrorNoError);
    }

    if (device->deviceClassId() == analogInputDeviceClassId) {
        return info->finish(Device::DeviceErrorNoError);
    }

    if (device->deviceClassId() == analogOutputDeviceClassId) {
        return info->finish(Device::DeviceErrorNoError);
    }

    qCWarning(dcUniPi()) << "Unhandled device class in setupDevice";
}

void DevicePluginUniPi::postSetupDevice(Device *device)
{
    Q_UNUSED(device)

    if (!m_reconnectTimer) {
        m_reconnectTimer = new QTimer(this);
        m_reconnectTimer->setSingleShot(true);
        connect(m_reconnectTimer, &QTimer::timeout, this, &DevicePluginUniPi::onReconnectTimer);
    }
}


void DevicePluginUniPi::executeAction(DeviceActionInfo *info)
{
    Device *device = info->device();
    Action action = info->action();

    if (device->deviceClassId() == digitalOutputDeviceClassId)  {

        if (action.actionTypeId() == digitalOutputPowerActionTypeId) {
            QString digitalOutputNumber = device->paramValue(digitalOutputDeviceCircuitParamTypeId).toString();
            bool stateValue = action.param(digitalOutputPowerActionPowerParamTypeId).value().toBool();

            if (m_unipi) {
                m_unipi->setDigitalOutput(digitalOutputNumber, stateValue);
            }
            if (m_neurons.contains(device->parentId().toString())) {
                Neuron *neuron = m_neurons.value(device->parentId().toString());
                QUuid requestId = neuron->setDigitalOutput(digitalOutputNumber, stateValue);
                if (requestId.isNull()) {
                    info->finish(Device::DeviceErrorHardwareFailure);
                } else {
                    m_asyncActions.insert(requestId, info);
                }
                return;
            }
            if (m_neuronExtensions.contains(device->parentId().toString())) {
                NeuronExtension *neuronExtension = m_neuronExtensions.value(device->parentId().toString());
                QUuid requestId = neuronExtension->setDigitalOutput(digitalOutputNumber, stateValue);
                if (requestId.isNull()) {
                    info->finish(Device::DeviceErrorHardwareFailure);
                } else {
                    m_asyncActions.insert(requestId, info);
                }
            }
            return info->finish(Device::DeviceErrorNoError);
        }
        return info->finish(Device::DeviceErrorActionTypeNotFound);
    }

    if (device->deviceClassId() == analogOutputDeviceClassId) {

        if (action.actionTypeId() == analogOutputOutputValueActionTypeId) {
            QString analogOutputNumber = device->paramValue(analogOutputDeviceCircuitParamTypeId).toString();
            double analogValue = action.param(analogOutputOutputValueActionOutputValueParamTypeId).value().toDouble();

            if (m_unipi) {
                m_unipi->setAnalogOutput(analogOutputNumber, analogValue);
            }
            if (m_neurons.contains(device->parentId().toString())) {
                Neuron *neuron = m_neurons.value(device->parentId().toString());
                QUuid requestId = neuron->setAnalogOutput(analogOutputNumber, analogValue);
                if (requestId.isNull()) {
                    info->finish(Device::DeviceErrorHardwareFailure);
                } else {
                    m_asyncActions.insert(requestId, info);
                }
            }
            if (m_neuronExtensions.contains(device->parentId().toString())) {
                NeuronExtension *neuronExtension = m_neuronExtensions.value(device->parentId().toString());
                QUuid requestId = neuronExtension->setAnalogOutput(analogOutputNumber, analogValue);
                if (requestId.isNull()) {
                    info->finish(Device::DeviceErrorHardwareFailure);
                } else {
                    m_asyncActions.insert(requestId, info);
                }
            }
            return info->finish(Device::DeviceErrorNoError);
        }
        return info->finish(Device::DeviceErrorActionTypeNotFound);
    }

    if (device->deviceClassId() == userLEDDeviceClassId) {
        if (action.actionTypeId() == userLEDPowerActionTypeId) {
            QString userLED = device->paramValue(userLEDDeviceCircuitParamTypeId).toString();
            bool stateValue = action.param(userLEDPowerActionPowerParamTypeId).value().toBool();
            if (m_neurons.contains(device->parentId().toString())) {
                Neuron *neuron = m_neurons.value(device->parentId().toString());
                QUuid requestId = neuron->setUserLED(userLED, stateValue);
                if (requestId.isNull()) {
                    info->finish(Device::DeviceErrorHardwareFailure);
                } else {
                    m_asyncActions.insert(requestId, info);
                }
            }
            if (m_neuronExtensions.contains(device->parentId().toString())) {
                NeuronExtension *neuronExtension = m_neuronExtensions.value(device->parentId().toString());
                QUuid requestId = neuronExtension->setUserLED(userLED, stateValue);
                if (requestId.isNull()) {
                    info->finish(Device::DeviceErrorHardwareFailure);
                } else {
                    m_asyncActions.insert(requestId, info);
                }
            }
            return info->finish(Device::DeviceErrorNoError);
        }
        return info->finish(Device::DeviceErrorActionTypeNotFound);
    }
    qCWarning(dcUniPi()) << "Unhandled device class in executeAction";
}


void DevicePluginUniPi::deviceRemoved(Device *device)
{
    Q_UNUSED(device);
    if(m_neurons.contains(device->id())) {
        Neuron *neuron = m_neurons.take(device->id());
        neuron->deleteLater();
    }
    if(m_neuronExtensions.contains(device->id())) {
        NeuronExtension *neuronExtension = m_neuronExtensions.take(device->id());
        neuronExtension->deleteLater();
    }

    if ((device->deviceClassId() == uniPi1DeviceClassId) || (device->deviceClassId() == uniPi1LiteDeviceClassId)) {
        if(m_unipi) {
            m_unipi->deleteLater();
            m_unipi = nullptr;
        }
    }

    if (myDevices().isEmpty()) {
        if (m_reconnectTimer) {
            m_reconnectTimer->deleteLater();
            m_reconnectTimer = nullptr;
        }

        if (m_modbusTCPMaster) {
            m_modbusTCPMaster->disconnectDevice();
            m_modbusTCPMaster->deleteLater();
            m_modbusTCPMaster = nullptr;
        }
        if (m_modbusRTUMaster) {
            m_modbusRTUMaster->disconnectDevice();
            m_modbusRTUMaster->deleteLater();
            m_modbusRTUMaster = nullptr;
        }
    }
}

void DevicePluginUniPi::onPluginConfigurationChanged(const ParamTypeId &paramTypeId, const QVariant &value)
{
    qCDebug(dcUniPi()) << "Plugin configuration changed";
    if (paramTypeId == uniPiPluginPortParamTypeId) {
        if (m_modbusTCPMaster) {
            m_modbusTCPMaster->setConnectionParameter(QModbusDevice::NetworkAddressParameter, value.toString());
        }
    }

    if (paramTypeId == uniPiPluginAddressParamTypeId) {
        if (m_modbusTCPMaster) {
            m_modbusTCPMaster->setConnectionParameter(QModbusDevice::NetworkPortParameter, value.toInt());
        }
    }

    if (paramTypeId == uniPiPluginSerialPortParamTypeId) {
        if (m_modbusRTUMaster) {
            m_modbusRTUMaster->setConnectionParameter(QModbusDevice::SerialPortNameParameter, value.toString());
        }
    }

    if (paramTypeId == uniPiPluginBaudrateParamTypeId) {
        if (m_modbusRTUMaster) {
            m_modbusRTUMaster->setConnectionParameter(QModbusDevice::SerialBaudRateParameter, value.toInt());
        }
    }

    if (paramTypeId == uniPiPluginParityParamTypeId) {
        if (m_modbusRTUMaster) {
            if (value == "Even") {
                m_modbusRTUMaster->setConnectionParameter(QModbusDevice::SerialParityParameter, QSerialPort::Parity::EvenParity);
            } else {
                m_modbusRTUMaster->setConnectionParameter(QModbusDevice::SerialParityParameter, QSerialPort::Parity::NoParity);
            }
        }
    }
}

void DevicePluginUniPi::onNeuronConnectionStateChanged(bool state)
{
    Neuron *neuron = static_cast<Neuron *>(sender());
    Device *device = myDevices().findById(m_neurons.key(neuron));
    if (!device)
        return;
    device->setStateValue(m_connectionStateTypeIds.value(device->deviceClassId()), state);
}

void DevicePluginUniPi::onNeuronDigitalInputStatusChanged(QString &circuit, bool value)
{
    Neuron *neuron = static_cast<Neuron *>(sender());

    foreach(Device *device, myDevices().filterByParentDeviceId(m_neurons.key(neuron))) {
        if (device->deviceClassId() == digitalInputDeviceClassId) {
            if (device->paramValue(digitalInputDeviceCircuitParamTypeId).toString() == circuit) {

                device->setStateValue(digitalInputInputStatusStateTypeId, value);
                return;
            }
        }
    }
}

void DevicePluginUniPi::onNeuronExtensionDigitalInputStatusChanged(QString &circuit, bool value)
{
    NeuronExtension *neuronExtension = static_cast<NeuronExtension *>(sender());

    foreach(Device *device, myDevices().filterByParentDeviceId(m_neuronExtensions.key(neuronExtension))) {
        if (device->deviceClassId() == digitalInputDeviceClassId) {
            if (device->paramValue(digitalInputDeviceCircuitParamTypeId).toString() == circuit) {

                device->setStateValue(digitalInputInputStatusStateTypeId, value);
                return;
            }
        }
    }
}

void DevicePluginUniPi::onNeuronDigitalOutputStatusChanged(QString &circuit, bool value)
{
    Neuron *neuron = static_cast<Neuron *>(sender());

    foreach(Device *device, myDevices().filterByParentDeviceId(m_neurons.key(neuron))) {
        if (device->deviceClassId() == digitalOutputDeviceClassId) {
            if (device->paramValue(digitalOutputDeviceCircuitParamTypeId).toString() == circuit) {

                device->setStateValue(digitalOutputPowerStateTypeId, value);
                return;
            }
        }
    }
}

void DevicePluginUniPi::onNeuronExtensionDigitalOutputStatusChanged(QString &circuit, bool value)
{
    NeuronExtension *neuronExtension = static_cast<NeuronExtension *>(sender());

    foreach(Device *device, myDevices().filterByParentDeviceId(m_neuronExtensions.key(neuronExtension))) {
        if (device->deviceClassId() == digitalOutputDeviceClassId) {
            if (device->paramValue(digitalOutputDeviceCircuitParamTypeId).toString() == circuit) {

                device->setStateValue(digitalOutputPowerStateTypeId, value);
                return;
            }
        }
    }
}

void DevicePluginUniPi::onNeuronAnalogInputStatusChanged(QString &circuit, double value)
{
    Neuron *neuron = static_cast<Neuron *>(sender());

    foreach(Device *device, myDevices().filterByParentDeviceId(m_neurons.key(neuron))) {
        if (device->deviceClassId() == analogInputDeviceClassId) {
            if (device->paramValue(analogInputDeviceCircuitParamTypeId).toString() == circuit) {

                device->setStateValue(analogInputInputValueStateTypeId, value);
                return;
            }
        }
    }
}

void DevicePluginUniPi::onNeuronExtensionAnalogInputStatusChanged(QString &circuit, double value)
{
    NeuronExtension *neuronExtension = static_cast<NeuronExtension *>(sender());

    foreach(Device *device, myDevices().filterByParentDeviceId(m_neuronExtensions.key(neuronExtension))) {
        if (device->deviceClassId() == analogInputDeviceClassId) {
            if (device->paramValue(analogInputDeviceCircuitParamTypeId).toString() == circuit) {
                device->setStateValue(analogInputInputValueStateTypeId, value);
                return;
            }
        }
    }
}

void DevicePluginUniPi::onNeuronAnalogOutputStatusChanged(QString &circuit, double value)
{
    Neuron *neuron = static_cast<Neuron *>(sender());

    foreach(Device *device, myDevices().filterByParentDeviceId(m_neurons.key(neuron))) {
        if (device->deviceClassId() == analogOutputDeviceClassId) {
            if (device->paramValue(analogOutputDeviceCircuitParamTypeId).toString() == circuit) {

                device->setStateValue(analogOutputOutputValueStateTypeId, value);
                return;
            }
        }
    }
}

void DevicePluginUniPi::onNeuronExtensionAnalogOutputStatusChanged(QString &circuit, double value)
{
    NeuronExtension *neuronExtension = static_cast<NeuronExtension *>(sender());

    foreach(Device *device, myDevices().filterByParentDeviceId(m_neuronExtensions.key(neuronExtension))) {
        if (device->deviceClassId() == analogOutputDeviceClassId) {
            if (device->paramValue(analogOutputDeviceCircuitParamTypeId).toString() == circuit) {

                device->setStateValue(analogOutputOutputValueStateTypeId, value);
                return;
            }
        }
    }
}

void DevicePluginUniPi::onNeuronUserLEDStatusChanged(QString &circuit, bool value)
{
    Neuron *neuron = static_cast<Neuron *>(sender());

    foreach(Device *device, myDevices().filterByParentDeviceId(m_neurons.key(neuron))) {
        if (device->deviceClassId() == userLEDDeviceClassId) {
            if (device->paramValue(userLEDDeviceCircuitParamTypeId).toString() == circuit) {

                device->setStateValue(userLEDPowerStateTypeId, value);
                return;
            }
        }
    }
}

void DevicePluginUniPi::onNeuronExtensionConnectionStateChanged(bool state)
{
    NeuronExtension *neuron = static_cast<NeuronExtension *>(sender());
    Device *device = myDevices().findById(m_neuronExtensions.key(neuron));
    if (!device)
        return;
    device->setStateValue(m_connectionStateTypeIds.value(device->deviceClassId()), state);
}

void DevicePluginUniPi::onRequestExecuted(QUuid requestId, bool success)
{
    if (m_asyncActions.contains(requestId)){
        DeviceActionInfo *info = m_asyncActions.take(requestId);
        if (success){
            info->finish(Device::DeviceErrorNoError);
        } else {
            info->finish(Device::DeviceErrorHardwareNotAvailable);
        }
    }
}

void DevicePluginUniPi::onRequestError(QUuid requestId, const QString &error)
{
    if (m_asyncActions.contains(requestId)){
        DeviceActionInfo *info = m_asyncActions.take(requestId);
        info->finish(Device::DeviceErrorHardwareNotAvailable, error);
    }
}

void DevicePluginUniPi::onNeuronExtensionUserLEDStatusChanged(QString &circuit, bool value)
{
    NeuronExtension *neuronExtension = static_cast<NeuronExtension *>(sender());

    foreach(Device *device, myDevices().filterByParentDeviceId(m_neuronExtensions.key(neuronExtension))) {
        if (device->deviceClassId() == userLEDDeviceClassId) {
            if (device->paramValue(userLEDDeviceCircuitParamTypeId).toString() == circuit) {

                device->setStateValue(userLEDPowerStateTypeId, value);
                return;
            }
        }
    }
}

void DevicePluginUniPi::onReconnectTimer()
{
    if(m_modbusRTUMaster) {
        if (!m_modbusRTUMaster->connectDevice()) {
            m_reconnectTimer->start(10000);
        }
    }
    if(m_modbusTCPMaster) {
        if (!m_modbusTCPMaster->connectDevice()) {
            m_reconnectTimer->start(10000);
        }
    }
}


void DevicePluginUniPi::onModbusTCPErrorOccurred(QModbusDevice::Error error)
{
    qCWarning(dcUniPi()) << "An error occured" << error;
}

void DevicePluginUniPi::onModbusRTUErrorOccurred(QModbusDevice::Error error)
{
    qCWarning(dcUniPi()) << "An error occured" << error;
}

void DevicePluginUniPi::onModbusTCPStateChanged(QModbusDevice::State state)
{
    bool connected = (state != QModbusDevice::UnconnectedState);

    foreach (DeviceId deviceId, m_neurons.keys()) {
        Device *device = myDevices().findById(deviceId);
        device->setStateValue(m_connectionStateTypeIds.value(device->deviceClassId()), connected);
    }

    if (!connected) {
        //try to reconnect in 10 seconds
        m_reconnectTimer->start(10000);
    }
    qCDebug(dcUniPi()) << "Connection status changed:" << connected;
}

void DevicePluginUniPi::onModbusRTUStateChanged(QModbusDevice::State state)
{
    bool connected = (state != QModbusDevice::UnconnectedState);

    foreach (DeviceId deviceId, m_neurons.keys()) {
        Device *device = myDevices().findById(deviceId);
        device->setStateValue(m_connectionStateTypeIds.value(device->deviceClassId()), connected);
    }

    if (!connected) {
        //try to reconnect in 10 seconds
        m_reconnectTimer->start(10000);
    }
    qCDebug(dcUniPi()) << "Connection status changed:" << connected;
}

void DevicePluginUniPi::onUniPiDigitalInputStatusChanged(const QString &circuit, bool value)
{
    qDebug(dcUniPi) << "Digital Input changed" << circuit << value;
    foreach(Device *device, myDevices()) {
        if (device->deviceClassId() == digitalInputDeviceClassId) {
            if (device->paramValue(digitalInputDeviceCircuitParamTypeId).toString() == circuit) {

                device->setStateValue(digitalInputInputStatusStateTypeId, value);
                return;
            }
        }
    }
}

void DevicePluginUniPi::onUniPiDigitalOutputStatusChanged(const QString &circuit, bool value)
{
    qDebug(dcUniPi) << "Digital Output changed" << circuit << value;
    foreach(Device *device, myDevices()) {
        if (device->deviceClassId() == digitalOutputDeviceClassId) {
            if (device->paramValue(digitalOutputDeviceCircuitParamTypeId).toString() == circuit) {

                device->setStateValue(digitalOutputPowerStateTypeId, value);
                return;
            }
        }
    }
}

void DevicePluginUniPi::onUniPiAnalogInputStatusChanged(const QString &circuit, double value)
{
    qDebug(dcUniPi) << "Digital Input changed" << circuit << value;
    foreach(Device *device, myDevices()) {
        if (device->deviceClassId() == analogInputDeviceClassId) {
            if (device->paramValue(analogInputDeviceCircuitParamTypeId).toString() == circuit) {
                device->setStateValue(analogInputInputValueStateTypeId, value);
                return;
            }
        }
    }
}

void DevicePluginUniPi::onUniPiAnalogOutputStatusChanged(const QString &circuit, double value)
{
    foreach(Device *parentDevice, myDevices()) {
        if ((parentDevice->deviceClassId() == uniPi1DeviceClassId) || (parentDevice->deviceClassId() == uniPi1LiteDeviceClassId)) {
            foreach(Device *device, myDevices().filterByParentDeviceId(parentDevice->id())) {
                if (device->deviceClassId() == analogOutputDeviceClassId) {
                    if (device->paramValue(analogOutputDeviceCircuitParamTypeId).toString() == circuit) {

                        device->setStateValue(analogOutputOutputValueStateTypeId, value);
                        return;
                    }
                }
            }
            break;
        }
    }
}

bool DevicePluginUniPi::neuronDeviceInit()
{
    if(!m_modbusTCPMaster) {
        int port = configValue(uniPiPluginPortParamTypeId).toInt();;
        QHostAddress ipAddress = QHostAddress(configValue(uniPiPluginAddressParamTypeId).toString());

        m_modbusTCPMaster = new QModbusTcpClient(this);
        m_modbusTCPMaster->setConnectionParameter(QModbusDevice::NetworkPortParameter, port);
        m_modbusTCPMaster->setConnectionParameter(QModbusDevice::NetworkAddressParameter, ipAddress.toString());
        //m_modbusTCPMaster->setTimeout(100);
        //m_modbusTCPMaster->setNumberOfRetries(1);

        connect(m_modbusTCPMaster, &QModbusTcpClient::stateChanged, this, &DevicePluginUniPi::onModbusTCPStateChanged);
        connect(m_modbusTCPMaster, &QModbusTcpClient::errorOccurred, this, &DevicePluginUniPi::onModbusTCPErrorOccurred);

        if (!m_modbusTCPMaster->connectDevice()) {
            qCWarning(dcUniPi()) << "Connect failed:" << m_modbusTCPMaster->errorString();
            return false;
        }
    }
    return true;
}

bool DevicePluginUniPi::neuronExtensionInterfaceInit()
{
    if(!m_modbusRTUMaster) {
        QString serialPort = configValue(uniPiPluginSerialPortParamTypeId).toString();
        int baudrate = configValue(uniPiPluginBaudrateParamTypeId).toInt();
        QString parity = configValue(uniPiPluginParityParamTypeId).toString();

        m_modbusRTUMaster = new QModbusRtuSerialMaster(this);
        m_modbusRTUMaster->setConnectionParameter(QModbusDevice::SerialPortNameParameter, serialPort);
        if (parity == "Even") {
            m_modbusRTUMaster->setConnectionParameter(QModbusDevice::SerialParityParameter, QSerialPort::Parity::EvenParity);
        } else {
            m_modbusRTUMaster->setConnectionParameter(QModbusDevice::SerialParityParameter, QSerialPort::Parity::NoParity);
        }
        m_modbusRTUMaster->setConnectionParameter(QModbusDevice::SerialBaudRateParameter, baudrate);
        m_modbusRTUMaster->setConnectionParameter(QModbusDevice::SerialDataBitsParameter, 8);
        m_modbusRTUMaster->setConnectionParameter(QModbusDevice::SerialStopBitsParameter, 1);
        //m_modbusRTUMaster->setTimeout(100);
        //m_modbusRTUMaster->setNumberOfRetries(1);

        connect(m_modbusRTUMaster, &QModbusRtuSerialMaster::stateChanged, this, &DevicePluginUniPi::onModbusRTUStateChanged);
        connect(m_modbusRTUMaster, &QModbusRtuSerialMaster::errorOccurred, this, &DevicePluginUniPi::onModbusRTUErrorOccurred);

        if (!m_modbusRTUMaster->connectDevice()) {
            qCWarning(dcUniPi()) << "Connect failed:" << m_modbusRTUMaster->errorString();
            return false;
        }
    }
    return true;
}
