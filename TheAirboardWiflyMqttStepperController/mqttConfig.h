#ifndef THEAIRBOARDWIFLYMQTTSTEPPERCONTROLLER_MQTT_CONFIG_H_
#define THEAIRBOARDWIFLYMQTTSTEPPERCONTROLLER_MQTT_CONFIG_H_


#include <PubSubClient.h>


// MQTT parameters
IPAddress mqttServerAddr(192, 168, 1, 52); // emonPi
const char * MQTT_CLIENT_ID = "theairboard";
const char * MQTT_USERNAME = "emonpi";
const char * MQTT_PASSWORD = "emonpimqtt2016";
const int MQTT_PORT = 1883;

unsigned long lastReconnectAttempt = 0UL;
const unsigned long RECONNECTION_ATTEMPT_INTERVAL = 5000UL;

const byte BUFFER_SIZE            = 32;
char topicBuffer[BUFFER_SIZE];
char payloadBuffer[BUFFER_SIZE];

// callback definition for MQTT
void callback(char* topic,
              uint8_t* payload,
              unsigned int length);

PubSubClient mqttClient(mqttServerAddr, MQTT_PORT, callback, wiflyClient);


// MQTT topic definitions

const char MQTT_PAYLOAD_CONNECTED[] PROGMEM = "CONNECTED";
const char MQTT_PAYLOAD_OK[] PROGMEM = "OK";
const char MQTT_PAYLOAD_ERROR[] PROGMEM = "ERROR";
const char MQTT_PAYLOAD_START[] PROGMEM = "START";
const char MQTT_PAYLOAD_END[] PROGMEM = "END";
const char MQTT_PAYLOAD_SLEEP[] PROGMEM = "SLEEP";

PGM_P const MQTT_PAYLOADS[] PROGMEM = {
    MQTT_PAYLOAD_CONNECTED, // idx = 0
    MQTT_PAYLOAD_OK,        // idx = 1
    MQTT_PAYLOAD_ERROR,     // idx = 2
    MQTT_PAYLOAD_START,     // idx = 3
    MQTT_PAYLOAD_END,       // idx = 4
    MQTT_PAYLOAD_SLEEP,     // idx = 5
};

typedef enum {
  MQTT_PAYLOAD_CONNECTED_IDX = 0,
  MQTT_PAYLOAD_OK_IDX = 1,
  MQTT_PAYLOAD_ERROR_IDX = 2,
  MQTT_PAYLOAD_START_IDX = 3,
  MQTT_PAYLOAD_END_IDX = 4,
  MQTT_PAYLOAD_SLEEP_IDX = 5,
} mqtt_payloads;

// status topics
const char MQTT_STATUS[] PROGMEM = "theairboard/status/mqtt";
const char INTERVAL_STATUS[] PROGMEM = "theairboard/status/interval";
const char IP_ADDR_STATUS[] PROGMEM = "theairboard/status/ip_addr";
const char UPTIME_STATUS[] PROGMEM = "theairboard/status/uptime";
const char BATTERY_STATUS[] PROGMEM = "theairboard/status/battery";
const char TEMPERATURE_STATUS[] PROGMEM = "theairboard/status/temperature";

PGM_P const STATUS_TOPICS[] PROGMEM = {
    MQTT_STATUS,          // idx = 0
    INTERVAL_STATUS,         // idx = 1
    IP_ADDR_STATUS,         // idx = 2
    UPTIME_STATUS,         // idx = 3
    BATTERY_STATUS,          // idx = 4
    TEMPERATURE_STATUS,   // idx = 5
};

typedef enum {
  MQTT_STATUS_IDX = 0,
  INTERVAL_STATUS_IDX = 1,
  IP_ADDR_STATUS_IDX = 2,
  UPTIME_STATUS_IDX = 3,
  BATTERY_STATUS_IDX = 4,
  TEMPERATURE_STATUS_IDX = 5,
} status_topics;


// control topics
const char STEPPER_SET_MOVE[] PROGMEM = "theairboard/control/stepper";

PGM_P const CONTROL_TOPICS[] PROGMEM = {
    STEPPER_SET_MOVE,          // idx = 0
};

typedef enum {
  STEPPER_SET_MOVE_IDX = 0,
} control_topics;


void publish_connected() {
  topicBuffer[0] = '\0';
  strcpy_P(topicBuffer,
           (char *)pgm_read_word(&(STATUS_TOPICS[MQTT_STATUS_IDX])));
  payloadBuffer[0] = '\0';
  strcpy_P(payloadBuffer,
           (char *)pgm_read_word(&(MQTT_PAYLOADS[MQTT_PAYLOAD_CONNECTED_IDX])));
  mqttClient.publish(topicBuffer, payloadBuffer);
}

void publish_status_interval() {
  topicBuffer[0] = '\0';
  strcpy_P(topicBuffer,
           (char *)pgm_read_word(&(STATUS_TOPICS[INTERVAL_STATUS_IDX])));
  payloadBuffer[0] = '\0';
  mqttClient.publish(topicBuffer,
                     ltoa(STATUS_UPDATE_INTERVAL, payloadBuffer, 10));
}

#if 0
void publish_ip_address() {
  topicBuffer[0] = '\0';
  strcpy_P(topicBuffer,
           (char *)pgm_read_word(&(STATUS_TOPICS[IP_ADDR_STATUS_IDX])));
  payloadBuffer[0] = '\0';
  IPAddress ip = WiFly.ip();
  sprintf(payloadBuffer, "%i%c%i%c%i%c%i", ip[0], '.', ip[1], '.', ip[2], '.',
          ip[3]);
  mqttClient.publish(topicBuffer, payloadBuffer);
}
#endif

void publish_uptime() {
  topicBuffer[0] = '\0';
  strcpy_P(topicBuffer,
           (char *)pgm_read_word(&(STATUS_TOPICS[UPTIME_STATUS_IDX])));
  payloadBuffer[0] = '\0';
  mqttClient.publish(topicBuffer, ltoa(millis(), payloadBuffer, 10));
}

// theairboard function definitions
void publish_battery();
void publish_temperature();


void publish_configuration() {
  publish_status_interval();
#if 0
  publish_ip_address();
#endif
}

void publish_status()
{
  publish_uptime();
  publish_battery();
  publish_temperature();
}

boolean mqtt_connect() {
  if (!wiflyConnectedToNetwork)
      wifly_connect();

  if (wiflyConnectedToNetwork) {
    if (mqttClient.connect(MQTT_CLIENT_ID, MQTT_USERNAME, MQTT_PASSWORD)) {
      // Once connected, publish an announcement ...
      publish_connected();
      publish_configuration();
      publish_status();
      // ... and subscribe to topics (should have list)
//      mqttClient.subscribe("theairboard/control/stepper");
      for (byte i = 0; i < ARRAY_SIZE(CONTROL_TOPICS); i++) {
        topicBuffer[0] = '\0';
        strcpy_P(topicBuffer, (PGM_P)pgm_read_word(&(CONTROL_TOPICS[i])));
        mqttClient.subscribe(topicBuffer);
      }
    }
    return mqttClient.connected();
  }
}


#endif  /* THEAIRBOARDWIFLYMQTTSTEPPERCONTROLLER_MQTT_CONFIG_H_ */
