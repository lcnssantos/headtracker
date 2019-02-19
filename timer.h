#define YES 1
#define NO 0

#define _8_BIT 1
#define _16_BIT 0

#define INTERNAL_CLOCK 0
#define TOCKI 1

#define HIGH_TO_LOW 1
#define LOW_TO_HIGH 0
#define PRESCALER_ON 0
#define PRESCALER_OFF 1

#define PRESCALER_256 7
#define PRESCALER_128 6
#define PRESCALER_64 5
#define PRESCALER_32 4
#define PRESCALER_16 3
#define PRESCALER_8 2
#define PRESCALER_4 1
#define PRESCALER_2 0

#define INTERRUPT_LOW_PRIORITY 0
#define INTERRUPT_HIGH_PRIORITY 1



typedef struct
{
    char Opr: 1;
    char Mode: 1;
    char ClkSrc: 1;
    char SrcEdg: 1;
    char PSA: 1;
    char PSAConfig: 3;
    char PreloadOn: 1;
    char Preload;
    void (*Init)(Timer0);
    void (*Write)(int);
    int (*Read)(void);
    void (*IntConfig)(char);
    void (*IntProc)(Time);
}Timer0;

void Timer0_Construct(Timer0 * Timer);
void Timer0_Init(Timer0 * Timer);
void Timer0_Write(int Value);
int Timer0_Read();
void Timer0_Interrupt_Config(char IntPr);
