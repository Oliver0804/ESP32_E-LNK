#ifndef I2C_H
#define I2C_H   
#include <arduino.h>

//I2C���ߺ���

//I2C���  
#define FT_CMD_WR     0X70    //����
#define FT_CMD_RD     0X71    //����

//#define FT6336_Power_VCC()  digitalWrite(11,HIGH)�
//#define FT6336_Power_GND()  digitalWrite(8,LOW)

#define FT6336_SCL_L() digitalWrite(A4,LOW)
#define FT6336_SCL_H() digitalWrite(A4,HIGH)
#define FT6336_SDA_L()  digitalWrite(A5,LOW)
#define FT6336_SDA_H()  digitalWrite(A5,HIGH) 
#define FT6336_RST_L() digitalWrite(A3,LOW)
#define FT6336_RST_H() digitalWrite(A3,HIGH)
#define Is_SDA_IN  digitalRead(A5)
//mode  setting
#define SDA_IN()    pinMode(A5, INPUT)    //
#define SDA_OUT()   pinMode(A5, OUTPUT)   //

//#define GND_OUT_Mode()   pinMode(A4, OUTPUT)  
//#define VCC_OUT_Mode()   pinMode(T3, OUTPUT)  
#define RST_OUT_Mode()   pinMode(A3, OUTPUT)  
#define SCL_OUT_Mode()   pinMode(A4, OUTPUT) 

//#define FT6336_PWR() digitalWrite(T3,HIGH)  
//#define FT6336_GND() digitalWrite(T0,LOW)

void I2C_Start(void);
void I2C_End(void);
void i2c_write_byte(unsigned char temp);
unsigned char i2c_read_byte(void);
unsigned char i2c_read_keep_byte(void);


unsigned char FT6336_WR_Reg(unsigned char reg,unsigned char *buf,unsigned char len);
void FT6336_RD_Reg(unsigned char reg,unsigned char *buf,unsigned char len);
void FT6336_Init(void);

unsigned char i2c_read_addr_byte(unsigned char device_addr,unsigned char read_addr);
void i2c_read_addr_str(unsigned char device_addr,unsigned char read_addr,unsigned char read_amount,unsigned char *read_buf);
void i2c_write_addr_byte(unsigned char device_addr,unsigned char write_addr,unsigned char write_dat);
unsigned int i2c_read_addr_int(unsigned char device_addr,unsigned char read_addr);
void i2c_write_addr_str(unsigned char device_addr,unsigned char write_addr,unsigned char write_amount,unsigned char *write_buf);

#endif
