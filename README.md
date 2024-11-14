**Problem statement:** Design and implement an embedded audio playback system capable of playing 8-bit .wav audio files stored in memory. The system should feature a GP8403 DAC module as the audio output interface and use I2C for this communication. Additionally, it should have a UART-based control console for user interaction.

**Discription on approach taken:** 

* The desired audio file is always stored in memory, on pressing 'P' the system starts to play audio. Anytime during operation if 'S' is pressed the system resets.
* Since the audio range is in 20KHz(max) the sampling frequency should be >40KHz; Usual sampling rates are 44.1KHz, hence using a timer for ~23usec suffices this operation.
* An interrupt is generated once the timer counts down to zero, the ISR is contained to send data to the DAC using I2C protocol.
* This repeates till the audio file stored is exhausted.
* The output of DAC is 5V, to interface it with 3.5mm audio jack it should be level shifted down to 1-2V. This is done using a buffer of gain ~0.3.
  

**Below is the Hardware architecture**

![Hardware arch](https://github.com/PrabhasDeshpande/Group14/blob/master/Hardware%20Arch.png).

**Below is the Control Flow diagram**

  ![Control Flow](https://github.com/PrabhasDeshpande/Group14/blob/master/control%20flow.png)

