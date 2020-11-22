/* 
Copyright © 2020 Предко Виктор. All rights reserved.
License GPLv3.
Author: Предко Виктор.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

  (Это свободная программа: вы можете перераспространять ее и/или изменять
   ее на условиях Стандартной общественной лицензии GNU в том виде, в каком
   она была опубликована Фондом свободного программного обеспечения; либо
   версии 3 лицензии, либо (по вашему выбору) любой более поздней версии.

   Эта программа распространяется в надежде, что она будет полезной,
   но БЕЗО ВСЯКИХ ГАРАНТИЙ; даже без неявной гарантии ТОВАРНОГО ВИДА
   или ПРИГОДНОСТИ ДЛЯ ОПРЕДЕЛЕННЫХ ЦЕЛЕЙ. Подробнее см. в Стандартной
   общественной лицензии GNU.

   Вы должны были получить копию Стандартной общественной лицензии GNU
   вместе с этой программой. Если это не так, см.
   <https://www.gnu.org/licenses/>.)

*/


// Размер прошивки:
//
// С отладочной информацией:
// Скетч использует 8104 байт (25%) памяти устройства. Всего доступно 32256 байт.
// Глобальные переменные используют 617 байт (30%) динамической памяти, оставляя 1431 байт для локальных переменных. Максимум: 2048 байт.
//
// Без поддержки LCD:
// Скетч использует 6578 байт (20%) памяти устройства. Всего доступно 32256 байт.
// Глобальные переменные используют 456 байт (22%) динамической памяти, оставляя 1592 байт для локальных переменных. Максимум: 2048 байт.
//
// Без serial отладочной информации:
// Скетч использует 3386 байт (10%) памяти устройства. Всего доступно 32256 байт.
// Глобальные переменные используют 139 байт (6%) динамической памяти, оставляя 1909 байт для локальных переменных. Максимум: 2048 байт.

#include <Wire.h> 
#include <ModifiedMovingAverage.h>

// Отладочная информация на LCD дисплей.
//#define LCD_DEBUG_ENABLE

// Отладочная информация в COM порт.
//#define SERIAL_DEBUG_ENABLE

#include "serial_debug_info.h"
#include "lcd_debug_info.h"

// Analog input pin that sensor is attached to
#define CURRENT_SENSOR A0
#define START_RELAY    2 // Реле нормально разомкнутое.
#define ALARM_RELAY    3 // Реле нормально замкнутое.

// 66 mV / 1 A   - 30 A sensor
// 100 mV / 1 A  - 20 A sensor
// 185 mV / 1 A  - 5 A sensor
const float K_Current = 66;

// Ток, выше которого необходимо подключение пускового конденсатора.
// Для двигателя мощностью 1.5 кВт, рабочий ток = 1500 / 220 около 7 А.
// Немного увеличиваем это значение.
const int operatingCurrent = 8;  // (A)

// Предварительно подсчитанный коэффициент для расчётной формулы показаний сенсора.
const int K0 = (K_Current * 1024 * 1.414) / 5000.0;

// Показание сенсора, выше которого следует включить пусковой конденсатор(1.5 рабочего тока).
const int startSensorValue = 512.0 + (operatingCurrent + operatingCurrent / 2) * K0;

#if defined(SERIAL_DEBUG_ENABLE) || defined(LCD_DEBUG_ENABLE)

  #define AMPLITUDE_CURRENT(sensor_value) (1.414 * ((sensor_value) - 512) / K0)
  #define EFFECTIVE_CURRENT(sensor_value) (((sensor_value) - 512) / K0)

#endif

// Максимальное время работы пускового конденсатора в миллисекундах.
const int maxStartTime = 3000;

template class ModifiedMovingAverage<int>;

ModifiedMovingAverage<int> averageValue(5);

void setup()
{
  // Подготавливаем реле для работы системы.
  digitalWrite(START_RELAY, HIGH);  // Пусковой конденсатор выключен.
  digitalWrite(ALARM_RELAY, HIGH);  // Питание двигателя включено.

  pins_init();

  // Считываем первое значение датчика тока.
  int sensorValue = getMaxValue();
  averageValue.SetFirstValue(sensorValue);
    
  LCD_INIT;
  LCD_DEBUG(0, 0, "amplitude I:", AMPLITUDE_CURRENT(sensorValue));
  LCD_DEBUG(0, 1, "effective I:", EFFECTIVE_CURRENT(sensorValue));

  SERIAL_BEGIN(9600);

  SERIAL_DEBUG(AMPLITUDE_CURRENT(sensorValue));
  SERIAL_DEBUG(EFFECTIVE_CURRENT(sensorValue));

  ReadySound(500);  // Полсекундный сигнал неисправности.
}

void loop()
{
  // текущее значение сенсора тока
  int sensorValue;
  
    sensorValue = getMaxValue();

    {
      SERIAL_DEBUG(sensorValue);

      LCD_DEBUG(0, 0, "amplitude I:", AMPLITUDE_CURRENT(sensorValue));
      LCD_DEBUG(0, 1, "effective I:", EFFECTIVE_CURRENT(sensorValue));

      SERIAL_DEBUG(AMPLITUDE_CURRENT(sensorValue));
      SERIAL_DEBUG(AMPLITUDE_CURRENT(sensorValue));
    }

    // Проверяем, не стало ли значение тока выше рабочего.
    if ( sensorValue > startSensorValue)
    {
      // запускаем двигатель
      AlarmSound(1);

      if (StartEngine(maxStartTime) == false)
      {
        // неудачный старт: сигналим и выключаем двигатель на 10 секунд.

        digitalWrite(ALARM_RELAY, LOW);

        // Звуковой сигнал около 10 секунд.

        AlarmSound(2);  // Двухсекундный сигнал неисправности.

        delay(2000);    // Двухсекундная пауза до перезапуска.

        AlarmSound(2);  // Двухсекундный сигнал неисправности.

        delay(2000);    // Двухсекундная пауза до перезапуска.

        AlarmSound(2);  // Двухсекундный сигнал неисправности.

        delay(2000);    // Двухсекундная пауза до перезапуска.

        // Повторно включаем двигатель.
        digitalWrite(ALARM_RELAY, HIGH);
      }
    }
    
    delay(1000); // проводим замеры каждую секунду
}

// Режим запуска двигателя.
// <param name = "startDuration">Продолжительность старта(миллисек.)
// (продолжительность включения пускового конденсатора)
// </param>
// <returns>true - если запуск удачен, иначе false</returns>
bool StartEngine(int startDuration)
{
  // Включаем пусковой конденсатор.
  digitalWrite(START_RELAY, LOW);

  uint32_t start_time = millis();

  SERIAL_DEBUGSTR("Start engine");
  SERIAL_DEBUG(start_time);
  
  do
  {
    // Получаем текущее значение сенсора тока.
    int sensorValue = getMaxValue();

    // Проверяем, не уменьшилось ли значение тока до рабочего.
    if ( sensorValue < startSensorValue)
    {
      // Выключаем пусковой конденсатор.
      digitalWrite(START_RELAY, HIGH);

      SERIAL_DEBUGSTR(millis());
      SERIAL_DEBUGSTR("Start Ok");
      // выходим из режима старта
      return true;
    }
  }
  while((millis() - start_time) < startDuration);
  
  // Выключаем пусковой конденсатор.
  digitalWrite(START_RELAY, HIGH);

  SERIAL_DEBUGSTR(millis());
  SERIAL_DEBUGSTR("Start failed");
  return false;  
}

// Время измерения тока(миллисекунд).
const int8_t measurementTime = 100;

// Функция возвращает максимальное значение показаний сенсора за время measurementTime
// Используется Экспоненциальное скользящее среднее произвольного порядка для сглаживания шума. 
int getMaxValue()
{
    int sensorValue;             //value read from the sensor
    int sensorMax = 0;
    
    uint32_t start_time = millis();

    while((millis()-start_time) < measurementTime)
    {
        sensorValue = averageValue.CalcNew(analogRead(CURRENT_SENSOR));
        
        if (sensorValue > sensorMax) 
        {
          /*record the maximum sensor value*/
          sensorMax = sensorValue;
        }
    }
        
    return sensorMax;
}

void pins_init()
{
    pinMode(CURRENT_SENSOR, INPUT);

    pinMode(START_RELAY, OUTPUT);
    pinMode(ALARM_RELAY, OUTPUT);
}
