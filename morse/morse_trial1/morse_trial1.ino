
/*
              MORSE EN-DE-CODER 1.06

 - A Morse encoder / decoder for the Arduino.


3 input methods for decoding and encoding:
-  Audio Morse signals on an analog input (for decoding).
-  Morse keying on digital input (for decoding).
-  ASCII text via serial communication (for encoding. Also for adjusting settings).

2 outputs for encoded and decoded Morse:
-  Morse-code toggled output pin (for encoded Morse).
-  ASCII text via serial communication (for decoded Morse) .

It will enter audio listen mode if nothing is keyed in within a second.

Serial input:
 Only International Morse Code (letters A-Z, numbers), and space, are
 encoded and sent as Morse code automatically unless whatever the "command"
 character is (default '<') is received.
 No punctuation supported (but the morse table can be extended easily for that)

Morse en-/de-coder < commands via serial:
 The ASCII "less than" character '<' denotes a command instead of encoding and
 sending Morse. The format is:
 
   <Lxxx
 
 where L is a letter and xxx is a three-digit decimal number 000-999.
 One exception is the <i command for info (displays current settings).
 The letter can be upper or lower case W, A, D, E or I. Examples:

   <w008 - sets speed to 8 wpm
   <a900 - sets audio threshold to be 900
   <D020 - sets debounce delay to be 20 milliseconds.
   <e000 - turn off Morse echo back to serial as ASCII and output pin as Morse
   <e001 - turn on Morse echo (any value above 0 will do)
   <i    - info - displays some settings

Full duplex:
 It can both transmit and receive simultaneously, but serial output will
 be a mixed mess unless echo is turned off.


NOTE: Values for tolerance (like dot time or dash time divided by 2 etc) are
     more or less arbitrarily/experimentally chosen.
     
     Sorry the source code is a big mess for now...

Based on Debounce example from the Arduino site for the morse keyer button
(http://www.arduino.cc/en/Tutorial/Debounce).
Loosely based on Morse Decoder 3.5 from 1992 for the Amiga by the same guy.
Contact: raronzen@gmail.com  (not checked too often..)


Copyright (C) 2010 raron


   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.


History:
1992.01.06 - Morse decoder 3.5 - 68000 Assembler version for the Amiga 600
               using a binary tree (dichotomic table) for Morse decoding
2010.11.11 - Rewrite for the Arduino
2010.11.27 - Added Morse encoding via reverse-dichotomic path tracing.
               Thus using the same Morse table / binary tree for encoding and decoding.
2010.11.28 - Added a simple audio Morse signal filter. Added a simple command parser
2010.11.29 - Added echo on/off command



TODO: Make the timings signed long again, to avoid rollover issue
     Avoid calling millis() all the time?
     Make functions or classes out of it sometime...
     Generally tidy it up
*/



// Simple analog input signal threshold (512 = middle-ish)  
// Set high enough to avoid noise, low enough to get signal
// ( 512 + noise < AudioThreshold < 512 + signal )
int AudioThreshold = 700;

// Word-per-minute speed
int wpm = 13;

// Used as a command character to adjust some settings via serial.
const char MorseCommand = '<';

// the debounce time. Keep well below dotTime!!
unsigned long debounceDelay = 20;

// Other Morse variables
unsigned long dotTime = 1200 / wpm;   // morse dot time length in ms
unsigned long dashTime = 3 * 1200 / wpm;
unsigned long wordSpace = 7 * 1200 / wpm;


const int analogPin = 0;       // Analog input pin for audio morse code
const int morseInPin = 7;      // The Morse keyer button
const int morseOutPin =  13;   // For Morse code output
unsigned long markTime = 0;    // timers for mark and space in morse signal
unsigned long spaceTime = 0;   // E=MC^2 ;p
boolean morseSpace = false;    // Flag to prevent multiple received spaces
boolean gotLastSig = true;     // Flag that the last received morse signal is decoded as dot or dash

const int morseTreetop = 31;   // character position of the binary morse tree top.
const int morseTableLength = (morseTreetop*2)+1;
const int morseTreeLevels = log(morseTreetop+1)/log(2);
int morseTableJumper = (morseTreetop+1)/2;
int morseTablePointer = morseTreetop;

// Morse code binary tree table (or, dichotomic search table)
char morseTable[] = "5H4S?V3I?F?U??2E?L?R???A?P?W?J1 6B?D?X?N?C?K?Y?T7Z?G?Q?M8??O9?0";

int morseSignals;              // nr of morse signals to send in one morse character
char morseSignal[] = "......"; // temporary string to hold one morse character's signals to send
int morseSignalPos = 0;
int sendingMorseSignalNr = 0;
unsigned long sendMorseTimer = 0;

boolean morseEcho = true; // Echoes character to encode back to serial and Morse signal input to output pin
boolean listeningAudio = false;
boolean sendingMorse = false;
boolean morseSignalState = false;
boolean lastKeyerState = false;

//unsigned long lastAudioSignalTime = 0;
unsigned long lastDebounceTime = 0;  // the last time the input pin was toggled



void setup()
{
 pinMode(morseInPin, INPUT);
 digitalWrite(morseInPin,HIGH); // internal pullup resistor on
 pinMode(morseOutPin, OUTPUT);
 Serial.begin(9600);
 Serial.println("Morse en-/de-coder by raron");
 markTime = millis();
 spaceTime = millis();
}



void loop()
{
 boolean morseKeyer = !digitalRead(morseInPin); // inverted for active-low input



 // If the switch changed, due to noise or pressing:
 if (morseKeyer != lastKeyerState)
 {
   lastDebounceTime = millis(); // reset timer
   listeningAudio = false;      // disable listen to audio-mode
 }
 


 // debounce the morse keyer, unless listening to audio morse signal
 if (!listeningAudio && (millis() - lastDebounceTime) > debounceDelay)
 {
   // whatever the reading is at, it's been there for longer
   // than the debounce delay, so take it as the actual current state:
   morseSignalState = morseKeyer;
   // differentiante mark and space times
   if (morseSignalState) markTime = lastDebounceTime; else spaceTime = lastDebounceTime;
 }



 // If no manual morse keying the last second, enter audio listen mode
 if (!morseKeyer && millis() - lastDebounceTime > 1000)  listeningAudio = true;



 // Filter audio morse signal
 if (listeningAudio)
 {
   int audioMorse = analogRead(analogPin);
   unsigned long currentTime = millis();

   // Check for an audio signal...
   if (audioMorse > AudioThreshold)
   {
     // If this is a new morse signal, reset morse signal timer
     if (currentTime - lastDebounceTime > dotTime/2)
     {
       markTime = currentTime;
       morseSignalState = true; // there is currently a signal
     }
     lastDebounceTime = currentTime;
   } else {
     // if this is a new pause, reset space time
     if (currentTime - lastDebounceTime > dotTime/2 && morseSignalState == true)
     {
       spaceTime = lastDebounceTime; // not too far off from last received audio
       morseSignalState = false;        // No more signal
     }
   }
 }



 // Morse output, or a feedback when keying.
 if (!sendingMorse && morseEcho) digitalWrite(morseOutPin, morseSignalState);



 // Encode Morse code or execute commands
 if (Serial.available() > 0 && !sendingMorse)
 {
   char encodeMorseChar = Serial.read();
   
   // if a command instead, adjust some settings
   if (encodeMorseChar == MorseCommand)
   {
     // An extremely crude and simple command parser
     // Expects (and wait for) 2 or 4 characters (>i or <Cxxx)
     int digits;
     int value = 0;
     do
     {
       digits = Serial.available();
     } while (digits < 1);
     // Read what setting
     char morseSet = Serial.read();
     // Read 3 digits unless setting is i for info
     if (morseSet != 'i' && morseSet != 'I')
     {
       do
       {
         digits = Serial.available();
       } while (digits < 3);
       // convert value from ASCII
       for (int i=0; i<digits; i++)
       {
         encodeMorseChar = Serial.read();
         value *= 10;
         value += (encodeMorseChar - '0');
       }
     }
     Serial.flush(); // just in case

     // Adjust and print the new setting
     Serial.println();
     switch (morseSet)
     {
       case 'a': // Audio input threshold value
       case 'A':
         AudioThreshold = value;
         if (AudioThreshold<0) AudioThreshold = 0; // not recommended
         Serial.print(" > Audio threshold:");
         Serial.print (value,DEC);
         Serial.println(" <");
         break;
       case 'd': // Debounce value
       case 'D':
         debounceDelay = (unsigned long) value;
         if (debounceDelay<0) debounceDelay = 0;
         Serial.print(" > Debounce (ms):");
         Serial.print (value,DEC);
         Serial.println(" <");
         break;
       case 'e': // Turn on / off Morse echo back to serial and Morse output pin
       case 'E':
         if (value > 0)
         {
            morseEcho = true;
            Serial.println(" > Echo: on <");
         } else {
           morseEcho = false;
           Serial.println(" > Echo: off <");
         }
         break;
       case 'w': // Morse speed setting in wpm
       case 'W':
         wpm = value;
         if (wpm <= 0) wpm = 1;
         dotTime = 1200 / wpm;
         dashTime = 3 * 1200 / wpm;
         wordSpace = 7 * 1200 / wpm;
         Serial.print(" > Morse speed (wpm):");
         Serial.print (value,DEC);
         Serial.println(" <");
         break;
       case 'i': // Display info (current settings).
       case 'I':
         Serial.print(" > Morse speed: ");
         Serial.print (wpm,DEC);
         Serial.print(" wpm (dot=");
         Serial.print (dotTime,DEC);
         Serial.print("ms, dash=");
         Serial.print (dashTime,DEC);
         Serial.print("ms) Debounce: ");
         Serial.print (debounceDelay,DEC);
         Serial.print(" ms. Audio threshold: ");
         Serial.print (AudioThreshold,DEC);
         Serial.println(" <");
         break;
       default:
         Serial.print(" > Unrecognized command <");
     }
     // Mark that we have executed a command (dont send morse)
     encodeMorseChar = MorseCommand;
   }

   if (encodeMorseChar != MorseCommand)
   {
     // change to capital letter if not
     if (encodeMorseChar > 'Z') encodeMorseChar -= 'z'-'Z';
 
     // Scan for the character to send in the Morse table
     int i;
     for (i=0; i<morseTableLength; i++) if (morseTable[i] == encodeMorseChar) break;
     int morseTablePos = i+1;  // 1-based position
     
     // Reverse dichotomic / binary tree path tracing
     
     // Find out what level in the binary tree the character is
     int test;
     for (i=0; i<morseTreeLevels; i++)
     {
       test = (morseTablePos + (0x0001 << i)) % (0x0002 << i);
       if (test == 0) break;
     }
     int startLevel = i;
     morseSignals = morseTreeLevels - i; // = the number of dots and/or dashes
     morseSignalPos = 0;
     
     // Travel the reverse path to the top of the morse table
     if (morseSignals > 0)
     {
       // build the morse signal (backwards from last signal to first)
       for (i = startLevel; i<morseTreeLevels; i++)
       {
         int add = (0x0001 << i);
         test = (morseTablePos + add) / (0x0002 << i);
         if (test & 0x0001 == 1)
         {
           morseTablePos += add;
           // Add a dot to the temporary morse signal string
           morseSignal[morseSignals-1 - morseSignalPos++] = '.';
         } else {
           morseTablePos -= add;
           // Add a dash to the temporary morse signal string
           morseSignal[morseSignals-1 - morseSignalPos++] = '-';
         }
       }
     } else {  // unless it was on the top to begin with (A space character)
       morseSignal[0] = ' ';
       morseSignalPos = 1;
       morseSignals = 1; // cheating a little; a wordspace for a "morse signal"
     }
     morseSignal[morseSignalPos] = '\0';
 
     // Echo back the letter with morse signal as ASCII to serial output
     if (morseEcho)
     {
       Serial.print(encodeMorseChar);
       Serial.print(morseSignal);
       Serial.print(" ");
     }
     if (morseTablePos-1 != morseTreetop)
     {
       Serial.println();
       Serial.print("..Hm..error? MorseTablePos = ");
       Serial.println(morseTablePos);
     }
     // start sending the the character
     sendingMorse = true;
     sendingMorseSignalNr = 0;
     sendMorseTimer = millis();
     if (morseSignal[0] != ' ') digitalWrite(morseOutPin, HIGH);
   }
 }



 // Send Morse signals to output
 if (sendingMorse)
 {
   switch (morseSignal[sendingMorseSignalNr])
   {
     case '.': // Send a dot (actually, stop sending a signal after a "dot time")
       if (millis() - sendMorseTimer >= dotTime)
       {
         digitalWrite(morseOutPin, LOW);
         sendMorseTimer = millis();
         morseSignal[sendingMorseSignalNr] = 'x'; // Mark the signal as sent
       }
       break;
     case '-': // Send a dash (same here, stop sending after a dash worth of time)
       if (millis() - sendMorseTimer >= dashTime)
       {
         digitalWrite(morseOutPin, LOW);
         sendMorseTimer = millis();
         morseSignal[sendingMorseSignalNr] = 'x'; // Mark the signal as sent
       }
       break;
     case 'x': // To make sure there is a pause between signals and letters
       if (sendingMorseSignalNr < morseSignals-1)
       {
         // Pause between signals in the same letter
         if (millis() - sendMorseTimer >= dotTime)
         {
           sendingMorseSignalNr++;
           digitalWrite(morseOutPin, HIGH); // Start sending the next signal
           sendMorseTimer = millis();       // reset the timer
         }
       } else {
         // Pause between letters
         if (millis() - sendMorseTimer >= dashTime)
         {
           sendingMorseSignalNr++;
           sendMorseTimer = millis();       // reset the timer
         }
       }
       break;
     case ' ': // Pause between words (minus pause between letters - already sent)
     default:  // Just in case its something else
       if (millis() - sendMorseTimer > wordSpace - dashTime) sendingMorse = false;
   }
   if (sendingMorseSignalNr >= morseSignals) sendingMorse = false; // Ready to encode more letters
 }



 // Decode morse code
 if (!morseSignalState)
 {
   if (!gotLastSig)
   {
     if (morseTableJumper > 0)
     {
       // if pause for more than half a dot, get what kind of signal pulse (dot/dash) received last
       if (millis() - spaceTime > dotTime/2)
       {
         // if signal for more than 1/4 dotTime, take it as a valid morse pulse
         if (spaceTime-markTime > dotTime/4)
         {
           // if signal for less than half a dash, take it as a dot, else if not, take it as a dash
           // (dashes can be really really long...)
           if (spaceTime-markTime < dashTime/2) morseTablePointer -= morseTableJumper;
           else morseTablePointer += morseTableJumper;
           morseTableJumper /= 2;
           gotLastSig = true;
         }
       }
     } else { // error if too many pulses in one morse character
       Serial.println("<ERROR: unrecognized signal!>");
       gotLastSig = true;
       morseTableJumper = (morseTreetop+1)/2;
       morseTablePointer = morseTreetop;
     }
   }
   // Write out the character if pause is longer than 2/3 dash time (2 dots) and a character received
   if ((millis()-spaceTime >= (dotTime*2)) && (morseTableJumper < 16))
   {
     char morseChar = morseTable[morseTablePointer];
     Serial.print(morseChar);
     morseTableJumper = (morseTreetop+1)/2;
     morseTablePointer = morseTreetop;
   }
   // Write a space if pause is longer than 2/3rd wordspace
   if (millis()-spaceTime > (wordSpace*2/3) && morseSpace == false)
   {
     Serial.print(" ");
     morseSpace = true ; // space written-flag
   }
   
 } else {
   // while there is a signal, reset some flags
   gotLastSig = false;
   morseSpace = false;
 }



 // save last state of the morse signal for debouncing
 lastKeyerState = morseKeyer;
}


