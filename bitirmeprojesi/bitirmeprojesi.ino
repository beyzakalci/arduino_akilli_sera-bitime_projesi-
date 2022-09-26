#include <dht.h>       // DHT kütüphanesinin eklenmesi
#include <Wire.h>  

 float sicaklik = 0;  //kullanılan modüllerin bağlı oldukları pinlerin tanımlanması
 float nem = 0;
 int suseviye = A1;
 int topraknem = A0;
 int seviyedeger;
 int nemdeger;
 #define motor 6

 dht DHT;               // dht kütüphane değişkeninin DHT olarak tanımlanması
 const int data=2; 
 
#define REMOTEXY_MODE__ESP8266_HARDSERIAL_POINT //remotexy nin esp8266 ile hardserial olarak haberleşeceğini tanımlama

#include <RemoteXY.h> //remotexy kütüphanesinin eklenmesi

// RemoteXY bağlantı ayarları
#define REMOTEXY_SERIAL Serial
#define REMOTEXY_SERIAL_SPEED 115200
#define REMOTEXY_WIFI_SSID "Beyza"
#define REMOTEXY_WIFI_PASSWORD "123456789"
#define REMOTEXY_SERVER_PORT 6377


// RemoteXY yapılandırması 
#pragma pack(push, 1)
uint8_t RemoteXY_CONF[] =   // 382 bytes
  { 255,34,0,44,0,119,1,16,24,5,129,0,7,41,14,4,1,204,83,196,
  177,99,97,107,108,196,177,107,0,67,1,4,46,20,5,1,16,27,11,67,
  1,4,62,20,5,1,16,27,11,67,1,34,46,20,5,1,16,27,11,129,
  0,34,57,22,4,1,204,83,117,32,83,101,118,105,121,101,115,105,0,129,
  0,9,74,11,4,1,204,73,197,159,196,177,107,108,97,114,0,2,1,8,
  81,15,7,1,35,201,16,16,65,195,167,196,177,107,0,75,97,112,97,108,
  196,177,0,67,1,34,62,22,5,1,16,27,11,129,0,10,57,8,4,1,
  204,78,101,109,0,129,0,33,41,22,4,1,204,84,111,112,114,97,107,32,
  78,101,109,0,129,0,22,31,20,5,1,36,83,69,82,65,95,49,0,131,
  4,7,11,20,4,1,6,16,83,101,114,97,95,49,0,131,5,2,3,40,
  6,2,202,16,65,110,97,115,97,121,102,97,0,129,0,2,50,31,5,2,
  16,75,117,108,108,97,110,196,177,99,196,177,32,65,100,196,177,0,129,0,
  2,68,11,5,2,16,197,158,105,102,114,101,0,7,36,2,56,57,8,2,
  29,25,1,21,7,36,2,74,57,8,2,29,25,1,11,131,4,7,17,20,
  4,3,6,16,83,101,114,97,95,50,0,131,4,7,23,20,4,4,6,16,
  83,101,114,97,95,51,0,130,1,34,85,25,9,2,6,129,0,35,87,22,
  5,2,16,71,105,114,105,197,159,32,89,97,112,0,129,0,33,74,25,4,
  1,204,72,97,118,97,108,97,110,100,196,177,114,109,97,0,2,1,38,81,
  15,7,1,35,201,30,30,65,195,167,196,177,107,0,75,97,112,97,108,196,
  177,0 };
  
//Bu yapı, kontrol arayüzünün tüm değişkenlerini ve olaylarını tanımlar.
struct {

    // input variables
  uint8_t switch_1; // =1 if switch ON and =0 if OFF 
  char edit_1[21];  // string UTF8 end zero  
  char edit_2[11];  // string UTF8 end zero  
  uint8_t switch_2; // =1 if switch ON and =0 if OFF 

    // output variables
  char text_1[11];  // string UTF8 end zero 
  char text_2[11];  // string UTF8 end zero 
  char text_3[11];  // string UTF8 end zero 
  char text_4[11];  // string UTF8 end zero 

    // other variable
  uint8_t connect_flag;  // =1 if wire connected, else =0 

} RemoteXY;
#pragma pack(pop)

#define PIN_SWITCH_1 8 //arayüzde kontrol edilecel switch ve ledlerin tanımlanması
#define PIN_SWITCH_2 7
#define LED_1 10
#define LED_2 11


void setup() 
{
  RemoteXY_Init (); 
  
  pinMode (PIN_SWITCH_1, OUTPUT); //yapılan işlemlere göre reaksiyon verecek olan pinlerin output olarak tanımlanması
  pinMode (PIN_SWITCH_2, OUTPUT);
  pinMode(motor, OUTPUT);
  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
  
  
}

void loop() 
{ 
  RemoteXY_Handler ();
  
  digitalWrite(PIN_SWITCH_1, (RemoteXY.switch_1==0)?LOW:HIGH);
  digitalWrite(PIN_SWITCH_2, (RemoteXY.switch_2==0)?LOW:HIGH);
  
  float readData = DHT.read11(data);  // DHT11'den veri okumaya başladık.
  sicaklik = DHT.temperature;      //  DHT11'den sicaklik değerini okuduk.
  nem = DHT.humidity;             //   DHT11'den nem değerini okuduk.

  dtostrf(sicaklik, 0, 1, RemoteXY.text_1); //sıcaklık ve nem sensöründeki verileri arayüze aktaran kod bloğu
  dtostrf(nem, 0, 1, RemoteXY.text_2);
  
  seviyedeger = analogRead(suseviye);   //su seviyesi sensöründeki verileri arayüze aktaran kod bloğu
  itoa (seviyedeger, RemoteXY.text_4, 10); 
  
  nemdeger = analogRead(topraknem);  //Toprak nem sensöründeki verileri arayüze aktaran kod bloğu
  itoa (nemdeger, RemoteXY.text_3, 10); 


  if (seviyedeger<250)
  {
    digitalWrite(LED_1, HIGH); //Su seviyesinin kritik ve normal düzeylerde olmasını haber verecek ikaz ledlerinin kontrolü
    delay(500);
    digitalWrite(LED_1, LOW);
    delay(500);
    digitalWrite(LED_2, LOW);
    
  
    } 
   else
   {
     digitalWrite(LED_2, HIGH);
    delay(500);
    digitalWrite(LED_2, LOW);
    delay(500);
    digitalWrite(LED_1, LOW);
     
    }

if (nemdeger>500) // Topraktaki nem değeri düştüğünde motoru çalıştırarak sulama yaptıran kod bloğu
{
  digitalWrite(motor,HIGH);

  }
 else
 {
  digitalWrite(motor,LOW);
  
  }

 


}
