/* MPU9150 Basic Example Code
 by: Kris Winer
 date: April 1, 2014
 license: Beerware - Use this code however you'd like. If you 
 find it useful you can buy me a beer some time.
 
 Demonstrate basic MPU-9150 functionality including parameterizing the register addresses, initializing the sensor, 
 getting properly scaled accelerometer, gyroscope, and magnetometer data out. Added display functions to 
 allow display to on breadboard monitor. Addition of 9 DoF sensor fusion using open source Madgwick and 
 Mahony filter algorithms. Sketch runs on the 3.3 V 8 MHz Pro Mini and the Teensy 3.1.
 
 SDA and SCL should have external pull-up resistors (to 3.3V).
 10k resistors are on the EMSENSR-9250 breakout board.
 
 Hardware setup:
 MPU9150 Breakout --------- Arduino
 VDD ---------------------- 3.3V
 VDDI --------------------- 3.3V
 SDA ----------------------- A4
 SCL ----------------------- A5
 GND ---------------------- GND
 
 Note: The MPU9150 is an I2C sensor and uses the Arduino Wire library. 
 Because the sensor is not 5V tolerant, we are using a 3.3 V 8 MHz Pro Mini or a 3.3 V Teensy 3.1.
 We have disabled the internal pull-ups used by the Wire library in the Wire.h/twi.c utility file.
 We are also using the 400 kHz fast I2C mode by setting the TWI_FREQ  to 400000L /twi.h utility file.
 */
 
//#include "ST_F401_84MHZ.h" 
//F401_init84 myinit(0);
#include "mbed.h"
#include "MPU9150.h"
#include "N5110.h"

// Using NOKIA 5110 monochrome 84 x 48 pixel display
// pin 9 - Serial clock out (SCLK)
// pin 8 - Serial data out (DIN)
// pin 7 - Data/Command select (D/C)
// pin 5 - LCD chip select (CS)
// pin 6 - LCD reset (RST)
//Adafruit_PCD8544 display = Adafruit_PCD8544(9, 8, 7, 5, 6);

float sum = 0;
uint32_t sumCount = 0, mcount = 0;
char buffer[14];

   MPU9150 MPU9150;
   
   Timer t;

   Serial pc(USBTX, USBRX); // tx, rx

   //        VCC,   SCE,  RST,  D/C,  MOSI,S CLK, LED
   N5110 lcd(PA_8, PB_10, PA_9, PA_6, PA_7, PA_5, PC_7);
   

        
int main()
{
  pc.baud(9600);  

  //Set up I2C
  i2c.frequency(400000);  // use fast (400 kHz) I2C  
  
  pc.printf("CPU SystemCoreClock is %d Hz\r\n", SystemCoreClock);   
  
  t.start();        
  
  lcd.init();
//  lcd.setBrightness(0.05);
  
    
  // Read the WHO_AM_I register, this is a good test of communication
  uint8_t whoami = MPU9150.readByte(MPU9150_ADDRESS, WHO_AM_I_MPU9150);  // Read WHO_AM_I register for MPU-9250
  pc.printf("I AM 0x%x\n\r", whoami); pc.printf("I SHOULD BE 0x68\n\r");
  
  if (whoami == 0x68) // WHO_AM_I should be 0x68
  {  
    pc.printf("MPU9150 WHO_AM_I is 0x%x\n\r", whoami);
    pc.printf("MPU9150 is online...\n\r");
    lcd.clear();
    lcd.printString("MPU9150 is", 0, 0);
    sprintf(buffer, "0x%x", whoami);
    lcd.printString(buffer, 0, 1);
    lcd.printString("shoud be 0x68", 0, 2);  
    wait(1);
    
    MPU9150.MPU9150SelfTest(SelfTest);
    pc.printf("x-axis self test: acceleration trim within %f % of factory value\n\r", SelfTest[0]);
    pc.printf("y-axis self test: acceleration trim within %f % of factory value\n\r", SelfTest[1]);
    pc.printf("z-axis self test: acceleration trim within %f % of factory value\n\r", SelfTest[2]);
    pc.printf("x-axis self test: gyration trim within %f % of factory value\n\r", SelfTest[3]);
    pc.printf("y-axis self test: gyration trim within %f % of factory value\n\r", SelfTest[4]);
    pc.printf("z-axis self test: gyration trim within %f % of factory value\n\r", SelfTest[5]);
    wait(1);
    MPU9150.resetMPU9150(); // Reset registers to default in preparation for device calibration
    MPU9150.calibrateMPU9150(gyroBias, accelBias); // Calibrate gyro and accelerometers, load biases in bias registers  
    pc.printf("x gyro bias = %f\n\r", gyroBias[0]);
    pc.printf("y gyro bias = %f\n\r", gyroBias[1]);
    pc.printf("z gyro bias = %f\n\r", gyroBias[2]);
    pc.printf("x accel bias = %f\n\r", accelBias[0]);
    pc.printf("y accel bias = %f\n\r", accelBias[1]);
    pc.printf("z accel bias = %f\n\r", accelBias[2]);
    wait(1);
    MPU9150.initMPU9150(); 
    pc.printf("MPU9150 initialized for active data mode....\n\r"); // Initialize device for active mode read of acclerometer, gyroscope, and temperature
    MPU9150.initAK8975A(magCalibration);
    pc.printf("AK8975 initialized for active data mode....\n\r"); // Initialize device for active mode read of magnetometer
   }
   else
   {
    pc.printf("Could not connect to MPU9150: \n\r");
    pc.printf("%#x \n",  whoami);
 
    lcd.clear();
    lcd.printString("MPU9150", 0, 0);
    lcd.printString("no connection", 0, 1);
    sprintf(buffer, "WHO_AM_I 0x%x", whoami);
    lcd.printString(buffer, 0, 2); 
 
    while(1) ; // Loop forever if communication doesn't happen
    }

    uint8_t MagRate = 10; // set magnetometer read rate in Hz; 10 to 100 (max) Hz are reasonable values
    MPU9150.getAres(); // Get accelerometer sensitivity
    MPU9150.getGres(); // Get gyro sensitivity
    mRes = 10.*1229./4096.; // Conversion from 1229 microTesla full scale (4096) to 12.29 Gauss full scale
    // So far, magnetometer bias is calculated and subtracted here manually, should construct an algorithm to do it automatically
    // like the gyro and accelerometer biases
    magbias[0] = -5.;   // User environmental x-axis correction in milliGauss
    magbias[1] = -95.;  // User environmental y-axis correction in milliGauss
    magbias[2] = -260.; // User environmental z-axis correction in milliGauss
 

 while(1) {
  
  // If intPin goes high, all data registers have new data
  if(MPU9150.readByte(MPU9150_ADDRESS, INT_STATUS) & 0x01) {  // On interrupt, check if data ready interrupt

    MPU9150.readAccelData(accelCount);  // Read the x/y/z adc values   
    // Now we'll calculate the accleration value into actual g's
    ax = (float)accelCount[0]*aRes; // - accelBias[0];  // get actual g value, this depends on scale being set
    ay = (float)accelCount[1]*aRes; // - accelBias[1];   
    az = (float)accelCount[2]*aRes; // - accelBias[2];  
   
    MPU9150.readGyroData(gyroCount);  // Read the x/y/z adc values
    // Calculate the gyro value into actual degrees per second
    gx = (float)gyroCount[0]*gRes; // - gyroBias[0];  // get actual gyro value, this depends on scale being set
    gy = (float)gyroCount[1]*gRes; // - gyroBias[1];  
    gz = (float)gyroCount[2]*gRes; // - gyroBias[2];   
  
    mcount++;
    if (mcount > 200/MagRate) {  // this is a poor man's way of setting the magnetometer read rate (see below) 
    MPU9150.readMagData(magCount);  // Read the x/y/z adc values
    // Calculate the magnetometer values in milliGauss
    // Include factory calibration per data sheet and user environmental corrections
    mx = (float)magCount[0]*mRes*magCalibration[0] - magbias[0];  // get actual magnetometer value, this depends on scale being set
    my = (float)magCount[1]*mRes*magCalibration[1] - magbias[1];  
    mz = (float)magCount[2]*mRes*magCalibration[2] - magbias[2];   
    mcount = 0;
    }
  }
   
    Now = t.read_us();
    deltat = (float)((Now - lastUpdate)/1000000.0f) ; // set integration time by time elapsed since last filter update
    lastUpdate = Now;
    
    sum += deltat;
    sumCount++;
    
//    if(lastUpdate - firstUpdate > 10000000.0f) {
//     beta = 0.04;  // decrease filter gain after stabilized
//     zeta = 0.015; // increasey bias drift gain after stabilized
 //   }
    
   // Pass gyro rate as rad/s
//  MPU9150.MadgwickQuaternionUpdate(ax, ay, az, gx*PI/180.0f, gy*PI/180.0f, gz*PI/180.0f,  my,  mx, mz);
  MPU9150.MahonyQuaternionUpdate(ax, ay, az, gx*PI/180.0f, gy*PI/180.0f, gz*PI/180.0f, my, mx, mz);

    // Serial print and/or display at 0.5 s rate independent of data rates
    delt_t = t.read_ms() - count;
    if (delt_t > 500) { // update LCD once per half-second independent of read rate

    pc.printf("ax = %f", 1000*ax); 
    pc.printf(" ay = %f", 1000*ay); 
    pc.printf(" az = %f  mg\n\r", 1000*az); 

    pc.printf("gx = %f", gx); 
    pc.printf(" gy = %f", gy); 
    pc.printf(" gz = %f  deg/s\n\r", gz); 
    
    pc.printf("gx = %f", mx); 
    pc.printf(" gy = %f", my); 
    pc.printf(" gz = %f  mG\n\r", mz); 
    
    tempCount = MPU9150.readTempData();  // Read the adc values
    temperature = ((float) tempCount) / 340.0f + 36.53f; // Temperature in degrees Centigrade
    pc.printf(" temperature = %f  C\n\r", temperature); 
    
    pc.printf("q0 = %f\n\r", q[0]);
    pc.printf("q1 = %f\n\r", q[1]);
    pc.printf("q2 = %f\n\r", q[2]);
    pc.printf("q3 = %f\n\r", q[3]);      
    
/*    lcd.clear();
    lcd.printString("MPU9150", 0, 0);
    lcd.printString("x   y   z", 0, 1);
    sprintf(buffer, "%d %d %d mg", (int)(1000.0f*ax), (int)(1000.0f*ay), (int)(1000.0f*az));
    lcd.printString(buffer, 0, 2);
    sprintf(buffer, "%d %d %d deg/s", (int)gx, (int)gy, (int)gz);
    lcd.printString(buffer, 0, 3);
    sprintf(buffer, "%d %d %d mG", (int)mx, (int)my, (int)mz);
    lcd.printString(buffer, 0, 4); 
 */  
  // Define output variables from updated quaternion---these are Tait-Bryan angles, commonly used in aircraft orientation.
  // In this coordinate system, the positive z-axis is down toward Earth. 
  // Yaw is the angle between Sensor x-axis and Earth magnetic North (or true North if corrected for local declination, looking down on the sensor positive yaw is counterclockwise.
  // Pitch is angle between sensor x-axis and Earth ground plane, toward the Earth is positive, up toward the sky is negative.
  // Roll is angle between sensor y-axis and Earth ground plane, y-axis up is positive roll.
  // These arise from the definition of the homogeneous rotation matrix constructed from quaternions.
  // Tait-Bryan angles as well as Euler angles are non-commutative; that is, the get the correct orientation the rotations must be
  // applied in the correct order which for this configuration is yaw, pitch, and then roll.
  // For more see http://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles which has additional links.
    yaw   = atan2(2.0f * (q[1] * q[2] + q[0] * q[3]), q[0] * q[0] + q[1] * q[1] - q[2] * q[2] - q[3] * q[3]);   
    pitch = -asin(2.0f * (q[1] * q[3] - q[0] * q[2]));
    roll  = atan2(2.0f * (q[0] * q[1] + q[2] * q[3]), q[0] * q[0] - q[1] * q[1] - q[2] * q[2] + q[3] * q[3]);
    pitch *= 180.0f / PI;
    yaw   *= 180.0f / PI; 
    yaw   -= 13.8f; // Declination at Danville, California is 13 degrees 48 minutes and 47 seconds on 2014-04-04
    roll  *= 180.0f / PI;

    pc.printf("Yaw, Pitch, Roll: %f %f %f\n\r", yaw, pitch, roll);
    pc.printf("average rate = %f\n\r", (float) sumCount/sum);
//    sprintf(buffer, "YPR: %f %f %f", yaw, pitch, roll);
//    lcd.printString(buffer, 0, 4);
//    sprintf(buffer, "rate = %f", (float) sumCount/sum);
//    lcd.printString(buffer, 0, 5);
    
    myled= !myled;
    count = t.read_ms(); 

    if(count > 1<<21) {
        t.start(); // start the timer over again if ~30 minutes has passed
        count = 0;
        deltat= 0;
        lastUpdate = t.read_us();
    }
    sum = 0;
    sumCount = 0; 
}
}
 
 }
