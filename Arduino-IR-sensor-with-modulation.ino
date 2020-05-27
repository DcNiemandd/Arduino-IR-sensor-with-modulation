#include "queue.h"

// Pinout  
#define TRANS_1         9  
#define REC_1           A2
#define TRANS_2         10
#define REC_2           A3
#define output          11
#define outputPulse     8
#define calibration     A0

// Defines
#define FREQ            20          //Hz    Elektronicky predradnik 30-50 kHz
#define FREQ_DEAD_ZONE  5            //Hz
#define omega           2 * PI * FREQ //rad/s
const int loopTime    = 2000;         //us    Vzorkovaci cas, PWM freq is 490 Hz
#define lengthOfQ       250
#define OFFSET          PI / 2        //rad 
#define OFFSET_DEAD_ZONE PI / 32      //rad   
  
double  lastLoopTime    = 0;
int     readValue       = 0;
int     loopCounter     = 0;
double  dataFreq1       = 0;
double  dataFreq2       = 0;
//double  dataOffset      = 0;
bool    outputPulseMem  = 0;
Queue   fronta1(lengthOfQ);
Queue   fronta2(lengthOfQ);

void setup() {
  // Output pins
  pinMode(TRANS_1, OUTPUT);
  pinMode(TRANS_2, OUTPUT);
  pinMode(output, OUTPUT);  
  //pinMode(outputPulse, OUTPUT);
  // Input pins
  pinMode(REC_1, INPUT);  
  pinMode(REC_2, INPUT);
  pinMode(calibration, INPUT);

  // PWM setup to 3906 Hz https://forum.arduino.cc/index.php/topic,16612.0.html
  // pins 9 and 10
  TCCR1B = TCCR1B & 0b11111000 | 0x02;
  
  Serial.begin(9600);
  lastLoopTime = micros();
}

void loop() {
  for(int i = 0; i < lengthOfQ + 3; i++)
  {  
  // Write modulated output 
  double timy = micros()/1000000.0;
  int transSinValue1 = int(128 * (1 +  sin((omega* timy))));
  int transSinValue2 = int(128 * (1 +  sin((omega* timy))));
  analogWrite(TRANS_1, transSinValue1); 
  analogWrite(TRANS_2, transSinValue2); 
  
  // Read input and save in field
  int readVal1 = analogRead(REC_1);
  int readVal2 = analogRead(REC_2); 
  fronta1.QPush(readVal1);
  fronta2.QPush(readVal2);
  // Set stable loop time
  while(lastLoopTime + loopTime > micros());
  lastLoopTime = micros();
  }
  
  // Math
    // Get freq from field
    dataFreq1 = (fronta1.AverageFreq(analogRead(calibration))*1000000)/loopTime;
    dataFreq2 = (fronta2.AverageFreq(analogRead(calibration))*1000000)/loopTime;;    
    //dataOffset = fronta1.Offset(fronta2, loopTime);
    //dataOffset *= dataFreq1 * 2 * PI;
    if(/*abs(dataOffset - OFFSET) < OFFSET_DEAD_ZONE and */abs(dataFreq1 - FREQ) < FREQ_DEAD_ZONE or abs(dataFreq2 - FREQ) < FREQ_DEAD_ZONE) // Absolute error from wanted freq and offset
    {           
      Serial.println("HAND"); 
      digitalWrite(output, 0);
      if(!outputPulseMem)        
        digitalWrite(outputPulse, 0);        
      else
        digitalWrite(outputPulse, 1);
      outputPulseMem = true;
    }
    else
    {
      Serial.println("NOTHING");      
      digitalWrite(outputPulse, 1);
      digitalWrite(output, 1);
      outputPulseMem = false;
    }      
    Serial.println((String)"Average Freq: " + dataFreq1 + "  " + dataFreq2);
    loopCounter = 0;
    
  delayMicroseconds(500);
  digitalWrite(outputPulse, 1);

}
