/**
 * @file subscribe_publish_cpp_sample.cpp
 * @brief simple MQTT publish in C++
 *
 * This example takes the parameters from the aws_iot_config.h file and establishes a connection to the AWS IoT MQTT Platform.
 * 
 * When a button click is detected on the attached hardware button (see comments in code for connection details)
 * a message is published to the a topic constructed from the top level topic and device serial defined in aws_iot_config.h
 *
 * If all the certs are correct, you should see the messages received by the application in a loop.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <stdint.h>
#include <wiringPi.h>

#include "aws_iot_config.h"
#include "aws_iot_log.h"
#include "aws_iot_version.h"
#include "aws_iot_mqtt_client_interface.h"

#include "clickButton.h"

/**
 * @brief Default cert location
 */
char certDirectory[PATH_MAX + 1] = "../certs";

/**
 * @brief Default MQTT HOST URL is pulled from the aws_iot_config.h
 */
char HostAddress[255] = AWS_IOT_MQTT_HOST;

/**
 * @brief Default MQTT port is pulled from the aws_iot_config.h
 */
uint32_t port = AWS_IOT_MQTT_PORT;

/**
 * @brief This parameter will avoid infinite loop of publish and exit the program after certain number of publishes
 */
uint32_t publishCount = 0;

void iot_subscribe_callback_handler(AWS_IoT_Client *pClient, char *topicName, uint16_t topicNameLen,
									IoT_Publish_Message_Params *params, void *pData) {
	IOT_UNUSED(pData);
	IOT_UNUSED(pClient);
	IOT_INFO("Subscribe callback");
	IOT_INFO("%.*s\t%.*s", topicNameLen, topicName, (int) params->payloadLen, params->payload);
}

void disconnectCallbackHandler(AWS_IoT_Client *pClient, void *data) {
	IOT_WARN("MQTT Disconnect");
	IoT_Error_t rc = FAILURE;

	if(NULL == pClient) {
		return;
	}

	IOT_UNUSED(data);

	if(aws_iot_is_autoreconnect_enabled(pClient)) {
		IOT_INFO("Auto Reconnect is enabled, Reconnecting attempt will start now");
	} else {
		IOT_WARN("Auto Reconnect not enabled. Starting manual reconnect...");
		rc = aws_iot_mqtt_attempt_reconnect(pClient);
		if(NETWORK_RECONNECTED == rc) {
			IOT_WARN("Manual Reconnect Successful");
		} else {
			IOT_WARN("Manual Reconnect Failed - %d", rc);
		}
	}
}


int main(int argc, char **argv) {
	bool infinitePublishFlag = true;

	char rootCA[PATH_MAX + 1];
	char clientCRT[PATH_MAX + 1];
	char clientKey[PATH_MAX + 1];
	char CurrentWD[PATH_MAX + 1];
	char cPayload[256];

	char cTopic[100];
	uint16_t cTopicLen;

	int32_t i = 0;

	IoT_Error_t rc = FAILURE;

	AWS_IoT_Client client;
	IoT_Client_Init_Params mqttInitParams = iotClientInitParamsDefault;
	IoT_Client_Connect_Params connectParams = iotClientConnectParamsDefault;

	IoT_Publish_Message_Params paramsQOS0;
	IoT_Publish_Message_Params paramsQOS1;

	IOT_INFO("\nAWS IoT SDK Version %d.%d.%d-%s\n", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH, VERSION_TAG);

	getcwd(CurrentWD, sizeof(CurrentWD));
	snprintf(rootCA, PATH_MAX + 1, "%s/%s/%s", CurrentWD, certDirectory, AWS_IOT_ROOT_CA_FILENAME);
	snprintf(clientCRT, PATH_MAX + 1, "%s/%s/%s", CurrentWD, certDirectory, AWS_IOT_CERTIFICATE_FILENAME);
	snprintf(clientKey, PATH_MAX + 1, "%s/%s/%s", CurrentWD, certDirectory, AWS_IOT_PRIVATE_KEY_FILENAME);

	IOT_DEBUG("rootCA %s", rootCA);
	IOT_DEBUG("clientCRT %s", clientCRT);
	IOT_DEBUG("clientKey %s", clientKey);
	mqttInitParams.enableAutoReconnect = false; // We enable this later below
	mqttInitParams.pHostURL = HostAddress;
	mqttInitParams.port = port;
	mqttInitParams.pRootCALocation = rootCA;
	mqttInitParams.pDeviceCertLocation = clientCRT;
	mqttInitParams.pDevicePrivateKeyLocation = clientKey;
	mqttInitParams.mqttCommandTimeout_ms = 20000;
	mqttInitParams.tlsHandshakeTimeout_ms = 5000;
	mqttInitParams.isSSLHostnameVerify = true;
	mqttInitParams.disconnectHandler = disconnectCallbackHandler;
	mqttInitParams.disconnectHandlerData = NULL;

	rc = aws_iot_mqtt_init(&client, &mqttInitParams);
	if(SUCCESS != rc) {
		IOT_ERROR("aws_iot_mqtt_init returned error : %d ", rc);
		return rc;
	}

	connectParams.keepAliveIntervalInSec = 10;
	connectParams.isCleanSession = true;
	connectParams.MQTTVersion = MQTT_3_1_1;
	connectParams.pClientID = (char *)AWS_IOT_MQTT_CLIENT_ID;
	connectParams.clientIDLen = (uint16_t) strlen(AWS_IOT_MQTT_CLIENT_ID);
	connectParams.isWillMsgPresent = false;

	IOT_INFO("Connecting...");
	rc = aws_iot_mqtt_connect(&client, &connectParams);
	if(SUCCESS != rc) {
		IOT_ERROR("Error(%d) connecting to %s:%d", rc, mqttInitParams.pHostURL, mqttInitParams.port);
		return rc;
	}
	/*
	 * Enable Auto Reconnect functionality. Minimum and Maximum time of Exponential backoff are set in aws_iot_config.h
	 *  #AWS_IOT_MQTT_MIN_RECONNECT_WAIT_INTERVAL
	 *  #AWS_IOT_MQTT_MAX_RECONNECT_WAIT_INTERVAL
	 */
	rc = aws_iot_mqtt_autoreconnect_set_status(&client, true);
	if(SUCCESS != rc) {
		IOT_ERROR("Unable to set Auto Reconnect to true - %d", rc);
		return rc;
	}

	// construct topic string using definitions from aws_iot_config.h 
	sprintf(cTopic, "%s/%s", DEVICE_MASTER_TOPIC, DEVICE_SERIAL_NUMBER);

	// calculate its length (required for subscription functions within SDK)
	cTopicLen = strlen(cTopic);

	//debugging topc construction from aws_iot_config.h config header file
	IOT_DEBUG("Constructed topic is: %s", cTopic);
	IOT_DEBUG("Constructed topic length is: %d", cTopicLen);

	paramsQOS0.qos = QOS0;
	paramsQOS0.payload = (void *) cPayload;
	paramsQOS0.isRetained = 0;

	paramsQOS1.qos = QOS1;
	paramsQOS1.payload = (void *) cPayload;
	paramsQOS1.isRetained = 0;

	// Setup Pi hardware to capture button clicks.
	if(wiringPiSetup() == -1){ //when initialize wiring failed,print messageto screen
                printf("setup wiringPi failed !");
                return 1;
        }

        // Define the details for the Button. Refer to the Pi wiring schematic for physcial details
	// Connect one terminal on the button to GRD, pin 6
	// The other terminal on the button to GPIO 16, pin 36
	// Pin 36 maps to pin 27 in WiringPi, so that's what we set here

        const int buttonPin1 = 27;

	// activate the pull up resistor on WiringPi 27. negates the need to use an external resistor
        ClickButton button1(buttonPin1, LOW, CLICKBTN_PULLUP);

        // define function as an int to store Button results
        int function = 0;

        // Setup button timers (all in milliseconds / ms)
        // (These are default if not set, but changeable for convenience)
	// There's an issue with detecting DOUBLE clicks on the Pi due to timing issues
	// I have tried tweaking these variables to resolve this, but without success
	// So be aware that you might not be able to generate DOUBLE clicks
	// If anyone can propose a fix for this, I would be very grateful

        button1.debounceTime   = 20;   // Debounce timer in ms
        button1.multiclickTime = 250;  // Time limit for multi clicks
        button1.longClickTime  = 1000; // time until "held-down clicks" register
	

	while((NETWORK_ATTEMPTING_RECONNECT == rc || NETWORK_RECONNECTED == rc || SUCCESS == rc)
		  && (publishCount > 0 || infinitePublishFlag)) {

		//Max time the yield function will wait for read messages
		rc = aws_iot_mqtt_yield(&client, 100);
		if(NETWORK_ATTEMPTING_RECONNECT == rc) {
			// If the client is attempting to reconnect we will skip the rest of the loop.
			continue;
		}

		// Read button state
		button1.Update();

                // Save click codes in function, as click codes are reset at next Update()
                if (button1.clicks != 0) function = button1.clicks;

		// define clickType to hold the string representation of the click pattern
		char clickType[32];

		// evaluate the click pattern and assign the string based on the value
                if(function == 1) sprintf(clickType, "SINGLE");

                if(function == 2) sprintf(clickType, "DOUBLE");

                if(function == 3) sprintf(clickType, "TRIPLE");

                if(function == -1) sprintf(clickType, "LONG");

                if(function == -2) sprintf(clickType, "DOUBLE LONG");

                if(function == -3) sprintf(clickType, "TRIPLE LONG");

		// if we have detected a click event, construct the JSON message that matches the message format on the actual AWS IoT Button
		// print this for debugging, publish it via the AWS IoT SDK using the preconfigured client object

		if(function) {
			sprintf(cPayload, "{\"serialNumber\": \"%s\", \"batteryVoltage\": \"5v USB\", \"clickType\": \"%s\"}", DEVICE_SERIAL_NUMBER, clickType);
        		IOT_DEBUG("Publishing payload: %s\n", cPayload);
			paramsQOS0.payloadLen = strlen(cPayload);
			rc = aws_iot_mqtt_publish(&client, cTopic, cTopicLen, &paramsQOS0);
		}		

		// reset the value of function for the next loop
                function = 0;

		// wait 5ms and loop again
                delay(5);

	}


	if(SUCCESS != rc) {
		IOT_ERROR("An error occurred in the loop.\n");
	} else {
		IOT_INFO("Publish done\n");
	}

	return rc;
}
