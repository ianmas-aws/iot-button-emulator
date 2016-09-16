# iot-button-emulator

A simple emulator for the AWS IoT Button implemeted in C++ for compilation on the Raspberry Pi.

Quick getting started guide (needs more work) - assumes you have Raspbian Jessie Minimal installed

Downloading and Building WiringPi

* Install git on your Pi with `sudo apt-get install git`
* Clone the wiringPi source repo with `git clone git://git.drogon.net/wiringPi`
* cd wiringPi && ./build

Downloading and Building the AWS IoT SDK for Embedded C

* Install cmake on your Pi with `sudo apt-get install cmake`
* Clone the AWS IoT SDK with `git clone https://github.com/aws/aws-iot-device-sdk-embedded-C`
* `git clone https://github.com/cpputest/cpputest`
* `mv cpputest/* aws-iot-device-sdk-embedded-C/external_libs/CppUTest/ && rm -rf cpputest`
* `wget https://github.com/ARMmbed/mbedtls/archive/mbedtls-2.1.1.zip`
* `unzip mbedtls-mbedtls-2.1.1`
* `mv mbedtls-mbedtls-2.1.1/* aws-iot-device-sdk-embedded-C/external_libs/mbedTLS/ && rm -rf mbedtls-mbedtls-2.1.1`
* `cd aws-iot-device-sdk-embedded-C`
* `make`

This will kick off the build the AWS IoT C SDK on your Raspberry Pi. This takes a while. As the Pi is a British invention, go and make a cup of tea at this point

Setting up the IoT Button Emulator

* Clone this repo in samples/linux/ with the other client samples with `cd samples/linux ; git clone https://github.com/ianmas-aws/iot-button-emulator`
* Set up your thing on the AWS IoT console or use the command line and download your device certificates & public/private keys
* Create a directory called samples/linux/certs  
* Put your downloaded device specific certificate, the CA root certificate and public/private keys in samples/linux/certs
* Edit aws_iot_config.h with the correct filenames for your certificates and keys, your device serial number, and your account specific AWS IoT endpoint
* Build with the `make` command. This build the mbedtls library on first build along with the button emulator
* Replicate the wiring setup shown in iot-button-emulator-wiring.jpeg
* run `sudo ./iot-button-emulator` (must be run as root for access to the GPIO)

* subscribe to the iot-button-emulator/+ topic with the AWS IoT Console

Press the connected button and watch the JSON messages flow
