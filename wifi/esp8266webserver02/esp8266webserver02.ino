enum parseState {
  waitForGet,
  waitForCommand,
  waitForArg1,
  waitForArg2,
};

const char * www[1] = {
  "GET/",
};

const char * commands[] = {
  "test1/",
  "test12/",
  "sirpa/"
};
const int numCommands = 3;
char flags[numCommands];
  
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
            for ( int i = 0; i < numCommands; i++ ) flags[i] = 0;
            commandCnt = numCommands;
          }
        }
        else
          cnt = 0;
        break;
      case waitForCommand:
        for ( int i = 0; i < numCommands; i++ ) {
          if ( flags[i] >= 0 ) {
            if ( in == commands[ i ][ flags[ i ] ] ) {
              flags[ i ]++;
              if ( flags[ i ] == strlen( commands[ i ] ) ) {
                Serial.print( "Found command " ); 
                Serial.println( i ); 
                pS = waitForGet;;
              }
            }
            else {
              flags[i] = -1;
              commandCnt--;
              Serial.print( "Command ruled out " );
              Serial.println( i );
              Serial.println( cnt );
            }
          }
        }
        if ( 0 == commandCnt ) { 
          pS = waitForGet; 
          Serial.println( "Reset command search" );
        }
        break;
     
    } /* Switch */
  }
}
