#include<reg51.h>
#include<intrins.h>
#include<stdlib.h>
typedef unsigned int u16;
typedef unsigned char u8;
//74HC595串行输入
sbit SER=P3^4;
sbit RCLK=P3^5;
sbit SRCLK=P3^6;
//方向键
sbit up=P3^1;
sbit down=P3^0;
sbit lt=P3^2;
sbit rt=P3^3;

u8	code Hc595_Coordx[]={0x7f,0xbf,0xdf,0xef,0xf7,0xfb,0xfd,0xfe};
u8	code Hc595_Coordy[]={0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};
u8	 foodx;
u8   foody;
u8   snake_x[20]={0};
u8   snake_y[20]={0};
u8 length;
u8 direction;
void initSnake();
void delay(u16 i);
void Hc595SendByte(u8 dat);
void move();
void Keypros();
void creatFood();
void snake_Grow(void);
void drawsnake();



void initSnake()
{
  	snake_x[0]=0;
	snake_y[0]=0;
	length=1;
	direction=2;
}
void delay(u16 i)
{
  while(i--);
}
void Hc595SendByte(u8 dat)
{
	u8 a;
	SRCLK=0;
	RCLK=0;
	for(a=0;a<8;a++)
	{
		SER=dat>>7;
		
        dat<<=1;
        // "<<="左移运算符，将dat左移一位，相当于将dat除以2
		SRCLK=1;
        //此处两个nop_()延时，延时时间根据需要调整，具体延时时间=12/fosc*2*（nop_()个数），原理是SRCLK上升沿时，数据从SER移入74HC595
		_nop_();
		_nop_();
		SRCLK=0;	
	}

	RCLK=1;
	_nop_();
	_nop_();
	RCLK=0;
}



void move()
{
     
    u8 len = length- 1;
    for(len; len > 0; len--)
    {
        snake_x[len] = snake_x[len - 1];
        snake_y[len] = snake_y[len - 1];	//一直到蛇头
    }
    switch(direction)
    {
    case 1:
        snake_y[0]--;
        break;
    case 2:
        snake_y[0]++;						 //再利用蛇头转变方向
        break;
    case 3:
        snake_x[0]--;
        break;
    case 4:
        snake_x[0]++;
        break;
    default:
        break;
    }

}


//贪吃蛇的上下左右行动控制
void Keypros()
{
	if(up==0)
	{
        P2 = 0xfe;
		delay(1000);  //消抖处理
		if(up==0)
		{
		  if(direction==3|direction==4)
		  direction=1;             //在地址1内写入数据num
		}
		while(!up);
	}
	if(down==0)
	{
		delay(1000);  //消抖处理
		if(down==0)
		{	if(direction==3|direction==4)
			direction=2;	  
		}
		while(!down);
	}
	if(lt==0)
	{
		delay(100);  //消抖处理
		if(lt==0)
		{
		    if(direction==1|direction==2)
			direction=3; 
		}
		while(!lt);
	}
	if(rt==0)
	{
		delay(1000);  //消抖处理
		if(rt==0)
		{
		   if(direction==1|direction==2)
		   direction=4; 
		}
		while(!rt);
	}		
}
void creatFood()	 //随机创造食物
{
    u8 i;
    foodx=rand()%8;
    foody=rand()%8;
    for(i=0; i<length; i++)
    {
        if((foodx==snake_x[i])&&(foody==snake_y[i]))	//如果食物与蛇的身体重叠，要重新创建
                {
                    creatFood();
                }       
    }
}
void snake_Grow(void)
{
    if(snake_x[0] == foodx && snake_y[0] == foody)
    {
        creatFood();
        snake_x[length] = snake_x[length - 1];
        snake_y[length] = snake_y[length - 1];
        length++;
    }
}



void drawsnake()   //点亮蛇和食物
{
     u8	i;
     for(i=0;i<length;i++)
	{
	  	Hc595SendByte(Hc595_Coordy[snake_y[i]]);
		  P0=Hc595_Coordx[snake_x[i]];
        delay(20);
		P0=0xff;

	    Hc595SendByte(Hc595_Coordy[foody]);   
	    P0=Hc595_Coordx[foodx];
        delay(20);
	    P0=0xff;		
	}
}

 void JudgeDeath(void)
{
    u8 i;

    // 判断蛇撞墙死亡
    if((snake_x[0]>7)||(snake_y[0]>7))
    {
         Hc595SendByte(0x00);
        while(1);
    }

    // 判断蛇撞到自己身体死亡
    for(i=4; i<length; i++)
    {
        if((snake_x[0]==snake_x[i])&&(snake_y[0]==snake_y[i]))
        {
            P0=0xFF;
            while(1);
        }
    }
}


void Timer0Init()
{
	TMOD|=0X01;//选择为定时器0模式，工作方式1，仅用TR0打开启动。
    //"|="  表示，如果该位为1，则该位不变，如果该位为0，则该位变为1，
    //例如：71 |= 3，结果为75，用位表示为 01001111 |= 00000011，结果为01001111

    //"&="  表示，如果该位为1，则该位变为0，如果该位为0，则该位不变
    //"~="  名称为取反赋值，如果该位为1，则该位变为0，如果该位为0，则该位变为1
    //"=~"  名称为按位取反，如果该位为1，则该位变为0，如果该位为0，则该位变为1
    //"+="  表示
    //"-="  表示
	TH0=0XD8;	//给定时器赋初值，定时10ms
	TL0=0XF0;	
	ET0=1;//打开定时器0中断允许
	EA=1;//打开总中断
	TR0=1;//打开定时器			
}

void main()
{   
   
	Timer0Init();
	initSnake();	
	while(1)
	{ 
	 drawsnake();
	 JudgeDeath();
	 snake_Grow();
	 Keypros();
	}
}

void Timer0() interrupt 1
{
	static u16 i;
	TH0=0XD8;	//给定时器赋初值，定时10ms
	TL0=0XF0;
	i++;
	if(i==50)
	{
	 i=0;
     move();
	
	}	
}











