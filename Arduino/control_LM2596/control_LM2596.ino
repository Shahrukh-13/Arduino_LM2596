#include <EEPROM.h>

#define VSET_GAIN_B0_ADDRESS      0
#define VSET_GAIN_B1_ADDRESS      1
#define VSET_GAIN_B2_ADDRESS      2
#define VSET_GAIN_B3_ADDRESS      3

#define VSET_OFFSET_B0_ADDRESS    4
#define VSET_OFFSET_B1_ADDRESS    5
#define VSET_OFFSET_B2_ADDRESS    6
#define VSET_OFFSET_B3_ADDRESS    7

#define MODE_ADDRESS              8

#define VSET_MAX_LSB_ADDRESS      9
#define VSET_MAX_MSB_ADDRESS      10

#define VSET_MIN_LSB_ADDRESS      11
#define VSET_MIN_MSB_ADDRESS      12

#define MIN_PWM_ADDRESS           13
#define MAX_PWM_ADDRESS           14

float vset;
String VSET;

String VSET_GAIN;
float vset_gain_float;
int32_t vset_gain_fixedpoint;

String VSET_OFFSET;
float vset_offset_float;
int32_t vset_offset_fixedpoint;

String MODE;
unsigned char mode;

String input = "";
String command = "";

String PWM;
float pwm_float;
unsigned int pwm_int;

String VSET_MAX;
float vset_max;
unsigned int vset_max_fixedpoint;

String VSET_MIN;
float vset_min;
unsigned int vset_min_fixedpoint;

String MIN_PWM;
unsigned int min_pwm_int;

String MAX_PWM;
unsigned int max_pwm_int;

void setup()
{
  Serial.begin(9600);
  pinMode(6,OUTPUT);
  vset=0;
  EEPROM_Reads();
}

void loop()
{
  Do_Serial();
  if(mode == 0)
  {
    if(vset < vset_min)
    {
      vset = vset_min;
    }
    if(vset>vset_max)
    {
      vset = vset_max;
    }
    
    pwm_float=float_map(vset,vset_min,vset_max,max_pwm_int,min_pwm_int); 
    pwm_int = (unsigned int)pwm_float*vset_gain_float + vset_offset_float;
    
    if(pwm_int>255)
    {
      pwm_int=255;
    }
    if(pwm_int<0)
    {
      pwm_int=0;
    }
    analogWrite(6,pwm_int);     
  }
  else if(mode == 1)
  {
    analogWrite(6,pwm_int);  
  }
  print_values();
}

float float_map(float x, float in_min, float in_max, float out_min, float out_max) 
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void EEPROM_Reads()
{
  if(EEPROM.read(VSET_GAIN_B0_ADDRESS) == 255 && EEPROM.read(VSET_GAIN_B1_ADDRESS) == 255 && EEPROM.read(VSET_GAIN_B2_ADDRESS) == 255 && EEPROM.read(VSET_GAIN_B3_ADDRESS) == 255)
  {
    vset_gain_float = 1;
    vset_gain_fixedpoint = vset_gain_float * 100;
    EEPROM.write(VSET_GAIN_B0_ADDRESS, (unsigned char)(vset_gain_fixedpoint & 0xFF));
    EEPROM.write(VSET_GAIN_B1_ADDRESS, (unsigned char)((vset_gain_fixedpoint >> 8) & 0xFF));
    EEPROM.write(VSET_GAIN_B2_ADDRESS, (unsigned char)((vset_gain_fixedpoint >> 16) & 0xFF));
    EEPROM.write(VSET_GAIN_B3_ADDRESS, (unsigned char)((vset_gain_fixedpoint >> 24) & 0xFF));
  }
  else 
  {
    int32_t x= (int32_t)((EEPROM.read(VSET_GAIN_B3_ADDRESS) << 24) | (EEPROM.read(VSET_GAIN_B2_ADDRESS) << 16) | (EEPROM.read(VSET_GAIN_B1_ADDRESS) << 8) | EEPROM.read(VSET_GAIN_B0_ADDRESS));
    vset_gain_float =(float)x/100;
  }

  if(EEPROM.read(VSET_OFFSET_B0_ADDRESS) == 255 && EEPROM.read(VSET_OFFSET_B1_ADDRESS) == 255 && EEPROM.read(VSET_OFFSET_B2_ADDRESS) == 255 && EEPROM.read(VSET_OFFSET_B3_ADDRESS) == 255)
  {
    vset_offset_float = 0;
    vset_offset_fixedpoint = vset_offset_float * 100;
    EEPROM.write(VSET_OFFSET_B0_ADDRESS, (unsigned char)(vset_offset_fixedpoint & 0xFF));
    EEPROM.write(VSET_OFFSET_B1_ADDRESS, (unsigned char)((vset_offset_fixedpoint >> 8) & 0xFF));
    EEPROM.write(VSET_OFFSET_B2_ADDRESS, (unsigned char)((vset_offset_fixedpoint >> 16) & 0xFF));
    EEPROM.write(VSET_OFFSET_B3_ADDRESS, (unsigned char)((vset_offset_fixedpoint >> 24) & 0xFF));
  }
  else 
  {
    int32_t y= ((EEPROM.read(VSET_OFFSET_B3_ADDRESS) << 24) | (EEPROM.read(VSET_OFFSET_B2_ADDRESS) << 16) | (EEPROM.read(VSET_OFFSET_B1_ADDRESS) << 8) | EEPROM.read(VSET_OFFSET_B0_ADDRESS));
    vset_offset_float =(float)y/100;
  }

  if(EEPROM.read(MODE_ADDRESS) == 255)
  {
    mode = 0;
    EEPROM.write(MODE_ADDRESS, mode);
  }
  else
  {
    mode = EEPROM.read(MODE_ADDRESS);
  }

  if(EEPROM.read(VSET_MAX_LSB_ADDRESS) == 255 && EEPROM.read(VSET_MAX_MSB_ADDRESS) == 255)
  {
    vset_max = 5;
    vset_max_fixedpoint = vset_max * 100;
    EEPROM.write(VSET_MAX_LSB_ADDRESS, (unsigned char)(vset_max_fixedpoint & 0xFF));
    EEPROM.write(VSET_MAX_MSB_ADDRESS, (unsigned char)((vset_max_fixedpoint >> 8) & 0xFF));
  }
  else 
  {
    unsigned int x= (unsigned int)((EEPROM.read(VSET_MAX_MSB_ADDRESS) << 8) | EEPROM.read(VSET_MAX_LSB_ADDRESS));
    vset_max =(float)x/100;
  }

  if(EEPROM.read(VSET_MIN_LSB_ADDRESS) == 255 && EEPROM.read(VSET_MIN_MSB_ADDRESS) == 255)
  {
    vset_min = 0;
    vset_min_fixedpoint = vset_min * 100;
    EEPROM.write(VSET_MIN_LSB_ADDRESS, (unsigned char)(vset_min_fixedpoint & 0xFF));
    EEPROM.write(VSET_MIN_MSB_ADDRESS, (unsigned char)((vset_min_fixedpoint >> 8) & 0xFF));
  }
  else 
  {
    unsigned int x= (unsigned int)((EEPROM.read(VSET_MIN_MSB_ADDRESS) << 8) | EEPROM.read(VSET_MIN_LSB_ADDRESS));
    vset_min =(float)x/100;
  }

  if(EEPROM.read(MIN_PWM_ADDRESS) == 255 && EEPROM.read(MAX_PWM_ADDRESS) == 255)
  {
    min_pwm_int = 0;
    max_pwm_int = 255;
    EEPROM.write(MIN_PWM_ADDRESS,min_pwm_int);
    EEPROM.write(MAX_PWM_ADDRESS,max_pwm_int);
  }
  else 
  {
    min_pwm_int = EEPROM.read(MIN_PWM_ADDRESS);
    max_pwm_int = EEPROM.read(MAX_PWM_ADDRESS);
  }
}

void Do_Serial()
{
  while (Serial.available() > 0)
  {
    input =Serial.readString();
    command =input.substring(0,input.indexOf(','));  // No newline
    Serial.println(command);
    if(command == "vset")
    {
     VSET=input.substring(input.indexOf(',')+1);
     vset=VSET.toFloat();
     Serial.println(vset);
    }

    else if(command == "vset_gain")
    {
     VSET_GAIN=input.substring(input.indexOf(',')+1);
     vset_gain_float=VSET_GAIN.toFloat();
     vset_gain_fixedpoint = vset_gain_float * 100;
     EEPROM.write(VSET_GAIN_B0_ADDRESS, (unsigned char)(vset_gain_fixedpoint & 0xFF));
     EEPROM.write(VSET_GAIN_B1_ADDRESS, (unsigned char)((vset_gain_fixedpoint >> 8) & 0xFF));
     EEPROM.write(VSET_GAIN_B2_ADDRESS, (unsigned char)((vset_gain_fixedpoint >> 16) & 0xFF));
     EEPROM.write(VSET_GAIN_B3_ADDRESS, (unsigned char)((vset_gain_fixedpoint >> 24) & 0xFF));
    }

    else if(command == "vset_offset")
    {
     VSET_OFFSET=input.substring(input.indexOf(',')+1);
     vset_offset_float=VSET_OFFSET.toFloat();
     vset_offset_fixedpoint = vset_offset_float * 100;
     EEPROM.write(VSET_OFFSET_B0_ADDRESS, (unsigned char)(vset_offset_fixedpoint & 0xFF));
     EEPROM.write(VSET_OFFSET_B1_ADDRESS, (unsigned char)((vset_offset_fixedpoint >> 8) & 0xFF));
     EEPROM.write(VSET_OFFSET_B2_ADDRESS, (unsigned char)((vset_offset_fixedpoint >> 16) & 0xFF));
     EEPROM.write(VSET_OFFSET_B3_ADDRESS, (unsigned char)((vset_offset_fixedpoint >> 24) & 0xFF));
    }

    else if(command == "mode")
    {
     MODE=input.substring(input.indexOf(',')+1);
     mode=MODE.toInt();
     EEPROM.write(MODE_ADDRESS, mode);
    }

    else if(command == "vset_max")
    {
     VSET_MAX=input.substring(input.indexOf(',')+1);
     vset_max=VSET_MAX.toFloat();
     vset_max_fixedpoint = vset_max * 100;
     EEPROM.write(VSET_MAX_LSB_ADDRESS, (unsigned char)(vset_max_fixedpoint & 0xFF));
     EEPROM.write(VSET_MAX_MSB_ADDRESS, (unsigned char)((vset_max_fixedpoint >> 8) & 0xFF));
    }

    else if(command == "vset_min")
    {
     VSET_MIN=input.substring(input.indexOf(',')+1);
     vset_min=VSET_MIN.toFloat();
     vset_min_fixedpoint = vset_min * 100;
     EEPROM.write(VSET_MIN_LSB_ADDRESS, (unsigned char)(vset_min_fixedpoint & 0xFF));
     EEPROM.write(VSET_MIN_MSB_ADDRESS, (unsigned char)((vset_min_fixedpoint >> 8) & 0xFF));
    }

    else if(command == "min_pwm")
    {
      MIN_PWM=input.substring(input.indexOf(',')+1);
      min_pwm_int=MIN_PWM.toInt();
      EEPROM.write(MIN_PWM_ADDRESS,min_pwm_int);    
    }

    else if(command == "max_pwm")
    {
      MAX_PWM=input.substring(input.indexOf(',')+1);
      max_pwm_int=MAX_PWM.toInt();    
      EEPROM.write(MAX_PWM_ADDRESS,max_pwm_int);
    }
    if(mode == 1)
    {
      if(command == "pwm")
      {
        PWM=input.substring(input.indexOf(',')+1);
        pwm_int=PWM.toInt();
      }
    }
  }
}

void print_values()
{
  Serial.print("Mode: ");
  Serial.print(mode);
  Serial.print("  Vset: ");
  Serial.print(vset);
  Serial.print("  Vset_max: ");
  Serial.print(vset_max);
  Serial.print("  Vset_min: ");
  Serial.print(vset_min);
  Serial.print("  Gain: ");
  Serial.print(vset_gain_float);
  Serial.print("  offset: ");
  Serial.print(vset_offset_float);
  Serial.print("  Min_pwm: ");
  Serial.print(min_pwm_int);
  Serial.print("  Max_pwm: ");
  Serial.print(max_pwm_int);
  Serial.print("  pwm: ");
  Serial.print(pwm_int);
  Serial.println();
}
