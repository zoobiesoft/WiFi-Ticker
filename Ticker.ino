#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>
#include <ESP8266WiFi.h>
#include "TickerTXT.h"

int pinCS = 12; // Attach CS to this pin, DIN to MOSI and CLK to SCK (cf http://arduino.cc/en/Reference/SPI )
int numberOfHorizontalDisplays = 9;
int numberOfVerticalDisplays = 1;
int z=0;
int pixelsInText;

Max72xxPanel matrix = Max72xxPanel(pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);

const char WiFiAPPSK[] = "1234567890";       // Note... need to change
WiFiServer server(80);
String tape =" 123 ";
int wait = 20; // In milliseconds

int spacer = 1;
int width = 5 + spacer; // The font width is 5 pixels

void setup() {
  Serial.begin(115200);
    
  matrix.setIntensity(3); // Use a value between 0 and 15 for brightness

// Adjust to your own needs
  matrix.setPosition(0, 0, 0);
  matrix.setPosition(1, 1, 0);
  matrix.setPosition(2, 2, 0);
  matrix.setPosition(3, 3, 0);
  matrix.setPosition(4, 4, 0);
  matrix.setPosition(5, 5, 0);
  matrix.setPosition(6, 6, 0);
  matrix.setPosition(7, 7, 0);
  matrix.setPosition(8, 8, 0);
//  ..
  matrix.setRotation(0, 1);
  matrix.setRotation(1, 1);
  matrix.setRotation(2, 1);
  matrix.setRotation(3, 1);
  matrix.setRotation(4, 1);
  matrix.setRotation(5, 1);
  matrix.setRotation(6, 1);
  matrix.setRotation(7, 1);
  matrix.setRotation(8, 1);

  setupWiFi();
  server.begin();

}

void loop() {

  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    //return;
  }

  // Read the first line of the request
  String command1 = client.readStringUntil('/');
  String command = client.readStringUntil('/');
  // Serial.println(command);

  if (command == "text") {
    tape = client.readStringUntil('/');
    z=1;
    tape.replace("%20", " ");
    pixelsInText = (tape.length() * 7)+8;
      //Serial.println(tape);
  } else {  // Prepare the response
      String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
      s += file1;  
      client.flush();
    
      // Send the response to the client
    while(s.length()>2000)
    {
      String dummy = s.substring(0,2000);
      client.print(dummy);
      s.replace(dummy," ");
    }
      
      client.print(s);
      delay(1);
      //Serial.println("Client disconnected");
    
      // The client will actually be disconnected 
      // when the function returns and 'client' object is destroyed
  }

  for ( int i = 0 ; i < width * tape.length() + matrix.width() - 1 - spacer; i++ ) {

    matrix.fillScreen(LOW);

    int letter = i / width;
    int x = (matrix.width() - 1) - i % width;
    int y = (matrix.height() - 8) / 2; // center the text vertically

    while ( x + width - spacer >= 0 && letter >= 0 ) {
      if ( letter < tape.length() ) {
        matrix.drawChar(x, y, tape[letter], HIGH, LOW, 1);
      }

      letter--;
      x -= width;
    }

    matrix.write(); // Send bitmap to display

    delay(wait);
  }
}

void setupWiFi()
{
  WiFi.mode(WIFI_AP);

  // Do a little work to get a unique-ish name. Append the
  // last two bytes of the MAC (HEX'd) to "Thing-":
  uint8_t mac[WL_MAC_ADDR_LENGTH];
  WiFi.softAPmacAddress(mac);
  String macID = String(mac[WL_MAC_ADDR_LENGTH - 2], HEX) +
                 String(mac[WL_MAC_ADDR_LENGTH - 1], HEX);
  macID.toUpperCase();
  String AP_NameString = "Ticker" + macID;

  char AP_NameChar[AP_NameString.length() + 1];
  memset(AP_NameChar, 0, AP_NameString.length() + 1);

  for (int i=0; i<AP_NameString.length(); i++)
    AP_NameChar[i] = AP_NameString.charAt(i);

  WiFi.softAP(AP_NameChar, WiFiAPPSK);
}
