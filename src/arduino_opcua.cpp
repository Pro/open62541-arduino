#include <Arduino.h>
#include "open62541.h"


#include "WiFi.h"

const char* ssid     = "eDO";
const char* password = "eDOfortiss";


UA_ServerConfig *config;
UA_Server *server;


const char *LogsLevelNames[6] = {"trace", "debug", "info", "warning", "error", "fatal"};
const char *LogsCategoryNames[6] = {"network", "channel", "session", "server", "client", "userland"};

void UA_Log_Serial(UA_LogLevel level, UA_LogCategory category, const char *msg, va_list args) {
	char tmpStr[400];
	snprintf(tmpStr, 400, "[OPC UA] %s/%s\t", LogsLevelNames[level], LogsCategoryNames[category]);
	char *start = &tmpStr[strlen(tmpStr)];

	vsprintf(start, msg, args);

	size_t len = strlen(tmpStr);
	tmpStr[len] = '\n';
	tmpStr[len + 1] = '\0';

	Serial.printf(tmpStr);
}



WiFiServer testServer(80);

void errorLoop() {
	while(true) {
		digitalWrite(5, HIGH);
		delay(100);
		digitalWrite(5, LOW);
		delay(100);
	}
}

void setupWifi() {
	// We start by connecting to a WiFi network

	Serial.println();
	Serial.println();
	Serial.print("Connecting to ");
	Serial.println(ssid);

	WiFi.begin(ssid, password);

	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}

	Serial.println("");
	Serial.println("WiFi connected.");
	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());

	Serial.flush();

	testServer.begin();
}

void setup() {
	Serial.begin(115200);
	pinMode(5, OUTPUT);      // set the LED pin mode

	Serial.println("Starting up...");
	for (int i=0; i<5; i++) {
		digitalWrite(5, HIGH);
		delay(500);
		digitalWrite(5, LOW);
		delay(500);
	}

	Serial.println("Setting up wifi...");
	setupWifi();

	Serial.println("Setting up opcua...");
	config = UA_ServerConfig_new_minimal(4840, NULL);
	/*config->logger = UA_Log_Serial;
	server = UA_Server_new(config);
	if (UA_Server_run_startup(server) != UA_STATUSCODE_GOOD) {
		errorLoop();
	}*/
}

void loopWifiClient(WiFiClient client) {
	if (!client)
		return;
	Serial.println("New Client.");           // print a message out the serial port
	String currentLine = "";                // make a String to hold incoming data from the client
	while (client.connected()) {            // loop while the client's connected
		if (client.available()) {             // if there's bytes to read from the client,
			char c = client.read();             // read a byte, then
			Serial.write(c);                    // print it out the serial monitor
			if (c == '\n') {                    // if the byte is a newline character

				// if the current line is blank, you got two newline characters in a row.
				// that's the end of the client HTTP request, so send a response:
				if (currentLine.length() == 0) {
					// HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
					// and a content-type so the client knows what's coming, then a blank line:
					client.println("HTTP/1.1 200 OK");
					client.println("Content-type:text/html");
					client.println();

					// the content of the HTTP response follows the header:
					client.print("Click <a href=\"/H\">here</a> to turn the LED on pin 5 on.<br>");
					client.print("Click <a href=\"/L\">here</a> to turn the LED on pin 5 off.<br>");

					// The HTTP response ends with another blank line:
					client.println();
					// break out of the while loop:
					break;
				} else {    // if you got a newline, then clear currentLine:
					currentLine = "";
				}
			} else if (c != '\r') {  // if you got anything else but a carriage return character,
				currentLine += c;      // add it to the end of the currentLine
			}

			// Check to see if the client request was "GET /H" or "GET /L":
			if (currentLine.endsWith("GET /H")) {
				digitalWrite(5, HIGH);               // GET /H turns the LED on
			}
			if (currentLine.endsWith("GET /L")) {
				digitalWrite(5, LOW);                // GET /L turns the LED off
			}
		}
	}
	// close the connection:
	client.stop();
	Serial.println("Client Disconnected.");
}

void loop() {

	WiFiClient client = testServer.available();   // listen for incoming clients
	loopWifiClient(client);
	//UA_Server_run_iterate(server, true);
}