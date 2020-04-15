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

function radiansToDegrees(degrees){
    return 180*degrees/Math.PI;
}

var driven_pendulum = {
    //models chaotic pendulum with periodic driving force
    friction: 0,
    displacement: 0,
    velocity: 0,
    force_period: 3,
    force_amplitude: 4,
    g: 10,
    tick_speed: 60,//ticks per simulation second;
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

var colour = {
    rgb: "rgb(0,0,0)",
    draw: function(self){
        self.rgb = RGBToString(HSVToRGB(self.hue,100,100));
        context.fillStyle = self.rgb;
        context.fillRect(100,100,400,400);
        context.fillStyle = "black";
        context.fillRect(290+200*Math.sin(Math.PI*self.pre_hue/180),290+200*Math.cos(Math.PI*self.pre_hue/180),20,20)
    },
    tick: 0,
    tick_speed: 60,
    hue: 0,
    pre_hue: 0,
    add_hue: 0,
    hue_period:30,
    doTick: function(self){
        driven_pendulum.doTick();
        self.add_hue=Math.round(self.tick*360/(self.hue_period*self.tick_speed))%360;
        self.pre_hue=(radiansToDegrees(driven_pendulum.displacement)%360+360)%360;
        self.draw(self);
        self.hue = (self.pre_hue+self.add_hue)%360
        self.tick++;
    }
}

setInterval(colour.doTick,colour.tick_speed,colour);