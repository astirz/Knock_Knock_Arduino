#include <Servo.h>

Servo myservo;             // This is where we record our sound measurement
unsigned long lastSoundDetectTime;  // Record the time that we measured a sound
int soundAlarmTime = 200;           // Number of milli seconds to keep the sound alarm high
boolean noiseAlarm = false;

const int LED1 = 5;
const int LED2 = 6;
const int LED3 = 7;

// Knocking sequence for the 3 LEDs
int LED1_seq = 1;   // Knock 2 times
int LED2_seq = 3;   // Knock 1 time
int LED3_seq = 3;   // Knock 3 times

boolean f_knocks = false;   // first sequence of knocks
boolean s_knocks = false;   // second sequence of knocks
boolean t_knocks = false;   // third sequence of knocks

int load = 0;
int knocks = 0;
int count = 0;

unsigned long f_knocksTime = 0;
unsigned long t_knocksTime = 0;
int durationForKnocks = 4000;   // Normally the knocking sequence I do lasts about 4 seconds

void setup()
{
  Serial.begin(9600);
  myservo.attach(9);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
}

void loop() {
    delay(1000); // Avoid high signal input detect at initial time
    while (knocks < 7) {    // Once system got all 6 knocks, it will exit the 'while' loop
      int mn = 1024;
      int mx = 0;

      for (int i = 0; i < 100; ++i) {
      int val = analogRead(A0);
      mn = min(mn, val);
      mx = max(mx, val);
      }

      int delta = mx - mn; // Larger delta corresponds to a larger amplitude waveform, and a louder sound:
      if (delta > 20) {  // Microphone detects "knock" delta value as 25 - by experiment
        // This will output the "Knock" message only once even if the signal remains at HIGH.
        if (!noiseAlarm) {                
          lastSoundDetectTime = millis();
                if (millis() - lastSoundDetectTime > 5000) {   // If second set of knocks is TRUE, go to LED3 function
          resetting();
        }
          knocks++;
          count++;
          noiseAlarm = true;
        }
      }
      if ((millis() - lastSoundDetectTime) > soundAlarmTime && noiseAlarm) {
        noiseAlarm =  false;
      }
      // Start to check the knocks that correspond to the LEDs
      turnLED1_ON();
      if (f_knocks) {    // If first set of knocks is TRUE, go to LED2 function
        turnLED2_ON();
        if (s_knocks) {   // If second set of knocks is TRUE, go to LED3 function
          turnLED3_ON();
        }
      }
    }
    delay(1000);    // Give system some time to register all 6 knocks
    
    while (f_knocks && s_knocks && t_knocks) {    // When all three sets of knocks TRUE
      
      // Check to see if User didn't follow the correct knocking 'rhythm'
      if (t_knocksTime - f_knocksTime < 2000) {
        // RESET and try again
          resetting();
      }
      
       // User follows the correct knocking 'rhythm'...
       // ...to attain the correct duration while knocking
      if ((t_knocksTime - f_knocksTime > 2000) && (t_knocksTime - f_knocksTime < durationForKnocks)) {
        myservo.write(10);              // Tell servo to go to position in variable 'pos'
        delay(90); 
        delay(5000);
        // End of Access, exit the system
        resetting();
      }
      
      // User took too long to enter the knocking 'rhythm'
      if (t_knocksTime - f_knocksTime > durationForKnocks) {
        // RESET and try again
          resetting();
      }
    }
    delay(1000);  // Let the system register the reset before detecting sound
  }

// First Knocking Sequence 
void turnLED1_ON() {
  if (count == LED1_seq && f_knocks == false) {
    Serial.println("LED 1 - ACCEPTED");
    digitalWrite(LED1, HIGH);
    count = 0;
    f_knocks = true;
    f_knocksTime = millis(); // Capture the time AFTER the 'first knocks' completed
  }
}

// Second Knocking Sequence
void turnLED2_ON() {
  if (count == LED2_seq && s_knocks == false) {
    Serial.println("LED 2 - ACCEPTED");
    digitalWrite(LED2, HIGH);
    count = 0;
    s_knocks = true;
  }
}

// Third Knocking Sequence
void turnLED3_ON() {
  if (count == LED3_seq && t_knocks == false) {
    Serial.println("LED 3 - ACCEPTED");
    digitalWrite(LED3, HIGH);
    count = 0;
    t_knocks = true;
    t_knocksTime = millis();  // Capture the time AFTER the 'third knocks' completed
  }
}

void resetting() {
  myservo.write(0); // Tell servo to go to initial position
  knocks = 0;
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);
  f_knocks = false;
  s_knocks = false;
  t_knocks = false;
}
