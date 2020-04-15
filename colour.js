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

for(var i=0; i<360; i++){
    var rgb = HSVToRGB(i,100,100);
    context.fillStyle = RGBToString(rgb);
    context.fillRect(i,10,1,580)
}