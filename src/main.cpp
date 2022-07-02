#include <Arduino.h>
#include"Ap_29demo.h"
//IO settings 引脚声明
/**
 * BUSY_Pin 数字引脚 D0
 */
int BUSY_Pin = D0; 
/**
 * RES_Pin 数字引脚 D1
 */
int RES_Pin = D1;
/**
 * DC_Pin 数字引脚 D2
 */
int DC_Pin = D2;
/**
 * CS_Pin 数字引脚 D5
 */
int CS_Pin = D5;
/**
 * SCK_Pin 数字引脚 D6
 */
int SCK_Pin = D6;
/**
 * SDI_Pin 数字引脚 D7
 */
int SDI_Pin = D7;

/**
 * SDI_Pin ( D7 ) 引脚设置为 LOW 低电平
 */
#define EPD_W21_MOSI_0  digitalWrite(SDI_Pin,LOW)

/**
 * SDI_Pin ( D7 ) 引脚设置为 HIGH 高电平
 */
#define EPD_W21_MOSI_1  digitalWrite(SDI_Pin,HIGH)
/**
 * SCK_Pin ( D6 ) 引脚设置为 LOW 低电平
 */
#define EPD_W21_CLK_0 digitalWrite(SCK_Pin,LOW)
/**
 * SCK_Pin ( D6 ) 引脚设置为 HIGH 高电平
 */
#define EPD_W21_CLK_1 digitalWrite(SCK_Pin,HIGH)
/**
 * CS_Pin ( D5 ) 引脚设置为 LOW 低电平
 */
#define EPD_W21_CS_0 digitalWrite(CS_Pin,LOW)
/**
 * CS_Pin ( D5 ) 引脚设置为 HIGH 高电平
 */
#define EPD_W21_CS_1 digitalWrite(CS_Pin,HIGH)
/**
 * DC_Pin ( D2 ) 引脚设置为 LOW 低电平
 */
#define EPD_W21_DC_0  digitalWrite(DC_Pin,LOW)
/**
 * DC_Pin ( D2 ) 引脚设置为 HIGH 高电平
 */
#define EPD_W21_DC_1  digitalWrite(DC_Pin,HIGH)
/**
 * RES_Pin ( D1 ) 引脚设置为 LOW 低电平
 */
#define EPD_W21_RST_0 digitalWrite(RES_Pin,LOW)
/**
 * RES_Pin ( D1 ) 引脚设置为 HIGH 高电平
 */
#define EPD_W21_RST_1 digitalWrite(RES_Pin,HIGH)
/**
 * 读取数字引脚  BUSY_Pin ( D0 ) 的 状态 是 HIGH(高电平）或 LOW（低电平）。
 */
#define isEPD_W21_BUSY digitalRead(BUSY_Pin)


//200*200///////////////////////////////////////
#define MONOMSB_MODE 1
#define MONOLSB_MODE 2 
#define RED_MODE     3
/**
 * 最大 行 字节
 */
#define MAX_LINE_BYTES 25 // =200/8
/**
 * 最大 列 字节
 */
#define MAX_COLUMN_BYTES  200

#define ALLSCREEN_GRAGHBYTES  5000

//////// FUNCTION 功能 //////
void driver_delay_us(unsigned int xus);
void driver_delay_xms(unsigned long xms);
void DELAY_S(unsigned int delaytime);
/**
 * SPI 延迟
 * @param xrate
 */
void SPI_Delay(unsigned char xrate);
/**
 * SPI 写入
 * @param value
 */
void SPI_Write(unsigned char value);
/**
 * 电子纸 写入 命令
 * @param command
 */
void Epaper_Write_Command(unsigned char command);
/**
 * 电子纸 写入 数据
 * @param command
 */
void Epaper_Write_Data(unsigned char command);
/**
 * 电子纸 读取
 */
void Epaper_READBUSY(void);
/**
 * 电子纸初始化
 */
void EPD_HW_Init(void);
/**
 * 电子纸 更新
 */
void EPD_Update(void);
/**
 * 电子纸 沉睡
 */
void EPD_DeepSleep(void);

/**
 * 电子纸 白屏 写入
 * @param BW_datas
 * @param R_datas
 * @return
 */
void EPD_WhiteScreen_ALL(const unsigned char *BW_datas,const unsigned char *R_datas);
/**
 * 电子纸 白屏 写入 清空
 */
void EPD_WhiteScreen_ALL_Clean(void);

void Sys_run(void)
{
   //Feed dog to prevent system reset
   //Feed dog to 防止系统重置
   ESP.wdtFeed();
  }
/**
 * 运行 LED 闪烁
 */
void LED_run(void)
{
  // 打开 LED 为 LOW 亮（注意 LOW 是电压电平，HIGH 是电压反电平）
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
  // 打开 LED 为 HIGH 熄灭（注意 LOW 是电压电平，HIGH 是电压反电平）
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  }

  /**
   * 引脚设置
   */
void setup() {
   pinMode(BUSY_Pin, INPUT); 
   pinMode(RES_Pin, OUTPUT);  
   pinMode(DC_Pin, OUTPUT);    
   pinMode(CS_Pin, OUTPUT);    
   pinMode(SCK_Pin, OUTPUT);    
   pinMode(SDI_Pin, OUTPUT);    
}

//Tips//
/*When the electronic paper is refreshed in full screen, the picture flicker is a normal phenomenon, and the main function is to clear the display afterimage in the previous picture.
  When the local refresh is performed, the screen does not flash.*/
/*When you need to transplant the driver, you only need to change the corresponding IO. The BUSY pin is the input mode and the others are the output mode. */
  
void loop() {
    //Full screen refresh
    EPD_HW_Init(); //Electronic paper initialization
    EPD_WhiteScreen_ALL(gImage_BW,gImage_R); //Refresh the picture in full screen
    EPD_DeepSleep(); //Enter deep sleep,Sleep instruction is necessary, please do not delete!!! 
    delay(2000);   
    
    //Clean
    EPD_HW_Init(); //Electronic paper initialization
    EPD_WhiteScreen_ALL_Clean();
    EPD_DeepSleep(); //Enter deep sleep,Sleep instruction is necessary, please do not delete!!!
    delay(2000); 
    while(1) 
    {
     Sys_run();//System run
     LED_run();//Breathing lamp
    }
}












///////////////////EXTERNAL FUNCTION////////////////////////////////////////////////////////////////////////
/////////////////////delay//////////////////////////////////////
void driver_delay_us(unsigned int xus)  //1us
{
  for(;xus>1;xus--);
}
void driver_delay_xms(unsigned long xms) //1ms
{  
    unsigned long i = 0 , j=0;

    for(j=0;j<xms;j++)
  {
        for(i=0; i<256; i++);
    }
}
void DELAY_S(unsigned int delaytime)     
{
  int i,j,k;
  for(i=0;i<delaytime;i++)
  {
    for(j=0;j<4000;j++)           
    {
      for(k=0;k<222;k++);
                
    }
  }
}
//////////////////////SPI///////////////////////////////////
void SPI_Delay(unsigned char xrate)
{
  unsigned char i;
  while(xrate)
  {
    for(i=0;i<2;i++);
    xrate--;
  }
}


void SPI_Write(unsigned char value)                                    
{                                                           
    unsigned char i;  
   SPI_Delay(1);
    for(i=0; i<8; i++)   
    {
        EPD_W21_CLK_0;
       SPI_Delay(1);
       if(value & 0x80)
          EPD_W21_MOSI_1;
        else
          EPD_W21_MOSI_0;   
        value = (value << 1); 
       SPI_Delay(1);
       driver_delay_us(1);
        EPD_W21_CLK_1; 
        SPI_Delay(1);
    }
}

void Epaper_Write_Command(unsigned char command)
{
  SPI_Delay(1);
  EPD_W21_CS_0;                   
  EPD_W21_DC_0;   // command write
  SPI_Write(command);
  EPD_W21_CS_1;
}
void Epaper_Write_Data(unsigned char command)
{
  SPI_Delay(1);
  EPD_W21_CS_0;                   
  EPD_W21_DC_1;   // command write
  SPI_Write(command);
  EPD_W21_CS_1;
}

/////////////////EPD settings Functions/////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////
//SSD1680
void EPD_HW_Init(void)
{
  EPD_W21_RST_0;  // Module reset      
  delay(10); //At least 10ms delay 
  EPD_W21_RST_1; 
  delay(10); //At least 10ms delay  
    
  Epaper_READBUSY();   
  Epaper_Write_Command(0x12);  //SWRESET
  Epaper_READBUSY();   
    
  Epaper_Write_Command(0x01); //Driver output control      
  Epaper_Write_Data(0xC7);
  Epaper_Write_Data(0x00);
  Epaper_Write_Data(0x00);

  Epaper_Write_Command(0x11); //data entry mode       
  Epaper_Write_Data(0x01);

  Epaper_Write_Command(0x44); //set Ram-X address start/end position   
  Epaper_Write_Data(0x00);
  Epaper_Write_Data(0x18);    //0x18-->(24+1)*8=200

  Epaper_Write_Command(0x45); //set Ram-Y address start/end position          
  Epaper_Write_Data(0xC7);    //0xC7-->(199+1)=200
  Epaper_Write_Data(0x00);
  Epaper_Write_Data(0x00);
  Epaper_Write_Data(0x00); 

  Epaper_Write_Command(0x3C); //BorderWavefrom
  Epaper_Write_Data(0x05);  
      
  Epaper_Write_Command(0x18); //Read built-in temperature sensor
  Epaper_Write_Data(0x80);  
  
  Epaper_Write_Command(0x4E);   // set RAM x address count to 0;
  Epaper_Write_Data(0x00);
  Epaper_Write_Command(0x4F);   // set RAM y address count to 0X199;    
  Epaper_Write_Data(0xC7);
  Epaper_Write_Data(0x00);
  Epaper_READBUSY();
  
}
//////////////////////////////All screen update////////////////////////////////////////////
void EPD_WhiteScreen_ALL(const unsigned char *BW_datas,const unsigned char *R_datas)
{
   unsigned int i;  
  Epaper_Write_Command(0x24);   //write RAM for black(0)/white (1)
   for(i=0;i<ALLSCREEN_GRAGHBYTES;i++)
   {               
     Epaper_Write_Data(pgm_read_byte(&BW_datas[i]));
   }
  Epaper_Write_Command(0x26);   //write RAM for black(0)/white (1)
   for(i=0;i<ALLSCREEN_GRAGHBYTES;i++)
   {               
     Epaper_Write_Data(~(pgm_read_byte(&R_datas[i])));
   }
   EPD_Update();   
}

void EPD_WhiteScreen_ALL_Clean(void)
{
   unsigned int i;  
  Epaper_Write_Command(0x24);   //write RAM for black(0)/white (1)
   for(i=0;i<ALLSCREEN_GRAGHBYTES;i++)
   {               
     Epaper_Write_Data(0xff);
   }
  Epaper_Write_Command(0x26);   //write RAM for black(0)/white (1)
   for(i=0;i<ALLSCREEN_GRAGHBYTES;i++)
   {               
     Epaper_Write_Data(0x00);
   }
   EPD_Update();   
}

/////////////////////////////////////////////////////////////////////////////////////////
void EPD_Update(void)
{
  Epaper_Write_Command(0x22); //Display Update Control
  Epaper_Write_Data(0xF7);   
  Epaper_Write_Command(0x20);  //Activate Display Update Sequence
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
     ESP.wdtFeed(); //Feed dog to prevent system reset
  } 
}



//////////////////////////////////END//////////////////////////////////////////////////
