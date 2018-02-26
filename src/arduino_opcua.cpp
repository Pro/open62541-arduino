#include <Arduino.h>
#include "open62541.h"
#include <NTPClient.h>


#include <WiFi.h>
#include <open62541.h>
#include "Nodeset.h"


const char* ssid     = "eDO";
const char* password = "eDOfortiss";

#define LED_RED 5
#define LED_YELLOW 18
#define LED_GREEN 19


UA_ServerConfig *config;
UA_Server *server;
Nodeset *nodeset;

const char *LogsLevelNames[6] = {"trace", "debug", "info", "warning", "error", "fatal"};
const char *LogsCategoryNames[6] = {"network", "channel", "session", "server", "client", "userland"};

int freq = 10000;
uint8_t ledChannel = 0;
uint8_t resolution = 8;
unsigned long lastStatusLedUpdate = 0;
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
	Serial.printf("Current time: %s\n", buf);
}

void UA_Log_Serial(UA_LogLevel level, UA_LogCategory category, const char *msg, va_list args) {
	char tmpStr[400];
	snprintf(tmpStr, 400, "[OPC UA] %s/%s\t", LogsLevelNames[level], LogsCategoryNames[category]);
	char *start = &tmpStr[strlen(tmpStr)];

	vsprintf(start, msg, args);

	size_t len = strlen(tmpStr);
	tmpStr[len] = '\n';
	tmpStr[len + 1] = '\0';

	Serial.printf(tmpStr);

	//memUsage();
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
	Serial.println(ssid);

	WiFi.begin(ssid, password);

	short ledState = 0;

	while (WiFi.status() != WL_CONNECTED) {
		ledState = !ledState;
		digitalWrite(LED_YELLOW, ledState);
		delay(200);
		Serial.print(".");
	}
	digitalWrite(LED_YELLOW, HIGH);

	Serial.println("");
	Serial.println("WiFi connected.");
	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());

	Serial.flush();
}

void setupTime() {

	Serial.println("Setting up time...");

	WiFiUDP ntpUDP;

	// You can specify the time server pool and the offset (in seconds, can be
	// changed later with setTimeOffset() ). Additionaly you can specify the
	// update interval (in milliseconds, can be changed using setUpdateInterval() ).
	NTPClient timeClient(ntpUDP);


	timeClient.begin();
	if (!timeClient.forceUpdate()) {
		Serial.println("Could not get NTP time.");
		return;
	}
	Serial.printf("Current epoch time = %lld\n", timeClient.getEpochTime());
	Serial.print(timeClient.getFormattedDate());
	Serial.print(timeClient.getFormattedTime());
	struct timeval current = {
			timeClient.getEpochTime(), // tv.sec
			0 // tv.usec
	};
	struct timezone zone = {
			3600, // tz_minuteswest
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

	//setupTime();

	Serial.println("Setting up opcua...");
	config = UA_ServerConfig_new_customBuffer(4840, NULL, 8192, 8192);
	String localIp = WiFi.localIP().toString();
	const UA_String customHostname = {
			.length = localIp.length(),
			.data = (UA_Byte *)localIp.c_str()
	};
	UA_ServerConfig_set_customHostname(config, customHostname);
	config->logger = UA_Log_Serial;
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

	nodeset = new Nodeset(server, UA_Log_Serial);
	nodeset->createNodes();

	digitalWrite(LED_YELLOW, LOW);
	digitalWrite(LED_GREEN, HIGH);

	ledcSetup(ledChannel, freq, resolution);
	ledcAttachPin(LED_GREEN, ledChannel);
	lastStatusLedUpdate = millis();
}


void loop() {

	UA_Server_run_iterate(server, true);

	/*for (int dutyCycle = 0; dutyCycle <= 125; dutyCycle++){
		ledcWrite(ledChannel, dutyCycle);
		delay(7);
	}

	for (int dutyCycle = 125; dutyCycle >= 0; dutyCycle--){
		ledcWrite(ledChannel, dutyCycle);
		delay(7);
	}*/

	lastStatusLedUpdate = millis();
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


}