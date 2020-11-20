#ifndef MODIFIED_MOVING_AVERAGE_H
#define MODIFIED_MOVING_AVERAGE_H

template <class T> 
class ModifiedMovingAverage 
{
public:
  // nm - число измерений
  ModifiedMovingAverage(T nm);
  
  // Устанавливает первое значение
  void SetFirstValue(T fv);

  // Вычисляет новое значение
  T CalcNew(T newValue);

private:
  // Последннее значение.
  T lastValue;

  // Интервал измерений
  T smoothingInterval;
};

#endif