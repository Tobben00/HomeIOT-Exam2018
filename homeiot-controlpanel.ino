#include "logoico.h"
#include <SparkTime.h>
#include <Adafruit_ST7735.h>
#include "fonts.h"
#include "Adafruit_mfGFX.h"
#include "Adafruit_ST7735.h"   
#include <math.h>

#define cs   A2     
#define dc   D0         
#define rst  NO_RST_PIN 

//Color defines
#define WHITE 0xD6BA

int speakerOut = D3;

Adafruit_ST7735 tft = Adafruit_ST7735(cs, dc, rst); 

SparkTime rtc;
UDP UDPClient;
unsigned long currentTime;

//Setter noen alarm variabler
int alarmhour = 0;
int alarmmin = 0;
int alarmsec = 0;
bool alarm = false;

bool garageIsOpen = false;

void setup() 
{
    //Gir tilgang til particle sin online api
    Particle.function("setAlarm", set);
    Particle.function("Preset", setPreset);    
     
    pinMode(speakerOut,OUTPUT);
       
    tft.initR(INITR_BLACKTAB);
    tft.fillScreen(ST7735_BLACK);
    
    tft.setTextSize(2);

    rtc.begin(&UDPClient, "time.nist.gov");
    rtc.setTimeZone(1); //Setter tidsonen til GMT+1 som er Norsk tid
    
    Particle.subscribe("useGarage", updateGarageTFTStatus, MY_DEVICES);
    
    // Svar for respons enheten
    Particle.subscribe("spotifyResponse", formatSpotifyData, MY_DEVICES);
}

void loop() 
{
    currentTime = rtc.now();
    int min = rtc.minute(currentTime);  
    int hour = rtc.hour(currentTime);
    int sec = rtc.second(currentTime);
    
    int day = rtc.day(currentTime);
    int month = rtc.month(currentTime);
    int year = rtc.year(currentTime);
    
    // Spotify intregrering triggeres
    String data = String(1);
    Particle.publish("Spotify", data, PRIVATE);

    //Under printes tiden med riktig visning 
    tft.setTextSize(2);
    
    if(hour < 10) 
    {
        tft.setCursor(10,10);
        tft.print("0");
        tft.setCursor(15,10);
        tft.print(hour);
    }
    
    if(hour >= 10) 
    {
        tft.setCursor(10,10);
        tft.print(hour); 
    }
    
    tft.setCursor(30,10);
    tft.print(":");
    
    if(min < 10) 
    {
        tft.setCursor(38,10);
        tft.print("0");
        tft.setCursor(50,10);
        tft.print(min);
    }
    
    if(min >= 10) 
    {
        tft.setCursor(38,10);
        tft.print(min); 
    } 
    
    tft.setCursor(60,10);
    tft.print(":");
    
    if(sec < 10) 
    {
        tft.setCursor(70,10);
        tft.print("0");
        tft.setCursor(82,10);
        tft.print(sec);
    }
    
    if(sec >= 10) {
        tft.setCursor(70,10);
        tft.print(sec); 
    } 
    
    tft.setCursor(10,30);
    tft.print(day);
    
    tft.print("/");
    tft.print(month);
    
    tft.print("/");
    tft.print(year); 
    
    if(alarm == false)
    {
        tft.setCursor(10,50);
        tft.setTextSize(0);
        tft.print("Alarm: OFF"); 
    }
    else
    {
        tft.setCursor(10,50);
        tft.setTextSize(0);
        tft.print("Alarm: ACTIVE");
        
        tft.setTextSize(1);
        if(alarmhour < 10) 
        {
            tft.setCursor(10,60);
            tft.print("0");
            tft.setCursor(16,60);
            tft.print(alarmhour);
        }
        
        if(alarmhour >= 10) 
        {
            tft.setCursor(10,60);
            tft.print(alarmhour); 
        }
    
        tft.setCursor(20,60);
        tft.print(":");
    
        if(alarmmin < 10)
        {
            tft.setCursor(24,60);
            tft.print("0");
            tft.setCursor(30,60);
            tft.print(alarmmin);
        }
        
        if(alarmmin >= 10) 
        {
            tft.setCursor(24,60);
            tft.print(alarmmin); 
        }
    }
    
    if(alarmhour == hour)
    {
        if(alarmmin == min)
        {
            while(sec<5)
            {
                alarmOn();
                sec+=5;
            }
        }
    }
    
    //Vis garasje status
    tft.setCursor(10, 70);
    tft.print("GarageStatus:");
    if(garageIsOpen == 0)
    {
        tft.setCursor(90, 70);
        tft.print("CLOSED");
    }
    else
    {
        tft.setCursor(90, 70);
        tft.print("OPEN");
    }
    
    // Spotify display inf0
    tft.setCursor(10,98);
    tft.print("---Tobys Spotify---");
    // Desperat forsøk på å få inn Spotify logo på tft displayet med drawBitmap fra byte data ico som er logoen i ren data...
    //tft.drawBitmap(130,10, ico, 208,48, WHITE);
    
    delay(1000);
    tft.fillScreen(ST7735_BLACK);
}

int setPreset(String comand)
{

    if(comand == "Breakfast")
    {
        alarm = true;
        alarmhour = 8;
        alarmmin = 0;
    }

    if(comand == "Dinner")
    {
        alarm = true;
        alarmhour = 17;
        alarmmin = 0;
    }
    
    if(comand == "Test")
    {
        alarm = true;
        tone(speakerOut, 2000);
    }

    if(comand == "Off")
    {
        alarm = false;
        alarmhour = 0;
        alarmmin = 0;
        tft.setCursor(10,50);
        tft.setTextSize(0);
        tft.print("Alarm: OFF");
        noTone(speakerOut);
    }
    return 1;
}

void alarmOn()
{
    for(int i = 0; i < 5; i++)
    {
        tone(speakerOut, 2000, 8000);
    }
 }
 
int set(String pTime)   
{
    const char* str = pTime;
    int myNr;

    if(sscanf(str, "%d", &myNr)  == EOF )
    {
        // Noe gikk galt her
    }
    int pHour = myNr / 100;
    int pMin = myNr % 100;
    alarm = true;
    alarmhour = pHour;
    alarmmin = pMin;
    return 1;
} 

void updateGarageTFTStatus(const char *event, const char *data)
{
    if(garageIsOpen == 0)
    {
        garageIsOpen = true;
    }
    else 
    {
        garageIsOpen = false;
    }
}

void formatSpotifyData(const char *event, const char *data) 
{
    String name = data;
    String stringclone = name;
    String stringclonenew = name;
    
    for (int iterate = 0; iterate < stringclone.length(); iterate++)
    {
        stringclone[iterate] = ' ';
    }
   
    for (int iterate = 0; iterate < name.length(); iterate++)
    {
        if (name[iterate] != ',')
        {
            stringclone[iterate] = name[iterate];
        }
        else break;
    }
    
    for (int iterate = 0; iterate < stringclone.length(); iterate++)
    {
        stringclonenew[iterate] = ' ';
    }
    
    char *a;
    int index;
    a = strchr(name, 'A');
    index = (int)(a - name);
    
    for (int iterate = 0; iterate < name.length(); iterate++)
    {
        if ( name[index] == name[iterate])
        
        {
            for (int itr = iterate; itr < name.length(); itr++)
            {
                stringclonenew[itr] = name[itr];
            }
        }
    }
    
    for (int itr = 0; itr < name.length(); itr++)
    {
        if (stringclone[itr] == '{' || stringclone[itr] == '}')
            stringclone[itr] = ' ';
    }
    
    for (int itr = 0; itr < name.length(); itr++)
    {
        if (stringclonenew[itr] == '{' || stringclonenew[itr] == '}')
            stringclonenew[itr] = ' ';
    }
    
    //Print det formaterte data på TFT displayet
    tft.setCursor(10,100);
    tft.print(stringclone);
    tft.setCursor(10,120);
    tft.print(stringclonenew);
}