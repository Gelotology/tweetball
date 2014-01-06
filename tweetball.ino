#include <WiFlyHQ.h>
#include <SoftwareSerial.h>

SoftwareSerial wifiSerial(9,10); // init WiflyHQ on pins 9 (RX) and 10 (TX) on the Arduino.
SoftwareSerial lcd(3,2); // init LCD on pin 2 (TX) on the Arduino. Pin 3 is unused.

WiFly wifly;

// Change these to match your WiFi network 

const char mySSID[] = "Your SSID";
const char myPassword[] = "Your Password";

const char site[] = "yoursite.com";

void terminal();


// Inititialize variables

int maxNumberLines = 4; // Define the maximum number of lines on your LCD for parsing (4 if you're using a 20X4 LCD)
int maxNumberChars = 20; // Define the maximum number of lines on your LCD  for parsing (20 if you're using a 20X4 LCD)

int startOfSegment = 0;

int parseString = 0;
int lineBreak = 0;
int lastLineBreak = 0;

char myTweetArray[160];
int tweetArrayLength = 160;

long lastTimeResult = millis();
long lastTime = millis();

int index = 0;
int getResults = 0;

void resetVariables() { // Reset all the variables and clean the tweet array to avoid caching issues

  getResults = 0;
  clearLCD(); 
  index = 0;
  parseString = 0;
  startOfSegment = 0; 

  for (int i = 0; i < 160; i++){
    myTweetArray[i] = ' ';
  }

}

// clear display and move cursor to the first line 

void clearLCD() { 

  lcd.write("                    "); 
  lcd.write("                    ");
  lcd.write("                    ");
  lcd.write("                    ");

  lcd.write(254); 
  lcd.write(128);

}

// move the LCD cursor to the next consecutive line, based on the for loop variable.

void gotoNextLine(int currentLine) { 
  
     
   if (currentLine == 0) {
     lcd.write(254); 
     lcd.write(192);
   }
   
   if (currentLine == 1) {
     lcd.write(254); 
     lcd.write(148);  
   }
   
   if (currentLine == 2) {
     lcd.write(254); 
     lcd.write(212); 
   }
              
}
   
void setup(void)
{

  lcd.begin(9600);  // start lcd serial

  clearLCD();
  lcd.write("The Tweetball");

  char buf[32];

  Serial.begin(115200);
  Serial.println("Starting");

  wifiSerial.begin(9600);
  if (!wifly.begin(&wifiSerial, &Serial)) {
    Serial.println("Failed to start wifly");
    terminal();
  }

  /* Join wifi network if not already associated */
  if (!wifly.isAssociated()) {
    /* Setup the WiFly to connect to a wifi network */

    Serial.println("Joining network");
    wifly.setSSID(mySSID);
    wifly.setPassphrase(myPassword);
    wifly.enableDHCP();

    if (wifly.join()) {
      Serial.println("Joined wifi network");
    } 
    else {
      Serial.println("Failed to join wifi network");
      terminal();
    }
  } 
  else {
    Serial.println("Already joined network");
  }


  wifly.setDeviceID("Wifly-WebClient");

  if (wifly.isConnected()) {
    Serial.println("Old connection active. Closing");
    wifly.close();
  }

  if (wifly.open(site, 80)) {
    Serial.print("Connected to ");
    Serial.println(site);


  } 
  else {
    Serial.println("Failed to connect");

  }

  clearLCD();
}

void loop() {


  while (wifly.available() > 0) {

    char ch = wifly.read();

    if (parseString > 0) {

      myTweetArray[index] = ch;
      index++;
    }

    // The ! marks the beginning of the tweet. We can start storing the characters into an array now.

    if (ch == '!') {

      parseString = 1;

    }

    // The ~ is the end of the tweet. Delete it from the array and stop storing characters.

    if (ch == '~') {

      myTweetArray[index-1] = ' ';
      parseString = 0;

    }
  } 


  if (Serial.available() > 0) {

    wifly.write(Serial.read());
  }


  if (getResults < 1) { 

    wifly.println("GET /ball.php HTTP/1.1");
    wifly.println("Host: yoursite.com");
    wifly.println();

    getResults = 1;
  }


  if ((millis() - lastTime) > 5000) {

    while (startOfSegment < tweetArrayLength) {
    
      clearLCD();

      // We want to loop through the function as much times as we have lines on the LCD. We've defined this in our maxNumberLines variable.

      for (int i = 0; i < maxNumberLines; i++) {

        // Every line has a maximum number of characters, as defined in maxNumberChars. In order to prevent words being cut off, we're going to look for the last "SPACE" in that line and store it in the int lineBreak.

        for (int j = startOfSegment; j < (startOfSegment + maxNumberChars + 1); j++){

          if (myTweetArray[j] == ' '){   

            lineBreak = j; 
          }       

        }

        // Sometimes words (often URLs) are just too long to be parsed properly. So we'll just cut them off. 

        if (lineBreak == lastLineBreak) {
          lineBreak = lastLineBreak + maxNumberChars;  
        } 

        // Display the line on the LCD.

        for (int k = startOfSegment; k < lineBreak; k++){

          if (startOfSegment < tweetArrayLength) {
            
            // small tweak to remove the first " " from every line other than the first. 
            
            if ((i > 0) && (k == startOfSegment) && (myTweetArray[startOfSegment] == ' ')) {
            } else {   
              lcd.write(myTweetArray[k]); 
            }
            startOfSegment++;
          }
        }

        lastLineBreak = lineBreak;
        gotoNextLine(i);

      }

      // The script will jump to the next x number of lines automatically. So we'll add a small delay to make sure you can actually read the display.

      delay(3000);

    }
    lastTime = millis();

  }


  if ((millis() - lastTimeResult) > 10000) {

    lastTimeResult = millis();
    resetVariables();

  }

}


/* Connect the WiFly serial to the serial monitor. */
void terminal()
{
  while (1) {
    if (wifly.available() > 0) {
      Serial.write(wifly.read());
    }


    if (Serial.available() > 0) {
      wifly.write(Serial.read());
    }
  }
}


