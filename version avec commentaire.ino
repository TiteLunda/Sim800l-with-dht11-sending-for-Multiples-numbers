#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
#include<TM1637Display.h>
#include <Wire.h>
#include "DHT.h"
#define DHTPIN 2    // pin connexion capteur de temperature et humidite
#define DHTTYPE DHT11
#define CLK 22    // afficheur temperature pin declaration
#define DIO 24    // afficheur temperature pin declaration
const int rs = 12, en = 11, d4 = 7, d5 = 6, d6 = 5, d7 = 4; // declaration pin de connexion de LCD
String Data_SMS;            //String that we're going to send via SMS

char Received_SMS;              //Here we store the full received SMS (with phone sending number and date/time) as char
short DHT_OK=-1;     //Used later it shows if there's the word "DHT"/"MLX" within the received SMS "-1" means they are not found

int h=0;
int t=0;
int fanPin=36;  // brancher ici le + ventillateur   
int humPin=38;  // brancher ici la led verte pour l'humidite

// Numro a utiliser pour l'envoie du message
String f1001 = "+243992151651"; 
String f1002 = "+243997906060"; 
String f1003 = "+243995332872"; 


SoftwareSerial sim800l(40, 42);  // pin de connexion du GSM, tx=40 and rx=42
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);   // pin de connexion de LCD
DHT dht(DHTPIN, DHTTYPE);
TM1637Display display = TM1637Display(CLK, DIO);   
const uint8_t degree[]={
  SEG_B|SEG_C|SEG_D|SEG_E|SEG_G,
  SEG_C|SEG_D|SEG_E|SEG_G,
  SEG_C|SEG_E|SEG_G,
  SEG_A|SEG_D|SEG_E|SEG_F|SEG_G
  };
  const uint8_t celsius[] = {
  SEG_A | SEG_B | SEG_F | SEG_G,            
  SEG_A | SEG_D | SEG_E | SEG_F
      };
    const uint8_t E[]={
    SEG_A|SEG_D|SEG_E|SEG_F|SEG_G,
    };    
void setup() {
  
  lcd.begin(20, 4);   //debut lcd
  lcd.setCursor(4,0);
  lcd.print("STATION METEO");
  lcd.setCursor(0,2);
  lcd.print("DEPOT PHARMACEUTIQUE");
  lcd.setCursor(7,3);
  lcd.print("DE GOMA");
  delay(3000);
  lcd.clear();
   lcd.setCursor(3,0);
  lcd.print("presente par:");
  lcd.setCursor(0,2);
  lcd.print("IT.Jacques Ciza");
  delay(3000);
  dht.begin();
  sim800l.begin(9600);   //Module baude rate, this is on max, it depends on the version
  delay(3000);
  Receive_Mode(); //fonction pour recevoir un sms  
  lcd.clear();
  display.clear();
  pinMode(fanPin,OUTPUT); // ventillateur pin mode pour la sortie
  pinMode(humPin,OUTPUT); // Humidite pin mode pour la sortie
  
}

void loop() {
  display.setBrightness(3);
  float h = dht.readHumidity();  // lecture humidite sur le capteur
  float t = dht.readTemperature(); // lecteur temperature sur le capteur
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);
  Data_SMS = "Temperature = "+String(t,1)+" C" + " \nHumidity ="+String(h,1)+ " %" +"\nTest Memoire: Ir"+"\Ciza Sambo Jacques";   //contenu du message a etre envoyer,
   String RSMS; 
  // verification si le capteur est connecte
  if (isnan(h) || isnan(t) || isnan(f)) {  
    lcd.setCursor(0, 0);
    lcd.print("Failed, DHT sensor");
    lcd.setCursor(0, 1);
    lcd.print("connetion error!");
    display.clear();
    display.setSegments(E,2,2);
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);
    lcd.setCursor(0, 0);
  lcd.print("Temp:" );
  lcd.setCursor(6, 0);
  lcd.print(t);
  display.showNumberDec(t,false,2,0);
  display.setSegments(celsius,2,2);
  lcd.setCursor(10, 0);
  lcd.print("°C ");
  lcd.setCursor(0, 1);
  lcd.print("Humid:");
  lcd.setCursor(6, 1);
  lcd.print(h);
    lcd.setCursor(10, 1);
  lcd.print("% ");
  lcd.setCursor(0, 3);
  lcd.print(f1001);

 // Condition de verification pour recevoir un sms;
    while(sim800l.available()>0){       //When SIM800L sends something to the Arduino... problably the SMS received... if something else it's not a problem
        
        Received_SMS=sim800l.read();  //"char Received_SMS" is now containing the full SMS received
        Serial.print(Received_SMS);   //Show it on the serial monitor (optional)     
        RSMS.concat(Received_SMS);    //concatenate "char received_SMS" to RSMS which is "empty"
        DHT_OK=RSMS.indexOf("DHT");   //And this is why we changed from char to String, it's to be able to use this function "indexOf"
           //"indexOf function looks for the substring "x" within the String (here RSMS) and gives us its index or position
                                      //For example if found at the beginning it will give "0" after 1 character it will be "1"
                                      //If it's not found it will give "-1", so the variables are integers
        
    }

  if(DHT_OK!=-1){                   
  delay(1000);
  Serial.println("found DHT");

  sendsms(Data_SMS, f1001);
  sendsms(Data_SMS, f1002);
  sendsms(Data_SMS, f1003);
  Receive_Mode();
  
  
  DHT_OK=-1;
 }



 // condition de la temperature
 if(t>30){
  //Envoie de la temperature et humidite sur sms
    Serial.println("temperature is high");  //Displays this message
    delay(200); //Delay to avoid reading the button state many times

  sendsms(Data_SMS, f1001); // you can use a variable of the type String
  Serial.println(Data_SMS);
  Serial.println("message sent."); 
  delay(3000);
    
  sendsms(Data_SMS, f1002); // you can also write any message that you want to send.
  Serial.println(Data_SMS);
  Serial.println("message sent."); 
  delay(3000);

  sendsms(Data_SMS, f1003); // you can also write any message that you want to send.
  Serial.println(Data_SMS);
  Serial.println("message sent."); 
  delay(3000);
    
  lcd.setCursor(0, 1);
  lcd.print("message sent to:");
  lcd.setCursor(0, 2);
  lcd.print(f1001);
  lcd.setCursor(0,3); 
  lcd.print(f1002);  
  lcd.setCursor(0,4);
  lcd.print(f1003);
  delay(10000);
  //fan
  digitalWrite(fanPin,HIGH);
  while(1){
    }
 }
 else{
  digitalWrite(fanPin,LOW);
  }
  if(h>50){
    digitalWrite(humPin,HIGH);
    }
 else{
 digitalWrite(humPin,LOW);
 }
  Serialcom();        
 // fonction pour envoyer le message
}
void Serialcom()
{
  delay(500);
  while (Serial.available()) 
  {
    sim800l.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while(sim800l.available()) 
  {
    Serial.write(sim800l.read());//Forward what Software Serial received to Serial Port
  }
}
//Fonction pour recevoir un SMS
void Receive_Mode(){      

  sim800l.println("AT"); //If everything is Okay it will show "OK" on the serial monitor
  Serialcom();
  sim800l.println("AT+CMGF=1"); // Configuring TEXT mode
  Serialcom();
  sim800l.println("AT+CNMI=2,2,0,0,0"); //Configure the SIM800L on how to manage the Received SMS... Check the SIM800L AT commands manual
  Serialcom();
}



// fonction pour envoyer le message
void sendsms(String message, String number)
{
String mnumber = "AT + CMGS = \""+number+"\""; 
  
sim800l.print("AT+CMGF=1\r");                      
  delay(1000);
sim800l.println(mnumber);  // recipient's mobile number, in international format
 
  delay(1000);
  sim800l.println(message);                         // message a envoyer
  delay(1000);
  sim800l.println((char)26);                        // End AT command with a ^Z, ASCII code 26
  delay(1000); 
  sim800l.println();
  delay(100);                                     //  fixation du Temps d'envoie
 
}


 
