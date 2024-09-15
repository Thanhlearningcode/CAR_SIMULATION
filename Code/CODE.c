#include <PWM.h>
#include <i2c_flex_lcd.c>

#define TRIG PIN_C5 
#define ECHO PIN_C6

// Global variables
int1 triggerState = 0;
int1 engineState = 0;
float adcValue;
signed int8 throttle, brake;
int8 loopCounter;
int32 pulseCount = 0;
int16 angle = 0, speed = 0, previousPulse = 0, currentPulse = 0;
float throttleADC, brakeADC, distanceADC;

// Function to read ADC values
void read_adc(int channel)
{
    set_adc_channel(channel);
    delay_us(20);
    float adcReading = read_adc();
    adcReading = 20 * adcReading / 1023;
    switch (channel)
    {
        case 3: throttleADC = adcReading; break;
        case 2: brakeADC = adcReading; break;
        case 4: distanceADC = adcReading; break;
    }
}

// Function to set PWM duties
void set_pwm_duties(int pwm1, int pwm2)
{
    set_pwm1_duty(pwm1);
    set_pwm2_duty(pwm2);
}

// External interrupt service routine
#int_EXT
void external_interrupt()
{
    pulseCount++;
}

// Timer0 interrupt service routine
#int_TIMER0
void timer0_interrupt()
{
    loopCounter++;
    if (loopCounter == 10)
    {
        if (engineState == 1)
            output_toggle(PIN_B5);
        loopCounter = 0;
        speed = (pulseCount * 625) / 64; // Speed calculation
        speed = speed / 20;
    }
    pulseCount = 0;
    set_timer0(6);
}

// Timer1 interrupt service routine
#INT_TIMER1
void servo_interrupt()
{
    if (triggerState == 0)
    {
        output_high(PIN_D0);
        triggerState = 1;
        currentPulse = 65535 - 1000 - angle;
        if (abs(currentPulse - previousPulse) < 50)
        {
            set_timer1(previousPulse);
        }
        else
        {
            set_timer1(currentPulse);
            previousPulse = currentPulse;
        }
    }
    else
    {
        output_low(PIN_D0);
        triggerState = 0;
        currentPulse = 65535 - 1000 - angle;
        if (abs(currentPulse - previousPulse) < 50)
        {
            set_timer1(previousPulse - 10000);
        }
        else
        {
            set_timer1(currentPulse - 10000);
            previousPulse = currentPulse;
        }
    }
}

// Function to beep a buzzer
void beep(unsigned int8 times, unsigned int8 duration) 
{
    unsigned int8 i, j;
    for (i = 0; i < times; i++)
    {
        for (j = 0; j < duration; j++)
        {
            output_high(PIN_C7);
            delay_ms(10);
        }
        output_low(PIN_C7);
        delay_ms(10);
    }
}

// Function to use ultrasonic sensor
void ultrasonic_sensor()
{
    adcValue = 0;
    output_high(TRIG);
    delay_us(10);
    output_low(TRIG);
    set_timer1(0);
    
    while (!input(ECHO) && (get_timer1() < 1000));
    set_timer1(0);
    while (input(ECHO) && (adcValue < 25000)) 
        adcValue = get_timer1();

    float distance = adcValue / 58.82;

    if (distance <= 20 && distance > 10)
    {
        beep(1, 5);
    }
    if (distance <= 10 && distance > 5)
    {
        beep(5, 5);
    }
    if (distance <= 5)
    {
        beep(20, 5);
    }
}

// Main function
void main()
{
    // Set I/O directions
    set_tris_a(0x2C);    // 0010 1100 - A5 A3 A2
    set_tris_b(0x9F);    // 1001 1111 - Inputs on B7, B4-B0
    set_tris_c(0x40);    // 0100 0000 - Input on C6 (ECHO)
    set_tris_d(0);       // Outputs on D0, D6, D7

    port_b_pullups(1);  // Enable pull-ups on port B
    
    // Initial outputs
    output_high(PIN_A0);
    output_high(PIN_A1); // Turn on LEDs A0 and A1
    output_low(PIN_B5);
    output_low(PIN_B6);
    output_low(PIN_C7); // Turn off buzzer
    set_pwm_duties(0, 0); // Stop motors
    output_d(0);         // Turn off all LEDs

    // Set up peripherals
    setup_adc(ADC_CLOCK_DIV_2);
    setup_adc_ports(AN0_AN1_AN2_AN3_AN4);

    lcd_init(0x4e, 16, 2);
    lcd_putc('\f');

    setup_timer_0(T0_INTERNAL | T0_DIV_256);
    setup_timer_1(T1_INTERNAL | T1_DIV_BY_1);
    setup_timer_2(T2_DIV_BY_16, 99, 1); // 0.8ms
    setup_ccp1(CCP_PWM);
    setup_ccp2(CCP_PWM);
    delay_us(10);
    
    ext_int_edge(H_TO_L);
    enable_interrupts(INT_TIMER0);
    enable_interrupts(INT_TIMER1);
    enable_interrupts(INT_EXT);
    enable_interrupts(GLOBAL);

    previousPulse = 64935;
    set_timer0(6);
    set_timer1(0);

    while (true)
    {
        if (input(PIN_B7) == 0)
        {
            delay_ms(10);
            if (input(PIN_B7) == 0)
            {
                output_toggle(PIN_B6);
                output_low(PIN_B5);
                output_low(PIN_D6);
                output_low(PIN_D7);
                engineState = ~engineState;
                
                if (engineState == 1)
                {
                    lcd_putc('\f');
                    lcd_gotoxy(6, 1);
                    printf(lcd_putc, "HELLO!");
                    lcd_gotoxy(1, 2);
                    printf(lcd_putc, "**START ENGINE**");
                    delay_ms(2000);
                    lcd_putc('\f');
                }
                else
                {
                    set_pwm_duties(0, 0);
                    lcd_putc('\f');
                    lcd_gotoxy(5, 1);
                    printf(lcd_putc, "GOODBYE!");
                    lcd_gotoxy(1, 2);
                    printf(lcd_putc, "**STOP ENGINE!**");
                    delay_ms(2000);
                    lcd_putc('\f');
                }
                
                set_pwm_duties(0, 0);
                while (input(PIN_B7) == 0);
            }
        }

        if (engineState)
        {
            lcd_gotoxy(1, 2);
            printf(lcd_putc, "Speed - %2Ld km/h", speed);
            
            read_adc(3); // Read throttle ADC
            read_adc(2); // Read brake ADC
            read_adc(4); // Read distance ADC
            
            if (distanceADC < 14 && distanceADC > 8)
                angle = 400;
            else
            {
                angle = (int) distanceADC;
                angle = 40 * angle;
            }

            if (throttleADC > 1) // Throttle applied
            {
                if (input(PIN_B2) == 0) // Forward
                {
                    output_low(PIN_D7);
                    lcd_gotoxy(1, 1);
                    printf(lcd_putc, "FORWARD");
                    
                    if (brakeADC < 1) // No brake applied
                    {
                        output_low(PIN_D6);
                        throttle = (int) throttleADC;
                        set_pwm_duties(0, throttle * 5);
                    }
                    else // Brake applied
                    {
                        output_high(PIN_D6);
                        lcd_gotoxy(1, 2);
                        printf(lcd_putc, "Speed - %2Ld km/h", speed);
                        brake = (int) brakeADC;
                        throttle = throttle - brake;
                        if (throttle < 0) throttle = 0;
                        set_pwm_duties(0, throttle * 5);
                        delay_ms(200);
                    }
                }
                if (input(PIN_B1) == 0) // Reverse
                {
                    ultrasonic_sensor();
                    output_high(PIN_D7);
                    lcd_gotoxy(1, 1);
                    printf(lcd_putc, "BACKWARD");
                    
                    if (brakeADC < 1) // No brake applied
                    {
                        output_low(PIN_D6);
                        throttle = (int) throttleADC;
                        set_pwm_duties(throttle * 5, 0);
                    }
                    else // Brake applied
                    {
                        output_high(PIN_D6);
                        lcd_gotoxy(1, 2);
                        printf(lcd_putc, "Speed - %2Ld km/h", speed);
                        brake = (int) brakeADC;
                        throttle
                        throttle = throttle - brake;
                        if (throttle < 0) throttle = 0;
                        set_pwm_duties(0, throttle * 5);
                        delay_ms(200);
                    }
                }
                if (input(PIN_B1) != 0 && input(PIN_B2) != 0)
                {
                    lcd_gotoxy(1, 1);
                    printf(lcd_putc, "        ");
                    output_low(PIN_D6);
                    output_low(PIN_D7);
                    set_pwm_duties(0, 0); // Stop motors
                }
            }
            else 
            {
                lcd_gotoxy(1, 1);
                if (input(PIN_B2) == 0) // Forward
                {
                    printf(lcd_putc, "FORWARD ");
                    output_low(PIN_D7);
                }
                if (input(PIN_B1) == 0) // Reverse
                {
                    printf(lcd_putc, "BACKWARD");
                    output_high(PIN_D7);
                }
                if (input(PIN_B1) != 0 && input(PIN_B2) != 0)
                {
                    printf(lcd_putc, "          ");
                    output_low(PIN_D7);
                }
                set_pwm_duties(0, 0); // Stop motors
            }
        }
    }
}
