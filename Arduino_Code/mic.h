#define BACKLOG 10

const int MIC = A0;

float findNormal(int range){
  long int signal_sum = 0;
  for(int i=0; i<range; i++){
    signal_sum+=analogRead(MIC);
    delay(0.001);
  }
  return float(signal_sum) / float(range);
}

float normal;

long int history[BACKLOG];
long long amp_sum = 0;
int tick = -BACKLOG;
const float amplification=1.0;
double amplitude=-1;

void doMicTick()
{
  int mic_value=analogRead(MIC);
  
  //Keep normal at average value
  normal += 0.1*(float(mic_value)-normal);
  
  double offset = float(mic_value) - normal;
  long int amp = round(offset*offset);
  double amplified = (offset)*amplification;

  //maintain running total of displacement.
  amp_sum+=amp;
  if(tick>=0){
    amp_sum-=history[tick];
    history[tick]=amp;
  }
  //Compute amplitude as root-mean square of displacement
  amplitude = sqrt(float(amp_sum)/float(BACKLOG));

  //Increment tick
  tick=(tick+1)%BACKLOG;
}
