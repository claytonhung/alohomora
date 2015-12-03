// This microcontroller will control the transmission and recieving
// on the phone side. An infared sensor is used to detect any motion.
// The detection will ensure that users, along with the receiving code is
// allowed to unlock the door.

// For Current Implementation
// Use detect based on the users signal
#include <VirtualWire.h>
// Pins Definition
#define receive_pin 2 //Receiver module pin
//Stepper Motor Pins
#define motorPin1 11 // IN1
#define motorPin2 10 // IN2
#define motorPin3 9 // IN3
#define motorPin4 8 // IN4
int pirPin = 4; //Pin info for the Infared Sensor (INPUT)

int steps = 0;
boolean directionOfMotor = true;
unsigned long lastTime;
unsigned long currentMillis;
int stepsLeft = 2047;
long time;

// Infared Sensor Pins
// Datasheet for this PIR sensor says calibration time between 10-60 seconds
int calibrationTime = 10; //give sensor 30 seconds to calibrate
long unsigned int timeOfNoDetection; // time when sensor outputs low impulse
long unsigned int pause = 5000; // time the sensor has to be low before we assuming motion has stopped
boolean noDetection = true; // see if sensor detected any motion (LOCKLOW)
boolean beginTimer; // start the timer

void setup(){
   Serial.begin(9600); // Debugging purposes.....................
   pinMode(pirPin, INPUT);
   digitalWrite(pirPin, LOW); // Set the sensor to low

   pinMode(motorPin1, OUTPUT);
   pinMode(motorPin2, OUTPUT);
   pinMode(motorPin3, OUTPUT);
   pinMode(motorPin4, OUTPUT);

   //give sensor some time to calibrate
   Serial.print("calibrating sensor ");
   for (int i = 0; i < calibrationTime; i++) {
     Serial.print(".");
     delay(1000);
   }
   Serial.println("done");
   Serial.println("SENSOR ACTIVE");
   delay(50);

   // Initialise the IO and ISR
   vw_set_rx_pin(receive_pin);
   vw_setup(2000);	 // Bits per sec
   vw_rx_start();       // Start the receiver PLL running
}

byte count = 1;

void loop(){
  // --- RECEIVING PART STARTS ---
   uint8_t buf[VW_MAX_MESSAGE_LEN];
   uint8_t buflen = VW_MAX_MESSAGE_LEN;
  //Check if Motion is detected AND message can be received from the transmitter
  if (digitalRead(pirPin) == HIGH) {
    vw_wait_rx(); //wait for full message to be received
    if(noDetection) {
      noDetection = false;
      Serial.println("---");
      Serial.print("Motion detected at ");
      Serial.print(millis()/1000); //the time motion was detected
      Serial.print(" seconds");
      delay(100);
    }
    beginTimer = true;
  }

  if (digitalRead(pirPin) == LOW) {
    if(beginTimer) {
      timeOfNoDetection = millis(); // save time of transition from high to low
      beginTimer = false; //reset this so its only done at the start of a LOW phase
    }
    // Check if sensor is not being used (LOW) for more than the "pause" time
    // we believe there is no motion that will happen again
    if(!noDetection && millis() - timeOfNoDetection > pause) {
      //ENTER SLEEP MODE SON
      noDetection = true;
      Serial.print("motion ended at ");
      Serial.print((millis() - pause/1000));
      Serial.println(" sec");
      delay(100);
    }
  }
// --- RECEIVING PART ENDS ---
}

void lockDoor () {
  while(stepsLeft > 0) {
    currentMillis = micros();
    if(currentMillis - lastTime >= 1000) {
      motorMove(1);
      time = time + micros() - lastTime;
      lastTime = micros();
      stepsLeft--;
    }
  }
    Serial.println(time);
    Serial.println("Uhh....wait.....");
    delay(2000);
    directionOfMotor =! directionOfMotor;
    stepsLeft = 2047;
}

void motorMove(int xw) {
  for(int x = 0; x < xw; x++) {
    switch(steps) {
      case 0:
       digitalWrite(motorPin1, LOW);
       digitalWrite(motorPin2, LOW);
       digitalWrite(motorPin3, LOW);
       digitalWrite(motorPin4, HIGH);
      break;
      case 1:
       digitalWrite(motorPin1, LOW);
       digitalWrite(motorPin2, LOW);
       digitalWrite(motorPin3, HIGH);
       digitalWrite(motorPin4, HIGH);
     break;
     case 2:
       digitalWrite(motorPin1, LOW);
       digitalWrite(motorPin2, LOW);
       digitalWrite(motorPin3, HIGH);
       digitalWrite(motorPin4, LOW);
     break;
     case 3:
       digitalWrite(motorPin1, LOW);
       digitalWrite(motorPin2, HIGH);
       digitalWrite(motorPin3, HIGH);
       digitalWrite(motorPin4, LOW);
     break;
     case 4:
       digitalWrite(motorPin1, LOW);
       digitalWrite(motorPin2, HIGH);
       digitalWrite(motorPin3, LOW);
       digitalWrite(motorPin4, LOW);
     break;
     case 5:
       digitalWrite(motorPin1, HIGH);
       digitalWrite(motorPin2, HIGH);
       digitalWrite(motorPin3, LOW);
       digitalWrite(motorPin4, LOW);
     break;
     case 6:
       digitalWrite(motorPin1, HIGH);
       digitalWrite(motorPin2, LOW);
       digitalWrite(motorPin3, LOW);
       digitalWrite(motorPin4, LOW);
     break;
     case 7:
       digitalWrite(motorPin1, HIGH);
       digitalWrite(motorPin2, LOW);
       digitalWrite(motorPin3, LOW);
       digitalWrite(motorPin4, HIGH);
     break;
     default:
       digitalWrite(motorPin1, LOW);
       digitalWrite(motorPin2, LOW);
       digitalWrite(motorPin3, LOW);
       digitalWrite(motorPin4, LOW);
     break;
   }
   setDirection();
  }
}

void setDirection() {
  if(directionOfMotor == 1) {
    steps++;
  }

  if(directionOfMotor == 0) {
    steps--;
  }

  if(steps>7) {
    steps = 0;
  }

  if(steps<0) {
    steps = 7;
  }
}

// GARBAGE CODE
//   // Check if a password message has been received
//    if (vw_get_message(buf, &buflen)) {
//      int i;
//        vw_wait_rx();
//        digitalWrite(led_pin, HIGH); // Turn on LED light when message received
//
////	Serial.print("Got: ");
////	for (i = 0; i < buflen; i++)
////	{
////	    Serial.print(buf[i], HEX);
////	    Serial.print(' ');
////	}
////	Serial.println();
//
//   }




// UNLOCK CODE
//  Serial.print("UNLOCKING DOOR");
//  delay(1000);
//  Serial.println("Stepper distance: " + stepper.distanceToGo());
//  stepper.moveTo(-stepper.currentPosition());
//  if(stepper.distanceToGo() == 0) {
//    stepper.moveTo(-stepper.currentPosition());
//  }
//  stepper.run();



//void unlockDoor() {
//
//}

//void transmitCodeBack() {
//  char msg[7] = {'0', '1', '1', '1', '0', '1', '0' };
//  msg[6] = count;
//  vw_send((uint8_t *)msg, 7);
//
//  if (vx_tx_active()) {
//    Serial.print("Sent.");
//  }
//
//  vw_wait_tx();
//  count = count + 1;
//}
