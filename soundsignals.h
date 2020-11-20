#ifndef SOUNDSIGNAL_H
#define SOUNDSIGNAL_H

const int SoundPin = 8;       // Пин подключения пьезоизлучателя – 9 дискретный
const int DelaySound = 500;   // Длительность воспроизведения сигнала(миллисекунд)
const uint8_t NumberOfIntervals = 50;
const int LowSound = 1500;
const int HighSound =3000;

const int IntervalDuration = DelaySound / NumberOfIntervals / 2;
const int IntervalLength = (HighSound - LowSound) / NumberOfIntervals;

void AlarmSound(uint8_t numberOfSignals);

void ReadySound(int duration);

#endif