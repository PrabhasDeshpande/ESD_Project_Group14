#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <tm4c123gh6pm.h>
#define CLOCK  16000000
#define DAC_I2C_ADDRESS 0x58


void GPIO_PORTF_INIT(void);

void Systick_Handler(void);
void systick(float time);


void I2C0_Init(void);
void DFR0971_Write(uint8_t command, uint8_t data);


void main(void)
{

    SYSCTL_RCGCGPIO_R |= 0x21;  //enable clock to portA and portF

    GPIO_PORTF_INIT();
    //systick(22);

    I2C0_Init();
    DFR0971_Write(01,11);

    while(1)
    {


    }

}


// Function to initialize I2C
void I2C0_Init(void) {
    SYSCTL_RCGCI2C_R |= 0x01;            // Enable clock to I2C0
    SYSCTL_RCGCGPIO_R |= 0x02;           // Enable clock to Port B
    while ((SYSCTL_PRGPIO_R & 0x02) == 0); // Wait until GPIOB is ready

    GPIO_PORTB_AFSEL_R |= 0x0C;          // Enable alternate function on PB2, PB3
    GPIO_PORTB_ODR_R |= 0x08;            // Enable open-drain on PB3 (SDA)
    GPIO_PORTB_DEN_R |= 0x0C;            // Enable digital I/O on PB2, PB3
    GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & 0xFFFF00FF) | 0x00003300; // Set I2C function on PB2, PB3

    I2C0_MCR_R = 0x10;                   // Enable I2C0 Master function
    I2C0_MTPR_R = 24;                    // Set SCL clock speed to 100 kHz with 16 MHz system clock
}


// Function to send data to a specific channel on DFR0971
void DFR0971_Write(uint8_t command, uint8_t data) {
    I2C0_MSA_R = (DAC_I2C_ADDRESS << 1); // Set the I2C address with write bit
    I2C0_MDR_R = command;                // Write command for specific channel
    I2C0_MCS_R = 0x03;                   // Start and Run condition
    while (I2C0_MCS_R & 0x01);           // Wait for I2C transmission

    I2C0_MDR_R = data;                   // Write the data (audio sample)
    I2C0_MCS_R = 0x05;                   // Run and Stop condition
    while (I2C0_MCS_R & 0x01);           // Wait for transmission to complete

    if (I2C0_MCS_R & 0x02) {             // Check for errors
        I2C0_MCS_R = 0x04;               // Send stop condition if error
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
    GPIO_PORTF_DATA_R ^= (0x04);

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

