enum parseState {
  waitForGet,
  waitForCommand,
  waitForArg1,
  waitForArg2,
};

const char * www[2] = {
  "GET /",                 /* HTTP request */
  "HTTP/1.0 404 \r\n\r\n"  /* Error response */
};

const int numCommands = 3;
const char * commands[ numCommands ] = {
  "test1/",
  "test12/",
  "sirpa/"
};
boolean flags[ numCommands ];
  
struct rules {
  int stringIdx;
  enum parseState nextS;
} parseRules[] = {
  { 0, waitForCommand },
  { 1, waitForGet }
};

enum parseState pS = waitForGet;


void setup( void )
{
  Serial.begin(9600); // set up Serial library at 9600 baud
  Serial.println("Hello from Arduino WEB-server"); 
//  Serial.println( strlen( www[0] ) );
}

int in;
int cnt;
int commandCnt;

void loop( void )
{
  if (Serial.available() > 0) {
    in = Serial.read(); 
    switch ( pS ) {
      case waitForGet:
        if ( in == www[0][cnt] ) {
          cnt++;
          if ( cnt == strlen( www[0] ) ) {
            Serial.println("Found GET command!"); 
            cnt = 0; 
            pS = waitForCommand;
            for ( int i = 0; i < numCommands; i++ ) flags[i] = true;
            commandCnt = numCommands;
          }
        }
        else
          cnt = 0;
        break;
      case waitForCommand:
        for ( int i = 0; i < numCommands; i++ ) {
          if ( flags[i] ) {
            if ( in == commands[ i ][ cnt ] ) {
              if ( ( cnt + 1 ) == strlen( commands[ i ] ) ) {
                Serial.print( "Found command " ); 
                Serial.println( commands[ i ] ); 
                cnt = -1; // Let the out of loop cnt++ increment to 0.
                pS = waitForGet;
                break; // Break out from the for loop
              }
            }
            else {
              flags[i] = false;
              commandCnt--;
              Serial.print( "Command ruled out " );
              Serial.println( i );
              Serial.println( cnt );
            }
          }
        }
        cnt++;
        if ( 0 == commandCnt ) { 
          pS = waitForGet; 
          cnt = 0;
          Serial.println( "Reset command search" );
        }
        break;
     } /* Switch */
  }
}
