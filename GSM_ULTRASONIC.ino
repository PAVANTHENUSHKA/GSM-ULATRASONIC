#include <SoftwareSerial.h>

// Define pins for the ultrasonic sensor
#define TRIG_PIN1  2 // Trigger pin
#define ECHO_PIN1  3 // Echo pin
#define TRIG_PIN2  11 // Trigger pin
#define ECHO_PIN2  12 // Echo pin

int rice_st=5;
int dhal_st=6;

// Define variables
long duration1; // Variable to store the duration of sound wave 
long duration2; // Variable to store the duration of sound wave
int distance1; // Variable to store the distance in centimeters
int distance2;

unsigned long long time1 = 0;
unsigned long long time2 = 0;


// Define GSM module pins
#define GSM_TX_PIN 8
#define GSM_RX_PIN 7

// Set up software serial for GSM module
SoftwareSerial gsmSerial(GSM_TX_PIN, GSM_RX_PIN);

// Phone number to send the message
String phoneNumber = "+94713382118"; // Replace with the recipient's phone number

// Threshold distance values
int threshold1 = 15; // Define threshold for distance1
int threshold2 = 15; // Define threshold for distance2

bool response_sent1 = false;
bool response_sent2 = false;

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  gsmSerial.begin(9600); // Start GSM module serial communication
  Serial.println("GSM Module Initialized");

  // Initialize the GSM module
  sendCommand("AT"); // Check if the module is responsive
  sendCommand("AT+CMGF=1"); // Set SMS to text mode

  // Initialize the trigger and echo pins
  pinMode(TRIG_PIN1, OUTPUT);
  pinMode(ECHO_PIN1, INPUT);
  pinMode(TRIG_PIN2, OUTPUT);
  pinMode(ECHO_PIN2, INPUT);

  pinMode(rice_st, OUTPUT);
  pinMode(dhal_st, OUTPUT);

  // Wait for the sensor to settle
  delay(1000);
}

void loop() {
  measureAndSendSMS(); // Call the function to measure distance and send SMS if necessary
}

// Function to measure distance and send SMS if thresholds are exceeded
void measureAndSendSMS() {
  // Send a pulse to the sensor to trigger the measurement
  digitalWrite(TRIG_PIN1, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN1, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN1, LOW);
  duration1 = pulseIn(ECHO_PIN1, HIGH);
  distance1 = duration1 * 0.034 / 2;

  // Print the distance to the serial monitor
  Serial.print("Distance1: ");
  Serial.print(distance1);
  Serial.println(" cm");


  digitalWrite(TRIG_PIN2, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN2, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN2, LOW);

  // Measure the duration of the echo pulse
  
  duration2 = pulseIn(ECHO_PIN2, HIGH);

  // Calculate the distance in centimeters
  
  distance2 = duration2 * 0.034 / 2;

  
  Serial.print("Distance2: ");
  Serial.print(distance2);
  Serial.println(" cm");

  // Check if distance1 exceeds threshold
  if (distance1 > threshold1) {
    digitalWrite(rice_st,HIGH);
    delay(500);
    digitalWrite(rice_st,LOW);

    // Send message for distance1
    Serial.println("rice is low level");

    if(!response_sent1 || (millis()- time1) > 3600*1000){
      sendMessage("Pour dhal into the container.");
      time1 = millis();
      Serial.println(double(time1));
      Serial.println("Send sms rice");
      response_sent1 = true;
    }    
    
  }
  else{response_sent1 = false;}

  // Check if distance2 exceeds threshold
  if (distance2 > threshold2) {
    digitalWrite(dhal_st,HIGH);
    delay(500);
    digitalWrite(dhal_st,LOW);

    // Send message for distance1
    Serial.println("dhal is low level");

    if(!response_sent2 || (millis()- time2) > 36000*1000){
      sendMessage("Pour rice into the container.");
      time2 = millis();
      Serial.println("Send sms dhal");
      response_sent2 = true;
    } 
  }
  else {response_sent2 = false;}
    // Send message for distance2
    

    
  

  // Wait a short time before taking the next measurement
  delay(500);
}

// Function to send SMS using GSM module
void sendMessage(String message) {
  // AT command to set GSM module in text mode
  Serial.println("sending...");
  gsmSerial.println("AT+CMGF=1");

  delay(1000); // Wait for a second

  // AT command to set the recipient phone number
  gsmSerial.print("AT+CMGS=\"");
  gsmSerial.print(phoneNumber);
  gsmSerial.println("\"");

  delay(1000); // Wait for a second

  // Send the message
  gsmSerial.println(message);

  delay(1000); // Wait for a second

  // End SMS transmission
  gsmSerial.println((char)26);

  delay(2000); // Wait for a second
}

// Function to send AT command and wait for a response
void sendCommand(String command) {
  gsmSerial.println(command);
  delay(1000); // Wait for the GSM module to process the command
  while (gsmSerial.available()) {
    char c = gsmSerial.read();
    Serial.print(c); // Print the response to the serial monitor
  }
}
