#ifndef Q_THINGS
#define Q_THINGS



class Queue{
private:  
  int Q_length;
  unsigned *data;  
  Queue(const Queue& Q);
public: 
  Queue(int _Q_length = 500): Q_length(_Q_length)  
  {
    data = new unsigned[Q_length];
  }
  ~Queue()
  {
    delete[] data;
  }
  
  void QPush(unsigned value)
  {
    for(int i = 1; i < Q_length; i++)
    {
      data[i-1] = data[i];
    }

    data[Q_length-1] = value;    
  }

  // Self-explanatory
  double AverageFreq()
  { /*   
    for(int i = 1; i < Q_length; i++)
    {
      Serial.print((String)" " + data[i-1]);
    }
    Serial.println((String)" " + data[Q_length]);//*/
        
    int maxPos[Q_length] = { 0 };
    int maxCount = 0;
    
    for(int i = 2; i < Q_length; i++)
    {      
      if(data[i-2] < data[i-1] and data[i-1] > data[i]) // data[i-1] is maximum
      {                        
        maxPos[maxCount] = (i-1);
        maxCount++;
      }  
    }
    if(maxCount==0)
      return 0.0;
      
    // v maxPos jsou ulozeny pozice maxim v ms
    long returnVal = 0;
    for(int i = 1; i < maxCount; i++)
    {
      returnVal += ((maxPos[i]-maxPos[i-1]));
    }
    return returnVal==0 ? 0 : (maxCount)/(double(returnVal));
  }

  // Finds Offset between two queues
  // this has offset 0
  // return in us
  double Offset(const Queue &that, int loopTime)
  {
    int maxPosThis[Q_length] = { 0 }; //should be dynamic
    int maxCountThis = 0;
    
    for(int i = 4; i < Q_length; ++i)
    {
      if(data[i-4] < data[i-3] and data[i-3] < data[i-2] and data[i-1] < data[i-2] and data[i] < data[i-1]) // data[i-2] is maximum
      {        
        maxPosThis[maxCountThis] = (i-1) * loopTime;
        maxCountThis++;
      }  
    }

    int maxPosThat[that.Q_length] = { 0 }; //should be dynamic
    int maxCountThat = 0;
    
    for(int i = 4; i < that.Q_length; ++i)
    {
      if(that.data[i-4] < that.data[i-3] and that.data[i-3] < that.data[i-2] and that.data[i-1] < that.data[i-2] and that.data[i] < that.data[i-1]) // data[i-2] is maximum
      {        
        maxPosThat[maxCountThat] = (i-1) * loopTime;
        maxCountThat++;
      }  
    }

    double returnVal = 0;

    for(int i = 1; i < maxCountThat or i < maxCountThis; ++i)
    {
      returnVal = maxPosThis[i] - maxPosThat[i];
    }

    
    return maxCountThis > maxCountThat ? returnVal/maxCountThat : returnVal/maxCountThis;
  }
};




#endif //Q_THINGS
