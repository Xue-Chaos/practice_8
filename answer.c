/* ����ͷ�ļ� */
#include<iocc2530.h>
#include<string.h>

/*�궨��*/
#define LED1 P1_0
#define LED2 P1_1
#define SW1  P1_2
#define uint16 unsigned short
/*�������*/
int count=0;//ͳ�ƶ�ʱ���������
char output[8];//���ת�����ַ���ʽ�Ĵ���������
uint16 light_val;//ADC�ɼ����
int Sw1_Count = 0;//�������µĴ���

/*��������*/
void InitCLK(void);//ϵͳʱ�ӳ�ʼ��������Ϊ32MHz
void InitUART0( );//����0��ʼ��
void InitT1( );//��ʱ��1��ʼ��
unsigned short Get_adc( );//ADC�ɼ�
void Uart_tx_string(char *data_tx,int len);//�����ڷ���ָ�����ȵ�����
void InitLED(void);//�Ƶĳ�ʼ��
void InitSW1(void);//������ʼ��

/*���庯��*/
void InitCLK(void)
{
  CLKCONCMD &= 0x80;
  while(CLKCONSTA & 0x40);
}

void InitLED()
{
  P1SEL &= ~0x03;//����P1_0��P1_1ΪGPIO��
  P1DIR |= 0x03;//����P1_0��P1_1Ϊ���
  LED1 = LED2 = 0;//����LED1��LED2�ĳ�ʼ״̬
}

void InitSW1()
{
  P1SEL &= ~0X04;       //����SW1Ϊ��ͨIO��
  P1DIR &= ~0X04;       //����SW1Ϊ�������� 
  
  P1INP &= ~0X04;       //����SW1Ϊ������ģʽ
  P2INP &= ~0x40;       //����SW1�����˿�Ϊ����    
  IEN2 |=0X10;          //ʹ��SW1�˿����ж�Դ     
  P1IEN|=0X04;          //ʹ��SW1�˿��ⲿ�ж�    
  PICTL|=0X02;          //�½��ش�����PICTL�жϱ�Ե�Ĵ���
}

void InitT1()
{
    T1CTL |= 0X09;//32��Ƶ����������ģʽ ��1001��
    IEN1 |=0X02;//��ʱ��1�ж�ʹ�� ��TIMIF|=0X40
    EA = 1; 
}

uint16 Get_adc( )
{
    APCFG |= 0x01;//����P0_0Ϊģ��˿�
    ADCCON3 = 0x80 | 0x20 | 0x00;//��0xA0;���òο���ѹ3.3V 256��Ƶ ʹ��AIN0ͨ��
     
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
   U0CSR |=0X80;//����ģʽ
   PERCFG |= 0x00;//USART0ʹ�ñ���λ��1 P0_2 P0_3
   P0SEL |= 0X3C;//����P0_2 P0_3Ϊ����
   U0UCR |= 0X80;//������ 8λ����λ ����żУ�� 1λֹͣλ

   U0GCR = 10; //���ò�����Ϊ38400 �������϶�Ӧ��
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
    T1IF = 0;//�����ʱ��1�жϱ�־
    
    
   if(Sw1_Count == 1)//��һ�ΰ��°���
   {
       /*.......������5��ʼ����һ�ΰ��°���SW1�������1��ѹ����������ݷ��͵�����
      ����LED1,LED2ÿ1����˸һ��....................*/
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
       /*.......������5����.......................................*/
    }
    else if(Sw1_Count == 2)//�ڶ��ΰ��°���
    {
      /*.......������6��ʼ���ڶ��ΰ��°���SW1�������3��ѹ����������ݷ��͵�����
      ����LED1,LED2ÿ3����˸һ��....................*/
      count++; //�ۼ��жϴ���
      if(count>=45)// ��ʱ3�뵽
      {
          LED1 = ~LED1;
          LED2 = ~LED2;
          count = 0;     
          light_val = Get_adc();
          light_val = Get_adc();
          sprintf(output, "%.2f", light_val);
          Uart_tx_string(output, sizeof(output));
      }
      /*.......������6����.......................................*/
    }
}

/************����SW1�жϷ����ӳ���**************/
#pragma vector=P1INT_VECTOR //��P1���ж�
__interrupt void EXTI1_ISR()
{
    if (P1IFG & 0X04) //����SW1����ʱ
    {
        if (SW1 == 0)//ȷʵ��SW1��ť�������ⲿ�ж�
        {
            if (Sw1_Count == 0)//��һ�ΰ��°���
            {
                Uart_tx_string("��ʼ1��ѭ���ɼ�", sizeof("��ʼ1��ѭ���ɼ�"));
                LED1 = 0;
                LED2 = 0;
                count = 0;
                Sw1_Count++;
            }
            else if (Sw1_Count == 1)//�ڶ��ΰ��°���
            {
                Uart_tx_string("��ʼ3��ѭ���ɼ�", sizeof("��ʼ3��ѭ���ɼ�"));
                LED1 = 0;
                LED2 = 0;
                count = 0;
                Sw1_Count++;
            }
            else if (Sw1_Count == 2)//�����ΰ��°���
            {
                Uart_tx_string("ֹͣ�ɼ�", sizeof("ֹͣ�ɼ�"));
                LED1 = 0;
                LED2 = 0;
                count = 0;
                Sw1_Count = 0;
            }

        }
    }
  
    //ע������ж�ʱ�������ֵ��Ϊ1��ִ�����жϺ���ؼǵ�����Ĳ�����Ҫ��0
    //�жϱ�־λ��0
    P1IFG&=~(0X1<<2);//��SW1�жϱ�־
    IRCON2&=~(0x1<<3);  //��P1�˿����жϱ�־ ��3λΪ0����˿�1���жϱ�־λ�����  P1IF=0;
}

void main( )
{
    InitCLK();//ϵͳʱ��32M
    InitLED();//�Ƶĳ�ʼ��  
    InitSW1();//������ʼ��
    InitUART0();//���ڳ�ʼ��
    InitT1();//��ʱ����ʼ��
    while(1)
    {
    }
  }