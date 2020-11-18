#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#define CURRENT_SENSOR A0 // Analog input pin that sensor is attached to

// 66 mV / 1 A   - 30 A sensor
// 100 mV / 1 A  - 20 A sensor
// 185 mV / 1 A  - 5 A sensor
#define K_Current 66
 
float amplitude_current;               //amplitude current
float effective_value;       //effective current 

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

//int J = 0;  // Число измерений в секунду

void setup()
{
  Serial.begin(9600); 
  pins_init();
	
	// initialize the LCD
	lcd.init();
}

void loop()
{
  int sensor_max;
  
  sensor_max = getMaxValue();
  Serial.print("sensor_max = ");
  Serial.println(sensor_max);
  
  //the VCC on the Grove interface of the sensor is 5v
  amplitude_current=(float)(sensor_max-512)/ 1024 * 5 / K_Current * 1000;
  
  effective_value=amplitude_current/1.414;
  
  //minimum_current=1/1024*5/185*1000000/1.414=18.7(mA)
  //Only for sinusoidal alternating current
  
  Serial.println("The amplitude of the current is(in A)");
  Serial.println(amplitude_current,1);//Only one number after the decimal point
  Serial.println("The effective value of the current is(in A)");
  Serial.println(effective_value,1);

    
  lcd.backlight();

  lcd.setCursor(0,0);
  lcd.print("Ampl I=");lcd.print(amplitude_current,1);

  lcd.setCursor(0,1);
  lcd.print("EffV I=");lcd.print(effective_value,1);

}

/*Function: Sample for 1000ms and get the maximum value from the S pin*/
int getMaxValue()
{
    int sensorValue;             //value read from the sensor
    int sensorMax = 0;
    
    uint32_t start_time = millis();

//    J = 0;
    
    while((millis()-start_time) < 1000)//sample for 1000ms
    {
        sensorValue = 0;
        
        for (int i = 10; i > 0; i--)
        {
          sensorValue += analogRead(CURRENT_SENSOR);
        }

        sensorValue /= 10;   // Среднее значение за 5 измерений
        
        if (sensorValue > sensorMax) 
        {
          /*record the maximum sensor value*/
          sensorMax = sensorValue;
        }

//        J++;
    }
    
//    Serial.print("Число измерений за 1 секунду = ");
//    Serial.println(J);
        
    return sensorMax;
}

void pins_init()
{
    pinMode(CURRENT_SENSOR, INPUT);
} 
