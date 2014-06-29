Basic code compiled on mbed.org's compiler for the MPU-9150 9-axis motion sensor running on an STM32F401RE Nucleo board at 84 MHz.
Sensor fusion is performed using the open-source Madgwick and Mahony sensor fusion filters, which can achieve ~5000 Hz filter update rates on this platform.
