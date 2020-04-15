var canvas = document.getElementById("canvas");
var context = canvas.getContext("2d");

context.strokeStyle = "white";
context.fillStyle = "orange";
context.fillRect(10,10,100,100);
context.strokeText("Test String",100,100);