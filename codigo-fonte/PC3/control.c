// Access from ARM Running Linux - updated for RPi4
#include "gpio_dev_mem.h"
#include <sys/poll.h>
#include <pthread.h>
#include <signal.h>

// Servo pin - GPIO17
#define servo_pin 17
#define minAngle -30 // 60
#define maxAngle 60 //150
#define Nlim 30

#define deg_0_pulse 0.5
#define deg_180_pulse 2.5
#define f 50

#define period 1000/f
#define k 100/f
#define deg_0_duty deg_0_pulse*k
#define pulse_range deg_180_pulse - deg_0_pulse
#define duty_range pulse_range * k

// dutyCyclechange function - using device memory

void dutyCyclechange(int pin, int degree, int N)
{
    int t1 = (50*degree+4)/9+1500;
    int t2 = 20000-t1;
    int i;
    for(i=0; i<N; i++)
    {
        GPIO_SET = 1<<pin;
        usleep(t1);
        GPIO_CLR = 1<<pin;
        usleep(t2);
    }
    // printf("Duty cycle = %.2f %%\n", (double)(100*t1)/(double)period);
}

void control(int data, int N)
{
    if (data == 1)
        dutyCyclechange(servo_pin, minAngle, N);
    else if (data == -1)
        dutyCyclechange(servo_pin, maxAngle, N);
    else if (data == 3)
        dutyCyclechange(servo_pin, 0, N);
}


int main(int argc, char *argv[])
{

    setup_io();
    OUT_GPIO(servo_pin);
    int data = atoi(argv[1]);
    int N = Nlim;
    /*
    int angle = atoi(argv[1]);
    if(angle<-90)
            angle = 0;
        else if(angle>90)
            angle = 90;
    */
    // int N = atoi(argv[2]);

    control(data, N);
}