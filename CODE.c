#include <PWM.h>
#include <i2c_flex_lcd.c>

#define TRIG PIN_C5 
#define ECHO PIN_C6

int1 tt=0;
int1 tt_ss=0;
float  i;
signed int8 GA,PHANH;
int8 loop;
int32 dem_xung=0;
int16 GOC,toc_do=0,xungcu=0,xungmoi=0;
float DA_GA,DA_PHANH,DA_VL;

void doc_adc(int a)
{
   set_adc_channel(a);
   delay_us(20);
   float b = read_adc();
   b = 20*b/1023;
   if(a==3) DA_GA=b; 
   if(a==2) DA_PHANH=b; 
   if(a==4) DA_VL=b;
}

void pwm(int a,int b)
{
   set_pwm1_duty(a);
   set_pwm2_duty(b);
}

#int_EXT
void ngat_ext()
{
   dem_xung++;
}


#int_TIMER0
void ngat_timer0()
{
   loop++;
   if(loop == 10)
   {
   if(tt_ss==1)
   output_toggle(PIN_B5);
   loop=0;
   toc_do = (dem_xung*625)/64; // 60/96 * thoi gian lay mau (64000 micro s)
   toc_do = toc_do/20;
   }
   dem_xung=0;
   set_timer0(6);
}


#INT_TIMER1
void servo()
{
      if(tt==0)
      {
         output_high(PIN_D0);
         tt=1;
         xungmoi=65535-1000-GOC;
         if(abs(xungmoi - xungcu)<50)
         {
            set_timer1(xungcu);
         }
         else
         {
            set_timer1(xungmoi);
            xungcu=xungmoi;
         }
      }
      else
      {
         output_low(PIN_D0);
         tt=0;
         xungmoi=65535-1000-GOC;
         if(abs(xungmoi - xungcu)<50)
         {
            set_timer1(xungcu-10000);
         }
         else
         {
            set_timer1(xungmoi-10000);
            xungcu=xungmoi;
         }
      }
}
void bipbip(unsigned int8 hoi,unsigned int8 tieng) 
{ 
   unsigned int8 i, j;
   for(i = 0; i < hoi; i ++){
      for(j = 0; j < tieng; j ++){
         output_high(pin_C7);
         delay_ms(10);
      }
      output_low(pin_C7);
      delay_ms(10);
   }
}
void us_sensor(){
   i=0;
   output_high(TRIG);
   delay_us(10);
   output_low(TRIG);
   set_timer1(0);
   
   while(!input(ECHO) && (get_timer1() <1000));
   set_timer1(0);
   while(input(ECHO) && (i<25000)) i = get_timer1();

   
   if((i/58.82)<=20 && (i/58.82)>10)
   {
      bipbip(1,5);
   }
   if((i/58.82)<=10 && (i/58.82)>5)
   {
      bipbip(5,5);
   }
   if((i/58.82)<=5)
   {
      bipbip(20,5);
   }

}

void main()
{
   set_tris_a(0x2C);    //A5 A3 A2 0010 1100
   set_tris_b(0x9F);    //1001 1111 7 4 3 2 1 0 in
   set_tris_c(0x40);       //0100 0000 INPUT ECHO C6
   set_tris_d(0);       //D0 D6 D7 out
   port_b_pullups(1);
   
   output_high(PIN_A0);
   output_high(PIN_A1);     //SANG DEN A0 A1 0000 0011
   output_low(PIN_B5);
   output_low(PIN_B6);
   output_low(PIN_C7);  //TAT LOA
   pwm(0,0);         //DUNG DONG CO
   output_d(0);         //TAT HET DEN


   setup_adc(ADC_CLOCK_DIV_2);
   setup_adc_ports(AN0_AN1_AN2_AN3_AN4);

   lcd_init(0x4e,16,2);
   lcd_putc('\f');

   setup_timer_0(T0_INTERNAL|T0_DIV_256);
  
   setup_timer_1(T1_INTERNAL | T1_DIV_BY_1);
   
   setup_timer_2(T2_DIV_BY_16,99,1); //0.8ms
   setup_ccp1(CCP_PWM);
   setup_ccp2(CCP_PWM);
   delay_us(10);
   
   ext_int_edge(h_to_l);
   enable_interrupts(int_timer0);
   enable_interrupts(int_timer1);
   enable_interrupts(int_ext);
   enable_interrupts(global);
   xungcu=64935;
   set_timer0(6);
   set_timer1(0);

   while(true)
   {
   if(input(PIN_B7)==0){
      delay_ms(10);
      if(input(PIN_B7)==0){
         output_toggle(PIN_B6);
         output_low(PIN_B5);
         output_low(PIN_D6);
         output_low(PIN_D7);
         tt_ss=~tt_ss;
         if(tt_ss==1)
         {
            lcd_putc('\f');
            lcd_gotoxy(6,1);
            printf(lcd_putc,"HELLO!");
            lcd_gotoxy(1,2);
            printf(lcd_putc,"**START ENGINE**");
            delay_ms(2000);
            lcd_putc('\f');
         }
         else
         {
            pwm(0,0);
            lcd_putc('\f');
            lcd_gotoxy(5,1);
            printf(lcd_putc,"GOODBYE!");
            lcd_gotoxy(1,2);
            printf(lcd_putc,"**STOP ENGINE!**");
            delay_ms(2000);
            lcd_putc('\f');
         }
         pwm(0,0);
         while(input(PIN_B7)==0);
      }
   }
   if(tt_ss)
   {
      
      lcd_gotoxy(1,2);
      printf(lcd_putc,"Speed - %2Ld km/h",toc_do);
      
      doc_adc(3);
      doc_adc(2);
      doc_adc(4);
      
      if(DA_VL<14 && DA_VL>8)
         GOC=400;
      else
      {
         GOC = (int) DA_VL;
         GOC = 40*GOC;
      }
   
      if( DA_GA > 1 )             //dap ga
      {
         if( input(PIN_B2)==0 )        //tien
         {
            output_low(PIN_D7);
            lcd_gotoxy(1,1);
            printf(lcd_putc,"FORWARD");
            
            if( DA_PHANH < 1 )          //chua dap phanh
            {
               output_low(PIN_D6);
               GA = (int) DA_GA;          //bam xung tang toc do
               pwm(0,GA*5);
            }
            else                        // dang dap phanh
            {
               output_high(PIN_D6);
               lcd_gotoxy(1,2);
               printf(lcd_putc,"Speed - %2Ld km/h",toc_do);
               PHANH = (int) DA_PHANH;
               GA = GA - PHANH;
               if(GA < 0) GA=0;
               pwm(0,GA*5);
               delay_ms(200);
            }
         }
         if( input(PIN_B1)==0 )        //lui
         {
            us_sensor();
            output_high(PIN_D7);
            lcd_gotoxy(1,1);
            printf(lcd_putc,"BACKWARD");
            
            if( DA_PHANH < 1 )          //chua dap phanh
            {
               output_low(PIN_D6);
               GA = (int) DA_GA;          //bam xung tang toc do
               pwm(GA*5,0);
            }
            else                        // dang dap phanh
            {
               output_high(PIN_D6);
               lcd_gotoxy(1,2);
               printf(lcd_putc,"Speed - %2Ld km/h",toc_do);
               PHANH = (int) DA_PHANH;
               GA = GA - PHANH;
               if(GA < 0) GA=0;
               pwm(0,GA*5);
               delay_ms(200);
            }
            }
         if( input(PIN_B1)!=0 &&  input(PIN_B2)!=0)
         {
            lcd_gotoxy(1,1);
            printf(lcd_putc,"        ");
            output_low(PIN_D6);
            output_low(PIN_D7);
            pwm(0,0);   
         }
      }
      else 
      {
      lcd_gotoxy(1,1);
      if(input(PIN_B2)==0)
      {
         printf(lcd_putc,"FORWARD ");
         output_low(PIN_D7);
      }
      if(input(PIN_B1)==0)
      {
         printf(lcd_putc,"BACKWARD");
         output_high(PIN_D7);
      }
      if(input(PIN_B1)!=0 &&  input(PIN_B2)!=0)
      {
         printf(lcd_putc,"          ");
         output_low(PIN_D7);
      }
      pwm(0,0);
      }
   }
   }
}


