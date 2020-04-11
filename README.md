# UniPi

Supports UniPi 1.1, UniPi Lite and all Neuron models as well as the Neuron extensions.

This plug-in does not make use of Evok, it communicates 
directly with the Modbus TCP layer, or over Modbus RTU with the Neuron extensions.
To add one-wire devices, use the nymea onewire plug-in.

## Supported Devices

* UniPi 1.1
* UniPi 1.1 light
* Neuron
	* all models
* Neuron Extensions
	* all models

General Features:
	* No internet connection required
	* Switch digital outputs
	* Read digital inputs
	* Switch user led
	* Set analog outputs
	* Read analog inputs

## Requirements

* UniPi 1.1 & UniPi 1.1 light
* Neuron
	* Neuron TCP modbus server must be installed.
* Neuron Extensions
	* Set the DIP settings accordind to the plug-in settings

* General requirements:
	* The package "nymea-plugin-unipi2" must be installed
	* For one-wire sensors the package "nymea-plugin-onewire" must be installed.


### Device setup for Neuron devices

Install the Neuron Modbus TCP Layer:

* wget https://github.com/UniPiTechnology/neuron-tcp-modbus-overlay/archive/v.1.0.3.zip
* unzip v.1.0.3.zip
* cd neuron-tcp-modbus-overlay-v.1.0.3/
* sudo bash install.sh

Before you can add IOs you need to setup the UniPi Gateway device, after that nymea
recognises the available IOs.

## More

https://www.unipi.technology
