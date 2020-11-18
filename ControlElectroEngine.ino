#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// Analog input pin that sensor is attached to
#define CURRENT_SENSOR A0
#define START_RELAY    D2 // Реле нормально разомкнутое.
#define ALARM_RELAY    D3 // Реле нормально замкнутое.

// 66 mV / 1 A   - 30 A sensor
// 100 mV / 1 A  - 20 A sensor
// 185 mV / 1 A  - 5 A sensor
#define K_Current 66
 
float amplitude_current;     //amplitude current
float effective_value;       //effective current 

// Время измерения тока(миллисекунд).
const int8_t measurementTime = 100;

// Ток, выше которого необходимо подключение пускового конденсатора.
// Для двигателя мощностью 1.5 кВт, рабочий ток = 1500 / 220 около 7 А.
// Немного увеличиваем это значение.
const int operatingCurrent = 8;  // (A)

// Предварительно подсчитанный коэффициент для расчётной формулы показаний сенсора.
const int K0 = (K_Current * 1024 * 1.414) / 5000;

// Показание сенсора, выше которого следует включить пусковой конденсатор(1.5 рабочего тока).
const int startSensorValue = 512 + (operatingCurrent + operatingCurrent / 2) * K0;

// Максимальное время работы пускового конденсатора в миллисекундах.
const int maxStartTime = 3000;

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
  // текущее значение сенсора тока
  int sensorValue;
  
    sensorValue = getMaxValue();

    // Проверяем, не стало ли значение тока выше рабочего.
    if ( sensorValue > startSensorValue)
    {
      // запускаем двигатель
      if (StartEngine(maxStartTime) == false)
      {
        // неудачный старт: сигналим и выключаем двигатель на 10 секунд.

        digitalWrite(ALARM_RELAY, HIGH);

        // Здесь код для сигнализирования...

        delay(10000); // пауза 10 секунд

        // Повторно включаем двигатель.
        digitalWrite(ALARM_RELAY, LOW);
      }
    }

    delay(1000); // проводим замеры каждую секунду
}

/// <summary>
/// Режим запуска двигателя.
/// <param name = "startDuration">Продолжительность старта(миллисек.)
/// (продолжительность включения пускового конденсатора)
/// </param>
/// <returns>true - если запуск удачен, иначе false</returns>
/// </summary>
bool StartEngine(int startDuration)
{
  // Включаем пусковой конденсатор.
  digitalWrite(START_RELAY, HIGH);

  uint32_t start_time = millis();
  
  do
  {
    // Получаем текущее значение сенсора тока.
    int sensorValue = getMaxValue();

    // Проверяем, не уменьшилось ли значение тока до рабочего.
    if ( sensorValue < startSensorValue)
    {
      // Выключаем пусковой конденсатор.
      digitalWrite(START_RELAY, LOW);

      // выходим из режима старта
      return true;
    }
  }
  while((millis() - start_time) < startDuration);
  
  return false;  
}


// Функция возвращает максимальное значение показаний сенсора за время measurementTime
int getMaxValue()
{
    int sensorValue;             //value read from the sensor
    int sensorMax = 0;
    
    uint32_t start_time = millis();

//    J = 0;
    
    while((millis()-start_time) < measurementTime)
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

    pinMode(START_RELAY, OUTPUT);
    pinMode(ALARM_RELAY, OUTPUT);
} 