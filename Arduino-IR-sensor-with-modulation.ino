#define LOGS

#include "queue.h"

// Pinout  
#define TRANS_1         9  // NEMENIT 
#define TRANS_2         10 // NEMENIT
#define REC_1           A2
#define REC_2           A3
#define output          LED_BUILTIN
#define outputPulse     8
#define calibration     A0

// Defines
#define FREQ            20          //Hz    Elektronicky predradnik 30-50 kHz
#define FREQ_DEAD_ZONE  15            //Hz
#define omega           2 * PI * FREQ //rad/s
const int loopTime    = 2000;         //us    Vzorkovaci cas, PWM freq is 490 Hz
#define lengthOfQ       250
#define OFFSET          PI / 2        //rad 
#define OFFSET_DEAD_ZONE PI / 32      //rad   
  
double  lastLoopTime    = 0;
int     readValue       = 0;
double  dataFreq1       = 0;
double  dataFreq2       = 0;
bool    outputPulseMem  = 0;
Queue   fronta1(lengthOfQ);
Queue   fronta2(lengthOfQ);

void setup() {
  // Output pins
  pinMode(TRANS_1, OUTPUT);
  pinMode(TRANS_2, OUTPUT);
  pinMode(output, OUTPUT);  
  pinMode(outputPulse, OUTPUT);
  // Input pins
  pinMode(REC_1, INPUT);  
  pinMode(REC_2, INPUT);
  pinMode(calibration, INPUT);

  // PWM setup to 3906 Hz https://forum.arduino.cc/index.php/topic,16612.0.html
  // pins 9 and 10
  TCCR1B = TCCR1B & 0b11111000 | 0x02;
  #ifdef LOGS
    Serial.begin(9600);
  #endif
  lastLoopTime = micros();
}

void loop() {
  for(int i = 0; i < lengthOfQ + 3; i++)
  {  
    // Write modulated output 
    double timy = micros()/1000000.0;
    analogWrite(TRANS_1, int(128 * (1 +  sin((omega* timy))))); 
    analogWrite(TRANS_2, int(128 * (1 +  sin((omega* timy))))); 
    
    // Read input and save in field
    fronta1.QPush(analogRead(REC_1));
    fronta2.QPush(analogRead(REC_2));
    
    // Set stable loop time
    while(lastLoopTime + loopTime > micros());
    lastLoopTime = micros();
  }
  
  // Math
  // Get freq from field
  dataFreq1 = (fronta1.AverageFreq(analogRead(calibration))*1000000)/loopTime;
  dataFreq2 = (fronta2.AverageFreq(analogRead(calibration))*1000000)/loopTime;
  
  // Absolute error from wanted freq
  if(abs(dataFreq1 - FREQ) < FREQ_DEAD_ZONE or abs(dataFreq2 - FREQ) < FREQ_DEAD_ZONE) 
  { 
    #ifdef LOGS          
      Serial.println("HAND"); 
    #endif
    if(!outputPulseMem)        
      digitalWrite(outputPulse, 0);   
    digitalWrite(output, 0);
    outputPulseMem = true;
  }
  else
  {
    #ifdef LOGS
      Serial.println("NOTHING");      
    #endif
    digitalWrite(outputPulse, 1);
    digitalWrite(output, 1);
    outputPulseMem = false;
  }   
  #ifdef LOGS   
    Serial.println((String)"Average Freq: " + dataFreq1 + "  " + dataFreq2);
  #endif
  
  delayMicroseconds(500);
  digitalWrite(outputPulse, 1);

}
