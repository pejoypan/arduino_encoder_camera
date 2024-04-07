/*

*/

// this constant won't change:
const int PIN_ENCODER_PULSE = 2;  // the pin that the pushbutton is attached to
const int PIN_START_IN = 4;
const int PIN_START_CHECK = 12;
const int PIN_CAMERA_1 = 6;    // the pin that the LED is attached to
const int PIN_CAMERA_2 = 8;
const int PIN_START_OUT = 10;

const int PIN_DEBUG = 7;

// Variables will change:
int ENCODER_current_state;        // current state of the button
int ENCODER_last_state = LOW;    // previous state of the button
int START_current_state;
int START_last_state = LOW;

const int START_CHECK_should_be = HIGH;

const unsigned int debounce_delay = 5;  // us
unsigned long ENCODER_last_change_time = 0;
unsigned long START_last_change_time = 0;

const unsigned int T_deg = 6;           // 360 / 60 = 6
unsigned int T;
const int pulse_per_deg = 100;          // single-phase, one-fold freq
const int high_keep_pulse_count = 123;

unsigned int delay_camera1_on = 43;     // (43) should configure
unsigned int delay_camera1_off;         // (43+123=166)

const int camera_1_to_2_deg = 31;       // probably won't change    
unsigned int delay_camera2_on;          // (43+100=143) 
unsigned int delay_camera2_off;         // (143+123=266)

const int camera_1_to_output_deg = 105; // probably won't change    
unsigned int delay_output_on;           // (43+300=343)
unsigned int delay_output_off;          // (343+123=566)

unsigned long encoder_counter = 0;

/// !!! 因为使用的是if-else所有的delay必须不同 且都必须和T不同 需检查

#define LIGHT_ON LOW
#define LIGHT_OFF HIGH

bool waiting_for_start = true;

void setup() {
  // initialize the button pin as a input:
  pinMode(PIN_ENCODER_PULSE, INPUT);
  pinMode(PIN_START_IN, INPUT);
  pinMode(PIN_START_CHECK, INPUT);

  pinMode(PIN_CAMERA_1, OUTPUT); // camera 1
  pinMode(PIN_CAMERA_2, OUTPUT); // camera 2
  pinMode(PIN_START_OUT, OUTPUT);

  pinMode(LED_RX, OUTPUT); 
  digitalWrite(LED_RX, LIGHT_OFF);
  pinMode(LED_TX, OUTPUT);
  digitalWrite(LED_TX, LIGHT_OFF);

  waiting_for_start = true;

  T = T_deg * pulse_per_deg;
  delay_camera1_off = delay_camera1_on + high_keep_pulse_count;

  delay_camera2_on = delay_camera1_on + camera_1_to_2_deg * pulse_per_deg;
  delay_camera2_off = delay_camera2_on + high_keep_pulse_count;

  delay_output_on = delay_camera1_on + camera_1_to_output_deg * pulse_per_deg;
  delay_output_off = delay_output_on + high_keep_pulse_count;

  // for debug use
  // pinMode(PIN_DEBUG, OUTPUT);
}

void loop() {
  if (waiting_for_start) {
    int reading = digitalRead(PIN_START_IN);
    digitalWrite(LED_BUILTIN, reading);

    if (reading != START_last_state) {
      START_last_change_time = micros();
    }

    if ((micros() - START_last_change_time) > debounce_delay) {

      if (reading != START_current_state) {
        START_current_state = reading;

        if (START_current_state == HIGH) {
          int START_CHECK_state = digitalRead(PIN_START_CHECK);
          if (START_CHECK_state == START_CHECK_should_be)
            waiting_for_start = false;
        }
      }
    }

    START_last_state = reading;

  } else {
    // read the pushbutton input pin:
    int reading = digitalRead(PIN_ENCODER_PULSE);

    if (reading != ENCODER_last_state) {
      ENCODER_last_change_time = micros();
    }

    // compare the buttonState to its previous state
    if ((micros() - ENCODER_last_change_time) > debounce_delay) {
      // if the state has changed, increment the counter

      if (reading != ENCODER_current_state) {
        ENCODER_current_state = reading;

        if (ENCODER_current_state == HIGH) {
          encoder_counter++;
        } 
      }
    }

    if (encoder_counter == delay_camera1_on) {
      digitalWrite(PIN_CAMERA_1, HIGH);
      digitalWrite(LED_RX, LIGHT_ON);
    } else if (encoder_counter == delay_camera1_off) {
      digitalWrite(PIN_CAMERA_1, LOW);
      digitalWrite(LED_RX, LIGHT_OFF);
    } else if (encoder_counter == delay_camera2_on) {
      digitalWrite(PIN_CAMERA_2, HIGH);
      digitalWrite(LED_TX, LIGHT_ON);
    } else if (encoder_counter == delay_camera2_off) {
      digitalWrite(PIN_CAMERA_2, LOW);
      digitalWrite(LED_TX, LIGHT_OFF);
    } else if (encoder_counter == delay_output_on) {
      digitalWrite(PIN_START_OUT, HIGH);
      digitalWrite(LED_BUILTIN, HIGH);
    } else if (encoder_counter == delay_output_off) {
      digitalWrite(PIN_START_OUT, LOW);
      digitalWrite(LED_BUILTIN, LOW);
    } else if (encoder_counter == T) {
      encoder_counter = 0;

      if (delay_camera1_on > T) {
        delay_camera1_on -= T;
      }
      if (delay_camera1_off > T) {
        delay_camera1_off -= T;
      }
      if (delay_camera2_on > T) {
        delay_camera2_on -= T;
      }
      if (delay_camera2_off > T) {
        delay_camera2_off -= T;
      }
      if (delay_output_on > T) {
        delay_output_on -= T;
      }
      if (delay_output_off > T) {
        delay_output_off -= T;
      }
    }

    // save the current state as the last state, for next time through the loop
    ENCODER_last_state = reading;
  }
}

