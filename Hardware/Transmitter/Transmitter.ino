#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include <string.h>

// Set up nRF24L01 radio on SPI bus plus pins 7 & 8
// MOSI 11
// SCK 13
// MISO 12
// CSN 2
// CE 3
RF24 radio(3,2);

static char phone_lock[] = "LOCK";
static char phone_unlock[] = "UNLOCK";
bool proxON = false;
// unlock password u359287050290824
// lock password l359287050290824

// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

// The various roles supported by this sketch
typedef enum { role_ping_out = 1, role_pong_back } role_e;
// The debug-friendly names of those roles
const char* role_friendly_name[] = { "invalid", "Ping out [TRANSMITTER]", "Pong back [RECIEVER]"};
// The role of the current running sketch
role_e role;


// Data is sent from the phone to this transmitter. 
// This transmitter is for lockID 1 
// if unlock, phone will send a 'u1'
// if lock, phone will send a 'l1'
//char phoneData = 'n'; // a char value that does not get sent from the phone (nULL)
//char phoneData = 'n';

const int payload_size = 6;
const int min_payload_size = 4;
//const int max_payload_size = 32;
const int max_payload_size = 32;
const int payload_size_increments_by = 1;
int next_payload_size = min_payload_size;
 char send_payload[] = "ABCDEFGHIJASBFSAOEJFASQWRQWRQRQ1";
char receive_payload[max_payload_size+1]; // +1 to allow room for a terminating NULL char

void setup(void)
{
  delay(20); // Just to get a solid reading on the role pin

//  role = role_pong_back;
  role = role_ping_out;

  Serial.begin(9600);
  Serial.print(F("ROLE: "));
  Serial.println(role_friendly_name[role]);


  radio.begin();
  radio.enableDynamicPayloads(); // enable dynamic payloads
  radio.setRetries(5,15); //increase the delay between retries(5) & # of retries (15)

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
char phoneData;
void loop(void)
{
  phoneData = Serial.read(); 
//  if (!phoneData == 'w') {
    if (phoneData == 'l' ) {
      radio.stopListening(); // stop listening so we can talk
      Serial.println(phone_lock);
      Serial.print("size: ");
      Serial.println(next_payload_size);
      phoneData = 'n';
      radio.write(phone_lock, min_payload_size);
//      radio.powerDown();
//      delay (10000);
//      radio.powerUp();
//      radio.startListening(); // Continue listeningz
    } else if (phoneData == 'u') {
      radio.stopListening(); // stop listening so we can talk
      Serial.println(phone_unlock);
      Serial.print("size: ");
      Serial.println(next_payload_size);
      phoneData = 'n';
      radio.write(phone_unlock, payload_size); 
//      radio.powerDown();
//      delay(10000);
//      radio.powerUp();
//      radio.startListening();  // Continue listening
    } else if (phoneData == 'p') {
       proxON = true;
       char send_payload[] = "ABCDEFGHIJASBFSAOEJFASQWRQWRQRQ1";
//       strcpy(send_payload, "ABCDEFGHIJASBFSAOEJFASQWRQWRQRQ1");
       Serial.print("you pressed p");
       Serial.print(send_payload);
    } else if (phoneData == 'b') {
      proxON= false;
             Serial.print("you pressed b");
                    strcpy(send_payload, "ABCEDFGHIJKLMNOPQRSTUVWXYZ789012");
//       char send_payload[] = "ABCEDFGHIJKLMNOPQRSTUVWXYZ789012";
       Serial.print(send_payload);
    }
//  }
  //
  // Ping out role.  Repeatedly send the current time
  //

  if (role == role_ping_out)
  {
    if (proxON){
       char send_payload[] = "ABCDEFGHIJASBFSAOEJFASQWRQWRQRQ1";
    }else{
       char send_payload[] = "ABCEDFGHIJKLMNOPQRSTUVWXYZ789012";
    }
    // The payload will always be the same, what will change is how much of it we send.
//    static char send_payload[] = "ABCDEFGHIJASBFSAOEJFASQWRQWRQRQ1";

    // First, stop listening so we can talk.
    radio.stopListening();

    // Take the time, and send it.  This will block until complete
//    Serial.print(F("Now sending length "));
//    Serial.println(next_payload_size);
    radio.write( send_payload, next_payload_size );

    // Now, continue listening
    radio.startListening();

    // Wait here until we get a response, or timeout
    unsigned long started_waiting_at = millis();
    bool timeout = false;
    while ( ! radio.available() && ! timeout )
      if (millis() - started_waiting_at > 500 )
        timeout = true;

    // Describe the results
    if ( timeout )
    {
      Serial.println(F("Failed, response timed out."));
    }
    else
    {
      // Grab the response, compare, and send to debugging spew
      uint8_t len = radio.getDynamicPayloadSize();
      
      // If a corrupt dynamic payload is received, it will be flushed
      if(!len){
        return; 
      }
      
      radio.read( receive_payload, len );

      // Put a zero at the end for easy printing
      receive_payload[len] = 0;

      // Spew it
//      Serial.print(F("Got response size="));
//      Serial.print(len);
//      Serial.print(F(" value="));
//      Serial.println(receive_payload);
    }
    
    // Update size for next time.
    next_payload_size += payload_size_increments_by;
    if ( next_payload_size > max_payload_size )
      next_payload_size = min_payload_size;

    // Try again 1s later
    delay(100);
  }
}
