#define BLYNK_TEMPLATE_ID "TMPL6hgJ57ZZW"
#define BLYNK_TEMPLATE_NAME "Counter and Organizer"
#define BLYNK_DEVICE_NAME "OLED Pill Organizer and Dispenser"
#define BLYNK_AUTH_TOKEN "chacZeY4aXm1GbokcrSb5iwuMtnuAm75"

#define BLYNK_PRINT Serial
#include <Arduino.h>
#include <analogWrite.h>
#include <WiFi.h>
#include <SPI.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP32Servo.h>

char auth[] = BLYNK_AUTH_TOKEN;
// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Sulcaton4A";
char pass[] = "09162000";

#define IR1 5  // IR sensor 1
#define IR2 18 // IR sensor 2
#define SCREEN_WIDTH 128 // OLED display width, in pixels 
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET 4 
#define SCREEN_ADDRESS 0X3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int pos;
int med1 = 0;
int med2 = 0;
bool counter1 = false;
bool counter2 = false;
int x = 0;
int y = 16;

// Servo

const int servoCount = 2;
static const int servosPins[servoCount] = {12, 14};

Servo servos[servoCount];

// -------------------

void countmed1(){
  if(counter1 == true){
    counter1 = false;
    med1 = med1 + 1;
    Serial.print("Number of med1 = ");
    Serial.println(med1/2);
    Blynk.virtualWrite(V0,med1/2);
    if(med1 >= 20){
      Blynk.logEvent("container1_full");
    }
  }
  
}

void countmed2(){
  if(counter2 == true){
    counter2 = false;
    med2 = med2 + 1;
    Serial.print("Number of med2 = ");
    Serial.println(med2/2);
    Blynk.virtualWrite(V1,med2/2);
    if(med2 >= 20){
      Blynk.logEvent("container2_full");
    }
  }
  
}

void docounter1() {
  counter1 = true;
}

void docounter2() {
  counter2 = true;
}

void myTimerEvent()
{
  // You can send any value at any time.
  // Please don't send more than 10 values per second.
  Blynk.virtualWrite(V3, millis() / 1000);
}

void oledAleart(){
    display.clearDisplay();
    display.setTextSize(2);    
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(1,4);
  if (med1 <= 4 && med2 <= 4){
    display.println(F("LOW DRUGS"));
    display.setCursor(1,30);
    display.setTextSize(1);
    display.println(F("Please fill"));
    display.setCursor(1,37);
    display.println(F("all container"));
  }
  else if(med1 >= 20 && med2 >= 20){
    display.println(F("FULL DRUGS"));
    display.setCursor(1,30);
    display.setTextSize(1);
    display.println(F("Have a"));
    display.setCursor(1,37);
    display.println(F("nice day")); 
  }
  display.display();
}


BlynkTimer timer;

BLYNK_WRITE(V3){
  int value = param.asInt();
  if(value = true){
    med1 = 0;
    med2 = 0;
    Blynk.virtualWrite(V0,med1/2);
    Blynk.virtualWrite(V1,med2/2);
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(20,2);
    display.println(F("RESETTING"));
    display.display();
    delay(3000);
  }
  
}


BLYNK_WRITE(V4){
  int value = param.asInt();
  int pos = 0;
  Serial.println("Now, Container 1 is working");
  Serial.println(value);
  if(value >= 1){
    med1 = med1 - 2;
    Serial.print("Number of med1 = ");
    Serial.println(med1/2);
    Blynk.virtualWrite(V0,med1/2);
    for(pos = 0; pos <= 66; pos += 1){
      servos[0].write(pos);
      delay(15);
    }  
    for(pos = 66; pos >= 0; pos -= 1){
      servos[0].write(pos);
    }    
    if(med1 <= 0 && med2 <= 0){
      Blynk.logEvent("out_of_medicine","Please fill up all containers !!!");
      display.clearDisplay();
      display.setTextSize(2);    
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(1,4);
      display.println(F("OUT OF"));
      display.setCursor(1,20);
      display.println(F("Medicine"));
    }
    else if(med1 <= 4){
    Blynk.logEvent("container_1_low","Please fill medicine in container 1");
    }  
  }
}

BLYNK_WRITE(V5){
  int value = param.asInt();
  int pos = 0;
  Serial.println("Now, Container 2 is working");
  Serial.println(value);
  if(value >= 1){
    med2 = med2 - 2;
    Serial.print("Number of med2 = ");
    Serial.println(med2/2);
    Blynk.virtualWrite(V1,med2/2);
    for(pos = 0; pos <= 66; pos += 1){
      servos[1].write(pos);
      delay(15);
    }  
    for(pos = 66; pos >= 0; pos -= 1){
      servos[1].write(pos);
    }    
    if(med1 <= 0 && med2 <= 0){
      Blynk.logEvent("out_of_medicine","Please fill up all containers !!!");
      display.clearDisplay();
      display.setTextSize(2);    
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(1,4);
      display.println(F("OUT OF"));
      display.setCursor(1,20);
      display.println(F("Medicine"));
    }
    else if(med1 <= 4){
    Blynk.logEvent("container_2_low","Please fill medicine in container 2");
    }  
  }
}


void setup() {
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);
  pinMode(IR1, INPUT);
  pinMode(IR2, INPUT);
  attachInterrupt(digitalPinToInterrupt(IR1), docounter1, RISING);
  attachInterrupt(digitalPinToInterrupt(IR2), docounter2, RISING);
  timer.setInterval(1000L, myTimerEvent);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)){
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.display();
  delay(2000);
  // Clear the buffer
  display.clearDisplay();
  display.setTextSize(2);              // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);    // Draw white text
  display.setCursor(1,4);              // Start at the top-left corner
  display.println(F("STARTING..."));
  display.setTextSize(1.5);
  display.setCursor(2,25);              // Start at the top-left corner
  display.println(F("PLEASE SETUP"));
  display.setCursor(2,40);
  display.println(F("ON BLYNK"));
  display.drawLine(0, 0, 0, 60, SSD1306_WHITE);
  display.drawLine(0, 60, 127, 60, SSD1306_WHITE);
  display.drawLine(127, 60, 127, 0, SSD1306_WHITE);
  display.drawLine(127, 0, 0, 0, SSD1306_WHITE);
  display.display();
  delay(5000);
  for(int i = 0; i < servoCount; ++i){
    servos[i].attach(servosPins[i]);
  } 

}


void loop() {
  Blynk.run();
  timer.run();
  countmed1();
  countmed2();
  oledAleart();
}
