#include <assert.h>
#include <unistd.h>
#include "timer.h"

static int _timeUnit_ = 10; // ms

void setTimeUnit(int milliseconds)
{
   assert (milliseconds > 0);

   _timeUnit_ = milliseconds;
}

int timeUnit()
{
   return _timeUnit_;
}

void spend(int numTimeUnits)
{
   assert (numTimeUnits > 0);

   usleep(_timeUnit_*numTimeUnits*1000);
}

