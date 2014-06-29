STM32F401
=========

STM32F401 Nucleo 6- and 9-axis sensor fusion examples

![](
https://mbed.org/media/thumbs/7f/05/7f052385386a0bc45f61f4c0e18a356c.png)


I have started to port Arduino sketches originally built to perform 6- and 9-axis sensor fusion using a variety of motion sensors including the MPU-6050 and MPU-9x50 from Invensense, the LSM9DS0 from ST Microelectronics, and various mash-ups of different sensors like the GY-80. These Arduino sketches currently run on the Arduino 3.3V, 8 MHz Pro Mini, the Arduino 5V,  16 MHz UNO, and the 3.3V 24-96 MHz Teensy 3.1 running teensyduino. These are all great platforms from a cost and convenience veiwpoint, but I want maximum performance and ultra-low power consumption for the end application which is portable motion sensing (orientation and position in space and time) and device control (controlling signals to remote devices based on the outputs of the motion sensors).

Here, the STM32 M4 Cortex family of microprocessors really shines, especially the STM32F40x. I am using the STM32F401 Nucleo board and compiling the programs using mbed.org's compiler. While it took a little while to get used to the differences between mbed and the Arduino IDE, and some of the functionality of the Arduino is missing or the syntax radically different, mbed's compiler makes translating the Arduino sketches straightforward and easier than mastering the Keil MDK toolchain even with STM32CubeMX to help. And, ST is basically giving these boards away (you can buy the STM32F401 Nucleo board for $10 at Mouser.com), literally in the case of the STM32F401 Discovery board, which I was given when I took ST's one-day "how to use the STM32F0x family of microcontrollers" class. The class is very much worth taking but one day is not enough time to learn how to use this chip well. The good news is that ST is determined to make this family of processors usable and they have essentially made the board design and the software open source so anyone can figure out how to build their own custom board with this processor. This is what lies ahead for me; in the meantime, I am just trying to learn how to use it with code and sensors I already know well. That is the purpose of this repository, to share what I find out with you.

You can see a more general discussion comparing various motion sensors and how well sensor fusion works on various microprocessor platforms ![here.](https://github.com/kriswiner/MPU-6050/wiki/Affordable-9-DoF-Sensor-Fusion)

And if you want an on-the-scene report from the June 11-12, 2014 Invensense Developer's Conference, see ![here.](https://github.com/kriswiner/MPU-6050/wiki/2014-Invensense-Developer%27s-Conference)
