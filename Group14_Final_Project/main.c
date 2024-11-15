#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <tm4c123gh6pm.h>
#define CLOCK  16000000
#define DAC_I2C_ADDRESS 0x58
#define L 6


void GPIO_PORTF_INIT(void);

void Systick_Handler(void);     // send data on interrupt to DAC: 23us
void systick(float time);

uint8_t audio[L] = {0x10, 0x20, 0x7F, 0xAB, 0xC9, 0xFF}; // add data here.....

int i = 0;     // index counter


void I2C1_Init(void);
void set_slave_addr(void);
void DFR0971_Write(uint8_t data , uint8_t command);


void main(void)
{

    I2C1_Init();
    set_slave_addr();

    DFR0971_Write(0x02,0x03); // DAC setting and I2C: start-run

    while(1)
    {
         systick(46);   // output at 22KHz
         i++;

         if(i == L-1)
             i = 0;

    }

}


// Function to initialize I2C
void I2C1_Init(void) {
    SYSCTL_RCGCI2C_R = (1 << 1);            // Enable clock to I2C1
    SYSCTL_RCGCGPIO_R = (1 << 1);           // Enable clock to Port B
    while ((SYSCTL_PRGPIO_R & 0x02) == 0); // Wait until GPIOB is ready

    GPIO_PORTB_AFSEL_R = (1 << 6) | (1 << 7);          // Enable alternate function on PB2, PB3



    GPIO_PORTB_DEN_R = (1 << 6) | (1 << 7);       // Enable digital I/O on PB2, PB3
    GPIO_PORTB_ODR_R  = (1 << 7)   ;        // Enable open-drain on PB3 (SDA)       // Enable open-drain on PB3 (SDA)
    GPIO_PORTB_PCTL_R = (3 << 28) | (3 << 24);      // Set I2C function on PB2, PB3

    I2C1_MCR_R = (1 << 4);                   // Enable I2C1 Master function
    I2C1_MTPR_R = (7 << 0);                    // Set SCL clock speed to 100 kHz with 16 MHz system clock
}

void set_slave_addr(void)
{
    I2C1_MSA_R = DAC_I2C_ADDRESS << 1 ;
    I2C1_MSA_R &= !(1 << 0);

}

// Function to send data to a specific channel on DFR0971
void DFR0971_Write(uint8_t data, uint8_t commands) {

    I2C1_MDR_R = data;
    I2C1_MCS_R = commands;
    while (I2C1_MCS_R & 0x01);           // Wait for I2C transmission

    if (I2C1_MCS_R & 0x02) {             // Check for errors
        I2C1_MCS_R = 0x04;               // Send stop condition if error
    }
}




void systick(float time)
{

    float val;
    val = (time*(CLOCK))/1000000;
    NVIC_ST_RELOAD_R = val;        // reload value
    NVIC_ST_CURRENT_R  = 0x0;         // current value
    NVIC_ST_CTRL_R = 0x07;          // enable and choice of clk and enable intruppt.


}




void Systick_Handler(void)
{

    if(i < L-1)
    {
        DFR0971_Write(audio[i] - 0x80,0x01);    // unsigned 8 bit wav file has 0x80 as zero // run-run-run

    }

    else if(i == L-1)
    {

        DFR0971_Write(audio[i] - 0x80,0x05);    // run-stop

    }

}


void GPIO_PORTF_INIT(void)
{
    SYSCTL_RCGCGPIO_R |= 0x20;  //enable CLOCK to portf

    GPIO_PORTF_LOCK_R = 0X4C4F434B;
    GPIO_PORTF_CR_R = 0X1F;

    GPIO_PORTF_DIR_R = 0x0E;    //set correct directions

    GPIO_PORTF_DEN_R = 0x1F;    //enable digital operation at pins
    GPIO_PORTF_PUR_R = 0x11;    //enable pullups for switches



}

