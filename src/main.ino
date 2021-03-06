////*************************************************************************
////----------------1.开发环境:Arduino IDE or Visual Studio 2010----------------
////----------------2.使用开发板型号：Arduino UNO          ----------------
////----------------3.单片机使用晶振：16M		            ----------------
////----------------4.店铺名称：大学生电子商铺/小强电子设计  ----------------
////----------------5.淘宝网址：Ilovemcu.taobao.com		 ----------------
////----------------	    52dpj.taobao.com                ----------------
////----------------6.作者：神秘藏宝室			    ----------------
////***********************************************************************
#include <Arduino.h>
#include <SPI.h>

//IO配置
#define LEDARRAY_A 2
#define LEDARRAY_B 3
#define LEDARRAY_C 4
#define LEDARRAY_D 5
#define LEDARRAY_G 6

//使用了硬件SPI，以下脚不能更改
#define LEDARRAY_DI 11	//数据出    MOSI
#define LEDARRAY_CLK 13	 //时钟    SCK
#define LEDARRAY_LAT 10		//595 刷新显示  SS

#define led 13


#define DISP_NUM 7				//液晶能显示的汉字个数
#define  MAX_WORD_NUM 22				//表示用バッファの容量
#define SHIFT_SPEED 3

//unsigned char Display_Buffer[NUM_WORDS*2];
unsigned char* Display_Buffer;
//unsigned char Display_Swap_Buffer[NUM_WORDS][32]={0};					//显示缓冲区
//unsigned char Display_Swap_Buffer[16][MAX_WORD_NUM*]={0};					//显示缓冲区  //配列のサイズに制限があるらしい。。。
unsigned char Display_Swap_Buffer[16][MAX_WORD_NUM*2]={0};
unsigned char Display_Swap_Buffer_2[16][40]={0};
unsigned char Display_Swap_Buffer_3[16][40]={0};
unsigned char Display_Swap_Buffer_temp[16][40]={0};
bool Shift_Bit = 0;
bool Flag_Shift = 0;
unsigned char Timer0_Count = 0;
unsigned char temp = 0x80;
//unsigned char Shift_Count = 0;
unsigned int Shift_Count = 0;
unsigned char Display_Word_Count = 0;

int Word_Num = 0;
int Stat = 1;

int shiftspeed;

#define Num_Of_Word 7 
const unsigned char Word[][32] = 
{
	 
0xFE,0xFE,0xFE,0xFE,0xF6,0xF2,0xE6,0xEE,0xDE,0xBE,0x7E,0xFE,0xFE,0xFE,0xFA,0xFD,
0xFF,0xFF,0xFF,0xFF,0xBF,0xDF,0xEF,0xE7,0xF3,0xF9,0xFB,0xFF,0xFF,0xFF,0xFF,0xFF,/*"小",0*/

0xFF,0x86,0xF6,0xF6,0x86,0xBF,0xBC,0xBD,0x85,0xF5,0xF4,0xF7,0xF7,0xF7,0xD7,0xEC,
0xFF,0x07,0xF7,0xF7,0x07,0xBF,0x03,0xBB,0xBB,0xBB,0x03,0xBF,0xB7,0xBB,0x81,0x3B,/*"强",1*/

};

const unsigned char Word2[][32] = 
{
	 
0x00,0x05,0x00,0x05,0x1f,0xf0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x20,0x00,0x30,0x00,0x1c,0x00,0x07,0xfc,0x00,0x00,0x00,0x00,/*ご*/

0x00,0xc0,0x60,0x60,0x30,0x20,0x10,0x00,0x03,0xfe,0xc0,0x20,0x60,0x20,0x20,0x20,
0x00,0x20,0x13,0xfe,0x10,0x20,0x30,0x20,0x20,0x20,0x60,0x20,0x47,0xff,0xc0,0x00,/*注*/

0x01,0x00,0x01,0x00,0x01,0x00,0xff,0xff,0x08,0x10,0x08,0x10,0x0c,0x30,0x04,0x20,
0x06,0x60,0x02,0xc0,0x01,0x80,0x03,0xc0,0x06,0x60,0x0c,0x30,0x38,0x1c,0xe0,0x07,/*文*/

0x00,0x00,0x00,0x00,0x7f,0xfc,0x00,0x60,0x00,0xc5,0x01,0x85,0x01,0x00,0x03,0x00,
0x02,0x00,0x02,0x00,0x02,0x00,0x03,0x00,0x01,0x80,0x00,0xe0,0x00,0x38,0x00,0x00,/*で*/

0x00,0x80,0x00,0x80,0x00,0x80,0xff,0xff,0x00,0x80,0x00,0x80,0x07,0x80,0x0c,0xc0,
0x08,0x40,0x08,0x40,0x0c,0x40,0x07,0xc0,0x00,0x80,0x01,0x80,0x03,0x00,0x0e,0x00,/*す*/

0x00,0x00,0x01,0x80,0x01,0x80,0x01,0x80,0x01,0x80,0x01,0x80,0x01,0x00,0x01,0x00,
0x01,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x01,0x80,0x00,0x00,0x00,0x00,/*！*/

0x00,0x00,0x01,0x80,0x01,0x80,0x01,0x80,0x01,0x80,0x01,0x80,0x01,0x00,0x01,0x00,
0x01,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x01,0x80,0x00,0x00,0x00,0x00,/*！*/

};


const unsigned char Word4[][32] = 
{
	 
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};

//・・・LED掲示板・・・スタッフへの連絡や客室注文を知らせします。
const unsigned char Word5[][32] = 
{

0x00,0x00,0x1f,0x00,0x04,0x00,0x04,0x00,0x04,0x00,0x04,0x00,0x04,0x00,0x04,0x00,
0x04,0x00,0x04,0x00,0x04,0x04,0x04,0x04,0x04,0x04,0x1f,0xfc,0x00,0x00,0x00,0x00,/*L*/

0x00,0x00,0x3f,0xf8,0x08,0x08,0x08,0x04,0x08,0x00,0x08,0x40,0x08,0x40,0x0f,0xc0,
0x08,0x40,0x08,0x40,0x08,0x00,0x08,0x04,0x08,0x08,0x3f,0xf8,0x00,0x00,0x00,0x00,/*E*/

0x00,0x00,0x3f,0xc0,0x08,0x30,0x08,0x18,0x08,0x08,0x08,0x0c,0x08,0x04,0x08,0x04,
0x08,0x04,0x08,0x0c,0x08,0x08,0x08,0x18,0x08,0x30,0x3f,0xc0,0x00,0x00,0x00,0x00,/*D*/

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x21,0xfc,0x21,0x04,0x21,0x04,0x21,0xfc,0xf9,0x04,0x21,0x04,0x21,0xfc,0x20,0x80,
0x39,0xff,0xe3,0x01,0x2e,0x31,0x23,0xe1,0x22,0x01,0x22,0x13,0x21,0xf2,0x60,0x0e,/*掲*/

0x00,0x00,0x3f,0xfc,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0x01,0x00,
0x01,0x00,0x11,0x08,0x11,0x08,0x31,0x0c,0x61,0x06,0xc1,0x03,0x01,0x00,0x07,0x00,/*示*/

0x10,0x00,0x11,0xff,0x11,0x00,0xfd,0x00,0x11,0x00,0x11,0xfe,0x39,0x42,0x35,0x46,
0x35,0x64,0x51,0x2c,0x51,0x38,0x91,0x10,0x13,0x38,0x12,0x2c,0x16,0x66,0x10,0xc3,/*板*/

};

const unsigned char Word6[][32]=
{

0x00,0x00,0x00,0x00,0x3f,0xf0,0x00,0x10,0x00,0x10,0x00,0x30,0x00,0x20,0x00,0x60,
0x00,0xc0,0x01,0xc0,0x03,0x60,0x06,0x30,0x0c,0x18,0x18,0x0c,0x70,0x06,0x00,0x00,/*ス*/

0x01,0x00,0x01,0x00,0x01,0x00,0x03,0xfc,0x02,0x04,0x06,0x0c,0x0d,0x88,0x18,0xd8,
0x00,0x50,0x00,0x30,0x00,0x60,0x00,0xc0,0x01,0x80,0x07,0x00,0x1c,0x00,0x00,0x00,/*タ*/

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x00,0x13,0x08,
0x19,0x08,0x08,0x18,0x00,0x10,0x00,0x30,0x00,0x60,0x00,0xc0,0x01,0x80,0x07,0x00,/*ッ*/

0x00,0x00,0x00,0x00,0x3f,0xfc,0x00,0x04,0x00,0x0c,0x00,0x08,0x00,0x18,0x00,0x10,
0x00,0x30,0x00,0x60,0x00,0xc0,0x01,0x80,0x07,0x00,0x1c,0x00,0x00,0x00,0x00,0x00,/*フ*/

0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x00,0x09,0x00,0x19,0x80,0x10,0x80,0x30,0xc0,
0x60,0x60,0x00,0x30,0x00,0x18,0x00,0x0c,0x00,0x06,0x00,0x00,0x00,0x00,0x00,0x00,/*へ*/

0x00,0x00,0x00,0x00,0x03,0xe0,0x0e,0xb8,0x18,0x8c,0x30,0x84,0x21,0x86,0x61,0x02,
0x43,0x02,0x42,0x02,0x46,0x06,0x4c,0x04,0x38,0x1c,0x00,0x70,0x00,0x00,0x00,0x00,/*の*/

0x00,0x20,0x60,0x20,0x37,0xff,0x10,0x20,0x03,0xfe,0x02,0x22,0x02,0x22,0xf3,0xfe,
0x12,0x22,0x12,0x22,0x13,0xfe,0x10,0x20,0x17,0xff,0x30,0x20,0x68,0x20,0xc7,0xff,/*連*/

0x10,0x20,0x30,0x20,0x60,0x7e,0xc4,0xc6,0x6d,0xac,0x18,0x18,0x30,0x30,0x64,0x6c,
0xfd,0xc7,0x14,0x00,0x10,0xfe,0x54,0x82,0x56,0x82,0xd2,0x82,0x90,0x82,0x10,0xfe,/*絡*/

0x00,0x00,0x10,0xc0,0x18,0x60,0x08,0x20,0x0c,0xfc,0x07,0x82,0x3c,0x02,0xe6,0x06,
0x02,0x1c,0x02,0x00,0x03,0x00,0x01,0x00,0x01,0x00,0x01,0x80,0x00,0x80,0x00,0x80,/*や*/

0x01,0x00,0x01,0x00,0x7f,0xfe,0x44,0x02,0x47,0xf2,0x08,0x20,0x14,0x40,0x63,0x80,
0x06,0xe0,0x1c,0x38,0xf0,0x0f,0x1f,0xf8,0x10,0x08,0x10,0x08,0x10,0x08,0x1f,0xf8,/*客*/

0x01,0x00,0x01,0x00,0x7f,0xfe,0x40,0x02,0x40,0x02,0x3f,0xfc,0x04,0x00,0x08,0x10,
0x1f,0xf8,0x01,0x08,0x01,0x00,0x3f,0xfc,0x01,0x00,0x01,0x00,0xff,0xff,0x00,0x00,/*室*/

0x00,0xc0,0x60,0x60,0x30,0x20,0x10,0x00,0x03,0xfe,0xc0,0x20,0x60,0x20,0x20,0x20,
0x00,0x20,0x13,0xfe,0x10,0x20,0x30,0x20,0x20,0x20,0x60,0x20,0x47,0xff,0xc0,0x00,/*注*/

0x01,0x00,0x01,0x00,0x01,0x00,0xff,0xff,0x08,0x10,0x08,0x10,0x0c,0x30,0x04,0x20,
0x06,0x60,0x02,0xc0,0x01,0x80,0x03,0xc0,0x06,0x60,0x0c,0x30,0x38,0x1c,0xe0,0x07,/*文*/

0x02,0x00,0x02,0x00,0x03,0xe0,0x3e,0x00,0x04,0x00,0x04,0x06,0x0f,0x9c,0x18,0x70,
0x31,0xc0,0x03,0x40,0x06,0x40,0x0c,0x40,0x08,0x00,0x0c,0x00,0x07,0xfc,0x00,0x00,/*を*/

0x00,0x00,0x1f,0x00,0x04,0x00,0x04,0x00,0x04,0x00,0x04,0x00,0x04,0x00,0x04,0x00,
0x04,0x00,0x04,0x00,0x04,0x04,0x04,0x04,0x04,0x04,0x1f,0xfc,0x00,0x00,0x00,0x00,/*L*/

0x00,0x00,0x3f,0xf8,0x08,0x08,0x08,0x04,0x08,0x00,0x08,0x40,0x08,0x40,0x0f,0xc0,
0x08,0x40,0x08,0x40,0x08,0x00,0x08,0x04,0x08,0x08,0x3f,0xf8,0x00,0x00,0x00,0x00,/*E*/

0x00,0x00,0x3f,0xc0,0x08,0x30,0x08,0x18,0x08,0x08,0x08,0x0c,0x08,0x04,0x08,0x04,
0x08,0x04,0x08,0x0c,0x08,0x08,0x08,0x18,0x08,0x30,0x3f,0xc0,0x00,0x00,0x00,0x00,/*D*/

0x21,0xfc,0x21,0x04,0x21,0x04,0x21,0xfc,0xf9,0x04,0x21,0x04,0x21,0xfc,0x20,0x80,
0x39,0xff,0xe3,0x01,0x2e,0x31,0x23,0xe1,0x22,0x01,0x22,0x13,0x21,0xf2,0x60,0x0e,/*掲*/

0x00,0x00,0x3f,0xfc,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0x01,0x00,
0x01,0x00,0x11,0x08,0x11,0x08,0x31,0x0c,0x61,0x06,0xc1,0x03,0x01,0x00,0x07,0x00,/*示*/

0x10,0x00,0x11,0xff,0x11,0x00,0xfd,0x00,0x11,0x00,0x11,0xfe,0x39,0x42,0x35,0x46,
0x35,0x64,0x51,0x2c,0x51,0x38,0x91,0x10,0x13,0x38,0x12,0x2c,0x16,0x66,0x10,0xc3,/*板*/

0x00,0x00,0x00,0x00,0x7f,0xfc,0x00,0x60,0x00,0xc5,0x01,0x85,0x01,0x00,0x03,0x00,
0x02,0x00,0x02,0x00,0x02,0x00,0x03,0x00,0x01,0x80,0x00,0xe0,0x00,0x38,0x00,0x00,/*で*/

0x00,0x00,0x01,0x80,0x01,0x80,0x01,0x80,0x01,0x80,0x01,0x80,0x01,0x00,0x01,0x00,
0x01,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x01,0x80,0x00,0x00,0x00,0x00,/*！*/


};

void setup()
{
    Serial.begin(115200);
        
  	pinMode(LEDARRAY_D, OUTPUT); 
	pinMode(LEDARRAY_C, OUTPUT);
	pinMode(LEDARRAY_B, OUTPUT);
	pinMode(LEDARRAY_A, OUTPUT);
	pinMode(LEDARRAY_G, OUTPUT);
	pinMode(LEDARRAY_DI, OUTPUT);
	pinMode(LEDARRAY_CLK, OUTPUT);
	pinMode(LEDARRAY_LAT, OUTPUT);
	
	//SPI init
	SPI.setDataMode(SPI_MODE0);
	SPI.setClockDivider(SPI_CLOCK_DIV4);
	SPI.setBitOrder(LSBFIRST);
	SPI.begin();
	
	Clear_Display();
        //Word_Num = sizeof(Word5)/sizeof(Word5[0]);
        //DispBuffer(Word5);
        //Serial.println(Word_Num);
        //Serial.println("DispBuffer(Word6):");
	//DispBuffer(Word6);
	shiftspeed = SHIFT_SPEED;
        Shift_Count = 200;
        Stat = 1;
        //DispBufferAll();
	
}

void loop()
{
    char val = 0;
    val = Serial.read();
    if(val == 'r'){
	//if(0){
  
		Serial.println("ilde") ;
		
		int cont = 10;
		while(cont >= 0){
			int i;
                        Word_Num = sizeof(Word2)/sizeof(Word2[0]);
			DispBuffer(Word2);
			for(i = 0;i<50;i++){
				
				Display(NULL, 0);
			}
			
			Clear_Display();
			for(i = 0;i<50;i++){
				Display(NULL,0);
			}
			cont--;
		}
	
		val = 0;
        Word_Num = sizeof(Word6)/sizeof(Word6[0]);
		DispBuffer(Word6);

    }
    else{
			int bShift = 0;
            if (shiftspeed <= 0){
				bShift = 1;
				shiftspeed = SHIFT_SPEED;
			}
                
			if (Stat == 1){
				Word_Num = sizeof(Word6)/sizeof(Word6[0]);
				DispBuffer(Word6);
                //Copy_Display();
				Stat = 2;
			}
                
			//Serial.println(Word_Num);
                
			if (Stat == 2){
				Display(Word_Num, bShift);
                
			}
              
			shiftspeed--;

            //delay(10);
            //Serial.println(Stat);
            //Serial.println(Shift_Count);
                

	}
}

//************************************************************
//文字をバッファにロードする
//************************************************************
void DispBuffer(const unsigned char dat[][32])
{
        //Word_Num = sizeof(dat)/sizeof(dat[0]);
        //Serial.println(Word_Num);
	for( int i = 0 ; i < 32 ; i+=2 )
	{
		for (int j=0; j<Word_Num*2; j+=2){
			Display_Swap_Buffer[i/2][j] = dat[j/2][i];		
			Display_Swap_Buffer[i/2][j+1] = dat[j/2][i+1];
                        //Serial.println(Display_Swap_Buffer[i/2][j],HEX) ;
                        //Serial.println(Display_Swap_Buffer[i/2][j+1],HEX) ;
		}
	
	}

        
}



void DispBufferAll()
{
        //Word_Num = sizeof(dat)/sizeof(dat[0]);
        //Serial.println(Word_Num);
        
        for( int i = 0 ; i < 32 ; i+=2 )
	{
		for (int j=0; j<7*2; j+=2){
			Display_Swap_Buffer[i/2][j] = Word2[j/2][i];		
			Display_Swap_Buffer[i/2][j+1] = Word2[j/2][i+1];
                        //Serial.println(Display_Swap_Buffer[i/2][j],HEX) ;
                        //Serial.println(Display_Swap_Buffer[i/2][j+1],HEX) ;
		}
	
	}
        
	for( int i = 0 ; i < 32 ; i+=2 )
	{
		for (int j=0; j<7*2; j+=2){
			Display_Swap_Buffer_2[i/2][j] = Word5[j/2][i];		
			Display_Swap_Buffer_2[i/2][j+1] = Word5[j/2][i+1];
                        //Serial.println(Display_Swap_Buffer[i/2][j],HEX) ;
                        //Serial.println(Display_Swap_Buffer[i/2][j+1],HEX) ;
		}
	
	}

        for( int i = 0 ; i < 32 ; i+=2 )
	{
		for (int j=0; j<20*2; j+=2){
			Display_Swap_Buffer_3[i/2][j] = Word6[j/2][i];		
			Display_Swap_Buffer_3[i/2][j+1] = Word6[j/2][i+1];
                        //Serial.println(Display_Swap_Buffer[i/2][j],HEX) ;
                        //Serial.println(Display_Swap_Buffer[i/2][j+1],HEX) ;
		}
	
	}

}


void Display_Swap_Buffer_Print()
{
	for( int i = 0 ; i < 32 ; i+=2 )
	{
		for (int j=0; j<Word_Num*2; j+=2){
                       //Serial.println(Display_Swap_Buffer[i/2][j],HEX) ;
                       //Serial.println(Display_Swap_Buffer[i/2][j+1],HEX) ;
		}
	
	}
}


//************************************************************
//清空缓冲区
//************************************************************
void Clear_Display()
{
	unsigned char i,j;
	for(j = 0 ; j < 16; j++)
	{
		for(i = 0 ; i < Word_Num*2 ;i++)
		{
			Display_Swap_Buffer[j][i] = 0xff; //0=显示  1=不显示 
		}
	}
}

//************************************************************
//copy缓冲区
//************************************************************
void Copy_Display()
{
	unsigned char i,j;
	for(j = 0 ; j < 16; j++)
	{
		for(i = 0 ; i < Word_Num*2 ;i++)
		{
			Display_Swap_Buffer_temp[j][i] = Display_Swap_Buffer[j][i];	
		}
	}
}

//************************************************************
//restore缓冲区
//************************************************************
void Restore_Display()
{
	unsigned char i,j;
	for(j = 0 ; j < 16; j++)
	{
		for(i = 0 ; i < Word_Num*2 ;i++)
		{
			Display_Swap_Buffer[j][i] = Display_Swap_Buffer_temp[j][i];					
		}
	}
}



//************************************************************
//num为字数  dat[][32]为字模的名称
//*************************************************************
void Display(int num, int bShift)					
{
	unsigned char i;
	unsigned char j;
	for( i = 0 ; i < 16 ; i++ )
	{
		
		/*
		for (j=0; j<num*2; j+=2){
			Display_Buffer[j] = dat[j/2][i];		
			Display_Buffer[j+1] = dat[j/2][i+1];
		}
		*/
		
		Display_Buffer = Display_Swap_Buffer[i];
		if (bShift == 1){
			unsigned char temp = Display_Buffer[0]; //最後のバイトをシフト用に、第一バイトを保存
			for(j=0; j<num*2-1; j++){
				Display_Buffer[j] = Display_Buffer[j]<<1|((Display_Buffer[j+1]&0x80)?1:0); //ビットシフト
				
			}
			Display_Buffer[num*2-1] = Display_Buffer[num*2-1]<<1|((temp&0x80)?1:0); //最後１ビットをシフト
                        //Shift_Count++;
		}
		
		for (j=0; j<DISP_NUM*2; j++){
			SPI.transfer(~Display_Buffer[DISP_NUM*2-1-j]);
		}
		digitalWrite(LEDARRAY_G, HIGH);		//更新数据时候关闭显示。等更新完数据，打开138显示行。防止重影。
		digitalWrite(LEDARRAY_LAT, HIGH);					//锁存数据
		delayMicroseconds(1);
	
		digitalWrite(LEDARRAY_LAT, LOW);
		delayMicroseconds(1);
		
		
		Scan_Line(i);						//选择第i行

		digitalWrite(LEDARRAY_G, LOW);
		
		delayMicroseconds(300);;		//延时一段时间，让LED亮起来。
		//delay(1000);		
	}	
}

//****************************************************
//扫描某一行
//****************************************************
void Scan_Line( unsigned char m)
{	
	switch(m)
	{
		case 0:			
			digitalWrite(LEDARRAY_D, LOW);digitalWrite(LEDARRAY_C, LOW);digitalWrite(LEDARRAY_B, LOW);digitalWrite(LEDARRAY_A, LOW); 					
			break;
		case 1:					
			digitalWrite(LEDARRAY_D, LOW);digitalWrite(LEDARRAY_C, LOW);digitalWrite(LEDARRAY_B, LOW);digitalWrite(LEDARRAY_A, HIGH); 		
			break;
		case 2:					
			digitalWrite(LEDARRAY_D, LOW);digitalWrite(LEDARRAY_C, LOW);digitalWrite(LEDARRAY_B, HIGH);digitalWrite(LEDARRAY_A, LOW); 		
			break;
		case 3:					
			digitalWrite(LEDARRAY_D, LOW);digitalWrite(LEDARRAY_C, LOW);digitalWrite(LEDARRAY_B, HIGH);digitalWrite(LEDARRAY_A, HIGH); 		
			break;
		case 4:
			digitalWrite(LEDARRAY_D, LOW);digitalWrite(LEDARRAY_C, HIGH);digitalWrite(LEDARRAY_B, LOW);digitalWrite(LEDARRAY_A, LOW); 		
			break;
		case 5:
			digitalWrite(LEDARRAY_D, LOW);digitalWrite(LEDARRAY_C, HIGH);digitalWrite(LEDARRAY_B, LOW);digitalWrite(LEDARRAY_A, HIGH); 		
			break;
		case 6:
			digitalWrite(LEDARRAY_D, LOW);digitalWrite(LEDARRAY_C, HIGH);digitalWrite(LEDARRAY_B, HIGH);digitalWrite(LEDARRAY_A, LOW); 		
			break;
		case 7:
			digitalWrite(LEDARRAY_D, LOW);digitalWrite(LEDARRAY_C, HIGH);digitalWrite(LEDARRAY_B, HIGH);digitalWrite(LEDARRAY_A, HIGH); 		
			break;
		case 8:
			digitalWrite(LEDARRAY_D, HIGH);digitalWrite(LEDARRAY_C, LOW);digitalWrite(LEDARRAY_B, LOW);digitalWrite(LEDARRAY_A, LOW); 		
			break;
		case 9:
			digitalWrite(LEDARRAY_D, HIGH);digitalWrite(LEDARRAY_C, LOW);digitalWrite(LEDARRAY_B, LOW);digitalWrite(LEDARRAY_A, HIGH); 		
			break;	
		case 10:
			digitalWrite(LEDARRAY_D, HIGH);digitalWrite(LEDARRAY_C, LOW);digitalWrite(LEDARRAY_B, HIGH);digitalWrite(LEDARRAY_A, LOW); 		
			break;
		case 11:
			digitalWrite(LEDARRAY_D, HIGH);digitalWrite(LEDARRAY_C, LOW);digitalWrite(LEDARRAY_B, HIGH);digitalWrite(LEDARRAY_A, HIGH); 		
			break;
		case 12:
			digitalWrite(LEDARRAY_D, HIGH);digitalWrite(LEDARRAY_C, HIGH);digitalWrite(LEDARRAY_B, LOW);digitalWrite(LEDARRAY_A, LOW); 		
			break;
		case 13:
			digitalWrite(LEDARRAY_D, HIGH);digitalWrite(LEDARRAY_C, HIGH);digitalWrite(LEDARRAY_B, LOW);digitalWrite(LEDARRAY_A, HIGH); 		
			break;
		case 14:
			digitalWrite(LEDARRAY_D, HIGH);digitalWrite(LEDARRAY_C, HIGH);digitalWrite(LEDARRAY_B, HIGH);digitalWrite(LEDARRAY_A, LOW); 		
			break;
		case 15:
			digitalWrite(LEDARRAY_D, HIGH);digitalWrite(LEDARRAY_C, HIGH);digitalWrite(LEDARRAY_B, HIGH);digitalWrite(LEDARRAY_A, HIGH); 		
			break;
		default : break;	
	}
}

//****************************************************
//发送数据
//****************************************************
void Send( unsigned char dat)
{
/*
	unsigned char i;
	digitalWrite(LEDARRAY_CLK, LOW);
	delayMicroseconds(1);;	
	digitalWrite(LEDARRAY_LAT, LOW);
	delayMicroseconds(1);;

	for( i = 0 ; i < 8 ; i++ )
	{
		if( dat&0x01 )
		{
			digitalWrite(LEDARRAY_DI, LOW);	
		}
		else
		{
			digitalWrite(LEDARRAY_DI, HIGH);
		}


		digitalWrite(LEDARRAY_CLK, HIGH);				//上升沿发送数据
			delayMicroseconds(1);;
		digitalWrite(LEDARRAY_CLK, LOW);
			delayMicroseconds(1);;		
		dat >>= 1;
			
	}
*/
		SPI.transfer(dat);
	
}
