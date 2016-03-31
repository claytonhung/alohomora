#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

// Set up nRF24L01 radio on SPI bus plus pins 2 & 5
// MOSI 11
// SCK 13
// MISO 12
// CSN 2
// CE 5
RF24 radio(5,2);

#define motorPin1 10 // IN1
#define motorPin2 9 // IN2
#define motorPin3 8 // IN3
#define motorPin4 7 // IN4
int pir = 6;
int stateLock = 4; // red
int stateunLock = 3;  // green

int steps = 0;
boolean directionOfMotor = true; //true = locked
unsigned long lastTime;
unsigned long currentMillis;
//int stepsLeft = 2047;
int stepsLeft = 2591;
long time;

static char unlock_char[] = "UNLOCK";
static char lock_char[] = "LOCK";
static char distance_char[] = "ABCDEFGHIJASBFSAOEJFASQWRQWR";

// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

// The various roles supported by this sketch
typedef enum { role_ping_out = 1, role_pong_back } role_e;
// The debug-friendly names of those roles
const char* role_friendly_name[] = { "invalid", "Ping out", "Pong back"};
// The role of the current running sketch
role_e role;

char phoneData = 'n';
static char phone_send[] = "LOCKUNLOCK";

const int min_payload_size = 4;
//const int max_payload_size = 32;
const int max_payload_size = 69;
const int payload_size_increments_by = 1;
int next_payload_size = min_payload_size;

char receive_payload[max_payload_size+1]; // +1 to allow room for a terminating NULL char

void setup(void)
{
  delay(20); // Just to get a solid reading on the role pin

    role = role_pong_back;
//  role = role_ping_out;

  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(motorPin3, OUTPUT);
  pinMode(motorPin4, OUTPUT);
  pinMode(pir, INPUT);
  pinMode(stateLock, OUTPUT);
  pinMode(stateunLock, OUTPUT);
  digitalWrite(stateLock, HIGH);
  digitalWrite(stateunLock, LOW);
  Serial.begin(9600);
  Serial.print(F("ROLE: "));
  Serial.println(role_friendly_name[role]);

  radio.begin();
  radio.enableDynamicPayloads(); // enable dynamic payloads
  radio.setRetries(5,15); // increase delay between retries & # of retries

  // This simple sketch opens two pipes for these two nodes to communicate
  // back and forth.
  // Open 'our' pipe for writing
  // Open the 'other' pipe for reading, in position #1 (we can have up to 5 pipes open for reading)

  if ( role == role_ping_out )
  {
    radio.openWritingPipe(pipes[0]);
    radio.openReadingPipe(1,pipes[1]);
  }
  else
  {
    radio.openWritingPipe(pipes[1]);
    radio.openReadingPipe(1,pipes[0]);
  }

  radio.startListening();
  radio.printDetails();
}

void loop(void)
{
  // Pong back role.  Receive each packet, dump it out, and send it back
  char c;
  if (Serial.available()) {
    c = Serial.read();
    if (c == 'l') {
            directionOfMotor = true;
      lockDoor(); //unlock first
      delay(3000);  
     lockDoor(); //lock now
    } else if (c == 'u') {
      if (!directionOfMotor) {
            Serial.println("UNLOCK WITH PHONE");
            lockDoor(); // Lock Door
          }
    }
  }
  
  if ( role == role_pong_back )
  {
    // if there is data ready
    while ( radio.available() )
    {

      // Fetch the payload, and see if this was the last one.
      uint8_t len = radio.getDynamicPayloadSize();

      // If a corrupt dynamic payload is received, it will be flushed
      if(!len){
        continue; 
      }

      radio.read( receive_payload, len );

      // Put a zero at the end for easy printing
      receive_payload[len] = 0;

      // Spew it
      Serial.print(F("Got RES size="));
      Serial.print(len);
      Serial.print(F(" value="));
      Serial.println(receive_payload);
      
      if (strcmp(receive_payload, unlock_char) ==0) {
        if (directionOfMotor) {
          Serial.println("UNLOCK WITH PHONE");
          lockDoor(); // It's actually unlock
          digitalWrite(stateunLock, LOW);
          digitalWrite(stateLock, HIGH);
        }
      }
      
      if (strcmp(receive_payload, lock_char) == 0) {
        if (!directionOfMotor) {
          Serial.println("LOCK WITH PHONE");
          lockDoor(); // Lock Door
          digitalWrite(stateLock, LOW);
          digitalWrite(stateunLock, HIGH);
        }
      }

      // THIS IS WHERE WE UNLOCK THE DOOR
      // THIS IS WHERE WE UNLOCK THE DOOR
      // THIS IS WHERE WE UNLOCK THE DOOR            
      // THIS IS WHERE WE UNLOCK THE DOOR
      // THIS IS WHERE WE UNLOCK THE DOOR
      if (strcmp(receive_payload, distance_char) == 0 && (digitalRead(pir) == HIGH)) {
        Serial.println("PROXIMITY UNLOCK");
        directionOfMotor = true;
        lockDoor(); //unlock first
        digitalWrite(stateLock, LOW);
        digitalWrite(stateunLock, HIGH);
        delay(5000); // let door be able to "rest" before locking again 
        lockDoor(); //lock now
        digitalWrite(stateLock, HIGH);
        digitalWrite(stateunLock, LOW);
      }

      // First, stop listening so we can talk
      radio.stopListening();

      // Send the final one back.
      radio.write( receive_payload, len );
      Serial.println(F("Sent response."));

      // Now, resume listening so we catch the next packets.
      radio.startListening();
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
  //    stepsLeft = 2047;
  stepsLeft = 2591;
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



// TURN MOTOR CODE 
//  directionOfMotor = true;
//      lockDoor(); //unlock first
//      delay(3000);  
//      lockDoor(); //lock now

