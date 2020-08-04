/* Model of electronics and code to be used for the interactive
 * lamp for my 13DTE project.
 * Created by Joseph Grace 21/04/2020
 * Last Edited 10/06/2020
 * Currently has no input/control measures
 * Color algorithm implemented
*/

#include <Adafruit_NeoPixel.h>
#include <LiquidCrystal.h>

#define PIXEL_NUM 8

const double TAU = 2*PI;

const int BAUD_RATE = 9600;

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
const int PIR = A2;
const int MIC = A1;

//Initialise LCD and NeoPixel Objects
Adafruit_NeoPixel strip(16, STRIP_PIN, NEO_GRBW+NEO_KHZ800);

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
  void doTick(){
    hue_velocity += hue_acceleration / tick_speed;
    hue += hue_velocity / tick_speed;
  }
private:
  float hue_acceleration;
  float hue_velocity = 0;//10*random(-1,2);
  int influence = 100;
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

struct value;

typedef struct state{
  state *parent;
  char state_name[14];
  state *sibling;
  state *child;
  struct value *val;
}state;

typedef struct value{
  state* parent;
  int vmin;
  int vmax;
  int vdef;
}value;

String state_initialisation = "Modes(Pure_Colour(Red[0,255,250],Green[0,255,250],Blue[0,255,250]),Random,Sound(Sensitivity[0,10,5]))";

struct value * value_init(String init_str, struct state * parent){
  Serial.println(init_str);
  value* node = new struct value;
  int i=init_str.indexOf(",");
  node->vmin=init_str.substring(0,i).toInt();
  int j=init_str.indexOf(",",i+1);
  node->vmax=init_str.substring(i+1,j).toInt();
  node->vdef=init_str.substring(j+1).toInt();
  node->parent=parent;
  return node;
}

state* mode_init(String init_str, state* parent){
  Serial.println(init_str);
  state * node = new struct state;
  int i=init_str.indexOf("(");
  String primary,secondary;
  if(i==-1){
    node->child=NULL;
    primary=init_str;
    secondary="";
  }else{
    primary=init_str.substring(0,i);
    secondary=init_str.substring(i+1,init_str.length()-1);
  }
  int j=primary.indexOf("[");
  if(j!=-1){
    node->val=value_init(primary.substring(j+1,primary.length()-1),node);
    primary.substring(0,j).toCharArray(node->state_name,14);
  }
  if(i==-1){
    return node;
  }
  Serial.println(primary+" "+secondary);
  int paren=0;
  state *next,*last;
  for(i=0,j=0; j<secondary.length()+1 ;j++){
    char c = secondary.charAt(j);
    if((j==secondary.length() or c==',') and paren==0){
      String sub = secondary.substring(i,j);
      Serial.println(primary+" sub: "+sub);
      next=mode_init(sub, node);
      if(i==0){
        node->child=next;
      }else{
        last->sibling=next;
        last=next;
      }
      i=j+1;
    }
    else if(c=='(' or c=='['){paren++;}
    else if(c==')' or c==']'){paren--;}
  }
  last->sibling=node->child;
  return node;
}

class LCDScreen: public LiquidCrystal{
public:
  LCDScreen(int rs_pin, int en_pin, int d1, int d2, int d3, int d4): LiquidCrystal(rs_pin, en_pin, d1, d2, d3, d4){
    begin(16,2);
    setCursor(0,0);
    print("Test String");
  }
  void writeArrow(String s, int line){
    int space = (14-int(s.length()))/2;
    if(space<0){space=0;}
    String filler="";
    for(int i=0; i<space; i++){
      filler+=" ";
    }
    setCursor(0,line);
    print("<");
    print(filler);
    print(s);
    print(filler+" ");
    setCursor(15,line);
    print(">");
  }
};


LCDScreen lcd(7,8,9,10,11,12);

void setup()
{
  //Serial.begin(BAUD_RATE);
  //state graph setup
  //state* head=mode_init(state_initialisation, NULL);
  //end state graph setup

  
  pinMode(LEFT, INPUT_PULLUP);
  pinMode(RIGHT, INPUT_PULLUP);
  pinMode(UP, INPUT_PULLUP);
  pinMode(DOWN, INPUT_PULLUP);
  pinMode(STRIP_PIN, OUTPUT);
  pinMode(LDR,INPUT);
  pinMode(PIR,INPUT);
  strip.begin();
  strip.clear();
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
  lcd.writeArrow("0123456789",0);
  lcd.writeArrow("Hello",1);
}

void writePixelToSerial(uint32_t rgb_color){
    for(int i=0; i<3; i++){
      Serial.write(rgb_color & 255);
      rgb_color >>= 8;
    }
}

void writeAllToSerial(){
  for(int i=0; i<3; i++){
    Serial.write(0);
  }
  for(int i=0; i<PIXEL_NUM; i++){
    uint32_t rgb_color = strip.ColorHSV(round(pixels[i]->hue*65536/360));
    writePixelToSerial(rgb_color);
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
    uint32_t rgb_color = strip.ColorHSV(round(pixels[i]->hue*65536/360),255,200);
    strip.setPixelColor(i*2,strip.gamma32(rgb_color));
  }
  //writeAllToSerial();
  strip.show();
  delay(50);
}
