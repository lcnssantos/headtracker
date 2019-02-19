#include "task.h"
#include "main.h"
#include "timecount.h"

void TaskScheduler(Task Task)
{
    Tasks[TaskCount] = Task;
    TaskCount++;
}

void TaskExecute(void)
{
    char i;
    for (i = 0; i < NUMBER_OF_TASKS; i++) {
        if (TimerCount.miliseconds - Tasks[i].lastTime > Tasks[i].interval) {
            if (Tasks[i].active == YES) {
                Tasks[i].Execute();
                Tasks[i].lastTime = TimerCount.miliseconds;
            }
        }
    }
}