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

#include "integrationpluginunipi.h"
#include "plugininfo.h"

#include <QJsonDocument>
#include <QTimer>
#include <QSerialPort>

IntegrationPluginUniPi::IntegrationPluginUniPi()
{
}


void IntegrationPluginUniPi::init()
{
    connect(this, &IntegrationPluginUniPi::configValueChanged, this, &IntegrationPluginUniPi::onPluginConfigurationChanged);
    //QLoggingCategory::setFilterRules(QStringLiteral("qt.modbus* = false"));

    m_connectionStateTypeIds.insert(uniPi1ThingClassId, uniPi1ConnectedStateTypeId);
    m_connectionStateTypeIds.insert(uniPi1LiteThingClassId, uniPi1LiteConnectedStateTypeId);
    m_connectionStateTypeIds.insert(neuronS103ThingClassId, neuronS103ConnectedStateTypeId);
    m_connectionStateTypeIds.insert(neuronM103ThingClassId, neuronM103ConnectedStateTypeId);
    m_connectionStateTypeIds.insert(neuronM203ThingClassId, neuronM203ConnectedStateTypeId);
    m_connectionStateTypeIds.insert(neuronM303ThingClassId, neuronM303ConnectedStateTypeId);
    m_connectionStateTypeIds.insert(neuronM403ThingClassId, neuronM403ConnectedStateTypeId);
    m_connectionStateTypeIds.insert(neuronM503ThingClassId, neuronM503ConnectedStateTypeId);
    m_connectionStateTypeIds.insert(neuronL203ThingClassId, neuronL203ConnectedStateTypeId);
    m_connectionStateTypeIds.insert(neuronL303ThingClassId, neuronL303ConnectedStateTypeId);
    m_connectionStateTypeIds.insert(neuronL403ThingClassId, neuronL403ConnectedStateTypeId);
    m_connectionStateTypeIds.insert(neuronL503ThingClassId, neuronL503ConnectedStateTypeId);
    m_connectionStateTypeIds.insert(neuronL513ThingClassId, neuronL513ConnectedStateTypeId);
    m_connectionStateTypeIds.insert(neuronXS10ThingClassId, neuronXS10ConnectedStateTypeId);
    m_connectionStateTypeIds.insert(neuronXS20ThingClassId, neuronXS20ConnectedStateTypeId);
    m_connectionStateTypeIds.insert(neuronXS30ThingClassId, neuronXS30ConnectedStateTypeId);
    m_connectionStateTypeIds.insert(neuronXS40ThingClassId, neuronXS40ConnectedStateTypeId);
    m_connectionStateTypeIds.insert(neuronXS50ThingClassId, neuronXS50ConnectedStateTypeId);
}

void IntegrationPluginUniPi::discoverThings(ThingDiscoveryInfo *info)
{
    ThingClassId thingClassId = info->thingClassId();

    if (thingClassId == digitalInputThingClassId) {
        foreach(Thing *parent, myThings()) {
            if ((parent->thingClassId() == uniPi1ThingClassId) || (parent->thingClassId() == uniPi1LiteThingClassId)) {
                foreach (QString circuit, m_unipi->digitalInputs()) {
                    ThingDescriptor descriptor(digitalInputThingClassId, QString("Digital input %1").arg(circuit), "UniPi 1", parent->id());
                    foreach(Thing *thing, myThings().filterByParentId(parent->id())) {
                        if (thing->paramValue(digitalInputThingCircuitParamTypeId) == circuit) {
                            qCDebug(dcUniPi()) << "Found already added Circuit:" << circuit;
                            descriptor.setThingId(thing->id());
                            break;
                        }
                    }
                    ParamList params;
                    params.append(Param(digitalInputThingCircuitParamTypeId, circuit));
                    descriptor.setParams(params);
                    info->addThingDescriptor(descriptor);
                }
                break;
            }
        }

        foreach (NeuronExtension *neuronExtension, m_neuronExtensions) {
            ThingId parentDeviceId = m_neuronExtensions.key(neuronExtension);
            foreach (QString circuit, neuronExtension->digitalInputs()) {
                ThingDescriptor descriptor(digitalInputThingClassId, QString("Digital input %1").arg(circuit), QString("Neuron extension %1, slave address %2").arg(neuronExtension->type()).arg(QString::number(neuronExtension->slaveAddress())), parentDeviceId);
                foreach(Thing *thing, myThings().filterByParentId(m_neuronExtensions.key(neuronExtension))) {
                    if (thing->paramValue(digitalInputThingCircuitParamTypeId) == circuit) {
                        qCDebug(dcUniPi()) << "Found already added Circuit:" << circuit;
                        descriptor.setThingId(thing->id());
                        break;
                    }
                }
                ParamList params;
                params.append(Param(digitalInputThingCircuitParamTypeId, circuit));
                descriptor.setParams(params);
                info->addThingDescriptor(descriptor);
            }
        }

        foreach (Neuron *neuron, m_neurons) {
            ThingId parentDeviceId = m_neurons.key(neuron);
            foreach (QString circuit, neuron->digitalInputs()) {
                ThingDescriptor descriptor(digitalInputThingClassId, QString("Digital input %1").arg(circuit), QString("Neuron %1").arg(neuron->type()), parentDeviceId);
                foreach(Thing *thing, myThings().filterByParentId(m_neurons.key(neuron))) {
                    if (thing->paramValue(digitalInputThingCircuitParamTypeId) == circuit) {
                        qCDebug(dcUniPi()) << "Found already added Circuit:" << circuit << parentDeviceId;
                        descriptor.setThingId(thing->id());
                        break;
                    }
                }
                ParamList params;
                params.append(Param(digitalInputThingCircuitParamTypeId, circuit));
                descriptor.setParams(params);
                info->addThingDescriptor(descriptor);
            }
        }
        return info->finish(Thing::ThingErrorNoError);
    } else if (thingClassId == digitalOutputThingClassId) {
        foreach(Thing *parent, myThings()) {
            if ((parent->thingClassId() == uniPi1ThingClassId) || (parent->thingClassId() == uniPi1LiteThingClassId)) {
                foreach (QString circuit, m_unipi->digitalOutputs()) {
                    ThingDescriptor descriptor(digitalOutputThingClassId, QString("Digital output %1").arg(circuit), "UniPi 1", parent->id());
                    foreach(Thing *thing, myThings().filterByParentId(parent->id())) {
                        if (thing->paramValue(digitalOutputThingCircuitParamTypeId) == circuit) {
                            qCDebug(dcUniPi()) << "Found already added Circuit:" << circuit << parent->id();
                            descriptor.setThingId(thing->id());
                            break;
                        }
                    }
                    ParamList params;
                    params.append(Param(digitalOutputThingCircuitParamTypeId, circuit));
                    descriptor.setParams(params);
                    info->addThingDescriptor(descriptor);
                }
                break;
            }
        }

        foreach (NeuronExtension *neuronExtension, m_neuronExtensions) {
            ThingId parentDeviceId = m_neuronExtensions.key(neuronExtension);
            foreach (QString circuit, neuronExtension->digitalOutputs()) {
                ThingDescriptor descriptor(digitalOutputThingClassId, QString("Digital output %1").arg(circuit), QString("Neuron extension %1, Slave address %2").arg(neuronExtension->type()).arg(QString::number(neuronExtension->slaveAddress())), parentDeviceId);
                foreach(Thing *thing, myThings().filterByParentId(m_neuronExtensions.key(neuronExtension))) {
                    if (thing->paramValue(digitalOutputThingCircuitParamTypeId) == circuit) {
                        qCDebug(dcUniPi()) << "Found already added Circuit:" << circuit << parentDeviceId;
                        descriptor.setThingId(thing->id());
                        break;
                    }
                }
                ParamList params;
                params.append(Param(digitalOutputThingCircuitParamTypeId, circuit));
                descriptor.setParams(params);
                info->addThingDescriptor(descriptor);
            }
        }

        foreach (Neuron *neuron, m_neurons) {
            ThingId parentDeviceId = m_neurons.key(neuron);
            foreach (QString circuit, neuron->digitalOutputs()) {
                ThingDescriptor descriptor(digitalOutputThingClassId, QString("Digital output %1").arg(circuit), QString("Neuron %1").arg(neuron->type()), parentDeviceId);
                foreach(Thing *thing, myThings().filterByParentId(m_neurons.key(neuron))) {
                    if (thing->paramValue(digitalOutputThingCircuitParamTypeId) == circuit) {
                        qCDebug(dcUniPi()) << "Found already added Circuit:" << circuit << parentDeviceId;
                        descriptor.setThingId(thing->id());
                        break;
                    }
                }
                ParamList params;
                params.append(Param(digitalOutputThingCircuitParamTypeId, circuit));
                descriptor.setParams(params);
                info->addThingDescriptor(descriptor);
            }
        }
        return info->finish(Thing::ThingErrorNoError);
    } else if (thingClassId == analogInputThingClassId) {
        foreach(Thing *parent, myThings()) {
            if ((parent->thingClassId() == uniPi1ThingClassId) || (parent->thingClassId() == uniPi1LiteThingClassId)) {
                foreach (QString circuit, m_unipi->analogInputs()) {
                    ThingDescriptor descriptor(analogInputThingClassId, QString("Analog input %1").arg(circuit), "UniPi", parent->id());
                    foreach(Thing *thing, myThings().filterByParentId(parent->id())) {
                        if (thing->paramValue(analogInputThingCircuitParamTypeId) == circuit) {
                            qCDebug(dcUniPi()) << "Found already added Circuit:" << circuit << parent->id();
                            descriptor.setThingId(thing->id());
                            break;
                        }
                    }
                    ParamList params;
                    params.append(Param(analogInputThingCircuitParamTypeId, circuit));
                    descriptor.setParams(params);
                    info->addThingDescriptor(descriptor);
                }
                break;
            }
        }

        foreach (NeuronExtension *neuronExtension, m_neuronExtensions) {
            ThingId parentDeviceId = m_neuronExtensions.key(neuronExtension);
            foreach (QString circuit, neuronExtension->analogInputs()) {
                ThingDescriptor descriptor(analogInputThingClassId, QString("Analog input %1").arg(circuit), QString("Neuron extension %1, Slave address %2").arg(neuronExtension->type()).arg(QString::number(neuronExtension->slaveAddress())), parentDeviceId);
                foreach(Thing *thing, myThings().filterByParentId(m_neuronExtensions.key(neuronExtension))) {
                    if (thing->paramValue(analogInputThingCircuitParamTypeId) == circuit) {
                        qCDebug(dcUniPi()) << "Found already added Circuit:" << circuit << parentDeviceId;
                        descriptor.setThingId(thing->id());
                        break;
                    }
                }
                ParamList params;
                params.append(Param(analogInputThingCircuitParamTypeId, circuit));
                descriptor.setParams(params);
                info->addThingDescriptor(descriptor);
            }
        }

        foreach (Neuron *neuron, m_neurons) {
            ThingId parentDeviceId = m_neurons.key(neuron);
            foreach (QString circuit, neuron->analogInputs()) {
                ThingDescriptor descriptor(analogInputThingClassId, QString("Analog input %1").arg(circuit), QString("Neuron %1").arg(neuron->type()), parentDeviceId);
                foreach(Thing *thing, myThings().filterByParentId(m_neurons.key(neuron))) {
                    if (thing->paramValue(analogInputThingCircuitParamTypeId) == circuit) {
                        qCDebug(dcUniPi()) << "Found already added Circuit:" << circuit << parentDeviceId;
                        descriptor.setThingId(thing->id());
                        break;
                    }
                }
                ParamList params;
                params.append(Param(analogInputThingCircuitParamTypeId, circuit));
                descriptor.setParams(params);
                info->addThingDescriptor(descriptor);
            }
        }
        return info->finish(Thing::ThingErrorNoError);
    } else if (thingClassId == analogOutputThingClassId) {
        //TODO add unipi 1 and unipi 1 lite

        foreach (NeuronExtension *neuronExtension, m_neuronExtensions) {
            ThingId parentDeviceId = m_neuronExtensions.key(neuronExtension);
            foreach (QString circuit, neuronExtension->analogOutputs()) {
                ThingDescriptor descriptor(analogOutputThingClassId, QString("Analog output %1").arg(circuit), QString("Neuron extension %1, Slave address %2").arg(neuronExtension->type()).arg(QString::number(neuronExtension->slaveAddress())), parentDeviceId);
                foreach(Thing *thing, myThings().filterByParentId(m_neuronExtensions.key(neuronExtension))) {
                    if (thing->paramValue(analogOutputThingCircuitParamTypeId) == circuit) {
                        qCDebug(dcUniPi()) << "Found already added Circuit:" << circuit << parentDeviceId;
                        descriptor.setThingId(thing->id());
                        break;
                    }
                }
                ParamList params;
                params.append(Param(analogOutputThingCircuitParamTypeId, circuit));
                descriptor.setParams(params);
                info->addThingDescriptor(descriptor);
            }
        }

        foreach (Neuron *neuron, m_neurons) {
            ThingId parentDeviceId = m_neurons.key(neuron);
            foreach (QString circuit, neuron->analogOutputs()) {
                ThingDescriptor descriptor(analogOutputThingClassId, QString("Analog output %1").arg(circuit), QString("Neuron %1").arg(neuron->type()), parentDeviceId);
                foreach(Thing *thing, myThings().filterByParentId(m_neurons.key(neuron))) {
                    if (thing->paramValue(analogOutputThingCircuitParamTypeId) == circuit) {
                        qCDebug(dcUniPi()) << "Found already added Circuit:" << circuit << parentDeviceId;
                        descriptor.setThingId(thing->id());
                        break;
                    }
                }
                ParamList params;
                params.append(Param(analogOutputThingCircuitParamTypeId, circuit));
                descriptor.setParams(params);
                info->addThingDescriptor(descriptor);
            }
        }
        return info->finish(Thing::ThingErrorNoError);
    } else if (thingClassId == userLEDThingClassId) {
        QList<ThingDescriptor> deviceDescriptors;
        foreach (NeuronExtension *neuronExtension, m_neuronExtensions) {
            ThingId parentDeviceId = m_neuronExtensions.key(neuronExtension);
            foreach (QString circuit, neuronExtension->userLEDs()) {
                ThingDescriptor descriptor(userLEDThingClassId, QString("User programmable LED %1").arg(circuit), QString("Neuron extension %1, Slave address %2").arg(neuronExtension->type()).arg(neuronExtension->slaveAddress()), parentDeviceId);
                foreach(Thing *thing, myThings().filterByParentId(m_neuronExtensions.key(neuronExtension))) {
                    if (thing->paramValue(userLEDThingCircuitParamTypeId) == circuit) {
                        qCDebug(dcUniPi()) << "Found already added Circuit:" << circuit << parentDeviceId;
                        descriptor.setThingId(thing->id());
                        break;
                    }
                }
                ParamList params;
                params.append(Param(userLEDThingCircuitParamTypeId, circuit));
                descriptor.setParams(params);
                info->addThingDescriptor(descriptor);
            }
        }

        foreach (Neuron *neuron, m_neurons) {
            ThingId parentDeviceId = m_neurons.key(neuron);
            foreach (QString circuit, neuron->userLEDs()) {
                ThingDescriptor descriptor(userLEDThingClassId, QString("User programmable LED %1").arg(circuit), QString("Neuron %1").arg(neuron->type()), parentDeviceId);
                foreach(Thing *thing, myThings().filterByParentId(m_neurons.key(neuron))) {
                    if (thing->paramValue(userLEDThingCircuitParamTypeId) == circuit) {
                        qCDebug(dcUniPi()) << "Found already added Circuit:" << circuit << parentDeviceId;
                        descriptor.setThingId(thing->id());
                        break;
                    }
                }
                ParamList params;
                params.append(Param(userLEDThingCircuitParamTypeId, circuit));
                descriptor.setParams(params);
                info->addThingDescriptor(descriptor);
            }
        }
        return info->finish(Thing::ThingErrorNoError);
    } else {
        qCWarning(dcUniPi()) << "Unhandled device class in discoverDevice" << thingClassId;
        return info->finish(Thing::ThingErrorThingClassNotFound);
    }
}

void IntegrationPluginUniPi::setupThing(ThingSetupInfo *info)
{
    Thing *thing = info->thing();

    if(thing->thingClassId() == uniPi1ThingClassId
            || thing->thingClassId() == uniPi1LiteThingClassId) {

        if (m_unipi)
            return info->finish(Thing::ThingErrorSetupFailed, QT_TR_NOOP("There is already a UniPi gateway in the system.")); //only one parent device allowed

        if(thing->thingClassId() == uniPi1ThingClassId) {
            m_unipi = new UniPi(UniPi::UniPiType::UniPi1, this);
        } else if (thing->thingClassId() == uniPi1LiteThingClassId) {
            m_unipi = new UniPi(UniPi::UniPiType::UniPi1Lite, this);
        }

        if (!m_unipi->init()) {
            qCWarning(dcUniPi()) << "Could not setup UniPi";
            m_unipi->deleteLater();
            m_unipi = nullptr;
            return info->finish(Thing::ThingErrorSetupFailed, QT_TR_NOOP("Error setting up UniPi."));
        }
        connect(m_unipi, &UniPi::digitalInputStatusChanged, this, &IntegrationPluginUniPi::onUniPiDigitalInputStatusChanged);
        connect(m_unipi, &UniPi::digitalOutputStatusChanged, this, &IntegrationPluginUniPi::onUniPiDigitalOutputStatusChanged);
        connect(m_unipi, &UniPi::analogInputStatusChanged, this, &IntegrationPluginUniPi::onUniPiAnalogInputStatusChanged);
        connect(m_unipi, &UniPi::analogOutputStatusChanged, this, &IntegrationPluginUniPi::onUniPiAnalogOutputStatusChanged);
        thing->setStateValue(m_connectionStateTypeIds.value(thing->thingClassId()), true);

        return info->finish(Thing::ThingErrorNoError);
    } else if(thing->thingClassId() == neuronS103ThingClassId ||
            thing->thingClassId() == neuronM103ThingClassId ||
            thing->thingClassId() == neuronM203ThingClassId ||
            thing->thingClassId() == neuronM303ThingClassId ||
            thing->thingClassId() == neuronM403ThingClassId ||
            thing->thingClassId() == neuronL203ThingClassId ||
            thing->thingClassId() == neuronL303ThingClassId ||
            thing->thingClassId() == neuronL403ThingClassId ||
            thing->thingClassId() == neuronL503ThingClassId ||
            thing->thingClassId() == neuronL513ThingClassId) {

        if (!neuronDeviceInit())
            return info->finish(Thing::ThingErrorSetupFailed, QT_TR_NOOP("Error setting up Neuron."));

        Neuron *neuron;
        if (thing->thingClassId() == neuronS103ThingClassId) {
            neuron = new Neuron(Neuron::NeuronTypes::S103, m_modbusTCPMaster, this);
        } else  if (thing->thingClassId() == neuronM103ThingClassId) {
            neuron = new Neuron(Neuron::NeuronTypes::M103, m_modbusTCPMaster, this);
        } else if (thing->thingClassId() == neuronM203ThingClassId) {
            neuron = new Neuron(Neuron::NeuronTypes::M203, m_modbusTCPMaster, this);
        } else if (thing->thingClassId() == neuronM303ThingClassId) {
            neuron = new Neuron(Neuron::NeuronTypes::M303, m_modbusTCPMaster, this);
        } else if (thing->thingClassId() == neuronM403ThingClassId) {
            neuron = new Neuron(Neuron::NeuronTypes::M403, m_modbusTCPMaster, this);
        } else if (thing->thingClassId() == neuronM503ThingClassId) {
            neuron = new Neuron(Neuron::NeuronTypes::M503, m_modbusTCPMaster, this);
        } else if (thing->thingClassId() == neuronL203ThingClassId) {
            neuron = new Neuron(Neuron::NeuronTypes::L203, m_modbusTCPMaster, this);
        } else if (thing->thingClassId() == neuronL303ThingClassId) {
            neuron = new Neuron(Neuron::NeuronTypes::L303, m_modbusTCPMaster, this);
        } else if (thing->thingClassId() == neuronL403ThingClassId) {
            neuron = new Neuron(Neuron::NeuronTypes::L403, m_modbusTCPMaster, this);
        } else if (thing->thingClassId() == neuronL503ThingClassId) {
            neuron = new Neuron(Neuron::NeuronTypes::L503, m_modbusTCPMaster, this);
        } else  if (thing->thingClassId() == neuronL513ThingClassId) {
            neuron = new Neuron(Neuron::NeuronTypes::L513, m_modbusTCPMaster, this);
        } else {
            return info->finish(Thing::ThingErrorSetupFailed, QT_TR_NOOP("Error unrecognized Neuron type."));
        }

        if (!neuron->init()) {
            qCWarning(dcUniPi()) << "Could not load the modbus map";
            neuron->deleteLater();
            neuron = nullptr;
            return info->finish(Thing::ThingErrorSetupFailed, QT_TR_NOOP("Error setting up Neuron device."));
        }
        m_neurons.insert(thing->id(), neuron);
        connect(neuron, &Neuron::requestExecuted, this, &IntegrationPluginUniPi::onRequestExecuted);
        connect(neuron, &Neuron::requestError, this, &IntegrationPluginUniPi::onRequestError);
        connect(neuron, &Neuron::connectionStateChanged, this, &IntegrationPluginUniPi::onNeuronConnectionStateChanged);
        connect(neuron, &Neuron::digitalInputStatusChanged, this, &IntegrationPluginUniPi::onNeuronDigitalInputStatusChanged);
        connect(neuron, &Neuron::digitalOutputStatusChanged, this, &IntegrationPluginUniPi::onNeuronDigitalOutputStatusChanged);
        connect(neuron, &Neuron::analogInputStatusChanged, this, &IntegrationPluginUniPi::onNeuronAnalogInputStatusChanged);
        connect(neuron, &Neuron::analogOutputStatusChanged, this, &IntegrationPluginUniPi::onNeuronAnalogOutputStatusChanged);
        connect(neuron, &Neuron::userLEDStatusChanged, this, &IntegrationPluginUniPi::onNeuronUserLEDStatusChanged);

        thing->setStateValue(m_connectionStateTypeIds.value(thing->thingClassId()), (m_modbusTCPMaster->state() == QModbusDevice::ConnectedState));

        return info->finish(Thing::ThingErrorNoError);
    } else if(thing->thingClassId() == neuronXS10ThingClassId ||
            thing->thingClassId() == neuronXS20ThingClassId ||
            thing->thingClassId() == neuronXS30ThingClassId ||
            thing->thingClassId() == neuronXS40ThingClassId ||
            thing->thingClassId() == neuronXS50ThingClassId) {

        if (!neuronExtensionInterfaceInit())
            return info->finish(Thing::ThingErrorSetupFailed, QT_TR_NOOP("Error setting up Neuron."));

        int slaveAddress;
        NeuronExtension *neuronExtension;

        if(thing->thingClassId() == neuronXS10ThingClassId) {
            slaveAddress = thing->paramValue(neuronXS10ThingSlaveAddressParamTypeId).toInt();
            neuronExtension = new NeuronExtension(NeuronExtension::ExtensionTypes::xS10, m_modbusRTUMaster, slaveAddress, this);
        } else if (thing->thingClassId() == neuronXS20ThingClassId) {
            slaveAddress = thing->paramValue(neuronXS20ThingSlaveAddressParamTypeId).toInt();
            neuronExtension = new NeuronExtension(NeuronExtension::ExtensionTypes::xS20, m_modbusRTUMaster, slaveAddress, this);
        } else if (thing->thingClassId() == neuronXS30ThingClassId) {
            slaveAddress = thing->paramValue(neuronXS30ThingSlaveAddressParamTypeId).toInt();
            neuronExtension = new NeuronExtension(NeuronExtension::ExtensionTypes::xS30, m_modbusRTUMaster, slaveAddress, this);
        } else if (thing->thingClassId() == neuronXS40ThingClassId) {
            slaveAddress = thing->paramValue(neuronXS40ThingSlaveAddressParamTypeId).toInt();
            neuronExtension = new NeuronExtension(NeuronExtension::ExtensionTypes::xS40, m_modbusRTUMaster, slaveAddress, this);
        } else if (thing->thingClassId() == neuronXS50ThingClassId) {
            slaveAddress = thing->paramValue(neuronXS50ThingSlaveAddressParamTypeId).toInt();
            neuronExtension = new NeuronExtension(NeuronExtension::ExtensionTypes::xS50, m_modbusRTUMaster, slaveAddress, this);
        } else {
            return info->finish(Thing::ThingErrorSetupFailed, QT_TR_NOOP("Error unrecognized extension type."));
        }
        if (!neuronExtension->init()) {
            qCWarning(dcUniPi()) << "Could not load the modbus map";
            neuronExtension->deleteLater();
            neuronExtension = nullptr;
            return info->finish(Thing::ThingErrorSetupFailed, QT_TR_NOOP("Error loading modbus map."));
        }
        connect(neuronExtension, &NeuronExtension::requestExecuted, this, &IntegrationPluginUniPi::onRequestExecuted);
        connect(neuronExtension, &NeuronExtension::requestError, this, &IntegrationPluginUniPi::onRequestError);
        connect(neuronExtension, &NeuronExtension::connectionStateChanged, this, &IntegrationPluginUniPi::onNeuronExtensionConnectionStateChanged);
        connect(neuronExtension, &NeuronExtension::digitalInputStatusChanged, this, &IntegrationPluginUniPi::onNeuronExtensionDigitalInputStatusChanged);
        connect(neuronExtension, &NeuronExtension::digitalOutputStatusChanged, this, &IntegrationPluginUniPi::onNeuronExtensionDigitalOutputStatusChanged);
        connect(neuronExtension, &NeuronExtension::analogInputStatusChanged, this, &IntegrationPluginUniPi::onNeuronExtensionAnalogInputStatusChanged);
        connect(neuronExtension, &NeuronExtension::analogOutputStatusChanged, this, &IntegrationPluginUniPi::onNeuronExtensionAnalogOutputStatusChanged);
        connect(neuronExtension, &NeuronExtension::userLEDStatusChanged, this, &IntegrationPluginUniPi::onNeuronExtensionUserLEDStatusChanged);

        m_neuronExtensions.insert(thing->id(), neuronExtension);
        thing->setStateValue(m_connectionStateTypeIds.value(thing->thingClassId()), (m_modbusRTUMaster->state() == QModbusDevice::ConnectedState));

        return info->finish(Thing::ThingErrorNoError);
    } else if (thing->thingClassId() == digitalOutputThingClassId) {
        return info->finish(Thing::ThingErrorNoError);
    } else if (thing->thingClassId() == digitalInputThingClassId) {
        return info->finish(Thing::ThingErrorNoError);
    } else if (thing->thingClassId() == userLEDThingClassId) {
        return info->finish(Thing::ThingErrorNoError);
    } else if (thing->thingClassId() == analogInputThingClassId) {
        return info->finish(Thing::ThingErrorNoError);
    } else if (thing->thingClassId() == analogOutputThingClassId) {
        return info->finish(Thing::ThingErrorNoError);
    } else {
        qCWarning(dcUniPi()) << "Unhandled device class in setupDevice:" << thing->thingClassId();
        return info->finish(Thing::ThingErrorThingClassNotFound);
    }
}

void IntegrationPluginUniPi::postSetupThing(Thing *thing)
{
    Q_UNUSED(thing)

    if (!m_reconnectTimer) {
        m_reconnectTimer = new QTimer(this);
        m_reconnectTimer->setSingleShot(true);
        connect(m_reconnectTimer, &QTimer::timeout, this, &IntegrationPluginUniPi::onReconnectTimer);
    }
}


void IntegrationPluginUniPi::executeAction(ThingActionInfo *info)
{
    Thing *thing = info->thing();
    Action action = info->action();

    if (thing->thingClassId() == digitalOutputThingClassId)  {

        if (action.actionTypeId() == digitalOutputPowerActionTypeId) {
            QString digitalOutputNumber = thing->paramValue(digitalOutputThingCircuitParamTypeId).toString();
            bool stateValue = action.param(digitalOutputPowerActionPowerParamTypeId).value().toBool();

            if (m_unipi) {
                m_unipi->setDigitalOutput(digitalOutputNumber, stateValue);
            } else if (m_neurons.contains(thing->parentId())) {
                Neuron *neuron = m_neurons.value(thing->parentId());
                QUuid requestId = neuron->setDigitalOutput(digitalOutputNumber, stateValue);
                if (requestId.isNull()) {
                    info->finish(Thing::ThingErrorHardwareFailure);
                } else {
                    m_asyncActions.insert(requestId, info);
                    connect(info, &ThingActionInfo::aborted, this, [requestId, this](){m_asyncActions.remove(requestId);});
                }
                return;
            } else if (m_neuronExtensions.contains(thing->parentId())) {
                NeuronExtension *neuronExtension = m_neuronExtensions.value(thing->parentId());
                QUuid requestId = neuronExtension->setDigitalOutput(digitalOutputNumber, stateValue);
                if (requestId.isNull()) {
                    info->finish(Thing::ThingErrorHardwareFailure);
                } else {
                    m_asyncActions.insert(requestId, info);
                    connect(info, &ThingActionInfo::aborted, this, [requestId, this](){m_asyncActions.remove(requestId);});
                }
                return;
            } else {
                qCWarning(dcUniPi()) << "Hardware not initilized" << thing->name();
                return info->finish(Thing::ThingErrorHardwareFailure);
            }
        } else {
            qCWarning(dcUniPi()) << "Unhandled ActionTypeId" << action.actionTypeId();
            return info->finish(Thing::ThingErrorActionTypeNotFound);
        }
    } else if (thing->thingClassId() == analogOutputThingClassId) {

        if (action.actionTypeId() == analogOutputOutputValueActionTypeId) {
            QString analogOutputNumber = thing->paramValue(analogOutputThingCircuitParamTypeId).toString();
            double analogValue = action.param(analogOutputOutputValueActionOutputValueParamTypeId).value().toDouble();

            if (m_unipi) {
                m_unipi->setAnalogOutput(analogOutputNumber, analogValue);
            } else if (m_neurons.contains(thing->parentId())) {
                Neuron *neuron = m_neurons.value(thing->parentId());
                QUuid requestId = neuron->setAnalogOutput(analogOutputNumber, analogValue);
                if (requestId.isNull()) {
                    info->finish(Thing::ThingErrorHardwareFailure);
                } else {
                    m_asyncActions.insert(requestId, info);
                    connect(info, &ThingActionInfo::aborted, this, [requestId, this](){m_asyncActions.remove(requestId);});
                }
                return;
            } else if (m_neuronExtensions.contains(thing->parentId())) {
                NeuronExtension *neuronExtension = m_neuronExtensions.value(thing->parentId());
                QUuid requestId = neuronExtension->setAnalogOutput(analogOutputNumber, analogValue);
                if (requestId.isNull()) {
                    info->finish(Thing::ThingErrorHardwareFailure);
                } else {
                    m_asyncActions.insert(requestId, info);
                    connect(info, &ThingActionInfo::aborted, this, [requestId, this](){m_asyncActions.remove(requestId);});
                }
                return;
            } else {
                qCWarning(dcUniPi()) << "Hardware not initilized" << thing->name();
                return info->finish(Thing::ThingErrorHardwareFailure);
            }
        } else {
            qCWarning(dcUniPi()) << "Unhandled ActionTypeId" << action.actionTypeId();
            return info->finish(Thing::ThingErrorActionTypeNotFound);
        }
    } else if (thing->thingClassId() == userLEDThingClassId) {
        if (action.actionTypeId() == userLEDPowerActionTypeId) {
            QString userLED = thing->paramValue(userLEDThingCircuitParamTypeId).toString();
            bool stateValue = action.param(userLEDPowerActionPowerParamTypeId).value().toBool();
            if (m_neurons.contains(thing->parentId())) {
                Neuron *neuron = m_neurons.value(thing->parentId());
                QUuid requestId = neuron->setUserLED(userLED, stateValue);
                if (requestId.isNull()) {
                    info->finish(Thing::ThingErrorHardwareFailure);
                } else {
                    m_asyncActions.insert(requestId, info);
                    connect(info, &ThingActionInfo::aborted, this, [requestId, this](){m_asyncActions.remove(requestId);});
                }
                return;
            } else if (m_neuronExtensions.contains(thing->parentId())) {
                NeuronExtension *neuronExtension = m_neuronExtensions.value(thing->parentId());
                QUuid requestId = neuronExtension->setUserLED(userLED, stateValue);
                if (requestId.isNull()) {
                    info->finish(Thing::ThingErrorHardwareFailure);
                } else {
                    m_asyncActions.insert(requestId, info);
                    connect(info, &ThingActionInfo::aborted, this, [requestId, this](){m_asyncActions.remove(requestId);});
                }
                return;
            } else {
                qCWarning(dcUniPi()) << "Hardware not initilized" << thing->name();
                return info->finish(Thing::ThingErrorHardwareFailure);
            }
        } else {
            qCWarning(dcUniPi()) << "Unhandled ActionTypeId" << action.actionTypeId();
            return info->finish(Thing::ThingErrorActionTypeNotFound);
        }
    } else {
        qCWarning(dcUniPi()) << "Unhandled device class in executeAction" << thing->thingClassId();
        info->finish(Thing::ThingErrorThingClassNotFound);
    }
}


void IntegrationPluginUniPi::thingRemoved(Thing *thing)
{
    if(m_neurons.contains(thing->id())) {
        Neuron *neuron = m_neurons.take(thing->id());
        neuron->deleteLater();
    } else if(m_neuronExtensions.contains(thing->id())) {
        NeuronExtension *neuronExtension = m_neuronExtensions.take(thing->id());
        neuronExtension->deleteLater();
    } else if ((thing->thingClassId() == uniPi1ThingClassId) || (thing->thingClassId() == uniPi1LiteThingClassId)) {
        if(m_unipi) {
            m_unipi->deleteLater();
            m_unipi = nullptr;
        }
    }

    if (myThings().isEmpty()) {
        if (m_reconnectTimer) {
            m_reconnectTimer->stop();
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

void IntegrationPluginUniPi::onPluginConfigurationChanged(const ParamTypeId &paramTypeId, const QVariant &value)
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

void IntegrationPluginUniPi::onNeuronConnectionStateChanged(bool state)
{
    Neuron *neuron = static_cast<Neuron *>(sender());
    Thing *thing = myThings().findById(m_neurons.key(neuron));
    if (!thing) {
        qCWarning(dcUniPi()) << "Could not find any device associated to Neuron obejct";
        return;
    }
    thing->setStateValue(m_connectionStateTypeIds.value(thing->thingClassId()), state);
}

void IntegrationPluginUniPi::onNeuronDigitalInputStatusChanged(QString &circuit, bool value)
{
    Neuron *neuron = static_cast<Neuron *>(sender());

    foreach(Thing *thing, myThings().filterByParentId(m_neurons.key(neuron))) {
        if (thing->thingClassId() == digitalInputThingClassId) {
            if (thing->paramValue(digitalInputThingCircuitParamTypeId).toString() == circuit) {

                thing->setStateValue(digitalInputInputStatusStateTypeId, value);
                return;
            }
        }
    }
}

void IntegrationPluginUniPi::onNeuronExtensionDigitalInputStatusChanged(QString &circuit, bool value)
{
    NeuronExtension *neuronExtension = static_cast<NeuronExtension *>(sender());

    foreach(Thing *thing, myThings().filterByParentId(m_neuronExtensions.key(neuronExtension))) {
        if (thing->thingClassId() == digitalInputThingClassId) {
            if (thing->paramValue(digitalInputThingCircuitParamTypeId).toString() == circuit) {

                thing->setStateValue(digitalInputInputStatusStateTypeId, value);
                return;
            }
        }
    }
}

void IntegrationPluginUniPi::onNeuronDigitalOutputStatusChanged(QString &circuit, bool value)
{
    Neuron *neuron = static_cast<Neuron *>(sender());

    foreach(Thing *thing, myThings().filterByParentId(m_neurons.key(neuron))) {
        if (thing->thingClassId() == digitalOutputThingClassId) {
            if (thing->paramValue(digitalOutputThingCircuitParamTypeId).toString() == circuit) {

                thing->setStateValue(digitalOutputPowerStateTypeId, value);
                return;
            }
        }
    }
}

void IntegrationPluginUniPi::onNeuronExtensionDigitalOutputStatusChanged(QString &circuit, bool value)
{
    NeuronExtension *neuronExtension = static_cast<NeuronExtension *>(sender());

    foreach(Thing *thing, myThings().filterByParentId(m_neuronExtensions.key(neuronExtension))) {
        if (thing->thingClassId() == digitalOutputThingClassId) {
            if (thing->paramValue(digitalOutputThingCircuitParamTypeId).toString() == circuit) {

                thing->setStateValue(digitalOutputPowerStateTypeId, value);
                return;
            }
        }
    }
}

void IntegrationPluginUniPi::onNeuronAnalogInputStatusChanged(QString &circuit, double value)
{
    Neuron *neuron = static_cast<Neuron *>(sender());

    foreach(Thing *thing, myThings().filterByParentId(m_neurons.key(neuron))) {
        if (thing->thingClassId() == analogInputThingClassId) {
            if (thing->paramValue(analogInputThingCircuitParamTypeId).toString() == circuit) {

                thing->setStateValue(analogInputInputValueStateTypeId, value);
                return;
            }
        }
    }
}

void IntegrationPluginUniPi::onNeuronExtensionAnalogInputStatusChanged(QString &circuit, double value)
{
    NeuronExtension *neuronExtension = static_cast<NeuronExtension *>(sender());

    foreach(Thing *thing, myThings().filterByParentId(m_neuronExtensions.key(neuronExtension))) {
        if (thing->thingClassId() == analogInputThingClassId) {
            if (thing->paramValue(analogInputThingCircuitParamTypeId).toString() == circuit) {
                thing->setStateValue(analogInputInputValueStateTypeId, value);
                return;
            }
        }
    }
}

void IntegrationPluginUniPi::onNeuronAnalogOutputStatusChanged(QString &circuit, double value)
{
    Neuron *neuron = static_cast<Neuron *>(sender());

    foreach(Thing *thing, myThings().filterByParentId(m_neurons.key(neuron))) {
        if (thing->thingClassId() == analogOutputThingClassId) {
            if (thing->paramValue(analogOutputThingCircuitParamTypeId).toString() == circuit) {

                thing->setStateValue(analogOutputOutputValueStateTypeId, value);
                return;
            }
        }
    }
}

void IntegrationPluginUniPi::onNeuronExtensionAnalogOutputStatusChanged(QString &circuit, double value)
{
    NeuronExtension *neuronExtension = static_cast<NeuronExtension *>(sender());

    foreach(Thing *thing, myThings().filterByParentId(m_neuronExtensions.key(neuronExtension))) {
        if (thing->thingClassId() == analogOutputThingClassId) {
            if (thing->paramValue(analogOutputThingCircuitParamTypeId).toString() == circuit) {

                thing->setStateValue(analogOutputOutputValueStateTypeId, value);
                return;
            }
        }
    }
}

void IntegrationPluginUniPi::onNeuronUserLEDStatusChanged(QString &circuit, bool value)
{
    Neuron *neuron = static_cast<Neuron *>(sender());

    foreach(Thing *thing, myThings().filterByParentId(m_neurons.key(neuron))) {
        if (thing->thingClassId() == userLEDThingClassId) {
            if (thing->paramValue(userLEDThingCircuitParamTypeId).toString() == circuit) {

                thing->setStateValue(userLEDPowerStateTypeId, value);
                return;
            }
        }
    }
}

void IntegrationPluginUniPi::onNeuronExtensionConnectionStateChanged(bool state)
{
    NeuronExtension *neuron = static_cast<NeuronExtension *>(sender());
    Thing *thing = myThings().findById(m_neuronExtensions.key(neuron));
    if (!thing) {
        qCWarning(dcUniPi()) << "Could not find any device associated to NeuronExtension obejct";
        return;
    }
    thing->setStateValue(m_connectionStateTypeIds.value(thing->thingClassId()), state);
}

void IntegrationPluginUniPi::onRequestExecuted(QUuid requestId, bool success)
{
    if (m_asyncActions.contains(requestId)){
        ThingActionInfo *info = m_asyncActions.take(requestId);
        if (success){
            info->finish(Thing::ThingErrorNoError);
        } else {
            info->finish(Thing::ThingErrorHardwareNotAvailable);
        }
    }
}

void IntegrationPluginUniPi::onRequestError(QUuid requestId, const QString &error)
{
    if (m_asyncActions.contains(requestId)){
        ThingActionInfo *info = m_asyncActions.take(requestId);
        info->finish(Thing::ThingErrorHardwareNotAvailable, error);
    }
}

void IntegrationPluginUniPi::onNeuronExtensionUserLEDStatusChanged(QString &circuit, bool value)
{
    NeuronExtension *neuronExtension = static_cast<NeuronExtension *>(sender());

    foreach(Thing *thing, myThings().filterByParentId(m_neuronExtensions.key(neuronExtension))) {
        if (thing->thingClassId() == userLEDThingClassId) {
            if (thing->paramValue(userLEDThingCircuitParamTypeId).toString() == circuit) {

                thing->setStateValue(userLEDPowerStateTypeId, value);
                return;
            }
        }
    }
}

void IntegrationPluginUniPi::onReconnectTimer()
{
    if(m_modbusRTUMaster) {
        if (!m_modbusRTUMaster->connectDevice()) {
            if (m_reconnectTimer)
                m_reconnectTimer->start(10000);
        }
    }
    if(m_modbusTCPMaster) {
        if (!m_modbusTCPMaster->connectDevice()) {
            if (m_reconnectTimer)
                m_reconnectTimer->start(10000);
        }
    }
}

void IntegrationPluginUniPi::onModbusTCPStateChanged(QModbusDevice::State state)
{
    bool connected = (state == QModbusDevice::State::ConnectedState);

    if (!connected) {
        //try to reconnect in 10 seconds
        if (m_reconnectTimer)
            m_reconnectTimer->start(10000);
    }
    qCDebug(dcUniPi()) << "Connection status changed:" << connected;
}

void IntegrationPluginUniPi::onModbusRTUStateChanged(QModbusDevice::State state)
{
    bool connected = (state == QModbusDevice::State::ConnectedState);

    if (!connected) {
        //try to reconnect in 10 seconds
        if (m_reconnectTimer)
            m_reconnectTimer->start(10000);
    }
    qCDebug(dcUniPi()) << "Connection status changed:" << connected;
}

void IntegrationPluginUniPi::onUniPiDigitalInputStatusChanged(const QString &circuit, bool value)
{
    qDebug(dcUniPi) << "Digital Input changed" << circuit << value;
    foreach(Thing *thing, myThings()) {
        if (thing->thingClassId() == digitalInputThingClassId) {
            if (thing->paramValue(digitalInputThingCircuitParamTypeId).toString() == circuit) {

                thing->setStateValue(digitalInputInputStatusStateTypeId, value);
                return;
            }
        }
    }
}

void IntegrationPluginUniPi::onUniPiDigitalOutputStatusChanged(const QString &circuit, bool value)
{
    qDebug(dcUniPi) << "Digital Output changed" << circuit << value;
    foreach(Thing *thing, myThings()) {
        if (thing->thingClassId() == digitalOutputThingClassId) {
            if (thing->paramValue(digitalOutputThingCircuitParamTypeId).toString() == circuit) {

                thing->setStateValue(digitalOutputPowerStateTypeId, value);
                return;
            }
        }
    }
}

void IntegrationPluginUniPi::onUniPiAnalogInputStatusChanged(const QString &circuit, double value)
{
    qDebug(dcUniPi) << "Digital Input changed" << circuit << value;
    foreach(Thing *thing, myThings()) {
        if (thing->thingClassId() == analogInputThingClassId) {
            if (thing->paramValue(analogInputThingCircuitParamTypeId).toString() == circuit) {
                thing->setStateValue(analogInputInputValueStateTypeId, value);
                return;
            }
        }
    }
}

void IntegrationPluginUniPi::onUniPiAnalogOutputStatusChanged(const QString &circuit, double value)
{
    foreach(Thing *parent, myThings()) {
        if ((parent->thingClassId() == uniPi1ThingClassId) || (parent->thingClassId() == uniPi1LiteThingClassId)) {
            foreach(Thing *thing, myThings().filterByParentId(parent->id())) {
                if (thing->thingClassId() == analogOutputThingClassId) {
                    if (thing->paramValue(analogOutputThingCircuitParamTypeId).toString() == circuit) {

                        thing->setStateValue(analogOutputOutputValueStateTypeId, value);
                        return;
                    }
                }
            }
            break;
        }
    }
}

bool IntegrationPluginUniPi::neuronDeviceInit()
{
    if(!m_modbusTCPMaster) {
        int port = configValue(uniPiPluginPortParamTypeId).toInt();;
        QHostAddress ipAddress = QHostAddress(configValue(uniPiPluginAddressParamTypeId).toString());

        m_modbusTCPMaster = new QModbusTcpClient(this);
        m_modbusTCPMaster->setConnectionParameter(QModbusDevice::NetworkPortParameter, port);
        m_modbusTCPMaster->setConnectionParameter(QModbusDevice::NetworkAddressParameter, ipAddress.toString());
        //m_modbusTCPMaster->setTimeout(100);
        //m_modbusTCPMaster->setNumberOfRetries(1);

        connect(m_modbusTCPMaster, &QModbusTcpClient::stateChanged, this, &IntegrationPluginUniPi::onModbusTCPStateChanged);

        if (!m_modbusTCPMaster->connectDevice()) {
            qCWarning(dcUniPi()) << "Connect failed:" << m_modbusTCPMaster->errorString();
            m_modbusTCPMaster->deleteLater();
            m_modbusTCPMaster = nullptr;
            return false;
        }
    }
    return true;
}

bool IntegrationPluginUniPi::neuronExtensionInterfaceInit()
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

        connect(m_modbusRTUMaster, &QModbusRtuSerialMaster::stateChanged, this, &IntegrationPluginUniPi::onModbusRTUStateChanged);

        if (!m_modbusRTUMaster->connectDevice()) {
            qCWarning(dcUniPi()) << "Connect failed:" << m_modbusRTUMaster->errorString();
            m_modbusRTUMaster->deleteLater();
            m_modbusRTUMaster = nullptr;
            return false;
        }
    }
    return true;
}
