#include "task.h"
#include "main.h"
#include "timecount.h"

/*
@purpose: Add a task to the scheduler
@paramaters: Task, the Task structure
@return: void
@version: V0.1
*/

void TaskScheduler(Task Task)
{
    Tasks[TaskCount] = Task;
    TaskCount++;
}

/*
@purpose: run the Tasks
@paramaters: void
@return: void
@version: V0.1
*/
void TaskExecute(void)
{
    char i;
    
    /* For each Task in side the Tasks Vector */
    for (i = 0; i < NUMBER_OF_TASKS; i++) 
    {
        /* If the Last execution was done more than the Time in Interval ago */
        if (TimerCount.miliseconds - Tasks[i].lastTime > Tasks[i].interval) 
        {
            /* If the task is active */
            if (Tasks[i].active == YES) 
            {
                /* run the task */
                Tasks[i].Execute();
                /* update last execution time */
                Tasks[i].lastTime = TimerCount.miliseconds;
            }
        }
    }
}
