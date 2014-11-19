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

//IO配置
#define LEDARRAY_D 2
#define LEDARRAY_C 3
#define LEDARRAY_B 4
#define LEDARRAY_A 5
#define LEDARRAY_G 6
#define LEDARRAY_DI 7
#define LEDARRAY_CLK 8
#define LEDARRAY_LAT 9

#define led 13


#define Num_Word 2				//液晶能显示的汉字个数

unsigned char Display_Buffer[4];
unsigned char Display_Swap_Buffer[Num_Word][32]={0};					//显示缓冲区
bool Shift_Bit = 0;
bool Flag_Shift = 0;
unsigned char Timer0_Count = 0;
unsigned char temp = 0x80;
unsigned char Shift_Count = 0;
unsigned char Display_Word_Count = 0;

#define Num_Of_Word 2 
const unsigned char Word[Num_Of_Word][32] = 
{
	 
0xFE,0xFE,0xFE,0xFE,0xF6,0xF2,0xE6,0xEE,0xDE,0xBE,0x7E,0xFE,0xFE,0xFE,0xFA,0xFD,
0xFF,0xFF,0xFF,0xFF,0xBF,0xDF,0xEF,0xE7,0xF3,0xF9,0xFB,0xFF,0xFF,0xFF,0xFF,0xFF,/*"小",0*/

0xFF,0x86,0xF6,0xF6,0x86,0xBF,0xBC,0xBD,0x85,0xF5,0xF4,0xF7,0xF7,0xF7,0xD7,0xEC,
0xFF,0x07,0xF7,0xF7,0x07,0xBF,0x03,0xBB,0xBB,0xBB,0x03,0xBF,0xB7,0xBB,0x81,0x3B,/*"强",1*/

};

const unsigned char Word2[Num_Of_Word][32] = 
{
	 
0x00,0x05,0x00,0x05,0x1f,0xf0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x20,0x00,0x30,0x00,0x1c,0x00,0x07,0xfc,0x00,0x00,0x00,0x00,

0x00,0xc0,0x60,0x60,0x30,0x20,0x10,0x00,0x03,0xfe,0xc0,0x20,0x60,0x20,0x20,0x20,
0x00,0x20,0x13,0xfe,0x10,0x20,0x30,0x20,0x20,0x20,0x60,0x20,0x47,0xff,0xc0,0x00,/*注*/
};

const unsigned char Word3[Num_Of_Word][32] = 
{
	 
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
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

	Clear_Display();
}

void loop()
{
        int val;
        val = Serial.read();
        if(val == 'r'){
          int cont = 20;
          while(cont >= 0){
            int i;
            for(i = 0;i<30;i++){
              Display(Word2);
            }
        
            for(i = 0;i<30;i++){
              Display(Word3);
            }
          cont--;
          }
        }	
}

//************************************************************
//清空缓冲区
//************************************************************
void Clear_Display()
{
	unsigned char i,j;
	for(j = 0 ; j < Num_Word; j++)
	{
		for(i = 0 ; i < 32 ;i++)
		{
			Display_Swap_Buffer[j][i] = 0xff;				//0=显示  1=不显示 	
		}
	}
}

//************************************************************
//计算移动数据，存在在缓冲器
//************************************************************
void Calc_Shift()
{
	unsigned char i;

	for(i = 0;i < 16;i++)
	{
		if((Display_Swap_Buffer[0][16+i]&0x80) == 0)									//前8位	移位
		{
			Display_Swap_Buffer[0][i] = (Display_Swap_Buffer[0][i] << 1)&0xfe; 			//最低位清零	
		}
		else
		{
			Display_Swap_Buffer[0][i] = (Display_Swap_Buffer[0][i] << 1)|0x01;			//最低位置一 	
		}

		if(Shift_Count%16 < 8 && Display_Word_Count < Num_Of_Word)
		{
			Shift_Bit = Word[Display_Word_Count][i]&temp;
		}
		else if(Shift_Count%16 < 16 && Display_Word_Count < Num_Of_Word)
		{
			Shift_Bit = Word[Display_Word_Count][16+i]&temp;
		}
		else
		{
			Shift_Bit = 1;						//把字移出缓冲区
		}

		if( Shift_Bit == 0)														//后8位移位
		{
			Display_Swap_Buffer[0][16+i] = (Display_Swap_Buffer[0][16+i] << 1)&0xfe; 		//最低位清零
		}
		else
		{
			Shift_Bit =	1;
			Display_Swap_Buffer[0][16+i] = (Display_Swap_Buffer[0][16+i] << 1)|0x01;		//最低位置一 			
		}
		
	}
	temp = (temp>>1)&0x7f;
	if(temp == 0x00)
	{
		temp = 0x80;
	}
}
//************************************************************
//num为字数  dat[][32]为字模的名称
//*************************************************************
void Display(const unsigned char dat[][32])					
{
	unsigned char i;

	for( i = 0 ; i < 32 ; i+=2 )
	{
		digitalWrite(LEDARRAY_G, HIGH);		//更新数据时候关闭显示。等更新完数据，打开138显示行。防止重影。
		
		Display_Buffer[0] = dat[0][i];		
		Display_Buffer[1] = dat[0][i+1];
		Display_Buffer[2] = dat[1][i];
		Display_Buffer[3] = dat[1][i+1];

		Send(Display_Buffer[3]);
		Send(Display_Buffer[2]);
		Send(Display_Buffer[1]);
		Send(Display_Buffer[0]);

		digitalWrite(LEDARRAY_LAT, HIGH);					//锁存数据
		delayMicroseconds(1);
	
		digitalWrite(LEDARRAY_LAT, LOW);
		delayMicroseconds(1);

		Scan_Line(i/2);						//选择第i行

		digitalWrite(LEDARRAY_G, LOW);
		
		delayMicroseconds(500);;		//延时一段时间，让LED亮起来。				
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
}
