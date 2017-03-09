#include <Arduino.h>

#include "config.h"

void callback(char *topic, uint8_t *payload, unsigned int length) {
  // handle message arrived
  /* topic = part of the variable header:has topic name of the topic where the
     publish received
       NOTE: variable header does not contain the 2 bytes with the
            publish msg ID
      payload = pointer to the first item of the buffer array that
                contains the message tha was published
               EXAMPLE of payload: lights,1
      length = the length of the payload, until which index of payload
  */

  // Copy the payload to the new buffer
  char *message =
      (char *)malloc((sizeof(char) * length) +
                     1); // get the size of the bytes and store in memory
  memcpy(message, payload, length * sizeof(char)); // copy the memory
  message[length * sizeof(char)] = '\0'; // add terminating character

  byte topicIdx = 0;
  boolean controlTopicFound = false;

  // find if topic is matched
  for (byte i = 0; i < ARRAY_SIZE(CONTROL_TOPICS); i++) {
    topicBuffer[0] = '\0';
    strcpy_P(topicBuffer, (PGM_P)pgm_read_word(&(CONTROL_TOPICS[i])));
    if (strcmp(topic, topicBuffer) == 0) {
      topicIdx = i;
      controlTopicFound = true;
      break;
    }
  }

  if (controlTopicFound) {
    if (topicIdx == STEPPER_SET_MOVE_IDX) { // topic is STEPPER_SET_MOVE
      // message is expected to be an integer
      byte stepper_set_move_idx = atoi(message);
      if (stepper_set_move_idx > 0) {
        stepper_set_move(stepper_set_move_idx);
      }
    }
  }

  // free memory assigned to message
  free(message);
}

/*--------------------------------------------------------------------------------------
  setup()
  Called by the Arduino framework once, before the main loop begins
  --------------------------------------------------------------------------------------*/
void setup()
{
  theairboard_init();

  wifly_configure();
  mqtt_connect();

  stepper_init();
}


/*--------------------------------------------------------------------------------------
  loop()
  Arduino main loop
  --------------------------------------------------------------------------------------*/
void loop()
{
  unsigned long now = millis();

  if (!mqttClient.connected()) {
    if (now - lastReconnectAttempt >= RECONNECTION_ATTEMPT_INTERVAL) {
      lastReconnectAttempt = now;
      // Attempt to reconnect
      if (mqtt_connect()) {
        lastReconnectAttempt = 0;
      }
    }
  } else {
    // Client connected
    mqttClient.loop();
  }

  if (now - statusPreviousMillis >= STATUS_UPDATE_INTERVAL) {
    if (mqttClient.connected()) {
      statusPreviousMillis = now;
      publish_status();
    }
  }
}
