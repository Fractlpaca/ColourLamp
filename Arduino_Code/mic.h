#define BACKLOG 10
#define MIC A0

float findNormal(int range){
  long int signal_sum = 0;
  for(int i=0; i<range; i++){
    signal_sum+=analogRead(MIC);
    delay(0.001);
  }
  return float(signal_sum) / float(range);
}
float normal;
//LiquidCrystal lcd(7,8,9,10,11,12);


long int history[BACKLOG];
long long amp_sum = 0;
int tick = -BACKLOG;
const float amplification=1.0;
double amplitude=-1;

void doMicTick()
{
  int mic_value=analogRead(MIC);
  normal += 0.1*(float(mic_value)-normal);
  double offset = float(mic_value) - normal;
  long int amp = round(offset*offset);
  double amplified = (offset)*amplification;
  amp_sum+=amp;
  if(tick>=0){
    amp_sum-=history[tick];
    history[tick]=amp;
  }
  amplitude = sqrt(float(amp_sum)/float(BACKLOG));
  Serial.print(offset);
  Serial.print(" ");
  Serial.println(amplitude);
  tick=(tick+1)%BACKLOG;
}
