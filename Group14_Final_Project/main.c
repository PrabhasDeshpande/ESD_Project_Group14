#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <tm4c123gh6pm.h>
#include <wav_ip.h>
#define CLOCK  16000000
#define DAC_I2C_ADDRESS 0x58



void GPIO_PORTF_INIT(void);
void Systick_Handler(void);     // send data on interrupt to DAC: 23us
void systick(float time);
void I2C0_Init(void);
void set_slave_addr(void);
void DFR0971_Config();
void DFR0971_Write(uint8_t data, uint8_t command);

int i = 0;     // index counter


void main(void)
{

    SYSCTL_RCGCGPIO_R |= 0x20;  //enable clock to portf
    SYSCTL_RCGCI2C_R |= 0x01;                             // Enable clock for I2C0 module
    SYSCTL_RCGCGPIO_R |= 0x02;
    GPIO_PORTF_INIT();

    I2C0_Init();
    set_slave_addr();


    DFR0971_Config();   // config for 5V output

    systick(23);   // output at 22KHz


    while(1)
    {
        // do nothing

    }

}


// Function to initialize I2C
void I2C0_Init(void) {

    SYSCTL_RCGCI2C_R |= 0x01;                             // Enable clock for I2C0 module
    SYSCTL_RCGCGPIO_R |= 0x02;                            // Enable clock for Port B

    while ((SYSCTL_PRGPIO_R & 0x02) == 0);                // Wait until Port B is ready

    GPIO_PORTB_AFSEL_R |= 0x0C;                           // Enable alternate function for PB2, PB3
    GPIO_PORTB_ODR_R |= 0x08;                             // Set PB3 (SDA) as open-drain
    GPIO_PORTB_DEN_R |= 0x0C;                             // Enable digital function on PB2, PB3
    GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & ~0xFF00) | 0x3300; // Assign I2C function to PB2, PB3

    I2C0_MCR_R = 0x10;                                    // Configure I2C0 as master
    I2C0_MTPR_R = 0x07;


}

void set_slave_addr(void)
{
    I2C0_MSA_R = DAC_I2C_ADDRESS << 1 | 0;
    I2C0_MSA_R &= !(1 << 0);

}

void DFR0971_Config()
{
    DFR0971_Write(0x01,0x03);
    DFR0971_Write(0x00,0x05);
    DFR0971_Write(0x02,0x03);
}

void DFR0971_Write(uint8_t data, uint8_t command) {
    I2C0_MSA_R = (DAC_I2C_ADDRESS << 1); // Set the I2C address with write operation
    I2C0_MDR_R = data;
    I2C0_MCS_R = command;                   // Start and Run condition
    while (I2C0_MCS_R & 0x01);           // Wait for transmission to complete

    if (I2C0_MCS_R & 0x02) {             // Check for errors
        I2C0_MCS_R = 0x04;               // Send stop condition if error occurs
        return;
    }

}




void systick(float time)
{

    float val;
    val = (time*(CLOCK))/1000000;   // time in us
    NVIC_ST_RELOAD_R = val;        // reload value
    NVIC_ST_CURRENT_R  = 0x0;         // current value
    NVIC_ST_CTRL_R = 0x07;          // enable and choice of clk and enable intruppt.


}




void Systick_Handler(void)
{

    DFR0971_Write(audio[i] - 0x80 << 4 ,0x01);    // make it 16bit and also for unsigned 0x80 acts as zero value
    i++;
    if(i > (sizeof(audio) / sizeof(audio[0]))-1){
        i = 0;
        DFR0971_Write(0x00,0x05);       // stop Txing at last index
        DFR0971_Config();               // reconfigure for next cycle: as it needs it every cycle
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

