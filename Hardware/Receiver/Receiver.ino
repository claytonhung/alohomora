// This microcontroller will control the transmission and recieving
// on the phone side. An infared sensor is used to detect any motion. 
// The detection will ensure that users, along with the receiving code is 
// allowed to unlock the door. 


// For Current Implementation
// Use detect based on the users signal 
#include <VirtualWire.h>
#include <AccelStepper.h>
#define HALFSTEP 

// Pins definition
#define led_pin 13 // LED pin to blink whenever device receives something
#define receive_pin 2 //Receiver module pin
#define pirPin 4 // Pin info for the Infared Sensor

//Stepper Motor Pins
#define motorPin1 8 // IN4
#define motorPin2 9 // IN3
#define motorPin3 10 // IN2
#define motorPin4 11 // IN1

int steps = 0;
boolean directionOfMotor = true;
unsigned long lastTime;
unsigned long currentMillis;
int stepsLeft = 2047;
long time;

// Infared Sensor Pins
// Datasheet for this PIR sensor says calibration time between 10-60 seconds
int calibrationTime = 30; //give sensor 30 seconds to calibrate
long unsigned int timeOfNoDetection; // time when sensor outputs low impulse
long unsigned int pause = 5000; // time the sensor has to be low before we assuming motion has stopped
boolean noDetection = true; // see if sensor detected any motion (LOCKLOW)
boolean beginTimer; // start the timer

void setup(){
   Serial.begin(9600); // Debugging purposes....
   Serial.println("setup");
//   digitalWrite(pirPin, LOW);
   
   Serial.print("calibrating sensor ");
     for (int i = 0; i < calibrationTime; i++) {
       Serial.print(".");
       delay(1000);
     }  
     Serial.println("done");
     Serial.println("SENSOR ACTIVE");
     delay(50);
     
   // Initialise the IO and ISR
//   vw_set_tx_pin(transmit_pin);
   vw_set_rx_pin(receive_pin);
//   vw_set_ptt_pin(transmit_en_pin);
//   vw_set_ptt_inverted(true); // Required for DR3100
   vw_setup(2000);	 // Bits per sec
   vw_rx_start();       // Start the receiver PLL running
}

byte count = 1;

void loop(){
  // --- RECEIVING PART STARTS ---
   uint8_t buf[VW_MAX_MESSAGE_LEN];
   uint8_t buflen = VW_MAX_MESSAGE_LEN;
  
  //Check if Motion is detected AND message can be received from the transmitter
  if (digitalRead(pirPin) == HIGH && vw_get_message(buf, &buflen)) {
    vw_wait_rx(); //wait for full message to be received
    
    if(noDetection) {
      noDetection = false;
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
      movement(1);
      time = time + micros() - lastTime;
      lastTime = micros();
      stepsLeft--;
    }
    Serial.println(time);
    Serial.println("Uhh....wait.....");
    delay(2000);
    directionOfMotor =! directionOfMotor;
    stepsLeft = 2047;
}

void movement(int xw){
  for(int x = 0; x < xw; x++) {
    switch(Steps) {
      case 0:
       digitalWrite(IN1, LOW);
       digitalWrite(IN2, LOW);
       digitalWrite(IN3, LOW);
       digitalWrite(IN4, HIGH);
      break;
      case 1:
       digitalWrite(IN1, LOW); 
       digitalWrite(IN2, LOW);
       digitalWrite(IN3, HIGH);
       digitalWrite(IN4, HIGH);
     break; 
     case 2:
       digitalWrite(IN1, LOW); 
       digitalWrite(IN2, LOW);
       digitalWrite(IN3, HIGH);
       digitalWrite(IN4, LOW);
     break; 
     case 3:
       digitalWrite(IN1, LOW); 
       digitalWrite(IN2, HIGH);
       digitalWrite(IN3, HIGH);
       digitalWrite(IN4, LOW);
     break; 
     case 4:
       digitalWrite(IN1, LOW); 
       digitalWrite(IN2, HIGH);
       digitalWrite(IN3, LOW);
       digitalWrite(IN4, LOW);
     break; 
     case 5:
       digitalWrite(IN1, HIGH); 
       digitalWrite(IN2, HIGH);
       digitalWrite(IN3, LOW);
       digitalWrite(IN4, LOW);
     break; 
     case 6:
       digitalWrite(IN1, HIGH); 
       digitalWrite(IN2, LOW);
       digitalWrite(IN3, LOW);
       digitalWrite(IN4, LOW);
     break; 
     case 7:
       digitalWrite(IN1, HIGH); 
       digitalWrite(IN2, LOW);
       digitalWrite(IN3, LOW);
       digitalWrite(IN4, HIGH);
     break; 
     default:
       digitalWrite(IN1, LOW); 
       digitalWrite(IN2, LOW);
       digitalWrite(IN3, LOW);
       digitalWrite(IN4, LOW);
     break; 
   }
   SetDirection();
  }
}

void setDirection() {
  if(directionOfMotor == 1) {
    Steps++;
  }
  
  if(directionOfMotor == 0) {
    Steps--;
  }
  
  if(Steps>7) {
    Steps = 0;
  }
  
  if(Steps<0) {
    Steps = 7; 
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
