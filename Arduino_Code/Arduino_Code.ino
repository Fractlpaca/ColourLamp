/* Model of electronics and code to be used for the interactive
 * lamp for my 13DTE project.
 * Created by Joseph Grace 21/04/2020
 * Last Edited 21/04/2020
 * Currently has no input/control measures
 * Color algorithm implemented
*/

#include <Adafruit_NeoPixel.h>
#include <LiquidCrystal.h>

#define PIXEL_NUM 16

const double TAU = 2*PI;

//Control buttons.
const int LEFT = 5;
const int UP = 4;
const int RIGHT = 3;
const int DOWN = 2;

//LED strip pin
const int STRIP_PIN = 6;

//Sensors
const int LDR = A0;
const int LDRSeriesResistor = 330;
const int PIR = A1;

//Initialise LiquidCrystal and NeoPixel Objects
LiquidCrystal lcd(7,8,9,10,11,12);
Adafruit_NeoPixel strip(PIXEL_NUM, STRIP_PIN, NEO_GRB);

//Interacting Color Class
class InteractingColor;
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
  void doTick(){
    hue_velocity += hue_acceleration / tick_speed;
    hue += hue_velocity / tick_speed;
  }
private:
  float hue_acceleration;
  float hue_velocity = 0;
  int influence = 1000;
  int tick = 0;
  int tick_speed = 60;
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

const float sensitivity = 129533;
const float offset = 0.77;
//Sensor Functions
int readLDR(){
  //returns brightness on LDR, calibrated to
  //be between 0(dark) and 255(light)
  float resistance = LDRSeriesResistor*(1024.0/analogRead(LDR)-1.0);
  int brightness= round(sensitivity/resistance - offset);
  return min(max(0,brightness),255);
}

void setup()
{
  pinMode(LEFT, INPUT_PULLUP);
  pinMode(RIGHT, INPUT_PULLUP);
  pinMode(UP, INPUT_PULLUP);
  pinMode(DOWN, INPUT_PULLUP);
  pinMode(STRIP_PIN, OUTPUT);
  pinMode(LDR,INPUT);
  pinMode(PIR,INPUT);
  /*lcd.begin(16,2);
  lcd.setCursor(0,0);
  lcd.print("Test String");*/
  strip.begin();
  strip.show();
  //create pixels
  for(int i=0; i<PIXEL_NUM+1; i++){
    pixels[i] = new InteractingColor(random(0,360));
  }
  //link pixels
  for(int i=0; i<PIXEL_NUM; i++){
    pixels[i]->link(pixels[(i+PIXEL_NUM-1)%PIXEL_NUM],0.42);
    pixels[i]->link(pixels[(i+1)%PIXEL_NUM],0.42);
    pixels[i]->link(pixels[PIXEL_NUM],0.16);
  }
}

void loop()
{
  driver.doTick();
  pixels[PIXEL_NUM]->hue = degrees(driver.displacement);
  for(int i=0; i<PIXEL_NUM; i++){
    pixels[i]->setAcceleration();
  }
  for(int i=0; i<PIXEL_NUM; i++){
    pixels[i]->doTick();
    uint32_t rgb_color = strip.ColorHSV(round(pixels[i]->hue*65536/360));
    strip.setPixelColor(i,rgb_color);
  }
  strip.show();
  delay(50);
}
