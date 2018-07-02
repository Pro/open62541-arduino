#include <Arduino.h>
#include "open62541.h"
#include <NTPClient.h>

#ifdef USE_WIFI
#include <WiFi.h>
#else
#include <SPI.h>
#include <Ethernet2.h>
#endif

#include <open62541.h>
#include "Nodeset.h"

// You should define the SSID and PWD using compiler defines, e.g. gcc -DWIFI_SSID="sometest"
#ifndef WIFI_SSID
#error WIFI_SSID not defined. Use compiler defines to set it to a value
#endif
#ifndef WIFI_PWD
#error WIFI_PWD not defined. Use compiler defines to set it to a value
#endif



#define LED_RED 5
#define LED_YELLOW 18
#define LED_GREEN 19


UA_ServerConfig *config;
UA_Server *server;
Nodeset *nodeset;

int freq = 10000;
uint8_t ledChannel = 0;
uint8_t resolution = 8;
uint8_t statusLedDutyCycleCount = 0;
uint8_t statusLedDutyCycleDir = 1;

void memUsage() {
	UBaseType_t uxHighWaterMark = uxTaskGetStackHighWaterMark( NULL );
	Serial.print( "Memory: " );
	Serial.print(uxHighWaterMark );
	Serial.print( " ");
	Serial.println( esp_get_free_heap_size() );
}

void printCurrentTime() {
	struct timeval tv;
	time_t nowtime;
	struct tm *nowtm;
	char tmbuf[64], buf[64];

	gettimeofday(&tv, NULL);
	nowtime = tv.tv_sec;
	nowtm = localtime(&nowtime);
	strftime(tmbuf, sizeof tmbuf, "%Y-%m-%d %H:%M:%S", nowtm);
	snprintf(buf, sizeof buf, "%s.%06ld", tmbuf, tv.tv_usec);
	Serial.printf("Current time: %s (UTC)\n", buf);
}

void errorLoop() {
	digitalWrite(LED_GREEN, LOW);
	while(true) {
		digitalWrite(LED_RED, HIGH);
		delay(100);
		digitalWrite(LED_RED, LOW);
		delay(100);
	}
}

void setupWifi() {
	// We start by connecting to a WiFi network

	Serial.println("Setting up wifi...");
	Serial.println();
	Serial.print("Connecting to ");
	Serial.println(WIFI_SSID);

	WiFi.begin(WIFI_SSID, WIFI_PWD);

	short ledState = 0;

	while (WiFi.status() != WL_CONNECTED) {
		ledState = !ledState;
		digitalWrite(LED_YELLOW, ledState);
		delay(200);
		Serial.print(".");
	}
	digitalWrite(LED_YELLOW, HIGH);

	Serial.println("");
	Serial.print("WiFi connected. IP address: ");
	Serial.println(WiFi.localIP());

	Serial.flush();
}

void setupTime() {

	Serial.println("Setting up time...");

	WiFiUDP ntpUDP;

	// You can specify the time server pool and the offset (in seconds, can be
	// changed later with setTimeOffset() ). Additionaly you can specify the
	// update interval (in milliseconds, can be changed using setUpdateInterval() ).
	NTPClient timeClient(ntpUDP, "de.pool.ntp.org");

	if (!timeClient.update()) {
		Serial.println("Could not get NTP time.");
		return;
	}
	struct timeval current = {
			timeClient.getEpochTime(), // tv.sec
			0 // tv.usec
	};
	struct timezone zone = {
			0, // tz_minuteswest
			0
	};
	settimeofday(&current, &zone);
	printCurrentTime();
}

void setup() {
	Serial.begin(115200);
	pinMode(LED_RED, OUTPUT);
	pinMode(LED_YELLOW, OUTPUT);
	pinMode(LED_GREEN, OUTPUT);

	Serial.println("Starting up...");
	for (int i=0; i<2; i++) {
		digitalWrite(LED_RED, HIGH);
		digitalWrite(LED_YELLOW, HIGH);
		digitalWrite(LED_GREEN, HIGH);
		delay(500);
		digitalWrite(LED_RED, LOW);
		digitalWrite(LED_YELLOW, LOW);
		digitalWrite(LED_GREEN, LOW);
		delay(500);
	}

	setupWifi();

	setupTime();

	Serial.println("Setting up opcua...");
	config = UA_ServerConfig_new_customBuffer(4840, NULL, 8192, 8192);
	String localIp = WiFi.localIP().toString();
	const UA_String customHostname = {
			.length = localIp.length(),
			.data = (UA_Byte *)localIp.c_str()
	};
	UA_ServerConfig_set_customHostname(config, customHostname);
	config->logger = UA_Log_Stdout;
	Serial.println("Creating server...");
	server = UA_Server_new(config);
	Serial.println("run server...");
	UA_StatusCode retVal = UA_Server_run_startup(server);
	if (retVal != UA_STATUSCODE_GOOD) {
		Serial.print("Creating OPC UA Server failed with code: ");
		Serial.println(UA_StatusCode_name(retVal));
		errorLoop();
	}
	Serial.println("OPC UA initialized");

	nodeset = new Nodeset(server, UA_Log_Stdout);
	nodeset->createNodes();

	digitalWrite(LED_YELLOW, LOW);
	digitalWrite(LED_GREEN, HIGH);

	ledcSetup(ledChannel, freq, resolution);
	ledcAttachPin(LED_GREEN, ledChannel);
}

time_t lastMemOutput = 0;

void loop() {

	UA_Server_run_iterate(server, true);

	if (statusLedDutyCycleDir == 1) {
		statusLedDutyCycleCount+=2;
		if (statusLedDutyCycleCount >= 125) {
			// change direction
			statusLedDutyCycleDir = 0;
		}
	} else {
		statusLedDutyCycleCount-=2;
		if (statusLedDutyCycleCount <= 2) {
			// change direction
			statusLedDutyCycleDir = 1;
		}
	}
	ledcWrite(ledChannel, statusLedDutyCycleCount);

	struct timeval tv;
	time_t nowtime;

	gettimeofday(&tv, NULL);
	nowtime = tv.tv_sec;

	if (nowtime - lastMemOutput > 5) {
		memUsage();
		lastMemOutput = nowtime;
	}
}