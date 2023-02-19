#include <Wire.h>
#include <Keyboard.h>
#include <Arduino.h>
static uint8_t nunchuck_buf[6];   // array to store nunchuck data,


// initialize the I2C system, join the I2C bus,
// and tell the nunchuck we're talking to it
static void nunchuck_init()
{
  Wire.begin();                // join i2c bus as master
  Wire.beginTransmission(0x52);// transmit to device 0x52
  Wire.write((uint8_t)0x40);// sends memory address
  Wire.write((uint8_t)0x00);// sends sent a zero.
  Wire.endTransmission();// stop transmitting
}
// Send a request for data to the nunchuck
// was "send_zero()"
static void nunchuck_send_request()
{
 Wire.beginTransmission(0x52);// transmit to device 0x52
 Wire.write((uint8_t)0x00);// sends one byte
 Wire.endTransmission();// stop transmitting
}
// Encode data to format that most wiimote drivers except
// only needed if you use one of the regular wiimote drivers
static char nunchuk_decode_byte (char x)
{
 x = (x ^ 0x17) + 0x17;
 return x;
}
// Receive data back from the nunchuck,
// returns 1 on successful read. returns 0 on failure
static int nunchuck_get_data()
{
 int cnt = 0;
 Wire.requestFrom (0x52, 6);// request data from nunchuck
 while (Wire.available ()) {
   // receive byte as an integer
   nunchuck_buf[cnt] = nunchuk_decode_byte( Wire.read() );
   cnt++;
 }
 nunchuck_send_request();  // send request for next data payload
 // If we recieved the 6 bytes, then go print them
 if (cnt >= 5) {
   return 1;   // success
 }
 return 0; //failure
}
 
// returns zbutton state: 1=pressed, 0=notpressed
static int nunchuck_zbutton()
{
 return ((nunchuck_buf[5] >> 0) & 1) ? 0 : 1;  // voodoo
}
// returns zbutton state: 1=pressed, 0=notpressed
static int nunchuck_cbutton()
{
 return ((nunchuck_buf[5] >> 1) & 1) ? 0 : 1;  // voodoo
}
// returns value of x-axis joystick
static int nunchuck_joyx()
{
 return nunchuck_buf[0];
}
// returns value of y-axis joystick
static int nunchuck_joyy()
{
 return nunchuck_buf[1];
}
// returns value of x-axis accelerometer
static int nunchuck_accelx()
{
 return nunchuck_buf[2];   // FIXME: this leaves out 2-bits of the data
}
// returns value of y-axis accelerometer
static int nunchuck_accely()
{
 return nunchuck_buf[3];   // FIXME: this leaves out 2-bits of the data
}
// returns value of z-axis accelerometer
static int nunchuck_accelz()
{
 return nunchuck_buf[4];   // FIXME: this leaves out 2-bits of the data
}
int loop_cnt = 0;
byte joyx, joyy, zbut, cbut, accx, accy, accz;

void _print() {
 Serial.print("Z Button:  ");
 Serial.print(zbut);
 Serial.print("\tC Button:  ");
 Serial.print(cbut);
 Serial.print("\tX Joy:  ");
 Serial.print(joyx);
 Serial.print("\tY Joy:  ");
 Serial.print(joyy);
 Serial.print("\tX Accel:  ");
 Serial.print(accx);
 Serial.print("\tY Accel:  ");
 Serial.print(accy);
 Serial.print("\tZ Accel:  ");
 Serial.print(accz);
 Serial.println();
}
void setup() {
 Serial.begin(9600);
 pinMode(10, INPUT_PULLUP);
 Serial.println("Before nuninit");
 nunchuck_init(); // send the initilization handshake
 Serial.println("Wii Nunchuck Ready");
 Keyboard.begin();
 Serial.println("keyboard ready");
}

bool wActive = false;
bool wRunActive = false;
bool sActive = false;
bool aActive = false;
bool dActive = false;
bool fActive = false;
bool cActive = false;
bool bActive = false;
bool eActive = false;

int period = 10;
unsigned long time_now = 0;

void loop() {
 if(millis() >= time_now + period){
   time_now += period;
    
   loop_cnt = 0;
   nunchuck_get_data();
   zbut = nunchuck_zbutton();  //  0 - 1
   cbut = nunchuck_cbutton();  //  0 - 1
   joyx = nunchuck_joyx();     //  15 - 221
   joyy = nunchuck_joyy();     //  29 - 229
   accx = nunchuck_accelx();   //  70 - 182
   accy = nunchuck_accely();   //  65 - 173
   accz = nunchuck_accelz();   //  0 - 255
   //_print();
    if(digitalRead(10) == LOW && !cActive){
        Keyboard.press(KEY_LEFT_CTRL);
        cActive = true;
    }
    if(digitalRead(10) == HIGH && cActive){
      Keyboard.release(KEY_LEFT_CTRL); 
        cActive = false;
    }

    // //turn wrist clockwise == Crouch
    // if (accx > 160 && !cActive){
    //   Keyboard.press(KEY_LEFT_CTRL);
    //   cActive = true;
    // }
    // if (joyy <159 && cActive ){
    //     Keyboard.release(KEY_LEFT_CTRL); 
    //     cActive = false;
    // }



    // if (accy > 150){
    //   Keyboard.print('w');
    // }



    //JOY HALF forward is WALK, JOY FULL forward is RUN
    //if crouching do not activate RUN
    if (joyy > 155 && !wActive){
        Keyboard.press('w');
        wActive = true;
    }
    if (joyy < 150 && wActive){
        Keyboard.release('w');
        wActive = false;
    }
    if (joyy > 200 && !wRunActive && !cActive){
        Keyboard.press(KEY_LEFT_SHIFT);
        wRunActive = true;
    }
    if (joyy <195 && wRunActive ){
        Keyboard.release(KEY_LEFT_SHIFT); 
        wRunActive = false;
    }

    //wrist up == JUMP & mantle
    if (accy < 95) {
      Keyboard.press(' ');
      delay(150);
      Keyboard.release(' ');
      delay(50);
      Keyboard.press(' ');
      delay(150);
      Keyboard.release(' ');
    }

    //backward on
    if (joyy < 100 && !sActive){
        Keyboard.press('s');
        sActive = true;
    }
    //release backward..
    if (joyy > 101 && sActive){
      Keyboard.release('s');
      sActive = false;
    }

    //right
    if (joyx > 150 && !dActive){
        Keyboard.press('d');
        dActive = true;
    }

    //left
    if (joyx < 115 && !aActive){
        Keyboard.press('a');
        aActive = true;
    }

    //if middle position deactivate left && right
    if (joyx < 145 && joyx > 115 && (dActive || aActive) ){
        Keyboard.release('d');
        Keyboard.release('a');
        dActive = false;
        aActive = false;
    }

    //big nunchuck button, USE button
    if (zbut == 1 && !eActive){
      Keyboard.press('e');
      eActive = true;
    }
    if (zbut == 0 && eActive){
      Keyboard.release('e');
      eActive = false;
    }


    //small nunchuck button, does reload
    if (cbut == 1){
        Keyboard.print('r');
    }
   
 }

}