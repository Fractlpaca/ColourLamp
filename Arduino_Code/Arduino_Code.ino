/* Model of electronics and code to be used for the interactive
 * lamp for my 13DTE project.
 * Created by Joseph Grace 21/04/2020
*/

#include <Adafruit_NeoPixel.h>
#include <LiquidCrystal.h>
#include "menu.h"
#include "pixel.h"
#include "mic.h"

const int BAUD_RATE = 9600;

//Control buttons.
const int LEFT = 3;
const int UP = 4;
const int RIGHT = 5;
const int DOWN = 2;

//LED strip pin
const int STRIP_PIN = 6;

//Sensors
const int LDR = A0;
const int LDRSeriesResistor = 330;
const int PIR = A2;

const float brightness_safety = 0.9;

//Initialise LCD and NeoPixel Objects
Adafruit_NeoPixel strip(PIXEL_NUM, STRIP_PIN, NEO_GRBW+NEO_KHZ800);

/*const float sensitivity = 129533;
const float offset = 0.77;
//Sensor Functions
int readLDR(){
  //returns brightness on LDR, calibrated to
  //be between 0(dark) and 255(light)
  float resistance = LDRSeriesResistor*(1024.0/analogRead(LDR)-1.0);
  int brightness= round(sensitivity/resistance - offset);
  return min(max(0,brightness),255);
}*/

class State{
public:
  int pure_colour[3]={0,100,75};
  int global_brightness=80;
  int sound_sens=100;
  int motion_sens=100;
  int light_sens=100;
  int white_value=75;
  int mode=RANDOM;
  int current=MODES;
  bool setting=false;
  void add(int *param, int num){
    *param=max(0,min(100,(*param)+num));
  }
}

state=State();

class LCDScreen: public LiquidCrystal{
public:
  LCDScreen(int rs_pin, int en_pin, int d1, int d2, int d3, int d4): LiquidCrystal(rs_pin, en_pin, d1, d2, d3, d4){
    begin(16,2);
    setCursor(0,0);
    updateState();
  }
  void writeArrow(String s, int line){
    int space = (14-int(s.length()))/2;
    if(space<0){space=0;}
    setCursor(0,line);
    print("<               ");
    setCursor(1+space, line);
    print(s);
    setCursor(15,line);
    print(">");
  }
  void writeCenter(String s, int line){
    int space = (16-int(s.length()))/2;
    if(space<0){space=0;}
    String filler="";
    for(int i=0; i<space; i++){
      filler+=" ";
    }
    setCursor(0,line);
    print("                ");
    setCursor(space, line);
    print(s);
  }
  void updateState(){
    if(state.setting){
      writeCenter(NAME_LIST[state.current],0);
      switch(state.current){
        case(BRIGHTNESS): writeArrow(String(state.global_brightness), 1); break;
        case(SOUND): writeArrow(String(state.sound_sens), 1); break;
        case(LIGHT): writeArrow(String(state.light_sens), 1); break;
        case(MOTION): writeArrow(String(state.motion_sens), 1); break;
        case(HUE): writeArrow(String(state.pure_colour[0]), 1); break;
        case(SATURATION): writeArrow(String(state.pure_colour[1]), 1); break;
        case(VALUE): writeArrow(String(state.pure_colour[2]), 1); break;
        case(WHITE): writeArrow(String(state.white_value), 1); break;
        default: state.current=false; break;
      }
    }else{
      writeArrow(NAME_LIST[state.current],1);
      switch(state.current){
        case(MODES): writeCenter("Menu",0); break;
        case(BRIGHTNESS): writeCenter("Menu",0); break;
        case(RANDOM): writeCenter(NAME_LIST[MODES],0); break;
        case(SOUND): writeCenter(NAME_LIST[RANDOM],0); break;
        case(LIGHT): writeCenter(NAME_LIST[RANDOM],0); break;
        case(MOTION): writeCenter(NAME_LIST[RANDOM],0); break;
        case(COLOUR): writeCenter(NAME_LIST[MODES],0); break;
        case(HUE): writeCenter(NAME_LIST[COLOUR],0); break;
        case(SATURATION): writeCenter(NAME_LIST[COLOUR],0); break;
        case(VALUE): writeCenter(NAME_LIST[COLOUR],0); break;
        case(WHITE): writeCenter(NAME_LIST[MODES],0); break;
      }
    }
  }
};


LCDScreen lcd(7,8,9,10,11,12);

void setup()
{
  Serial.begin(BAUD_RATE);
  
  
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
    pixels[i] = new InteractingColor(random(0,360),0.05);
  }
  //link pixels
  for(int i=0; i<PIXEL_NUM; i++){
    pixels[i]->link(pixels[(i+PIXEL_NUM-1)%PIXEL_NUM],0.42);
    pixels[i]->link(pixels[(i+1)%PIXEL_NUM],0.42);
    pixels[i]->link(pixels[PIXEL_NUM],0.12);
  }
}

long long milliseconds_since_last_press=0;

void on_button_press(int button){
  int time_delta;
  if(state.setting){
    if(state.current==HUE) time_delta=13;
    else time_delta=50;
  }
  else time_delta=200;
  if(millis()-milliseconds_since_last_press<time_delta){
    return;
  }
  milliseconds_since_last_press=millis();
  switch(state.current){
    case MODES:
      switch(button){
        case LEFT: state.current=BRIGHTNESS;break;
        case RIGHT: state.current=BRIGHTNESS;break;
        case DOWN: state.current=state.mode;
      }
      break;
    case BRIGHTNESS:
      switch(button){
        case LEFT: 
          if(state.setting){
            state.add(&state.global_brightness,-1);
          }else{
            state.current=MODES;
          }
          break;
        case RIGHT:
          if(state.setting){
            state.add(&state.global_brightness,1);
          }else{
            state.current=MODES;
          }
          break;
        case DOWN: state.setting=true;break;
        case UP: state.setting=false;break;
      }
      break;
    case RANDOM:
      switch(button){
        case LEFT: state.current=WHITE; break;
        case RIGHT: state.current=COLOUR; break;
        case DOWN: state.mode=RANDOM;state.current=SOUND; break;
        case UP: state.current=MODES; break;
      }
      break;
    case COLOUR:
      switch(button){
        case LEFT: state.current=RANDOM; break;
        case RIGHT: state.current=WHITE; break;
        case DOWN: state.mode=COLOUR; state.current=HUE; break;
        case UP: state.current=MODES; break;
      }
      break;
    case SOUND:
      switch(button){
        case LEFT: 
          if(state.setting){
            state.add(&state.sound_sens,-1);
          }else{
            state.current=MOTION;
          }
          break;
        case RIGHT:
          if(state.setting){
            state.add(&state.sound_sens,1);
          }else{
            state.current=LIGHT;
          }
          break;
        case DOWN: state.setting=true; break;
        case UP: state.current=state.setting?SOUND:RANDOM; state.setting=false; break;
      }
      break;
    case LIGHT:
      switch(button){
        case LEFT: 
          if(state.setting){
            state.add(&state.light_sens,-1);
          }else{
            state.current=SOUND;
          }
          break;
        case RIGHT:
          if(state.setting){
            state.add(&state.light_sens,1);
          }else{
            state.current=MOTION;
          }
          break;
        case DOWN: state.setting=true; break;
        case UP: state.current=state.setting?LIGHT:RANDOM; state.setting=false; break;
      }
      break;
    case MOTION:
      switch(button){
        case LEFT: 
          if(state.setting){
            state.add(&state.motion_sens,-1);
          }else{
            state.current=LIGHT;
          }
          break;
        case RIGHT:
          if(state.setting){
            state.add(&state.motion_sens,1);
          }else{
            state.current=SOUND;
          }
          break;
        case DOWN: state.setting=true; break;
        case UP: state.current=state.setting?MOTION:RANDOM; state.setting=false; break;
      }
      break;
    case HUE:
      switch(button){
        case LEFT: 
          if(state.setting){
            state.pure_colour[0]=(state.pure_colour[0]+359)%360;
          }else{
            state.current=VALUE;
          }
          break;
        case RIGHT:
          if(state.setting){
            state.pure_colour[0]=(state.pure_colour[0]+361)%360;
          }else{
            state.current=SATURATION;
          }
          break;
        case DOWN: state.setting=true; break;
        case UP: state.current=state.setting?HUE:COLOUR; state.setting=false; break;
      }
      break;
    case SATURATION:
      switch(button){
        case LEFT: 
          if(state.setting){
            state.add(&state.pure_colour[1],-1);
          }else{
            state.current=HUE;
          }
          break;
        case RIGHT:
          if(state.setting){
            state.add(&state.pure_colour[1],1);
          }else{
            state.current=VALUE;
          }
          break;
        case DOWN: state.setting=true; break;
        case UP: state.current=state.setting?SATURATION:COLOUR; state.setting=false; break;
      }
      break;
    case VALUE:
      switch(button){
        case LEFT: 
          if(state.setting){
            state.add(&state.pure_colour[2],-1);
          }else{
            state.current=SATURATION;
          }
          break;
        case RIGHT:
          if(state.setting){
            state.add(&state.pure_colour[2],1);
          }else{
            state.current=HUE;
          }
          break;
        case DOWN: state.setting=true; break;
        case UP: state.current=state.setting?VALUE:COLOUR; state.setting=false; break;
      }
      break;
    case WHITE:
      switch(button){
        case LEFT:
          if(state.setting){
            state.add(&state.white_value,-1);
          }else{
            state.current=COLOUR;
          }
          break;
        case RIGHT:
          if(state.setting){
            state.add(&state.white_value,1);
          }else{
            state.current=RANDOM;
          }
          break;
        case DOWN: state.mode=WHITE;state.setting=true; break;
        case UP: state.current=state.setting?WHITE:MODES; state.setting=false; break;
      }
      break;
  }
  lcd.updateState();
}

void regulate(float quota){
  float e = totalEnergy(pixels);
  if(e<quota){
    for(int i=0; i<PIXEL_NUM; i++){
      if(pixels[i]->hue_friction>0.1){
        pixels[i]->hue_friction*=0.9;
      }
      if(pixels[i]->hue_velocity<180){
        pixels[i]->hue_velocity*=1.1;
      }
    }
  }else{
    for(int i=0; i<PIXEL_NUM; i++){
      if(pixels[i]->hue_friction<2000){
        pixels[i]->hue_friction*=1.1;
      }
      if(pixels[i]->hue_velocity>0.01){
        pixels[i]->hue_velocity*=0.9;
      }
    }
  }
}

int motion_pixel=-1;
long long last_motion_kick=0;
long long last_motion_activation=0;

void doRandomTick(){
  float brightness_multiplier = state.global_brightness / 100.0;

  //Simulate driven pendulum
  driver.doTick();
  pixels[PIXEL_NUM]->hue = degrees(driver.displacement);
  Serial.print(String(totalEnergy(pixels)));
  Serial.print(" ");
  if(digitalRead(PIR)){
    if(motion_pixel==-1) last_motion_activation = millis();
    
    if(motion_pixel==-1 || millis()-last_motion_kick > 15000){
      motion_pixel=random(0,PIXEL_NUM);
      pixels[motion_pixel]->hue+=state.sound_sens/100.0*180;
      last_motion_kick = millis();
    }
    lcd.setCursor(0,0);
    lcd.print("*");
    float quota = 1000.0 + state.motion_sens*10000.0;
    regulate(quota);
    
    Serial.print(String(quota));
  }else{
    motion_pixel=-1;
    lcd.setCursor(0,0);
    lcd.print(" ");
    for(int i=0; i<PIXEL_NUM; i++){
      pixels[i]->hue_friction=0.05;
    }
    //regulate(1000);
    Serial.print("1000");
  }
  Serial.print(" ");
  Serial.println(String(pixels[0]->hue_friction*100));

  
  for(int i=0; i<PIXEL_NUM; i++){
    pixels[i]->setHueAcceleration();
  }
  
  for(int i=0; i<PIXEL_NUM; i++){
    pixels[i]->doHueTick();
    uint32_t rgb_color = strip.ColorHSV(round(pixels[i]->hue*65536/360),255,255*brightness_safety*brightness_multiplier);
    strip.setPixelColor(i,strip.gamma32(rgb_color));
    strip.show();
  }
}


void loop()
{
  //writeAllToSerial();
  strip.show();
  if(!digitalRead(LEFT)){
    on_button_press(LEFT);
  }else if(!digitalRead(UP)){
    on_button_press(UP);
  }else if(!digitalRead(DOWN)){
    on_button_press(DOWN);
  }else if(!digitalRead(RIGHT)){
    on_button_press(RIGHT);
  }
  float brightness_multiplier = state.global_brightness / 100.0;
  switch(state.mode){
    case RANDOM:
      doRandomTick();
      break;
    case COLOUR:
      for(int i=0; i<PIXEL_NUM; i++){
        uint32_t rgb_color = strip.ColorHSV(state.pure_colour[0]*65536/360,state.pure_colour[1]*255/100,brightness_safety*brightness_multiplier*state.pure_colour[2]*255/100);
        strip.setPixelColor(i,strip.gamma32(rgb_color));
        strip.show();
      }
    case WHITE:
      int white_value = (float(state.white_value) / 100.0) * (float(state.global_brightness) / 100.0) * 255;
      uint32_t white = strip.Color(0,0,0,white_value);
      strip.fill(white, 0, 16);
      strip.show();
  }
  mic_tick();
}
