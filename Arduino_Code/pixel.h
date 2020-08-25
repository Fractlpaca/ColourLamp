
#define PIXEL_NUM 16

const double TAU = 2*PI;

//Interacting Color Class
class InteractingColor{
public:
  InteractingColor(int hue){
    this->hue=hue;
  }
  float hue;
  int neighbor_num = 0;
  InteractingColor* neighbors[3];
  float neighbor_weights[3];
  void link(InteractingColor* neighbor, float weight);
  void setAcceleration(){
    hue_acceleration = 0;
    for(int i=0; i<neighbor_num; i++){
      float neighbor_hue = neighbors[i]->hue;
      hue_acceleration+=neighbor_weights[i]*sin(radians(neighbor_hue-hue));
    }
    hue_acceleration*=influence;
      
  }

  void setVelocity(){
    hue_velocity=0;
    for(int i=0; i<neighbor_num; i++){
      float neighbor_hue = neighbors[i]->hue;
      hue_velocity+=influence * neighbor_weights[i]*sin(radians(neighbor_hue-hue));
    }
  }

  void kick(int amount){
    hue_velocity += amount;
  }

  void displace(int amount){
    hue += amount;
  }
  
  void doTick(){
    hue_acceleration -= friction * hue_velocity;
    hue_velocity += hue_acceleration / tick_speed;
    hue += hue_velocity / tick_speed;
    //hue += hue_acceleration / tick_speed;
  }
private:
  float hue_acceleration;
  float hue_velocity = 0;//10*random(-1,2);
  float friction = 0.05;
  int influence = 100;
  int tick = 0;
  int tick_speed = 24;
};

void InteractingColor::link(InteractingColor* neighbor, float weight){
    if(neighbor_num>2){return;}
    neighbors[neighbor_num] = neighbor;
    neighbor_weights[neighbor_num] = weight;
    neighbor_num++;
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
private:
  float velocity= 0;
  float force_period = 3;
  float force_amplitude= 4;
  float g = 10;
  int tick = 0;
  int tick_speed= 80;//ticks per simulation second;
};

InteractingColor* pixels[PIXEL_NUM+1];

DrivenPendulum driver(random(0,1000)/1000.0-0.5);
