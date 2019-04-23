/*-------------------------------------------------------------------------*/
var express = require("express"),
    url = require("url"),
    fs = require("fs"),
    app = express();
/*-------------------------------------------------------------------------*/
var server = require("http").createServer(app).listen(8000, function(){
    console.log("Server Running.....\nListening : localhost:8000");
});
app.use('/static', express.static(__dirname + "/static"));


/*-------------------------------------------------------------------------*/
var io = require('socket.io').listen(server);

/*----------------Mysql------------------------------*/
var mysql = require('mysql');
var con = mysql.createConnection({
  host: "localhost",
  user: "huytq",
  password: "Quanghuy@123",
  database: "wsn"
});

con.connect(function() {
  console.log("Mysql Connected!");
});

/*-----------------------------MQTT---------------------------------*/
var mqtt_server = "mqtt://172.20.10.13"
var mqtt = require("mqtt"); 

var option = {
	username: "huytq",
	password: "Quanghuy@123",
	clean: true
};

var mqtt_client = mqtt.connect(mqtt_server, option);

mqtt_client.on("connect", function (){
    console.log("Connected to MQTT server!");
});

mqtt_client.on("connect", function () {
   mqtt_client.subscribe("DHT11");
});

mqtt_client.on("connect", function () {
   mqtt_client.publish("Button");
});

/*-------------------------------Value-----------------------------------------*/
var sensor = {          
    num: 0,
    temperature: 0,
    humidity: 0,
    time: ""
};

var save = {            
    temperature: [],
    humidity: [],
    time: []
};

setInterval(function () {
    var timenow = getDateTimeNow();
    if(sensor.num!=0)
    {
        sensor.temperature = parseInt(sensor.temperature/sensor.num);
        sensor.humidity = parseInt(sensor.humidity/sensor.num);
        sensor.time = timenow;
        /*-Send data-*/
        io.sockets.emit("dht11", JSON.stringify(sensor));
        /*-Back up-*/
        saveData(sensor.temperature, sensor.humidity, sensor.time);
        /*-Reset-*/
        sensor.temperature = 0; sensor.humidity = 0; sensor.num = 0;
    }
    else
    {
        var num_end = save.temperature.length-1;
	sensor.temperature = save.temperature[num_end];
	sensor.humidity = save.humidity[num_end];
	sensor.time = timenow;
        if(save.temperature.length>0)
            io.sockets.emit("dht11", JSON.stringify(sensor));
	saveData(sensor.temperature, sensor.humidity, sensor.time);
	sensor.temperature = 0; sensor.humidity = 0; sensor.num = 0;
    }
},1000);

function getDateTimeNow() {
    var date = new Date();
    timenow = date.getFullYear() + "-" + date.getMonth() + "-" + date.getDate() + " " + date.getHours() + ":" + date.getMinutes() + ":" + date.getSeconds();
    return timenow;
};

function saveData(sv_temp, sv_hum, sv_time) {
    save.temperature.push(sv_temp);
    save.humidity.push(sv_hum);
    save.time.push(sv_time);
};

mqtt_client.on("message", function (topic, message) {
    console.log("Subed");
    data = message.toString();
    //console.log(data);
    a = JSON.parse(data);
    sensor.temperature += a.temp;
    sensor.humidity += a.hum;
    sensor.num ++;
    var time = getDateTimeNow();
    sensor.time += time;
    var sql = "INSERT INTO THCS(temp,hum,time) VALUES("+a.temp+","+a.hum+",'"+time+"')";
	con.query(sql,  function(err, result){
		if (err) {
		        throw err;
		} else {
			console.log("insert Mysql Success");
		}
	});
    
});


/*-------------------------------------------------------------------------*/
var status_device = {
    device: 'on'
};

io.sockets.on("connection", function (socket) {
    io.sockets.emit("status_device", JSON.stringify(status_device));
    socket.emit("chart-full", JSON.stringify(save));
    socket.on("status_device", function (data) {
        if(data=="on"){
            bat();
        }else {
            tat();
	}
        status_device.device = data;
        console.log("Device: " + data);
        io.sockets.emit("status_device", data);
    });
    socket.on("Range", function (data) {
		value = data.led1.toString();
		mqtt_client.publish("Range", value);
		console.log("led pwm change: led1 %d ",data.led1);	
    });
    
}); 	
function bat(){
	mqtt_client.publish("Button", "ON");
	console.log("Pubed ON");
};

function tat(){
	mqtt_client.publish("Button", "OFF");
	console.log("Pubed OFF");
};

/*-------------------------------------------------------------------------------*/

con.connect(function(err) {
  	con.query("SELECT * FROM THCS order by id desc limit 10", function (err, result) {
    		//data = JSON.stringify(result);
		for(var i=0; i<10;i++){
			temp =result[i].temp;
			hum = result[i].hum;
			time = result[i].time;
			saveData(temp,hum,time);
		}
	});
});
/*-------------------------------------------------------------------------------*/









