#define PIXEL_NUM 12

const double TAU = 2*PI;

class Sat{
public:
  Sat(){}
  Sat(float s){
    sat=s;
  }
  float sat=255.0;
  float sat_velocity=0.0;
  float sat_delta=0.02;
  void doSatTick(){
    sat += sat_delta * sat_velocity;
    sat=max(0,min(sat,255.0));
  }
};

class Hue{
public:
  Hue(){}
  Hue(float h, float f){
    hue=h;
    hue_friction=f;
  }
  
  float hue=0;
  float hue_velocity=0;
  float hue_acceleration=0;
  float hue_delta=0.02;
  float hue_friction=0.05;
  
  void doHueTick(){
    hue_velocity += hue_delta * (hue_acceleration - hue_friction * hue_velocity);
    hue += hue_delta * hue_velocity;
  }
};


//Interacting Color Class
class InteractingColor: public Hue, public Sat{
public:
  InteractingColor():Hue(),Sat(){}
  InteractingColor(int h, int f):Hue(h,f),Sat(){}
  
  int neighbor_num = 0;
  InteractingColor* neighbors[3];
  float neighbor_weights[3];
  
  void link(InteractingColor* neighbor, float weight);
  
  void setHueAcceleration(){
    hue_acceleration = 0;
    for(int i=0; i<neighbor_num; i++){
      float neighbor_hue = neighbors[i]->hue;
      hue_acceleration+=neighbor_weights[i]*sin(radians(neighbor_hue-hue));
    }
    hue_acceleration*=hue_lightness;
      
  }

  void setVelocity(){
    hue_velocity=0;
    for(int i=0; i<neighbor_num; i++){
      float neighbor_hue = neighbors[i]->hue;
      hue_velocity+=hue_lightness * neighbor_weights[i]*sin(radians(neighbor_hue-hue));
    }
  }

  void setSatVelocity(){
    sat_velocity=0;
    for(int i=0; i<2; i++){
      float neighbor_sat = neighbors[i]->sat;
      sat_velocity+=sat_lightness * neighbor_weights[i] * (neighbor_sat-sat);
    }
  }

  void influenceSatVelocity(float target){
    sat_velocity+=sat_lightness * (target-sat);
  }

  void influenceHueVelocity(float target, float amount){
    sat_velocity+= amount * sin(radians(target-sat));
  }
  
  int hue_lightness = 100;
  float sat_lightness=50;
};

void InteractingColor::link(InteractingColor* neighbor, float weight){
  if(neighbor_num>2){return;}
  neighbors[neighbor_num] = neighbor;
  neighbor_weights[neighbor_num] = weight;
  neighbor_num++;
}


float totalEnergy(InteractingColor ** pixels){
  float ret=0;
  for(int i=0; i<PIXEL_NUM; i++){
    ret+=(pixels[i]->hue_velocity)*(pixels[i]->hue_velocity);
  }
  return ret;
}


//Driven pendulum class
class DrivenPendulum{
public:
   float friction = 0.01;
   float displacement;
   DrivenPendulum(float displacement){
      this->displacement = displacement;
   }
   void doTick(){
      float drive = force_amplitude * sin(TAU*tick/(tick_speed*force_period));
      float gravity_force = -g*sin(displacement);
      float acceleration = drive + gravity_force - friction*velocity;
      velocity+=acceleration/tick_speed;
      displacement+=velocity/tick_speed;
      tick++;
   }
   
  float velocity= 0;
  float force_period = 3;
  float force_amplitude= 4;
  float g = 10;
  int tick = 0;
  int tick_speed= 80;//ticks per simulation second;
};

InteractingColor* pixels[PIXEL_NUM+1];
InteractingColor realPixels[PIXEL_NUM+1];

DrivenPendulum driver(random(0,1000)/1000.0-0.5);
