# UniPi

Supports UniPi 1.1, UniPi Lite and all Neuron models as well as the Neuron extensions.

This plug-in does not make use of Evok, it communicates 
directly with the Modbus TCP layer, or over Modbus RTU with the Neuron extensions.
To add one-wire devices, use the nymea onewire plug-in.

## Device setup

Install the Neuron Modbus TCP Layer:

* wget https://github.com/UniPiTechnology/neuron-tcp-modbus-overlay/archive/v.1.0.3.zip
* unzip v.1.0.3.zip
* cd neuron-tcp-modbus-overlay-v.1.0.3/
* sudo bash install.sh

Before you can add IOs you need to setup the UniPi Gateway device, after that nymea
knows the available IOs.
