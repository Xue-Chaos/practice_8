/* 包含头文件 */
#include<iocc2530.h>
#include<string.h>

/*宏定义*/
#define LED1 P1_0
#define LED2 P1_1
#define SW1  P1_2
#define uint16 unsigned short
/*定义变量*/
int count=0;//统计定时器溢出次数
char output[8];//存放转换成字符形式的传感器数据
uint16 light_val;//ADC采集结果
int Sw1_Count = 0;//按键按下的次数

/*声明函数*/
void InitCLK(void);//系统时钟初始化函数，为32MHz
void InitUART0( );//串口0初始化
void InitT1( );//定时器1初始化
unsigned short Get_adc( );//ADC采集
void Uart_tx_string(char *data_tx,int len);//往串口发送指定长度的数据
void InitLED(void);//灯的初始化
void InitSW1(void);//按键初始化

/*定义函数*/
void InitCLK(void)
{
  CLKCONCMD &= 0x80;
  while(CLKCONSTA & 0x40);
}

void InitLED()
{
  P1SEL &= ~0x03;//设置P1_0、P1_1为GPIO口
  P1DIR |= 0x03;//设置P1_0和P1_1为输出
  LED1 = LED2 = 0;//设置LED1和LED2的初始状态
}

void InitSW1()
{
  P1SEL &= ~0X04;       //设置SW1为普通IO口
  P1DIR &= ~0X04;       //设置SW1为输入引脚 
  
  P1INP &= ~0X04;       //设置SW1为上下拉模式
  P2INP &= ~0x40;       //设置SW1所属端口为上拉    
  IEN2 |=0X10;          //使能SW1端口组中断源     
  P1IEN|=0X04;          //使能SW1端口外部中断    
  PICTL|=0X02;          //下降沿触发；PICTL中断边缘寄存器
}

void InitT1()
{
    T1CTL |= 0X09;//32分频，自由运行模式 （1001）
    IEN1 |=0X02;//定时器1中断使能 或TIMIF|=0X40
    EA = 1; 
}

uint16 Get_adc( )
{
    APCFG |= 0x01;//设置P0_0为模拟端口
    ADCCON3 = 0x80 | 0x20 | 0x00;//或0xA0;设置参考电压3.3V 256分频 使用AIN0通道
     
    while(!ADCIF);
    ADCIF=0;
    unsigned long value;
    value = ADCH;
    value = value<<8;
    value |=ADCL;
    value = value*330;
    value = value>>15;
    return (uint16)value;
}
void InitUART0( )
{
   U0CSR |=0X80;//串口模式
   PERCFG |= 0x00;//USART0使用备用位置1 P0_2 P0_3
   P0SEL |= 0X3C;//设置P0_2 P0_3为外设
   U0UCR |= 0X80;//流控无 8位数据位 无奇偶校验 1位停止位

   U0GCR = 10; //设置波特率为38400 （见书上对应表）
   U0BAUD = 59;
  
    UTX0IF = 0;    
    EA = 1;   
}

void Uart_tx_string(char *data_tx,int len)  
{   unsigned int j;  
    for(j=0;j<len;j++)  
    {   U0DBUF = *data_tx++;  
        while(UTX0IF == 0);  
        UTX0IF = 0;   
    }
} 



#pragma vector = T1_VECTOR
__interrupt void t1( )
{
    T1IF = 0;//清除定时器1中断标志
    
    
   if(Sw1_Count == 1)//第一次按下按键
   {
       /*.......答题区5开始：第一次按下按键SW1，处理隔1秒把光敏传感数据发送到串口
      ，且LED1,LED2每1秒闪烁一次....................*/
       count++;
       if (count == 15)
       {
           LED1 = ~LED1;
           LED2 = ~LED2;
           count = 0;
           light_val = Get_adc();
           sprintf(output, "%.2f", light_val);
           Uart_tx_string(output, sizeof(output));
       }
       /*.......答题区5结束.......................................*/
    }
    else if(Sw1_Count == 2)//第二次按下按键
    {
      /*.......答题区6开始：第二次按下按键SW1，处理隔3秒把光敏传感数据发送到串口
      ，且LED1,LED2每3秒闪烁一次....................*/
      count++; //累加中断次数
      if(count>=45)// 定时3秒到
      {
          LED1 = ~LED1;
          LED2 = ~LED2;
          count = 0;     
          light_val = Get_adc();
          light_val = Get_adc();
          sprintf(output, "%.2f", light_val);
          Uart_tx_string(output, sizeof(output));
      }
      /*.......答题区6结束.......................................*/
    }
}

/************按键SW1中断服务子程序**************/
#pragma vector=P1INT_VECTOR //第P1组中断
__interrupt void EXTI1_ISR()
{
    if (P1IFG & 0X04) //按键SW1按下时
    {
        if (SW1 == 0)//确实是SW1按钮触发了外部中断
        {
            if (Sw1_Count == 0)//第一次按下按键
            {
                Uart_tx_string("开始1秒循环采集", sizeof("开始1秒循环采集"));
                LED1 = 0;
                LED2 = 0;
                count = 0;
                Sw1_Count++;
            }
            else if (Sw1_Count == 1)//第二次按下按键
            {
                Uart_tx_string("开始3秒循环采集", sizeof("开始3秒循环采集"));
                LED1 = 0;
                LED2 = 0;
                count = 0;
                Sw1_Count++;
            }
            else if (Sw1_Count == 2)//第三次按下按键
            {
                Uart_tx_string("停止采集", sizeof("停止采集"));
                LED1 = 0;
                LED2 = 0;
                count = 0;
                Sw1_Count = 0;
            }

        }
    }
  
    //注意产生中断时会把以下值变为1，执行完中断后务必记得下面的操作：要清0
    //中断标志位清0
    P1IFG&=~(0X1<<2);//清SW1中断标志
    IRCON2&=~(0x1<<3);  //清P1端口组中断标志 第3位为0代表端口1组中断标志位被清除  P1IF=0;
}

void main( )
{
    InitCLK();//系统时钟32M
    InitLED();//灯的初始化  
    InitSW1();//按键初始化
    InitUART0();//串口初始化
    InitT1();//定时器初始化
    while(1)
    {
    }
  }