#include "thingProperties.h"

int sensor_pin = 0;
int pulse_signal = 0;
//float BPM = 0.0;

int upper_threshold = 860;
int lower_threshold = 800;
int counter = 0;

bool any_peak_detected = false;
bool first_peak_detected = false;
unsigned long first_pulse_time = 0;
unsigned long second_pulse_time = 0;
unsigned long pulse_period = 0;

void setup() {
  Serial.begin(9600);
  delay(1500);
  initProperties();

  //Connect to cloud and get into/erros
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();

  //Wait for cloud connection
  while (ArduinoCloud.connected() != 1) {
    ArduinoCloud.update();
    delay(500);
  }
}

void loop() {
  pulse_signal = analogRead(sensor_pin);
  //Serial.println(pulse_signal);

  // Find leading edge of heart beat
  if (pulse_signal > upper_threshold && any_peak_detected == false) {
    if (first_peak_detected == false) {
      first_pulse_time = millis();
      first_peak_detected = true;
    } else {
      second_pulse_time = millis();
      pulse_period = second_pulse_time - first_pulse_time;
      first_pulse_time = second_pulse_time;
    }
    any_peak_detected = true;
  }

  //Find trailing edge
  if (pulse_signal < lower_threshold) {
    any_peak_detected = false;
  }

  BPM = (1.0 / pulse_period) * 60.0 * 1000;
  Serial.println(BPM);

  delay(50);

  counter++;
  if (counter > 200){
    ArduinoCloud.update();
    Serial.println(BPM);
    counter = 0;
  }
}
