#include <Adafruit_NeoPixel.h>        //include声明函数
#include <Wire.h>
#include "I2Cdev.h"
#include <U8glib.h>
#include "oled.h"
//=================================================================

#include <Microduino_ColorLED.h> //引用彩灯库
#define PIN       D6         //彩灯引脚为D6
#define Light_PIN A2    //光照传感器接A2引脚
#define NUM 1   //参数表示最大级联ColorLED为1个数
#define Light_value1 600
#define Light_value2 900    //光强预设值，把光分为3个阶级，黄<600<绿<900<红
int sensorValue;      //传感器光照
ColorLED strip = ColorLED(NUM, PIN);  //第一个参数表示最大级联ColorLED个数，第二个参数表示使用的端口。

//=================================================================

#include <Sodaq_SHT2x.h>
#include <Microduino_Tem_Hum.h>
//zqyzqyzqyzqyzqyzqyzqyzqyzqyzqyzqyzqyzqy
#include "userDef.h"
#include <Microduino_Key.h>
DigitalKey KeyButton(keyPin);
double db;
//zqyzqyzqyzqyzqyzqyzqyzqyzqyzqyzqyzqyzqy

#define PIXEL_PIN A0 // Digital IO pin connected to the NeoPixels.
#define PIXEL_COUNT 6

//Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);      //PIXEL_COUNT表示连的LED的数量；PIXEL_PIN表示LED灯所连的引脚； NEO_GRB + NEO_KHZ800表示显示颜色和变化闪烁频率
#define temp1 18    //#define定义温度常量
#define temp2 23
#define hum1 45     //#define定义湿度常量
#define hum2 65

Tem_Hum termo;    //调用Sensor-Tem&Hum传感器

float sensor_tem;
float sensor_hum;   //float定义基本数据单精度常量

#define SSID        "yang"      //热点名称
#define PASSWORD    "78968284"    //Wi-Fi密码
#define DEVICEID    "576794744"    //OneNet上的设备ID
String apiKey = "vP1UP9RJKKVWHt01rBhDyjDXPKU=";   //与设备绑定的APIKey

#define HOST_NAME   "api.heclouds.com"
#define HOST_PORT   (80)
#define INTERVAL_SENSOR   1000             //定义传感器采样时间间隔
#define INTERVAL_NET      2000             //定义发送时间

#include <ESP8266.h>

char buf[10];
unsigned long sensorlastTime = millis();

String jsonToSend; //用于存储发送的json格式参数
char  sensor_tem_c[7], sensor_hum_c[7], sensor_lux_c[7],Time_c[7] ,db_c[7]; //传感器温度、湿度、光照，时间，分贝换成char数组传输

#include <SoftwareSerial.h>
#define EspSerial mySerial
#define UARTSPEED  9600
SoftwareSerial mySerial(2, 3); /* RX:D3, TX:D2 */
ESP8266 wifi(&EspSerial);
unsigned long net_time1 = millis();                          //数据上传服务器时间
unsigned long sensor_time = millis();                        //传感器采样时间计时器
String postString;                                //用于存储发送数据的字符串                    

void setup() {
Serial.begin(9600);//初始化串口通信，并设置波特率9600
pinMode(Light_PIN,INPUT);//打开LED灯
Wire.begin();
  strip.begin();//彩灯初始
  strip.setBrightness(30);  //设置彩灯亮度
  strip.show();// Initialize all pixels to ‘off’
 while (!Serial); // wait for Leonardo enumeration, others continue immediately
    Serial.print(F("setup begin\r\n"));
    delay(100);

 WifiInit(EspSerial, UARTSPEED);

  Serial.print(F("FW Version:"));
  Serial.println(wifi.getVersion().c_str());

  if (wifi.setOprToStationSoftAP()) {
    Serial.print(F("to station + softap ok\r\n"));
  } else {
    Serial.print(F("to station + softap err\r\n"));
  }

  if (wifi.joinAP(SSID, PASSWORD)) {
    Serial.print(F("Join AP success\r\n"));

    Serial.print(F("IP:"));
    Serial.println( wifi.getLocalIP().c_str());
  } else {
    Serial.print(F("Join AP failure\r\n"));
  }

  if (wifi.disableMUX()) {
    Serial.print(F("single ok\r\n"));
  } else {
    Serial.print(F("single err\r\n"));
  }

  Serial.print(F("setup end\r\n"));

}

void loop() {
read();//调用自定义函数
if (sensorValue < Light_value1){//若光强小于600
strip.setAllLED(map(sensorValue, 0, 600, 0, 255),map(sensorValue, 0, 600, 0, 255),0);
strip.show();
}
//“map(val,x,y,m,n)”函数为映射函数，可将某个区间的值（x-y）变幻成（m-n），val则是需要用来映射的数据，这里是将0到600的光对应用0到255的黄光表示
else if (sensorValue >= Light_value1 && sensorValue < Light_value2){//若光强大于600小于900
strip.setAllLED(0, map(sensorValue, 600, 900, 0, 255), 0);
strip.show();
}//将600到900的光分别用0到255的绿光表示
else if (sensorValue >= 900){
strip.setAllLED(map(sensorValue, 900, 1200, 0, 255), 0, 0);
strip.show();
}//将900到1200的光用0到255的红光表示

/*if (sensor_tem < temp1 && sensor_hum < hum1)//if判断语句
colorSet(strip.Color(255, 255, 0));//黄色（可参见颜色的合成规律）
//colorSet(int color)设置颜色的函数
//strip.Color(R,G,B)修改RGB的值可以改变所显现的颜色，R：红色；G：绿色；B：蓝色；

else if ((sensor_tem >= temp1 && sensor_tem <= temp2) && (sensor_hum >= hum1 && sensor_hum <= hum2))
colorSet(strip.Color(0, 255, 0));//绿色
else if (sensor_tem > temp2 && sensor_hum > hum2 )
colorSet(strip.Color(255, 0, 0));//红色*/

if (sensor_time > millis())  sensor_time = millis();      
  if(millis() - sensor_time > INTERVAL_SENSOR)              //传感器采样时间间隔  
  {  
    getSensorData();                                        //读串口中的传感器数据
    sensor_time = millis();
  }   
  if (net_time1 > millis())  net_time1 = millis();  
  if (millis() - net_time1 > INTERVAL_NET)                  //发送数据时间间隔
  {                
    updateSensorData();                                     //将数据上传到服务器的函数
    net_time1 = millis();
  }
  //zqyzqyzqyzqyzqyzqyzqyzqyzqyzqyzqyzqyzqy
   speakerDoing(isAlaram);//蜂鸣器处理
   updateButton();//按键检测
   //double db=getDB();
   db=analogRead(micPin)+137;
   analyticTime(Time);//分析时间
  //zqyzqyzqyzqyzqyzqyzqyzqyzqyzqyzqyzqyzqy
 u8g.firstPage();
  do {
    setFont_M;
    u8g.drawBitmapP(0, 0, 15, 83, picture);
  }while (u8g.nextPage());
}

void read()
{
sensor_tem = SHT2x.GetTemperature();
sensor_hum = SHT2x.GetHumidity();
sensorValue = analogRead(Light_PIN);//光检测
if(KeyButton.readEvent()!=SHORT_PRESS) {
Time=millis()/1000;}
else{
Time=millis()/1000-SetTime;}
//Serial.println(sensor_tem );//将数据从Arduino传递到PC且单独占据一行，此数据可在串口监视器中看到
//Serial.print("–");
//Serial.println( sensor_hum);
//Serial.println(sensorValue);//彩色led灯根据光强调节颜色和亮度
delay(1000);
}
//unit32_t c表示定义灯的颜色


/*double getDB() {
  int voice_data = analogRead(micPin);
  voice_data=map(voice_data,0,1023,0,5);
  double db = (20. * log(10)) * (voice_data / 1.0);
  return db;
}*/

void analyticTime(unsigned long Time){
  if(Time>SetTime){
    isAlaram= true;
  }
}

void updateButton() {
    if(KeyButton.readEvent()==SHORT_PRESS) {
      delay(15);
      isAlaram = false;
    }
}

void speakerDoing(boolean isAlaram) {
  if (isAlaram) {
    tone(buzzerPin, i);
    //strip.setPixelColor(0, strip.Color(125, 125, 125));
    //strip.show();
  } else {
    noTone(buzzerPin);
   // strip.setPixelColor(0, strip.Color(0, 0, 0));
    //strip.show();
  }
}

/*void colorSet(uint32_t c) {
//for 函数循环语句
for (uint16_t i = 0; i < strip.numPixels(); i++) {
strip.setPixelColor(i, c);//i:表示第几个灯开始，从0开始算第一个灯；c 表示灯的颜色
strip.show();//表示LED显示
//delay(wait);
}*/

void getSensorData(){  
    delay(1000);
    dtostrf(sensor_tem, 2, 1, sensor_tem_c);
    dtostrf(sensor_hum, 2, 1, sensor_hum_c);
    dtostrf(sensorValue, 3, 1, sensor_lux_c);
    dtostrf(Time, 2, 1, Time_c);
    dtostrf(db, 2, 1, db_c);
}

void updateSensorData() {
  if (wifi.createTCP(HOST_NAME, HOST_PORT)) { //建立TCP连接，如果失败，不能发送该数据
    Serial.print("create tcp ok\r\n");

jsonToSend="{\"Temperature\":";
    dtostrf(sensor_tem,1,2,buf);
    jsonToSend+="\""+String(buf)+"\"";
    jsonToSend+=",\"Humidity\":";
    dtostrf(sensor_hum,1,2,buf);
    jsonToSend+="\""+String(buf)+"\"";
    jsonToSend+=",\"Light\":";
    dtostrf(sensorValue,1,2,buf);
    jsonToSend+="\""+String(buf)+"\"";
    jsonToSend+=",\"Time\":";
    dtostrf(Time,1,2,buf);
    jsonToSend+="\""+String(buf)+"\"";
    jsonToSend+=",\"DB\":";
    dtostrf(db,1,2,buf);
    jsonToSend+="\""+String(buf)+"\"";
    jsonToSend+="}";

    postString="POST /devices/";
    postString+=DEVICEID;
    postString+="/datapoints?type=3 HTTP/1.1";
    postString+="\r\n";
    postString+="api-key:";
    postString+=apiKey;
    postString+="\r\n";
    postString+="Host:api.heclouds.com\r\n";
    postString+="Connection:close\r\n";
    postString+="Content-Length:";
    postString+=jsonToSend.length();
    postString+="\r\n";
    postString+="\r\n";
    postString+=jsonToSend;
    postString+="\r\n";
    postString+="\r\n";
    postString+="\r\n";

  const char *postArray = postString.c_str();                 //将str转化为char数组
  Serial.println(postArray);
  wifi.send((const uint8_t*)postArray, strlen(postArray));    //send发送命令，参数必须是这两种格式，尤其是(const uint8_t*)
  Serial.println("send success");   
     if (wifi.releaseTCP()) {                                 //释放TCP连接
        Serial.print("release tcp ok\r\n");
        } 
     else {
        Serial.print("release tcp err\r\n");
        }
      postArray = NULL;                                       //清空数组，等待下次传输数据
  
  } else {
    Serial.print("create tcp err\r\n");
  }
}
