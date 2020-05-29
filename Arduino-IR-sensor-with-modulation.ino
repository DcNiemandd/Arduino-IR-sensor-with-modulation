#include "settings.h"
#include "queue.h"
  
unsigned long  lastLoopTime    = 0;
unsigned long  time_started    = 0;
int     readValue       = 0;
double  dataFreq1       = 0;
double  dataFreq2       = 0;
bool    outputMem       = false;
Queue   fronta1(lengthOfQ);
Queue   fronta2(lengthOfQ);
void(* resetFunc) (void) = 0;   //declare reset function at address 0


void setup() {
  // Output pins
  pinMode(TRANS_1,      OUTPUT);
  pinMode(TRANS_2,      OUTPUT);
  pinMode(output,       OUTPUT);  
  pinMode(outputPulse,  OUTPUT); 
  pinMode(error,        OUTPUT);
  // Input pins
  pinMode(REC_1,        INPUT);  
  pinMode(REC_2,        INPUT);
  pinMode(calibration,  INPUT);

  
  #if defined(LOGS) or defined(LOGS_readed_vals)
    Serial.begin(9600);
  #endif
  
  // PWM setup to 3906 Hz https://forum.arduino.cc/index.php/topic,16612.0.html
  // pins 9 and 10
  TCCR1B = TCCR1B & 0b11111000 | 0x02;
  if(TRANS_1 != 9 or TRANS_2 != 10)
    {
    #ifdef LOGS
      Serial.println("ERROR: Transmitters' pins are set wrong!");      
    #endif 
    digitalWrite(outputPulse, OUTPUT_NEG);  
    digitalWrite(output, OUTPUT_NEG);  
    digitalWrite(error, 1);  
    while(true);
    } 
  lastLoopTime = micros();
}

void loop() {
  for(int i = 0; i < lengthOfQ + 3; i++)
  {  
    // Write modulated output 
    double timy = micros()/1000000.0;
    analogWrite(TRANS_1, int(128 * (1 +  sin(omega * timy)))); 
    analogWrite(TRANS_2, int(128 * (1 +  sin(omega * timy)))); 

    // Read input and save in field
    int read1 = analogRead(REC_1);
    int read2 = analogRead(REC_2);
    fronta1.QPush(read1);
    fronta2.QPush(read2);
    
    // "IOcontroll"
    IOcontroll();
    
    // Set stable loop time
    while(lastLoopTime + loopTime > micros());
    lastLoopTime = micros();
  }
  
  // Math
  // Get freq from field
  unsigned int calibr = analogRead(calibration);
  dataFreq1 = (fronta1.AverageFreq(calibr)*1000000)/loopTime;
  dataFreq2 = (fronta2.AverageFreq(calibr)*1000000)/loopTime;

  // Cause of negative FREQ_DEAD_ZONE 
  dataFreq1 = dataFreq1 == 0 ? FREQ + 5 * FREQ_DEAD_ZONE : dataFreq1;
  dataFreq2 = dataFreq2 == 0 ? FREQ + 5 * FREQ_DEAD_ZONE : dataFreq2;
  
  // Absolute error from wanted freq
  if(abs(dataFreq1 - FREQ) < FREQ_DEAD_ZONE or abs(dataFreq2 - FREQ) < FREQ_DEAD_ZONE) 
  { 
    #ifdef LOGS          
      Serial.print("HAND  "); 
    #endif         
    
    if(outputMem == false)
    {
      time_started = millis();           
    }    
    outputMem = true; 
  }
  else
  {
    #ifdef LOGS
      Serial.print("NOTHING  ");      
    #endif
    outputMem = false;
  }   
  #ifdef LOGS   
    Serial.println((String)"Average Freq: " + dataFreq1 + "  " + dataFreq2);
  #endif

  IOcontroll();
}

void IOcontroll()
{
          //Serial.println((String)"outputMem: " + outputMem + " Zacal: " + time_started + " Ted je: " + millis() + " Rozdil: " + (millis() - time_started));       
    unsigned long timy = millis();
    
    // Pulzes    
    if((time_started + 1000ul * PULSE_TIME) > timy)
    {      
      digitalWrite(outputPulse, !OUTPUT_NEG);
    }
    else
    {
      digitalWrite(outputPulse, OUTPUT_NEG);            
    } 
    
    // Stable output 
    if(outputMem and ((time_started + 1000ul * MAX_TIME) > timy))   
    {   
      digitalWrite(output, !OUTPUT_NEG);  
    }
    else
    {
      digitalWrite(output, OUTPUT_NEG);
    }

    if((4294900000ul < timy) and !outputMem and ((time_started + 1000ul * PULSE_TIME) < timy))
    {      
      //resetFunc();  //call reset
    }
    
    // Error timing   
    #if enable_ERROR
      if(outputMem and ((time_started + 1000ul * ERROR_TIME) < timy))     
        {
        #ifdef LOGS
          Serial.println("ERROR: Sensing too long!");      
        #endif 
        digitalWrite(outputPulse, OUTPUT_NEG);  
        digitalWrite(output, OUTPUT_NEG);  
        digitalWrite(error, 1);  
        while(true);
        }     
    #endif 
}
