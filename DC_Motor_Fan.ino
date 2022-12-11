#include <Wire.h> //Include wire library for I2C comms
#include <LiquidCrystal.h> //Include liquid crystal library for LCD comms
//Lab 3 Arduino Code. References Elegoo open source examples and content from the arduino "wire.h" and "LiquidCrystal.h" reference guide.
//By Noah B. and Karl M.
//9 December 2022

//Initializes program variables

//RTC
int8_t secondsData = 0x00;
int8_t minutesData = 0x00;
int8_t hoursData = 0x00;
int8_t dayData = 0x00;
int8_t dateData = 0x00;
int8_t monthData = 0x00;
int8_t yearData = 0x00;

int8_t tenSec = 0x00;
int8_t sec = 0x00;
int8_t tenMin = 0x00;
int8_t Min = 0x00;
int8_t mode = 0x00;
int8_t ampm = 0x00;
int8_t tenHours = 0x00;
int8_t hours = 0x00;
String day = "No Data";
int8_t tenDate = 0x00;
int8_t date = 0x00;
int8_t tenMonth = 0x00;
int8_t month = 0x00;
String MonthStr = "No Data";
int8_t tenYear = 0x00;
int8_t year = 0x00;
int8_t numericDay = 0x00;
String ampmText = "";

int8_t Seconds = 0x00;
int8_t Minutes = 0x00;
int8_t Hours = 0x00;
int8_t Day = 0x00;
int8_t Date = 0x00;
int8_t Month = 0x00;
int8_t Year = 0x00;

//Fan
int stateCounter = 0;
unsigned long last_button_time = 0;
unsigned long button_time = 0;
boolean CCW = false;
int analogActive = 0;
int analogNot = 0;
int digitalActive = 0;
int digitalNot = 0;
String dir = "";


//**Set setEnable = 1 to set RTC time on startup.*8
int setEnable = 0;

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 39, en = 37, d4 = 28, d5 = 26, d6 = 24, d7 = 22;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {
//Establish Fan I/O Pins
pinMode(2, INPUT);
pinMode(44, OUTPUT);
pinMode(45, OUTPUT);
pinMode(4, OUTPUT);
pinMode(5, OUTPUT);
//Establish Interrupt
attachInterrupt(digitalPinToInterrupt(2), buttonDetect, RISING);

//Initiate Serial Comms
  Serial.begin(9600); //Establishes baud rate an opens serial port for communicating with COM3 port on laptop.
  Wire.begin(); //Join I2C bus


//User time inputs: Use if setting current time (setEnable == 1). These are ignored when setEnable == 0.
mode = 1;
 

tenSec = 0; //Set the tens digit of the seconds display.
sec = 0; //Set the ones digit of the seconds display
tenMin = 1; //Set the tens digit of the minutes display.
Min = 1; //Set the ones digit of the minutes dislay
tenHours = 7; //Set the tens digit of the hour display. If in 12 hour mode, bit 1 HIGH for PM and LOW for AM (e.g. for PM: 0x02 = 1 is tens place and PM bit high.)
hours = 1; //Sets the ones digit of the hours display
numericDay = 5; //Set the numeric day of the week (1-7)
tenDate = 0; //Set the tens digit of the numerical day of the month.
date = 1; //Set the ones digit of the enumerical day of the month.
tenMonth = 1; //Set the tens digit of the month.
month = 2; //Set the ones digit of the month.
tenYear = 2; //Set the ten digit of the year.
year = 2; //Set the ones digit of the year. 






//Convert user entered time into bytes that can be sent to the RTC.

Seconds = (tenSec << 4) | sec;
Minutes = (tenMin << 4) | Min;
Hours = ((mode << 6) | (tenHours << 4)) | hours;
Day = numericDay;
Date = (tenDate << 4)| date;
Month = (tenMonth << 4) | month;
Year = (tenYear << 4) | year;

  //Initialize the RTC
  //Set Current Time, if enabled, the system will use this information to initialize the RTC on startup.
  if (setEnable == 1){
      Wire.begin(); //Begin I2C communicaiton
      Wire.beginTransmission(0x68); //Transmit to device at address 0x68 (address of RTC found in reference manual)
      Wire.write(0x00);
      Wire.write(Seconds);
      Wire.write(Minutes);
      Wire.write(Hours);
      Wire.write(Day);
      Wire.write(Date);
      Wire.write(Month);
      Wire.write(Year);
      

    
  }

  Wire.endTransmission(); //End I2C communication.
  
  //Initialize the LCD 
  // set up the LCD's number of columns and rows
  lcd.begin(16, 2);
  // Print a setup message to the LCD.
  lcd.print("Initializing...");

}

void loop() {
  //Request sensor data from RTC
  Wire.beginTransmission(0x68); //Specify RTC address
  Wire.write(0x00); //Specify register to pull data from (0x00 is where clock data is stored.)
  Wire.endTransmission(); //End transmission
  Wire.requestFrom(0x68, 7); //Pull 7 bytes of data read from the RTC. (These will be six bytes starting at address 0x00, which yield time and date info.


 //Store serial data into memory.
  
    secondsData = Wire.read();
    minutesData = Wire.read();
    hoursData = Wire.read();
    dayData = Wire.read();
    dateData = Wire.read();
    monthData = Wire.read();
    yearData = Wire.read();

//Convert data into readable 8 bit binary.
    tenSec = secondsData >> 4 & ((1<<3)-1);
    sec = secondsData & ((1<<4)-1);

    tenMin = minutesData >> 4 & ((1<<3)-1);
    Min = minutesData & ((1<<4)-1);

//Handle the 12/24 hour setting and am/pm
    mode = hoursData >> 6 & ((1<<1)-1);
    if (mode == 1){
      ampm = hoursData >> 5 & ((1<<1)-1);
      if (ampm == 1){
        ampmText = "PM";
      }
      else{
        ampmText = "AM";
      }
      
      tenHours = hoursData >> 4 & ((1<<1)-1);
    }
    else{
      ampm = 0xFF;
      ampmText = "";
      tenHours = hoursData >> 5 & ((1<<2)-1);
    }
    hours = hoursData & ((1<<4)-1);

//Assign strings to the days of the week.
    if (dayData == 1){
      day = "Sunday";
    }
    else if (dayData == 2){
      day = "Monday";
    }
    else if (dayData == 3){
      day = "Tuesday";
    }
    else if (dayData == 4){
      day = "Wednesday";
    }
    else if (dayData == 5){
      day = "Thursday";
    }
    else if (dayData == 6){
      day = "Friday";  
    }
    else if (dayData == 7){
      day = "Saturday";
    }
    else{
      day = "Invalid Day!";
    }

    tenDate = dateData >> 4 & ((1<<2)-1);
    date = dateData & ((1<<4)-1);

    tenMonth = monthData >> 4 & ((1<<1)-1);
    month = monthData & ((1<<4)-1);
    
//Assign strings to the month value
    if (month == 1 && tenMonth == 0){
      MonthStr = "JAN";
    }
    else if (month == 2 && tenMonth == 0){
      MonthStr = "FEB";
    }
    else if (month == 3 && tenMonth == 0){
      MonthStr = "MAR";
    }
    else if (month == 4 && tenMonth == 0){
      MonthStr = "APR";
    }
    else if (month == 5 && tenMonth == 0){
      MonthStr = "MAY";
    }
    else if (month == 6 && tenMonth == 0){
      MonthStr = "JUN";
    }
    else if (month == 7 && tenMonth == 0){
      MonthStr = "JUL";
    }
    else if (month == 8 && tenMonth == 0){
      MonthStr = "AUG";
    }
    else if (month == 9 && tenMonth == 0){
      MonthStr = "SEPT";
    }
    else if (month == 0 && tenMonth == 1){
      MonthStr = "OCT";
    }
    else if (month == 1 && tenMonth == 1){
      MonthStr = "NOV";
    }
    else if (month == 2 && tenMonth == 1){
      MonthStr = "DEC";
    }
    else{
      MonthStr = "NA!";
    }

    tenYear = yearData >> 4 & ((1<<4)-1);
    year = yearData & ((1<<4)-1);

    //End RTC Module
    //Begin Fan Module
   //Check if CCW bit is 1, indicating that the user is requesting the fan to change directions.
    if (CCW == true){
      //Assign active vs inactive pins for CCW operation (see fan circuit diaram).
      digitalActive = 45;
      analogActive = 5;
      digitalNot = 44;
      analogNot = 4;
      dir = "R"; //Set the display text for fan direction
    }
    else{
      //Assign active vs inactive pins for CW operation
      digitalActive = 44;
      analogActive = 4;
      digitalNot = 45;
      analogNot = 5;
      dir = "F"; //Set the display text for fan operation.
    }

    //stateCounter variable controls fan speeds. Each time button is pressed, it trips a interrupt which increments stateCounter. 
    //The rogram polls stateCounter on each loop, and determines the instruction set to use based on the counter iteration.
    //The counter resets on the 4th press. On reset, it flis CCW and changes the fan direction.
    if (stateCounter == 0){
      analogWrite(analogActive, 0);
      analogWrite(analogNot, 0);
      digitalWrite(digitalActive, LOW);
      digitalWrite(digitalNot, LOW);
    }
    else if (stateCounter == 1){
      analogWrite(analogActive, 102);
      analogWrite(analogNot, 0);
      digitalWrite(digitalActive, HIGH);
      digitalWrite(digitalNot, LOW);
    }
    else if (stateCounter == 2){
      analogWrite(analogActive, 179);
      analogWrite(analogNot, 0);
      digitalWrite(digitalActive, HIGH);
      digitalWrite(digitalNot, LOW);
    }
    else if (stateCounter == 3){
      analogWrite(analogActive, 255);
      analogWrite(analogNot, 0);
      digitalWrite(digitalActive, HIGH);
      digitalWrite(digitalNot, LOW);
    }
     
    else{
      stateCounter = 0;
      analogWrite(analogActive, 0);
      analogWrite(analogNot, 0);
      digitalWrite(digitalActive, LOW);
      digitalWrite(digitalNot, LOW);
    }
    
  //Write data to LCD for display
  
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 0);
  //Begin writing data
  lcd.print(tenHours);
  lcd.print(hours);
  lcd.print(":");
  lcd.print(tenMin);
  lcd.print(Min);
  lcd.print(":");
  lcd.print(tenSec);
  lcd.print(sec);
  lcd.print(" ");
  lcd.print(ampmText);
  lcd.print(" ");
  lcd.print("FS:");
  lcd.print(stateCounter);
  lcd.setCursor(0, 1);
  lcd.print(date);
  lcd.print(" ");
  lcd.print(MonthStr);
  lcd.print(" ");
  lcd.print("20");
  lcd.print(tenYear);
  lcd.print(year);
  lcd.print("  ");
  lcd.print("FD:");
  lcd.print(dir);

  delay(100);
  }  
  //Button press interrupt
  void buttonDetect(){
    //ignore inuts made within 500ms of each other
    button_time = millis();
    if (button_time - last_button_time > 500){
      stateCounter++; //increment state counter
      if (stateCounter == 4){
        CCW = !CCW; //Flip CCW on 4th button press
      }
      last_button_time = button_time; //Update time since last press.
    }

  }
