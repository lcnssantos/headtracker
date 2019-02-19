#include "timer.h"


typedef struct
{
    unsigned long miliseconds;
    void (* Update)(Time);
}Time;

void TimeCountConstruct(Time * Time);
void TimeCount_Update(Time * Time);