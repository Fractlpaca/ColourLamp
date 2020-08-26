/* Sensor Testing for micropnone
 * Created by Joseph Grace 22/04/2020
 * Last Edited 22/04/2020
 * Prints to serial signal centred around 127
*/

#define HISTORY_SIZE 100
#define BACKLOG 10
#define MIC A1


//LiquidCrystal lcd(7,8,9,10,11,12);


int signal_history[HISTORY_SIZE];
int amp_history[HISTORY_SIZE];
long int signal_sum = 0;
long int amp_sum = 0;
int tick = -HISTORY_SIZE;
unsigned long int actual_tick = 0;
const float amplification=8.0;
int minimum = 1024, maximum=0;

void mic_tick()
{
  float mic_value=analogRead(MIC);
  float average = float(signal_sum)/float(HISTORY_SIZE);
  float offset = mic_value-average;
  float amplitude = abs(offset)*amplification;
  float amplified = (mic_value-average)*amplification;
  unsigned char c = round(min(255,max(0,amplified+127)));
  float amp_average = float(amp_sum)/float(BACKLOG);
  //Serial.println(String(c)+" "+String(amp_average));
  //Serial.write(c);
  
  signal_sum += round(mic_value);
  if(tick >= -BACKLOG){
    amp_sum += round(amplitude);
  }
  if(tick>=0){
    signal_sum -= signal_history[tick];
    signal_history[tick]=round(mic_value);
    
    amp_sum -= amp_history[(tick-BACKLOG+HISTORY_SIZE)%HISTORY_SIZE];
    amp_history[tick] = round(amplitude);
  }else{
    signal_history[tick+HISTORY_SIZE]=round(mic_value);
    amp_history[tick+HISTORY_SIZE]=round(amplitude);
  }
  tick = (tick+1)%HISTORY_SIZE;
  
  actual_tick++;
}
