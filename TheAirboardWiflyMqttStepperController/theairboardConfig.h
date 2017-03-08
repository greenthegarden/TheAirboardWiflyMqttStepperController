#ifndef THEAIRBOARDWIFLYMQTTSTEPPERCONTROLLER_THEAIRBOARD_CONFIG_H_
#define THEAIRBOARDWIFLYMQTTSTEPPERCONTROLLER_THEAIRBOARD_CONFIG_H_


// The Airboard (https://github.com/theairboard/TheAirBoard)
#include <TheAirBoard.h>

TheAirBoard board;

void theairboard_init() {
  analogWrite(GREEN, 1);        // switch ON indicator at low power
  Serial.begin(BAUD_RATE);
}

void publish_battery() {
  topicBuffer[0] = '\0';
  strcpy_P(topicBuffer, (char*)pgm_read_word(&(STATUS_TOPICS[BATTERY_STATUS_IDX])));
  payloadBuffer[0] = '\0';
  dtostrf(board.batteryChk(), 1, FLOAT_DECIMAL_PLACES, payloadBuffer);
  mqttClient.publish(topicBuffer, payloadBuffer);
}

void publish_temperature() {
  topicBuffer[0] = '\0';
  strcpy_P(topicBuffer, (char*)pgm_read_word(&(STATUS_TOPICS[TEMPERATURE_STATUS_IDX])));
  payloadBuffer[0] = '\0';
  dtostrf(board.getTemp(), 1, FLOAT_DECIMAL_PLACES, payloadBuffer);
  mqttClient.publish(topicBuffer, payloadBuffer);
}


#endif /* THEAIRBOARDWIFLYMQTTSTEPPERCONTROLLER_THEAIRBOARD_CONFIG_H_ */
