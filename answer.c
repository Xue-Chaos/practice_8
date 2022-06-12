#include <ioCC2530.h>  

#define D3 P1_0     // P1_0定义为P1_0
#define D4 P1_1     // P1_1定义为P1_1
#define D5 P1_3     // P1_2定义为P1_3
#define D6 P1_4     // P1_3定义为P1_4
#define SW1  P1_2

char data[]="现在LED灯处于微亮状态！";
char data1[]="现在LED灯处于显示数据状态！";
char data2[]="现在LED灯处于熄灭状态!";

unsigned char C_AN;         //长时间按键不松开标志
unsigned char ZC_AN_2;      //正常按键第二下标志
unsigned char count;        //键值
unsigned char count_DSQ;    //定时器计数器

void delay(int delaytime)
{
  int i=0,j=0;
  for(i=0;i<240;i++)
    for(j=0;j<delaytime;j++);
}

void initLed()
{
/*********答题区1开始：所有的LED灯不亮******************************/  
 // P1SEL&=~0x1F;  //P1.0~P1.4设置为GPIO
  P1DIR|=0x1B;   //P1.0 P1.1 P1.3 P1.4 设置为输出
 // P1DIR&=~0x04;  //P1.2设置为输入
  D3=D4=D5=D6=0;     //所有LED灯上电处于熄灭状态 P1 = 0X00
/*********答题区1结束******************************/ 
}

void initial_usart_tx()
{
    PERCFG = 0X00;
    P0SEL |= 0X3C;
    P2DIR &= ~0XC0;
    U0CSR = 0X80;
/*********答题区5开始：波特率为115200******************************/   
    U0GCR = 11;
    U0BAUD = 216;      //设置串口波特率为115200
/*********答题区5******************************/    
    UTX0IF = 0;
}

void initial_t1()     //定时器1设置
{
/*答题区6开始：128分频、正计数/倒计数模式、比较值T1CC0为62500,并开启总中断*/   
    T1CC0L = 62500&0xFF;//或T1CC0L = 0X24; 计算定时0.25秒250毫秒//实际定时500毫秒
    T1CC0H = ((62500&0xFF00)>>8);//或T1CC0H = 0XF4;     
    T1CTL |= 0X0F;    //启动定时器1，128分频，正计数倒计数模式
//    TIMIF |= 0X40;    //使能了定时器1的溢出中断允许 
    EA = 1;            //使能单片机总中断
/*********答题区6结束******************************/         
}

void uart_tx_string(char *data_tx, int len)
{
    unsigned int j;
    for(j=0;j<len;j++)
    {
        U0DBUF = *data_tx++;
        while(UTX0IF == 0);
        UTX0IF = 0;
    }  
}

#pragma vector = T1_VECTOR
__interrupt void TI_ISR(void)
{
    IRCON = 0X00;
    if(C_AN == 1)        //流水灯，每秒变化
    {
/*题区7开始：按照D4->D3->D6->D5的顺序,每隔1秒依次点亮一个灯*/
        if(count_DSQ == 2)
        {
            D4 = 1;
            D3 = 0;
            D6 = 0;
            D5 = 0;
        }  
        if(count_DSQ == 4)
        {
            D4 = 0;
            D3 = 1;
            D6 = 0;
            D5 = 0;
        }
        if(count_DSQ == 6)
        {
            D4 = 0;
            D3 = 0;
            D6 = 1;
            D5 = 0;
        }
        if(count_DSQ == 8)
        {
            D4 = 0;
            D3 = 0;
            D6 = 0;
            D5 = 1;
            count_DSQ = 0;
        }
    }   
/************答题区7结束***************/
    count_DSQ++;
    
    if(ZC_AN_2 == 1)    //0x0~0xf显示，每秒加1
    {
        if(count_DSQ == 2)
        {
            D4 = 0;
            D3 = 0;
            D6 = 0;
            D5 = 0;
        }  
        if(count_DSQ == 4)
        {
            D4 = 0;
            D3 = 0;
            D6 = 0;
            D5 = 1;
        }
        if(count_DSQ == 6)
        {
            D4 = 0;
            D3 = 0;
            D6 = 1;
            D5 = 0;
        }
        if(count_DSQ == 8)
        {
            D4 = 0;
            D3 = 0;
            D6 = 1;
            D5 = 1;
        }
        if(count_DSQ == 10)
        {
            D4 = 0;
            D3 = 1;
            D6 = 0;
            D5 = 0;
        }  
        if(count_DSQ == 12)
        {
            D4 = 0;
            D3 = 1;
            D6 = 0;
            D5 = 1;
        }
        if(count_DSQ == 14)
        {
            D4 = 0;
            D3 = 1;
            D6 = 1;
            D5 = 0;
        }
        if(count_DSQ == 16)
        {
            D4 = 0;
            D3 = 1;
            D6 = 1;
            D5 = 1;
        }
        if(count_DSQ == 18)
        {
            D4 = 1;
            D3 = 0;
            D6 = 0;
            D5 = 0;
        }  
        if(count_DSQ == 20)
        {
            D4 = 1;
            D3 = 0;
            D6 = 0;
            D5 = 1;
        }
        if(count_DSQ == 22)
        {
            D4 = 1;
            D3 = 0;
            D6 = 1;
            D5 = 0;
        }
        if(count_DSQ == 24)
        {
            D4 = 1;
            D3 = 0;
            D6 = 1;
            D5 = 1;
        }
        if(count_DSQ == 26)
        {
            D4 = 1;
            D3 = 1;
            D6 = 0;
            D5 = 0;
        }  
        if(count_DSQ == 28)
        {
            D4 = 1;
            D3 = 1;
            D6 = 0;
            D5 = 1;
        }
        if(count_DSQ == 30)
        {
            D4 = 1;
            D3 = 1;
            D6 = 1;
            D5 = 0;
        }
        if(count_DSQ == 32)
        {
            D4 = 1;
            D3 = 1;
            D6 = 1;
            D5 = 1;
            count_DSQ = 0;
        }
    }
  
}

void main(void)
{
    CLKCONCMD &= ~0X7F;        //0X80 选择了一个32MHz的时钟源
    while(CLKCONSTA & 0X40);   //等待晶振稳定
    initial_usart_tx();
    initial_t1();             //调用定时器设置函数
    initLed();
    while(1)
    {
      
        if(SW1 == 0)
        {
            delay(30);
            if(SW1 == 0)
            {
                D4 = 0;D3 = 0;D6 = 0;D5 = 0;  //熄灭灯
                T1IE = 1;                     //开定时器
                C_AN = 1;                     //开长按不松开标志
                ZC_AN_2 = 0;                  //清理按键2处理标志
                count_DSQ = 0;                //定时器计数器清0
                while(!SW1);                  //等待键放松
                T1IE = 0;                     //键放松后关定时器
                count_DSQ = 0;                //定时器计数器清0
                C_AN = 0;                     //键放松后长按键标志清0
                count++;	              //记录按键次数
                switch(count) 
                {
                    case 1: 
                      
                      /*答题区2开始：串口发送“现在LED灯处于微亮状态！”*/
                      uart_tx_string(data, sizeof(data));
                      D4 = 1;D3 = 1;D6 = 1;D5 = 1;
                      delay(1);
                      D4 = 0;D3 = 0;D6 = 0;D5 = 0;
                      delay(39);
                      break;
                      /***************答题区2结束**********************/
                    case 2: 
                      /*答题区3开始：先让所有的LED灯不亮，使能定时器1中断，
                      使用LED灯实现每1秒加1显示0x0~0xF，
                      并串口发送“现在LED灯处于显示数据状态！”*/
                      D4 = 0;D3 = 0;D6 = 0;D5 = 0;
                      T1IE = 1;
                      ZC_AN_2 = 1;
                      uart_tx_string(data1, sizeof(data1));
                      break;
                      /********************答题区3结束**********************/
                    default:
                      /*答题区4开始：键值清零，禁止定时器1中断，
                      串口发送“现在LED灯处于熄灭状态！”*/
//                      P1=0X00;
                      count=0;
                      T1IE = 0;
                      uart_tx_string(data2, sizeof(data2));
                      break; 
                      /************答题区4结束**************/
                }
            }
        }
        if(count == 1)  //微亮
        {
            D4 = 1;
            D3 = 1;
            D6 = 1;
            D5 = 1;
            delay(1);
            D4 = 0;
            D3 = 0;
            D6 = 0;
            D5 = 0;
            delay(39);
        }        
    }  
}