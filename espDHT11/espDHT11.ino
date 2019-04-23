// Them thu vien
#include <ESP8266WiFi.h>
#include "DHT.h"
#define DHTPIN 5              
#define DHTTYPE DHT11

int pin = 2;
int led = 14;

DHT dht(DHTPIN, DHTTYPE, 15); 


const char* ssid = "Quang Huy"; 
const char* password = "quanghuy061296";  
WiFiServer server(80);       

void setup() 
{
  
  Serial.begin(115200);       
  delay(10);
  pinMode(pin, OUTPUT);
  pinMode(led, OUTPUT);
  digitalWrite(pin, HIGH);
  pinMode(4, INPUT);
  dht.begin();                
  Serial.println();           
  Serial.println();
  Serial.print("Ket noi toi mang ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Da ket noi WiFi");
  server.begin();             
  Serial.println("Khoi dong Server");
  Serial.println(WiFi.localIP());
  
}

void loop()
{
  WiFiClient client = server.available(); 
  if (!client)
  {
    return;
  }
  Serial.println("Co mot client moi ket noi xem du lieu");
  while(!client.available()){
    delay(1);
  }
  float h = dht.readHumidity();               
  float t = dht.readTemperature();              
  String req = client.readStringUntil('\r');    //Đọc dòng đầu tiên của yêu cầu gửi lên
  Serial.println(req);
  client.flush();

  // Kiem tra yeu cau la gi
  if (req.indexOf("/off") != -1){
    digitalWrite(pin, HIGH);
    
  }
  else if (req.indexOf("/on") != -1) {
    digitalWrite(pin, LOW);
    
  }
  int Data = digitalRead(4);
  int QT = analogRead(A0);
  for (int i = 0; i<256; i++){
    char a[100];
    sprintf(a, "GET /1/%d/pwm", i);
    if ( req.indexOf(a) >= 0){
      analogWrite(led,i);
    }
  }
  client.flush();
  // Chuan bi tao web de phan hoi
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
  s += "<head>";
  s += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
  s += "<meta http-equiv=\"refresh\" content=\"60\" />";
  s += "<script src=\"https://code.jquery.com/jquery-2.1.3.min.js\"></script>";
  s += "<link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.4/css/bootstrap.min.css\">";
  s += "<style>body{font-size: 24px;} .voffset {margin-top: 30px;}";
  s += "</style>";
  s += "</head>";
  s += "<div class=\"container\">";
  s += "<h1 style=\"text-align:center;\">Tran Quang Huy - D15DT04</h1>";
  s += "<h1 style=\"color:red;\">Thong so moi truong:</h1>";
  s += "<div class=\"row voffset\">";
  s += "<div class=\"col-md-3\">Nhiet do:</div><div class=\"col-md-3\">" + String(t) + "*C</div>";
  s += "<div class=\"col-md-3\">Do am:</div><div class=\"col-md-3\">"+String(h) + "%</div>";
  s += "<br>";
  s += "<br>";
  s += "<h1 style=\"color:red;\">CAM BIEN ANH SANG</h1>";
  s += "<div class=\"col-md-3\">Quang tro: </div><div class=\"col-md-3\">" + String(QT) + "</div>";
  s += "<div class=\"col-md-3\">Light sensor: </div><div class=\"col-md-3\">" + String(Data) + "</div>";
  s += "</div>";
  s += "<br>";
  s += "<br>";
  s += "<h1 style=\"color:red;\">DIEU KHIEN THIET BI</h1>";
  s += "<div class=\"row\">";
  s += "<div class=\"col-md-5\"><input class=\"btn btn-block btn-lg btn-primary\" type=\"button\" value=\"BAT LED\" onclick=\"on()\"></div>";
  s += "<div class=\"col-md-5\"><input class=\"btn btn-block btn-lg btn-danger\" type=\"button\" value=\"TAT LED\" onclick=\"off()\"></div>";
  s += "</div>";
  //s += "<h4 class=\"font-weight-bold\">LED</h4>";
  s += "<p class=\"font-italic\">Led Current State: <span id=\"led_status\"></span></p>";
  s += "<script type=\"text/javascript\">function on() {$.get(\"/on\");  document.getElementById(\"led_status\").innerHTML = \"On\";}</script>";
  s += "<script type=\"text/javascript\">function off() {$.get(\"/off\");  document.getElementById(\"led_status\").innerHTML = \"Off\";}</script>";
  s += "<body style=\"background-color:powderblue;\">";
  s += "<br>";
  s += "<br>";
  s += "<h1 style=\"color:MediumSeaGreen;\">Brightness:</h1>";
  s += "<div class=\"led\">";
  s += "<input type=\"range\" min=\"0\" max=\"255\" value=\"50\" class=\value_led\" id=\"myRange\">";
  s += "<p>Value: <span id=\"demo\"></span></p>";
  s += "</div>";
  s += "<script>";
  s += "var value_led = document.getElementById(\"myRange\");";
  s += "var output = document.getElementById(\"demo\");";
  s += "var ntc = new XMLHttpRequest();";
  s += "output.innerHTML = value_led.value;";
  s += "value_led.oninput = function(){ output.innerHTML = this.value;  ntc.open('GET', \"1/\" + output.innerHTML+\"/pwm\", true);  ntc.send();}";
  s += "</script>";
  s += "</body>";
  
  //s += "<h1>      </h1>";
  //s += "<h1>      </h1>";
  //s += "<h1>      </h1>";
  //s += "<h1>Have a nice day <3!!!!!!!</h1>";
  
  //s += "</div>";
  
  client.print(s);//gửi phản hồi đến client
  delay(1);
  Serial.println("Client da thoat"); 
}
