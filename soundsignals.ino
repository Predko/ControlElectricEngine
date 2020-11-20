#include "soundsignals.h"

/* 
  const int SoundPin = 8;       // Пин подключения пьезоизлучателя – 9 дискретный
  const int DelaySound = 500;   // Длительность воспроизведения сигнала(миллисекунд)
  const uint8_t NumberOfIntervals = 50;
  const int LowSound = 1500;
  const int HighSound =3000;

  const int IntervalDuration = DelaySound / NumberOfIntervals / 2;
  const int IntervalLength = (HighSound - LowSound) / NumberOfIntervals;
 */

void AlarmSound(uint8_t numberOfSignals)
{
  for (int i = 0; i < numberOfSignals; i++)
  {
    for (int j = 0; j < NumberOfIntervals; j++)
    {
      tone(SoundPin, LowSound + j * IntervalLength);
      delay(IntervalDuration);
    }

    for (int j = 0; j < NumberOfIntervals; j++)
    {
      tone(SoundPin, HighSound - j * IntervalLength);
      delay(IntervalDuration);
    }
  }

  noTone(SoundPin); // Выключаем звук
}


void ReadySound(int duration)
{
  tone(SoundPin, HighSound);

  delay(duration);

  noTone(SoundPin); // Выключаем звук
}