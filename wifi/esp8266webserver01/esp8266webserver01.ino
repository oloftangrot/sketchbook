/*
Start up a very mini web server
 ******* This requires the Mega 2560 board *****
 ******* This requires the Mega 2560 board *****
 ******* This requires the Mega 2560 board *****
 ******* This requires the Mega 2560 board *****
 ******* This requires the Mega 2560 board *****
 ******* This requires the Mega 2560 board *****
 ******* This requires the Mega 2560 board *****
 */


#define TIMEOUT 5000 // mS
#define GREENLED 4
#define REDLED 5
#define BLUELED 3

boolean RED_State = false;
boolean GREEN_State = false;
boolean BLUE_State = false;

void setup() 
{
 pinMode(REDLED,OUTPUT); 
 pinMode(GREENLED,OUTPUT);
 pinMode(BLUELED,OUTPUT);

 Serial.begin(115200);
 Serial1.begin(115200);

 int CommandStep = 1;

 //This initializes the Wifi Module as a server 
 BlinkLED(REDLED,CommandStep,50); 
 SendCommand("AT+RST", "Ready", true);
 BlinkLED(GREENLED,CommandStep,50);
 CommandStep++;

 BlinkLED(REDLED,CommandStep,50); 
 SendCommand("AT+GMR", "OK", true);
 BlinkLED(GREENLED,CommandStep,50);
 CommandStep++;

 delay(3000);

 BlinkLED(REDLED,CommandStep,50); 
 SendCommand("AT+CIFSR", "OK", true);
 BlinkLED(GREENLED,CommandStep,50);
 CommandStep++;


 BlinkLED(REDLED,CommandStep,50); 
 SendCommand("AT+CIPMUX=1","OK",true);
 BlinkLED(GREENLED,CommandStep,50);
 CommandStep++;

 BlinkLED(REDLED,CommandStep,50); 
 SendCommand("AT+CIPSERVER=1,80","OK",true);
 BlinkLED(GREENLED,CommandStep,50);

 digitalWrite(GREENLED,HIGH);
 //----------------------------------------------------
}

void BlinkLED(int LEDPin, int NumberOfBlinks, int OnDuration)
{
 for (int x=1; x <= NumberOfBlinks ; x ++){
 digitalWrite(LEDPin,HIGH);
 delay(OnDuration);
 digitalWrite(LEDPin,LOW);
 delay(OnDuration); 
 }
}


void loop(){
 String IncomingString="";
 char SingleChar;
 boolean StringReady = false;


 //*** Handle each character that is coming in from the ESP8266 ****
 while(Serial1.available()) {
   IncomingChar (Serial1.read ());
 } 
 
 while(Serial.available()) {
   Serial1.write(Serial.read());
 }
}

void ProcessCommand (const char * data)
{
 Serial.println (data);

 String IncomingLine = String(data);

 if (IncomingLine.indexOf("GET / ") != -1) {
 char ClientIdChar = IncomingLine.charAt(5);
 echoFind("OK"); 
 delay(100);
 SendHTML(String(ClientIdChar));
 }

} 


void SendHTML(String ClientId){
 Serial.println("Someone requested a HTML Page on Client Id:" + ClientId );

 SendClient("<HTML><HEAD><TITLE>Pete's Mini8266 Server</TITLE></HEAD>" 
 "<BODY><H1>Welcome to Pete's ESP8266 \"hacking\" project</H1>",ClientId);
 SendClient("<BR><BR> Up time: ",ClientId);
 SendClient(String(millis()),ClientId);
 SendClient("</BODY></HTML>",ClientId);
 SendCommand("AT+CIPCLOSE="+ ClientId,"OK",true);
}

void SendClient(String ToClientString,String ClientID){
 SendCommand("AT+CIPSEND=" + ClientID + ","+ (ToClientString.length() +2) ,">",false);
 SendCommand(ToClientString,"OK",false);
}



void IncomingChar (const byte InChar)
{
 static char InLine [500]; //Hope we don't get more than that in one line
 static unsigned int Position = 0;

 switch (InChar)
 {
 case '\r': // Don't care about carriage return so throw away.
 break;
 
 case '\n': 
 InLine [Position] = 0; 
 ProcessCommand (InLine);
 Position = 0; 
 break;

 default:
 InLine [Position++] = InChar;
 } 

} 


//************** This section specific to simple AT command with no need to parse the response ************
// Warning: Will drop any characters coming in while waiting for the indicated response.
boolean SendCommand(String cmd, String ack, boolean halt_on_fail)
{
 Serial1.println(cmd); // Send command to module

 // Otherwise wait for ack.
 if (!echoFind(ack)) // timed out waiting for ack string
 if (halt_on_fail)
 errorHalt(cmd+" failed");// Critical failure halt.
 else
 return false; // Let the caller handle it.
 return true; // ack blank or ack found
}

// Read characters from WiFi module and echo to serial until keyword occurs or timeout.
boolean echoFind(String keyword)
{
 byte current_char = 0;
 byte keyword_length = keyword.length();

 // Fail if the target string has not been sent by deadline.
 long deadline = millis() + TIMEOUT;
 while(millis() < deadline)
 {
 if (Serial1.available())
 {
 char ch = Serial1.read();
 Serial.write(ch);
 if (ch == keyword[current_char])
 if (++current_char == keyword_length)
 {
 Serial.println();
 return true;
 }
 }
 }
 return false; // Timed out
}

// Print error message and loop stop.
void errorHalt(String msg)
{
 Serial.println(msg);
 Serial.println("HALT");
 digitalWrite(REDLED,HIGH);
 digitalWrite(GREENLED,HIGH);
 while(true) {
 };
}


