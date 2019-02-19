#define NUMBER_OF_TASKS 5

typedef struct
{
    int interval;
    unsigned long lastTime;
    char active: 1;
    void (*Execute)(void);
}Task;


void TaskScheduler(Task Task);
void TaskExecute(void);

Task Tasks[NUMBER_OF_TASKS];

char TaskCount;