// This microcontroller will control the transmission and recieving
// on the phone side. An infared sensor is used to detect any motion.
// The detection will ensure that users, along with the receiving code is
// allowed to unlock the door.

// For Current Implementation
// Use detect based on the users signal
#include <VirtualWire.h>
// Pins Definition
#define receive_pin 2 //Receiver module pin
//Stepper Motor
#define motorPin1 11 // IN1
#define motorPin2 10 // IN2
#define motorPin3 9 // IN3
#define motorPin4 8 // IN4
int pirPin = 4;    //the digital pin connected to the PIR sensor's output

int calibrationTime = 10;
long unsigned int lowIn; // time when sensor outputs a low impulse

//the amount of milliseconds the sensor has to be low
//before we assume all motion has stopped
long unsigned int pause = 2000;
boolean lockLow = true;
boolean takeLowTime;

int steps = 0;
boolean directionOfMotor = true;
unsigned long lastTime;
unsigned long currentMillis;
int stepsLeft = 2047;
long time;


/////////////////////////////
//SETUP
void setup(){
  Serial.begin(9600);
  pinMode(pirPin, INPUT);
  digitalWrite(pirPin, LOW); // Set the sensor to low

  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(motorPin3, OUTPUT);
  pinMode(motorPin4, OUTPUT);

  //give the sensor some time to calibrate
  Serial.print("calibrating sensor ");
    for(int i = 0; i < calibrationTime; i++){
      Serial.print(".");
      delay(1000);
      }
    Serial.println(" done");
    Serial.println("SENSOR ACTIVE");
    delay(50);

  // Initialize the receiver module with Virtual Wire
  vw_set_rx_pin(receive_pin);
  vw_setup(3000);
  vw_rx_start();
  }

////////////////////////////
//LOOP
void loop(){
     uint8_t buf[VW_MAX_MESSAGE_LEN];
     uint8_t buflen = VW_MAX_MESSAGE_LEN;
     if(digitalRead(pirPin) == HIGH && vw_get_message(buf, &buflen)){
//       vw_wait_rx();
//       	Serial.print("Got: ");
//	for (int i = 0; i < buflen; i++)
//	{
//	    Serial.print(buf[i], HEX);
//	    Serial.print(' ');
//	}
//	Serial.println();
       if(lockLow){
         //makes sure we wait for a transition to LOW before any further output is made:
         lockLow = false;
         Serial.println("---");
         Serial.print("motion detected at ");
         Serial.print(millis()/1000);
         Serial.println(" sec");
         delay(50);
         }
         takeLowTime = true; 
         directionOfMotor = true;   
         lockDoor();
     }
//     } else if(vw_get_message(buf, &buflen)) {
////       vw_wait_rx();
////       Serial.print("Got: ");
////       
////       for(int i = 0; i < buflen; i++) {
////         Serial.print(buf[i], HEX);
////         Serial.print(' ');
////       }
////       
////       Serial.println();
////       byte lockOrUnlock = (buf[1], HEX);
////       if (lockOrUnlock == 16) {
////         Serial.print("UNLOCK BY ITSELF");
////         directionOfMotor = true;
////         lockDoor();
////       } 
//       
//     }

     if(digitalRead(pirPin) == LOW){
       if (vw_get_message(buf, &buflen)) {
         vw_wait_rx();
         Serial.print("Gotthem: ");
         for(int i =0; i < buflen; i++) {
           Serial.print(buf[i]);
           Serial.print(' ');
         }
         Serial.println();
       }
       if(takeLowTime){
        lowIn = millis();          //save the time of the transition from high to LOW
        takeLowTime = false;       //make sure this is only done at the start of a LOW phase
        }
       //if the sensor is low for more than the given pause,
       //we assume that no more motion is going to happen
       if(!lockLow && millis() - lowIn > pause){
           //makes sure this block of code is only executed again after
           //a new motion sequence has been detected
           lockLow = true;
           Serial.print("motion ended at ");      //output
           Serial.print((millis() - pause)/1000);
           Serial.println(" sec");
           delay(50);
           }
     }
     
       
       
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



