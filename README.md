# iot-button-emulator

A simple emulator for the AWS IoT Button implemeted in C++ for compilation on the Raspberry Pi.

Quick getting started guide (needs more work)

* Install & build the AWS IoT C SDK on your Raspberry Pi. Get it here https://github.com/aws/aws-iot-device-sdk-embedded-C

* Clone this repo in linux_mqtt_mbedtls-2.1.0/samples/linux/ with the other client samples
* Set up your thing on the AWS IoT console or use the command line and download your device certificates & public/private keys
* Create a directory called linux_mqtt_mbedtls-2.1.0/samples/linux/certs  
* Put your downloaded device specific certificate, the CA root certificate and public/private keys in linux_mqtt_mbedtls-2.1.0/samples/linux/certs
* Edit aws_iot_config.h with the correct filenames for your certificates and keys, your device serial number, and your account specific AWS IoT endpoint
* Build with the make command
* Replicate the wiring setup shown in iot-button-emulator-wiring.jpeg
* run ./iot-button-emulator

* subscribe to the iot-button-emulator/+ topic with the AWS IoT Console

Press the connected button and watch the JSON messages flow
