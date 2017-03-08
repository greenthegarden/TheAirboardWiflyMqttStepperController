#ifndef THEAIRBOARDWIFLYMQTTSTEPPERCONTROLLER_STEPPER_CONFIG_H_
#define THEAIRBOARDWIFLYMQTTSTEPPERCONTROLLER_STEPPER_CONFIG_H_


#include <AccelStepper.h>



#define HALFSTEP 8  //Half-step mode (8 step control signal sequence)

// Stepper
#define Pin1  10     // IN1 on the ULN2003 driver 1
#define Pin2  11     // IN2 on the ULN2003 driver 1
#define Pin3  12     // IN3 on the ULN2003 driver 1
#define Pin4  13     // IN4 on the ULN2003 driver 1

AccelStepper stepper(HALFSTEP, Pin1, Pin3, Pin2, Pin4);

const int SPEED = 1000;    // The current speed in steps/second
const int ACCELERATION = 100;
const int TOTAL_STEPS = 10000; // enter in total amount of steps to open shade from 0% to 100%


// stepper status topics
const char REQUEST_IDX_STEPPER_TOPIC[] PROGMEM = "stepper/status/request_idx";
const char REQUEST_POSITION_STEPPER_TOPIC[] PROGMEM = "stepper/status/request_position";
const char CURRENT_POSITION_STEPPER_TOPIC[] PROGMEM = "stepper/status/current_position";
const char TARGET_POSITION_STEPPER_TOPIC[] PROGMEM = "stepper/status/target_position";
const char DISTANCE_TO_GO_STEPPER_TOPIC[] PROGMEM = "stepper/status/distance_to_go";
const char SPEED_STEPPER_TOPIC[] PROGMEM = "stepper/status/speed";

PGM_P const STEPPER_STATUS_TOPICS[] PROGMEM = {
    REQUEST_IDX_STEPPER_TOPIC,          // idx = 0
    REQUEST_POSITION_STEPPER_TOPIC,     // idx = 1
    CURRENT_POSITION_STEPPER_TOPIC,        // idx = 2
    TARGET_POSITION_STEPPER_TOPIC,         // idx = 3
    DISTANCE_TO_GO_STEPPER_TOPIC,         // idx = 4
    SPEED_STEPPER_TOPIC,      // idx = 5
};

typedef enum {
  REQUEST_IDX_STEPPER_TOPIC_IDX = 0,
  REQUEST_POSITION_STEPPER_TOPIC_IDX = 1,
  CURRENT_POSITION_STEPPER_TOPIC_IDX = 2,
  TARGET_POSITION_STEPPER_TOPIC_IDX = 3,
  DISTANCE_TO_GO_STEPPER_TOPIC_IDX = 4,
  SPEED_STEPPER_TOPIC_IDX = 5,
} stepper_status_topics;

void publish_stepper_request_idx(byte idx) {
  topicBuffer[0] = '\0';
  strcpy_P(topicBuffer,
           (char *)pgm_read_word(&(STEPPER_STATUS_TOPICS[REQUEST_IDX_STEPPER_TOPIC_IDX])));
  payloadBuffer[0] = '\0';
  mqttClient.publish(topicBuffer, itoa(idx, payloadBuffer, 10));
}

void publish_stepper_request_position(double position) {
  topicBuffer[0] = '\0';
  strcpy_P(topicBuffer,
           (char *)pgm_read_word(&(STEPPER_STATUS_TOPICS[REQUEST_POSITION_STEPPER_TOPIC_IDX])));
  payloadBuffer[0] = '\0';
  dtostrf(position, 1, 2, payloadBuffer);
  mqttClient.publish(topicBuffer, payloadBuffer);
}

void publish_stepper_current_position() {
  topicBuffer[0] = '\0';
  strcpy_P(topicBuffer,
           (char *)pgm_read_word(&(STEPPER_STATUS_TOPICS[CURRENT_POSITION_STEPPER_TOPIC_IDX])));
  payloadBuffer[0] = '\0';
  mqttClient.publish(topicBuffer, ltoa(stepper.currentPosition(), payloadBuffer, 10));
}

void publish_stepper_target_position() {
  topicBuffer[0] = '\0';
  strcpy_P(topicBuffer,
           (char *)pgm_read_word(&(STEPPER_STATUS_TOPICS[TARGET_POSITION_STEPPER_TOPIC_IDX])));
  payloadBuffer[0] = '\0';
  mqttClient.publish(topicBuffer, ltoa(stepper.targetPosition(), payloadBuffer, 10));
}

void publish_stepper_distance_to_go() {
  topicBuffer[0] = '\0';
  strcpy_P(topicBuffer,
           (char *)pgm_read_word(&(STEPPER_STATUS_TOPICS[DISTANCE_TO_GO_STEPPER_TOPIC_IDX])));
  payloadBuffer[0] = '\0';
  mqttClient.publish(topicBuffer, ltoa(stepper.distanceToGo(), payloadBuffer, 10));
}

void publish_stepper_speed() {
  topicBuffer[0] = '\0';
  strcpy_P(topicBuffer,
           (char *)pgm_read_word(&(STEPPER_STATUS_TOPICS[SPEED_STEPPER_TOPIC_IDX])));
  payloadBuffer[0] = '\0';
  mqttClient.publish(topicBuffer, itoa(stepper.speed(), payloadBuffer, 10));
}


void stepper_init() {
  stepper.setMaxSpeed(SPEED);
  stepper.setAcceleration(ACCELERATION);
  stepper.setSpeed(SPEED);
  publish_stepper_speed();
}

void stepper_move(double position) {
  stepper.moveTo(TOTAL_STEPS * position);
  publish_stepper_request_position(position);
  publish_stepper_current_position();
  publish_stepper_target_position();
  publish_stepper_distance_to_go();
  while(stepper.distanceToGo() != 0) {
    stepper.run();
  }
  stepper.stop();
}

void stepper_set_move(int idx) {
  publish_stepper_request_idx(idx);
  switch(idx) {
    case 1 : // open 0%
      stepper_move(0.00);
      break;
    case 2 : // open 25%
      stepper_move(0.25);
      break;
    case 3 : // open 50%
      stepper_move(0.50);
      break;
    case 4 : // open 75%
      stepper_move(0.75);
      break;
    case 5 : // open 100%
      stepper_move(1.0);
      break;
    default:
      break;
  }
}


#endif  /* THEAIRBOARDWIFLYMQTTSTEPPERCONTROLLER_STEPPER_CONFIG_H_ */
