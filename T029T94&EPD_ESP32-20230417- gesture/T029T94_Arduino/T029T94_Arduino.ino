#include "i2c.h"
#include "FT6336.h"
//EPD
#include <SPI.h>
#include"Ap_29demo.h"
//IO settings

int IRQ_Pin = A0; 
int  touch_flag;
#define Is_INT_IN digitalRead(IRQ_Pin)

//IO settings
int BUSY_Pin = A14; 
int RES_Pin = A15; 
int DC_Pin = A16; 
int CS_Pin = A17; 
//SCLK--GPIO23
//MOSI---GPIO18
unsigned char oldData[768];
#define EPD_W21_CS_0 digitalWrite(CS_Pin,LOW)
#define EPD_W21_CS_1 digitalWrite(CS_Pin,HIGH)
#define EPD_W21_DC_0  digitalWrite(DC_Pin,LOW)
#define EPD_W21_DC_1  digitalWrite(DC_Pin,HIGH)
#define EPD_W21_RST_0 digitalWrite(RES_Pin,LOW)
#define EPD_W21_RST_1 digitalWrite(RES_Pin,HIGH)
#define isEPD_W21_BUSY digitalRead(BUSY_Pin)
////////FUNCTION//////
    
void SPI_Write(unsigned char value);
void Epaper_Write_Command(unsigned char cmd);
void Epaper_Write_Data(unsigned char data);
//EPD

void Epaper_HW_SW_RESET(void);
void EPD_HW_Init(void); //Electronic paper initialization
void EPD_Update(void);

void EPD_Part_Init(void);//Local refresh initialization
void EPD_Part_Update(void); 

void EPD_WhiteScreen_White(void);
void EPD_DeepSleep(void);
//Display 
void EPD_WhiteScreen_ALL(const unsigned char *datas);
void EPD_SetRAMValue_BaseMap(const unsigned char * datas);
void EPD_Dis_Part(unsigned int x_start,unsigned int y_start,const unsigned char * datas,unsigned int PART_COLUMN,unsigned int PART_LINE);
void EPD_Dis_Part_myself(unsigned int x_startA,unsigned int y_startA,const unsigned char * datasA,
                         unsigned int x_startB,unsigned int y_startB,const unsigned char * datasB,
                         unsigned int x_startC,unsigned int y_startC,const unsigned char * datasC,
                         unsigned int x_startD,unsigned int y_startD,const unsigned char * datasD,
                         unsigned int x_startE,unsigned int y_startE,const unsigned char * datasE,
                         unsigned int PART_COLUMN,unsigned int PART_LINE
                        );
//Display canvas function
void EPD_HW_Init_GUI(void); //EPD init GUI
void EPD_Display(unsigned char *Image); 
void EPD_HW_Init_P(void);
void EPD_Standby(void);

void EPD_HW_Init_Fast(void);
void EPD_WhiteScreen_ALL_Fast(const unsigned char *datas);
void EPD_WhiteScreen_ALL_Fast_Clean(void);                         
void EPD_SetRAMValue_BaseMap_Fast( const unsigned char * datas);
void EPD_Dis_Part_myself11(unsigned int x_startA,unsigned int y_startA,const unsigned char * datasA,
                         unsigned int x_startB,unsigned int y_startB,const unsigned char * datasB,
                         unsigned int PART_COLUMN,unsigned int PART_LINE
                        );  


void setup() {
  Serial.begin(115200);
  pinMode(A0, INPUT_PULLUP); 
  //epd
   pinMode(BUSY_Pin, INPUT); 
   pinMode(RES_Pin, OUTPUT);  
   pinMode(DC_Pin, OUTPUT);    
   pinMode(CS_Pin, OUTPUT);    
   //SPI
   SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0)); 
   SPI.begin ();  
  
}
//Tips//
/*When the electronic paper is refreshed in full screen, the picture flicker is a normal phenomenon, and the main function is to clear the display afterimage in the previous picture.
  When the local refresh is performed, the screen does not flash.*/
/*When you need to transplant the driver, you only need to change the corresponding IO. The BUSY pin is the input mode and the others are the output mode. */
int EpdNum;
int EpdFlag;
int num1=0;
void loop() {
 
  unsigned char i,temp1;
  unsigned char buf1[15];
  Serial.print("System is run OK\r\n"); //FT6336U
  FT6336_Init();  //Touch init  


  EPD_HW_Init_Fast(); //EPD init Fast
  EPD_WhiteScreen_ALL_Fast_Clean(); 


  while(1) 
  { 
   if(Is_INT_IN==0) //Touch is OK
    {  
      TPR_Structure.TouchSta |= TP_COORD_UD;       
      if(TPR_Structure.TouchSta &TP_COORD_UD)   
        {
          TPR_Structure.TouchSta=0;  //
            FT6336_Scan();  
            if(EpdFlag==0)
            {
                FT6336_RD_Reg(0x01,buf1,1);//Read gesture
                Serial.println(buf1[0],HEX); //gesture           
                if((buf1[0]==0x10||buf1[0]==0x18)&&(EpdNum==0))
                  {
                    num1++;
                     if(num1==1)
                     {
                        EPD_HW_Init_Fast(); //EPD init Fast
                        EPD_WhiteScreen_ALL_Fast(gImage_1);//EPD_picture1
                     }
                     if(num1==2)
                     {
                        EPD_HW_Init_Fast(); //EPD init Fast
                        EPD_WhiteScreen_ALL_Fast(gImage_2);//EPD_picture1
                     }
                     if(num1==3)
                     {
                        num1=3;
                        EpdFlag=1;
                        EPD_HW_Init(); //EPD init Fast
                        EPD_SetRAMValue_BaseMap(gImage_basemap1);//EPD_picture1   
                        EPD_Dis_Part_myself11(32,64*2+24,Num[EpdNum],
                                            32,64*3+24,Num[2],64,96);   
                     }
                    }
              }
                
            /*
            UP：0x10
            DOWN：0x18
            LEFT：0x1C
            RIGHT：0x14
            ZOOM IN：0x48
            ZOOM OUT：0x49
             */

    if(EpdFlag==1)       
    {
      switch(touch_count)
      {
          case 1:
              if((TPR_Structure.x[0]!=0)&&(TPR_Structure.y[0]!=0)
                &&(TPR_Structure.x[0]>0)&&(TPR_Structure.x[0]<128) //
                &&(TPR_Structure.y[0]>0)&&(TPR_Structure.y[0]<100))//
              {
                  EpdNum++;
                  if(EpdNum==10)
                    EpdNum=0;
                   printf("%d\r\n",EpdNum); //FT6336U  
                  EPD_Dis_Part_myself11(32,64*2+24,Num[EpdNum],
                                        32,64*3+24,Num[2],64,96);                         
                     
              
                }
              else if((TPR_Structure.x[0]!=0)&&(TPR_Structure.y[0]!=0)
                &&(TPR_Structure.x[0]>0)&&(TPR_Structure.x[0]<128) //
                &&(TPR_Structure.y[0]>160)&&(TPR_Structure.y[0]<296))//
              {   
                  if(EpdNum!=0)
                    EpdNum--;
                  if(EpdNum==0)
                    EpdNum=9;            
                  printf("%d\r\n",EpdNum);  //FT6336U

                EPD_Dis_Part_myself11(32,64*2+24,Num[EpdNum],
                    32,64*3+24,Num[2],64,96);    
                  
                } 
              break;
          case 2:
              if((TPR_Structure.x[0]!=0)&&(TPR_Structure.y[0]!=0)
                &&(TPR_Structure.x[1]!=0)&&(TPR_Structure.y[1]!=0)
                &&(TPR_Structure.x[0]<128)&&(TPR_Structure.y[0]<296)
                &&(TPR_Structure.x[1]<128)&&(TPR_Structure.y[1]<296))//
              {
               printf(":%d\r\n",touch_count); //FT6336U
               printf("x0:%d,y0:%d\r\n",TPR_Structure.x[0],TPR_Structure.y[0]);
               printf("x1:%d,y1:%d\r\n",TPR_Structure.x[1],TPR_Structure.y[1]);
              }
              break;
          
          default:
            break;
            
          }
        for(i=0;i<2;i++)
        {
          TPR_Structure.x[i]=0;
          TPR_Structure.y[i]=0;
        }
              
      }
    }
  }
   
}
}




















///////////////////EXTERNAL FUNCTION////////////////////////////////////////////////////////////////////////
//////////////////////SPI///////////////////////////////////
void SPI_Write(unsigned char value)                                    
{                                                           
  SPI.transfer(value);
}

void Epaper_Write_Command(unsigned char cmd)
{
  EPD_W21_CS_1;
  EPD_W21_CS_0;
  EPD_W21_DC_0;  // D/C#   0:command  1:data

  SPI_Write(cmd);
  EPD_W21_CS_1;
}

void Epaper_Write_Data(unsigned char data)
{
  EPD_W21_CS_1;
  EPD_W21_CS_0;
  EPD_W21_DC_1;  // D/C#   0:command  1:data

  SPI_Write(data);
  EPD_W21_CS_1;
}



  
/////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//SSD1680
void EPD_HW_Init(void)
{
  EPD_W21_RST_0;  // Module reset   
  delay(10);//At least 10ms delay 
  EPD_W21_RST_1;
  delay(10); //At least 10ms delay 
  
  Epaper_READBUSY();   
  Epaper_Write_Command(0x12);  //SWRESET
  Epaper_READBUSY();   
    
  Epaper_Write_Command(0x01); //Driver output control      
  Epaper_Write_Data(0x27);
  Epaper_Write_Data(0x01);
  Epaper_Write_Data(0x00);

  Epaper_Write_Command(0x11); //data entry mode       
  Epaper_Write_Data(0x01);

  Epaper_Write_Command(0x44); //set Ram-X address start/end position   
  Epaper_Write_Data(0x00);
  Epaper_Write_Data(0x0F);    //0x0F-->(15+1)*8=128

  Epaper_Write_Command(0x45); //set Ram-Y address start/end position          
  Epaper_Write_Data(0x27);   //0x0127-->(295+1)=296
  Epaper_Write_Data(0x01);
  Epaper_Write_Data(0x00);
  Epaper_Write_Data(0x00); 

  Epaper_Write_Command(0x3C); //BorderWavefrom
  Epaper_Write_Data(0x05);  
      
  Epaper_Write_Command(0x21); //  Display update control
  Epaper_Write_Data(0x00);    
  Epaper_Write_Data(0x80);          
      
  Epaper_Write_Command(0x18); //Read built-in temperature sensor
  Epaper_Write_Data(0x80);  

  Epaper_Write_Command(0x4E);   // set RAM x address count to 0;
  Epaper_Write_Data(0x00);
  Epaper_Write_Command(0x4F);   // set RAM y address count to 0X199;    
  Epaper_Write_Data(0x27);
  Epaper_Write_Data(0x01);
  Epaper_READBUSY();
  
}
void EPD_HW_Init_Fast(void)
{
  EPD_W21_RST_0;  // Module reset   
  delay(10);//At least 10ms delay 
  EPD_W21_RST_1;
  delay(10); //At least 10ms delay 
  
  Epaper_Write_Command(0x12);  //SWRESET
  Epaper_READBUSY();   
  
  Epaper_Write_Command(0x18); //Read built-in temperature sensor
  Epaper_Write_Data(0x80);  
      
  Epaper_Write_Command(0x22); // Load temperature value
  Epaper_Write_Data(0xB1);    
  Epaper_Write_Command(0x20); 
  Epaper_READBUSY();   

  Epaper_Write_Command(0x1A); // Write to temperature register
  Epaper_Write_Data(0x64);    
  Epaper_Write_Data(0x00);  
            
  Epaper_Write_Command(0x22); // Load temperature value
  Epaper_Write_Data(0x91);    
  Epaper_Write_Command(0x20); 
  Epaper_READBUSY();   
}

/////////////////////////////////////////////////////////////////////////////////////////
void EPD_Update(void)
{   
  Epaper_Write_Command(0x22); //Display Update Control
  Epaper_Write_Data(0xF7);   
  Epaper_Write_Command(0x20); //Activate Display Update Sequence
  Epaper_READBUSY();  

}
void EPD_Update_Fast(void)
{   
  Epaper_Write_Command(0x22); //Display Update Control
  Epaper_Write_Data(0xC7);   
  Epaper_Write_Command(0x20); //Activate Display Update Sequence
  Epaper_READBUSY();   

}
void EPD_Part_Update(void)
{
  Epaper_Write_Command(0x22);//Display Update Control 
  Epaper_Write_Data(0xFF);   
  Epaper_Write_Command(0x20); //Activate Display Update Sequence
  Epaper_READBUSY();      
}
void EPD_DeepSleep(void)
{  
  Epaper_Write_Command(0x10); //enter deep sleep
  Epaper_Write_Data(0x01); 
  delay(100);
}
void Epaper_READBUSY(void)
{ 
  while(1)
  {   //=1 BUSY
     if(isEPD_W21_BUSY==0) break;
  }  
}
//////////////////////////////All screen update////////////////////////////////////////////
void EPD_WhiteScreen_ALL(const unsigned char * datas)
{
   unsigned int i;
    Epaper_Write_Command(0x24);   //write RAM for black(0)/white (1)
   for(i=0;i<4736;i++)
   {               
     Epaper_Write_Data(datas[i]);
   }
   EPD_Update();   
}
void EPD_WhiteScreen_ALL_Fast(const unsigned char *datas)
{
   unsigned int i;  
  Epaper_Write_Command(0x24);   //write RAM for black(0)/white (1)
   for(i=0;i<4736;i++)
   {               
      Epaper_Write_Data(datas[i]);
   } 
   
   EPD_Update_Fast();  
}
void EPD_WhiteScreen_ALL_Fast_Clean(void)
{
   unsigned int i;  
  Epaper_Write_Command(0x24);   //write RAM for black(0)/white (1)
   for(i=0;i<4736;i++)
   {               
     Epaper_Write_Data(0xFF);
   } 
   
   EPD_Update_Fast();  
}
///////////////////////////Part update//////////////////////////////////////////////
void EPD_SetRAMValue_BaseMap( const unsigned char * datas)
{
  unsigned int i;   
  const unsigned char  *datas_flag;   
  datas_flag=datas;
  Epaper_Write_Command(0x24);   //Write Black and White image to RAM
   for(i=0;i<4736;i++)
   {               
     Epaper_Write_Data(datas[i]);
   }
   datas=datas_flag;
  Epaper_Write_Command(0x26);   //Write Black and White image to RAM
   for(i=0;i<4736;i++)
   {               
     Epaper_Write_Data(datas[i]);
   }
   EPD_Update();   
}
void EPD_SetRAMValue_BaseMap_Fast( const unsigned char * datas)
{
  unsigned int i;   
  Epaper_Write_Command(0x24);   //Write Black and White image to RAM
   for(i=0;i<4736;i++)
   {               
      Epaper_Write_Data(datas[i]);
   }
  Epaper_Write_Command(0x26);   //Write Black and White image to RAM
   for(i=0;i<4736;i++)
   {               
     Epaper_Write_Data(datas[i]);
   }  
    EPD_Update_Fast();   
}

void EPD_Dis_Part(unsigned int x_start,unsigned int y_start,const unsigned char * datas,unsigned int PART_COLUMN,unsigned int PART_LINE)
{
  unsigned int i;  
  unsigned int x_end,y_start1,y_start2,y_end1,y_end2;
  x_start=x_start/8;//
  x_end=x_start+PART_LINE/8-1; 
  
  y_start1=0;
  y_start2=y_start;
  if(y_start>=256)
  {
    y_start1=y_start2/256;
    y_start2=y_start2%256;
  }
  y_end1=0;
  y_end2=y_start+PART_COLUMN-1;
  if(y_end2>=256)
  {
    y_end1=y_end2/256;
    y_end2=y_end2%256;    
  }   
  // Add hardware reset to prevent background color change
  EPD_W21_RST_0;  // Module reset   
  delay(10);//At least 10ms delay 
  EPD_W21_RST_1;
  delay(10); //At least 10ms delay 
  
  //Lock the border to prevent flashing
  Epaper_Write_Command(0x3C); //BorderWavefrom,
  Epaper_Write_Data(0x80); 
  //  
  Epaper_Write_Command(0x44);       // set RAM x address start/end, in page 35
  Epaper_Write_Data(x_start);    // RAM x address start at 00h;
  Epaper_Write_Data(x_end);    // RAM x address end at 0fh(15+1)*8->128 
  Epaper_Write_Command(0x45);       // set RAM y address start/end, in page 35
  Epaper_Write_Data(y_start2);    // RAM y address start at 0127h;
  Epaper_Write_Data(y_start1);    // RAM y address start at 0127h;
  Epaper_Write_Data(y_end2);    // RAM y address end at 00h;
  Epaper_Write_Data(y_end1);    // ????=0 


  Epaper_Write_Command(0x4E);   // set RAM x address count to 0;
  Epaper_Write_Data(x_start); 
  Epaper_Write_Command(0x4F);   // set RAM y address count to 0X127;    
  Epaper_Write_Data(y_start2);
  Epaper_Write_Data(y_start1);
  
  
   Epaper_Write_Command(0x24);   //Write Black and White image to RAM
   for(i=0;i<PART_COLUMN*PART_LINE/8;i++)
   {                         
     Epaper_Write_Data(datas[i]);
   } 
   EPD_Part_Update();

}
////////////////////////////////////TIME///////////////////////////////////////////////////
void EPD_Dis_Part_myself(unsigned int x_startA,unsigned int y_startA,const unsigned char * datasA,
                         unsigned int x_startB,unsigned int y_startB,const unsigned char * datasB,
                         unsigned int x_startC,unsigned int y_startC,const unsigned char * datasC,
                         unsigned int x_startD,unsigned int y_startD,const unsigned char * datasD,
                         unsigned int x_startE,unsigned int y_startE,const unsigned char * datasE,
                         unsigned int PART_COLUMN,unsigned int PART_LINE
                        )
{
  unsigned int i;  
  unsigned int x_end,y_start1,y_start2,y_end1,y_end2;
  
  //Data A////////////////////////////
  x_startA=x_startA/8;//Convert to byte
  x_end=x_startA+PART_LINE/8-1; 
  
  y_start1=0;
  y_start2=y_startA-1;
  if(y_startA>=256)
  {
    y_start1=y_start2/256;
    y_start2=y_start2%256;
  }
  y_end1=0;
  y_end2=y_startA+PART_COLUMN-1;
  if(y_end2>=256)
  {
    y_end1=y_end2/256;
    y_end2=y_end2%256;    
  }   
  //Reset
  EPD_W21_RST_0;  // Module reset   
  delay(10);//At least 10ms delay 
  EPD_W21_RST_1;
  delay(10); //At least 10ms delay 
    
  Epaper_Write_Command(0x3C); //BorderWavefrom
  Epaper_Write_Data(0x80);  
//
  Epaper_Write_Command(0x44);       // set RAM x address start/end, in page 35
  Epaper_Write_Data(x_startA);    // RAM x address start at 00h;
  Epaper_Write_Data(x_end);    // RAM x address end at 0fh(15+1)*8->128 
  Epaper_Write_Command(0x45);       // set RAM y address start/end, in page 35
  Epaper_Write_Data(y_start2);    // RAM y address start at 0127h;
  Epaper_Write_Data(y_start1);    // RAM y address start at 0127h;
  Epaper_Write_Data(y_end2);    // RAM y address end at 00h;
  Epaper_Write_Data(y_end1);    


  Epaper_Write_Command(0x4E);   // set RAM x address count to 0;
  Epaper_Write_Data(x_startA); 
  Epaper_Write_Command(0x4F);   // set RAM y address count to 0X127;    
  Epaper_Write_Data(y_start2);
  Epaper_Write_Data(y_start1);
  
  
   Epaper_Write_Command(0x24);   //Write Black and White image to RAM
   for(i=0;i<PART_COLUMN*PART_LINE/8;i++)
   {                         
     Epaper_Write_Data(datasA[i]);
   } 
  //Data B/////////////////////////////////////
  x_startB=x_startB/8;//Convert to byte
  x_end=x_startB+PART_LINE/8-1; 
  
  y_start1=0;
  y_start2=y_startB-1;
  if(y_startB>=256)
  {
    y_start1=y_start2/256;
    y_start2=y_start2%256;
  }
  y_end1=0;
  y_end2=y_startB+PART_COLUMN-1;
  if(y_end2>=256)
  {
    y_end1=y_end2/256;
    y_end2=y_end2%256;    
  }   
  
  Epaper_Write_Command(0x44);       // set RAM x address start/end, in page 35
  Epaper_Write_Data(x_startB);    // RAM x address start at 00h;
  Epaper_Write_Data(x_end);    // RAM x address end at 0fh(15+1)*8->128 
  Epaper_Write_Command(0x45);       // set RAM y address start/end, in page 35
  Epaper_Write_Data(y_start2);    // RAM y address start at 0127h;
  Epaper_Write_Data(y_start1);    // RAM y address start at 0127h;
  Epaper_Write_Data(y_end2);    // RAM y address end at 00h;
  Epaper_Write_Data(y_end1);   


  Epaper_Write_Command(0x4E);   // set RAM x address count to 0;
  Epaper_Write_Data(x_startB); 
  Epaper_Write_Command(0x4F);   // set RAM y address count to 0X127;    
  Epaper_Write_Data(y_start2);
  Epaper_Write_Data(y_start1);
  
  
   Epaper_Write_Command(0x24);   //Write Black and White image to RAM
   for(i=0;i<PART_COLUMN*PART_LINE/8;i++)
   {                         
     Epaper_Write_Data(datasB[i]);
   }   
   
  //Data C//////////////////////////////////////
  x_startC=x_startC/8;//Convert to byte
  x_end=x_startC+PART_LINE/8-1; 
  
  y_start1=0;
  y_start2=y_startC-1;
  if(y_startC>=256)
  {
    y_start1=y_start2/256;
    y_start2=y_start2%256;
  }
  y_end1=0;
  y_end2=y_startC+PART_COLUMN-1;
  if(y_end2>=256)
  {
    y_end1=y_end2/256;
    y_end2=y_end2%256;    
  }   
  
  Epaper_Write_Command(0x44);       // set RAM x address start/end, in page 35
  Epaper_Write_Data(x_startC);    // RAM x address start at 00h;
  Epaper_Write_Data(x_end);    // RAM x address end at 0fh(15+1)*8->128 
  Epaper_Write_Command(0x45);       // set RAM y address start/end, in page 35
  Epaper_Write_Data(y_start2);    // RAM y address start at 0127h;
  Epaper_Write_Data(y_start1);    // RAM y address start at 0127h;
  Epaper_Write_Data(y_end2);    // RAM y address end at 00h;
  Epaper_Write_Data(y_end1);   


  Epaper_Write_Command(0x4E);   // set RAM x address count to 0;
  Epaper_Write_Data(x_startC); 
  Epaper_Write_Command(0x4F);   // set RAM y address count to 0X127;    
  Epaper_Write_Data(y_start2);
  Epaper_Write_Data(y_start1);
  
  
   Epaper_Write_Command(0x24);   //Write Black and White image to RAM
   for(i=0;i<PART_COLUMN*PART_LINE/8;i++)
   {                         
     Epaper_Write_Data(datasC[i]);
   }    
   
  //Data D//////////////////////////////////////
  x_startD=x_startD/8;//Convert to byte
  x_end=x_startD+PART_LINE/8-1; 
  
  y_start1=0;
  y_start2=y_startD-1;
  if(y_startD>=256)
  {
    y_start1=y_start2/256;
    y_start2=y_start2%256;
  }
  y_end1=0;
  y_end2=y_startD+PART_COLUMN-1;
  if(y_end2>=256)
  {
    y_end1=y_end2/256;
    y_end2=y_end2%256;    
  }   
  
  Epaper_Write_Command(0x44);       // set RAM x address start/end, in page 35
  Epaper_Write_Data(x_startD);    // RAM x address start at 00h;
  Epaper_Write_Data(x_end);        // RAM x address end at 0fh(15+1)*8->128 
  Epaper_Write_Command(0x45);       // set RAM y address start/end, in page 35
  Epaper_Write_Data(y_start2);    // RAM y address start at 0127h;
  Epaper_Write_Data(y_start1);    // RAM y address start at 0127h;
  Epaper_Write_Data(y_end2);    // RAM y address end at 00h;
  Epaper_Write_Data(y_end1);    


  Epaper_Write_Command(0x4E);   // set RAM x address count to 0;
  Epaper_Write_Data(x_startD); 
  Epaper_Write_Command(0x4F);   // set RAM y address count to 0X127;    
  Epaper_Write_Data(y_start2);
  Epaper_Write_Data(y_start1);
  
  
   Epaper_Write_Command(0x24);   //Write Black and White image to RAM
   for(i=0;i<PART_COLUMN*PART_LINE/8;i++)
   {                         
     Epaper_Write_Data(datasD[i]);
   } 
  //Data E//////////////////////////////////////
  x_startE=x_startE/8;//Convert to byte
  x_end=x_startE+PART_LINE/8-1; 
  
  y_start1=0;
  y_start2=y_startE-1;
  if(y_startE>=256)
  {
    y_start1=y_start2/256;
    y_start2=y_start2%256;
  }
  y_end1=0;
  y_end2=y_startE+PART_COLUMN-1;
  if(y_end2>=256)
  {
    y_end1=y_end2/256;
    y_end2=y_end2%256;    
  }   
  
  Epaper_Write_Command(0x44);       // set RAM x address start/end, in page 35
  Epaper_Write_Data(x_startE);    // RAM x address start at 00h;
  Epaper_Write_Data(x_end);    // RAM x address end at 0fh(15+1)*8->128 
  Epaper_Write_Command(0x45);       // set RAM y address start/end, in page 35
  Epaper_Write_Data(y_start2);    // RAM y address start at 0127h;
  Epaper_Write_Data(y_start1);    // RAM y address start at 0127h;
  Epaper_Write_Data(y_end2);    // RAM y address end at 00h;
  Epaper_Write_Data(y_end1);    


  Epaper_Write_Command(0x4E);   // set RAM x address count to 0;
  Epaper_Write_Data(x_startE); 
  Epaper_Write_Command(0x4F);   // set RAM y address count to 0X127;    
  Epaper_Write_Data(y_start2);
  Epaper_Write_Data(y_start1);
  
  
   Epaper_Write_Command(0x24);   //Write Black and White image to RAM
   for(i=0;i<PART_COLUMN*PART_LINE/8;i++)
   {                         
     Epaper_Write_Data(datasE[i]);
   }   
   EPD_Part_Update();

}

void EPD_Dis_Part_myself11(unsigned int x_startA,unsigned int y_startA,const unsigned char * datasA,
                         unsigned int x_startB,unsigned int y_startB,const unsigned char * datasB,
                         unsigned int PART_COLUMN,unsigned int PART_LINE
                        )
{
  unsigned int i;  
  unsigned int x_end,y_start1,y_start2,y_end1,y_end2;
  
  //Data A////////////////////////////
  x_startA=x_startA/8;//Convert to byte
  x_end=x_startA+PART_LINE/8-1; 
  
  y_start1=0;
  y_start2=y_startA-1;
  if(y_startA>=256)
  {
    y_start1=y_start2/256;
    y_start2=y_start2%256;
  }
  y_end1=0;
  y_end2=y_startA+PART_COLUMN-1;
  if(y_end2>=256)
  {
    y_end1=y_end2/256;
    y_end2=y_end2%256;    
  }   
  //Reset
  EPD_W21_RST_0;  // Module reset   
  delay(10);//At least 10ms delay 
  EPD_W21_RST_1;
  delay(10); //At least 10ms delay 
  
  Epaper_Write_Command(0x3C); //BorderWavefrom
  Epaper_Write_Data(0x80);  
//
  Epaper_Write_Command(0x44);       // set RAM x address start/end, in page 35
  Epaper_Write_Data(x_startA);    // RAM x address start at 00h;
  Epaper_Write_Data(x_end);    // RAM x address end at 0fh(15+1)*8->128 
  Epaper_Write_Command(0x45);       // set RAM y address start/end, in page 35
  Epaper_Write_Data(y_start2);    // RAM y address start at 0127h;
  Epaper_Write_Data(y_start1);    // RAM y address start at 0127h;
  Epaper_Write_Data(y_end2);    // RAM y address end at 00h;
  Epaper_Write_Data(y_end1);    


  Epaper_Write_Command(0x4E);   // set RAM x address count to 0;
  Epaper_Write_Data(x_startA); 
  Epaper_Write_Command(0x4F);   // set RAM y address count to 0X127;    
  Epaper_Write_Data(y_start2);
  Epaper_Write_Data(y_start1);
  
  
   Epaper_Write_Command(0x24);   //Write Black and White image to RAM
   for(i=0;i<PART_COLUMN*PART_LINE/8;i++)
   {                         
      Epaper_Write_Data(datasA[i]);
   } 
  //Data B/////////////////////////////////////
  x_startB=x_startB/8;//Convert to byte
  x_end=x_startB+PART_LINE/8-1; 
  
  y_start1=0;
  y_start2=y_startB-1;
  if(y_startB>=256)
  {
    y_start1=y_start2/256;
    y_start2=y_start2%256;
  }
  y_end1=0;
  y_end2=y_startB+PART_COLUMN-1;
  if(y_end2>=256)
  {
    y_end1=y_end2/256;
    y_end2=y_end2%256;    
  }   
  
  Epaper_Write_Command(0x44);       // set RAM x address start/end, in page 35
  Epaper_Write_Data(x_startB);    // RAM x address start at 00h;
  Epaper_Write_Data(x_end);    // RAM x address end at 0fh(15+1)*8->128 
  Epaper_Write_Command(0x45);       // set RAM y address start/end, in page 35
  Epaper_Write_Data(y_start2);    // RAM y address start at 0127h;
  Epaper_Write_Data(y_start1);    // RAM y address start at 0127h;
  Epaper_Write_Data(y_end2);    // RAM y address end at 00h;
  Epaper_Write_Data(y_end1);   


  Epaper_Write_Command(0x4E);   // set RAM x address count to 0;
  Epaper_Write_Data(x_startB); 
  Epaper_Write_Command(0x4F);   // set RAM y address count to 0X127;    
  Epaper_Write_Data(y_start2);
  Epaper_Write_Data(y_start1);
  
  
   Epaper_Write_Command(0x24);   //Write Black and White image to RAM
   for(i=0;i<PART_COLUMN*PART_LINE/8;i++)
   {                         
      Epaper_Write_Data(datasB[i]);
   }    
   EPD_Part_Update();

}
