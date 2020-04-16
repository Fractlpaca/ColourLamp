var canvas = document.getElementById("canvas");
var context = canvas.getContext("2d");

function HSVToRGB(hue, saturation, value){
    //hue as angle in degrees, saturation and value as percentage.
    //return rgb colour with components as 0-255;
    //ignore saturation and value, assume 100%
    //initialise components to 0
    var red=0;
    var green=0;
    var blue=0;
    if(hue<60){
        red=255;
        green=Math.round(255*hue/60);
    }else if(hue<120){
        red=Math.round(255*(120-hue)/60);
        green=255;
    }else if(hue<180){
        green=255;
        blue=Math.round(255*(hue-120)/60);
    }else if(hue<240){
        green=Math.round(255*(240-hue)/60);
        blue=255;
    }else if(hue<300){
        blue=255;
        red=Math.round(255*(hue-240)/60);
    }else if(hue<360){
        blue=Math.round(255*(360-hue)/60);
        red=255;
    }
    return [red,green,blue];
}

function RGBToString(rgb){
    return `rgb(${rgb[0]},${rgb[1]},${rgb[2]})`
}

function radiansToDegrees(radians){
    return 180*radians/Math.PI;
}

function degreesToRadians(degrees){
    return Math.PI*degrees/180;
}

//models chaotic pendulum with periodic driving force
var driven_pendulum = {
    friction: 0.01,
    displacement: (Math.random()-0.5)*0.5,
    velocity: 0,
    force_period: 3,
    force_amplitude: 4,
    g: 10,
    tick_speed: 80,//ticks per simulation second;
    tick: 0,
    doTick: function(){
        var drive = this.force_amplitude * Math.sin(2*Math.PI*this.tick/(this.tick_speed*this.force_period));
        var gravity_force = -this.g*Math.sin(this.displacement);
        var acceleration = drive + gravity_force - this.friction*this.velocity;
        this.velocity+=acceleration/this.tick_speed;
        this.displacement+=this.velocity/this.tick_speed;
        this.tick++;
    }
};

//colour with hue driven by chaotic pendulum simulation
var driven_colour = {
    rgb: "rgb(0,0,0)",
    draw: function(self=this){
        self.rgb = RGBToString(HSVToRGB(self.hue,100,100));
        context.fillStyle = self.rgb;
        context.fillRect(290,290,20,20);
        //context.fillStyle = "white";
        //context.fillRect(290+200*Math.sin(driven_pendulum.displacement),290+200*Math.cos(driven_pendulum.displacement),20,20)
    },
    tick: 0,
    tick_speed: 20,
    hue: 0,
    pre_hue: 0,
    add_hue: 0,
    hue_period:30,
    doTick: function(self=this){
        driven_pendulum.doTick();
        self.add_hue=Math.round(self.tick*360/(self.hue_period*self.tick_speed))%360;
        self.pre_hue=(radiansToDegrees(driven_pendulum.displacement)%360+360)%360;
        self.draw(self);
        self.hue = (self.pre_hue+self.add_hue)%360
        self.tick++;
    }
};

//generic colour class with control over hue
class Colour{
    constructor(hue){
        this.hue = hue;
        this.hue_velocity = 0;
        this.hue_acceleration = 0;
        this.tick = 0;
        this.tick_speed = 60;
    }
    doTick(){
        this.hue_velocity+=this.hue_acceleration/this.tick_speed;
        this.hue += 360+this.hue_velocity/this.tick_speed;
        this.hue %= 360;
        this.tick++;
    }
};

//makes array of colour objects and chaotic driven colour
var component_list = [];
//component_list.push(driven_colour);
for(var i = 0; i<8; i++){
    component_list.push(new Colour(Math.floor(Math.random()*360)));
}

function run(){
    //amount of influence neigbours have on each other
    var influence = 500;
    //do each colour tick
    for(var i=0; i<8; i++){
        component_list[i].doTick();
    }

    //draw each colour
    context.clearRect(0,0,600,600);
    for(var i=0; i<8; i++){
        var rgb = RGBToString(HSVToRGB(component_list[i].hue,100,100));
        context.fillStyle = rgb;
        context.fillRect(Math.cos(i*Math.PI/4)*200+230,Math.sin(i*Math.PI/4)*-200+230,140,140);
    }

    //tick driven colour and draw pendulum
    driven_colour.doTick()
    

    //link together colours
    for(var i=0; i<8; i++){
        var previous_hue = component_list[(i+7)%8].hue;
        var next_hue = component_list[(i+1)%8].hue;
        var hue = component_list[i].hue;
        var theta_1 = degreesToRadians(previous_hue - hue);
        var theta_2 = degreesToRadians(next_hue - hue);
        var theta_3 = degreesToRadians(driven_colour.hue - hue);
        component_list[i].hue_acceleration = influence*(0.4*Math.sin(theta_1)+0.4*Math.sin(theta_2)+0.2*Math.sin(theta_3));
    }
}

setInterval(run,50);