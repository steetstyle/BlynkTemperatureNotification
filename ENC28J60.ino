#include <UIPEthernet.h>
#include <BlynkSimpleUIPEthernet.h>

#include <DHT11.h>
#define DHTPIN_3 3 // pin 0 ve pin 1'e baglamayiniz
#define DHTTYPE DHT11 // DHT11 modulunu tanimliyoruz

DHT11 dht1;


char auth[] = "1if6tFg0OgQeCac_WKWzkWbcVJ69xezy";


#define ROLE_3 3
#define RESET_PIN_NUMBER 7

float SENSOR_1_ISI = 0;
float SENSOR_1_NEM = 0;

float SENSOR_1_ISI_KALIBRE = 0;

float MAX_ISI = 0;


BLYNK_READ(V10)
{
   float sensorData = SENSOR_1_ISI; 
  Blynk.virtualWrite(V10, sensorData + SENSOR_1_ISI_KALIBRE);
}

BLYNK_READ(V11)
{
   float sensorData = SENSOR_1_NEM; 
  Blynk.virtualWrite(V11, sensorData);
}


BLYNK_WRITE(30)  {
    SENSOR_1_ISI_KALIBRE = param.asInt();
}




BLYNK_WRITE(29)  {
    MAX_ISI = param.asInt();
}



void setup()
{


  pinMode(ROLE_3, INPUT);
  digitalWrite(RESET_PIN_NUMBER, HIGH);
  delay(200);
  pinMode(RESET_PIN_NUMBER, OUTPUT);     
 
 
  digitalWrite(ROLE_3, HIGH);

  dht1.setup(ROLE_3);

  Blynk.begin(auth);
  Blynk.syncVirtual( V29, V30);

}

float lastChangedTemperatureTime = 0;
float lastConnectedTimeToInternet = 0;
float temperatureReadDelayTime = 9000;

float lastChangedNotifyTime = 0;
float notifyDelayTime = 6000;

void loop()
{

    if (Blynk.connected()){

        Blynk.run();

        float nowTimestamp = millis();
        
        if(millis()- lastChangedTemperatureTime > temperatureReadDelayTime ){
            SENSOR_1_ISI = dht1.getTemperature();
            SENSOR_1_NEM = dht1.getHumidity();
            lastChangedTemperatureTime = nowTimestamp;
        }
        
        if(MAX_ISI == 0 && (millis() - lastChangedNotifyTime > notifyDelayTime)){
              lastChangedNotifyTime = nowTimestamp;
        }
        else if(SENSOR_1_ISI + SENSOR_1_ISI_KALIBRE >= MAX_ISI && (millis() - lastChangedNotifyTime > notifyDelayTime)){
              Blynk.notify("SENSOR_1 MAX_ISI SINIRINI AŞTI " + String(SENSOR_1_ISI + SENSOR_1_ISI_KALIBRE) + " DERECE");
              lastChangedNotifyTime = nowTimestamp;
        }
        
        lastConnectedTimeToInternet = nowTimestamp;

    }

    else{
      if(millis()- lastConnectedTimeToInternet > temperatureReadDelayTime * 2 ){
            digitalWrite(RESET_PIN_NUMBER, LOW);
      }
    }

}

BLYNK_CONNECTED(){
    Blynk.syncVirtual( V29, V30);
}
