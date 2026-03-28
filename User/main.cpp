
//
//  TX  [PD6   PD4] SWIO
//  GNG [VSS   PC4] KEY
//  PWM [PA2   PC2] LED
//  VDD [VDD   PC1] BUZZER
//
// ┌────────┬──────────┬──────────┬─────────────────┐
// │ Сигнал │ Вывод 1  │ Вывод 2  │ Описание        │
// ├────────┼──────────┼──────────┼─────────────────┤
// │ TX     │ PD6      │ PD4      │ SWIO            │
// │ GND    │ VSS      │ PC4      │ KEY (кнопка)    │
// │ PWM    │ PA2      │ PC2      │ LED             │
// │ VDD    │ VDD      │ PC1      │ Buzzer (зуммер) │
// └────────┴──────────┴──────────┴─────────────────┘
//

#include <debug.h>
#include "uButton.h"
#include "pwm.hpp"

#include "eeprom.hpp"

uint8_t ADC_Read_Single(uint8_t channel, uint16_t * value);

// Создать handle
// EEPROM_HandleTypeDef heeprom = EEPROM_HANDLE_DEFAULT();

uButton b;
Pwm pwm;

uEeprom eeprom_power;        // Мощность мотора
uEeprom eeprom_boostEnable;  // Настройка того что будет использоваться буст
uEeprom eeprom_boostPower;   //
uEeprom eeprom_boostTime;    // Время буста в ms

int motor_volume;

// uint16_t configCurrentPower = 10;  // Текущая мощность 0..100

uint16_t comandMotorOn = 0;   // Признак того что мотор должен работать
uint16_t comandMotorOff = 0;  // Признак того что мотор должен работать

extern void Motor_Init (void);

extern "C" void tone1 (uint16_t frequency, uint16_t duration_ms);

void TIM1_PWMOut_CH2N_Init (u16 arr, u16 psc, u16 ccp);

void ScreenBoostPower (void);
void ScreenNormal (void);

Screen screen = {Screen::NORMAL};

long map(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


/* Global define */
// Удобные макросы (можно положить в отдельный .h)
#define CLRscr "\033[2J\033[H"
#define FG(color) "\033[38;5;" #color "m"
#define BG(color) "\033[48;5;" #color "m"
#define RESET1 "\033[0m"
#define BOLD "\033[1m"
#define UNDERLINE "\033[4m"

void EXTI_INT_INIT (void) {
    // EXTI_InitTypeDef EXTI_InitStructure = {0};
    //  RCC_APB2PeriphClockCmd (RCC_APB2Periph_AFIO, ENABLE);
    //  EXTI_InitStructure.EXTI_Line = EXTI_Line9;
    //  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Event;
    //  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    //  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    //  EXTI_Init (&EXTI_InitStructure);


    EXTI_InitTypeDef EXTI_InitStructure = {0};
    // GPIO_InitTypeDef GPIO_InitStructure = {0};

    // Включить AFIO для настройки EXTI
    RCC_APB2PeriphClockCmd (RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOC, ENABLE);

    // Настроить PC4 как вход с подтяжкой вверх
    // GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    // GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  // Подтяжка к VCC
    // GPIO_Init(GPIOC, &GPIO_InitStructure);

    // Привязать EXTI4 к порту C (PC0)
    GPIO_EXTILineConfig (GPIO_PortSourceGPIOC, GPIO_PinSource0);

    // Настроить EXTI4
    EXTI_InitStructure.EXTI_Line = EXTI_Line0;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Event;          // EVENT, не Interrupt!
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  // По нажатию (0)
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init (&EXTI_InitStructure);
}

void userEEPROM() {

    printf (".READ CONFIG Power\n");
    eeprom_power.init (0, 0, 100, 50, (char *)"Power");

    printf (".READ CONFIG Boost Enable\n");
    eeprom_boostEnable.init (1, 0, 1, 0, (char *)"Boost Enable");

    printf (".READ CONFIG Boost Power\n");
    eeprom_boostPower.init (2, 0, 100, 10, (char *)"Boost Power");

    printf (".READ CONFIG Boost Time\n");
    eeprom_boostTime.init (3, 0, 1000, 100, (char *)"Boost Time");
}

int main (void) {


   // PWR_EnterSTANDBYMode (PWR_STANDBYEntry_WFI);

    // Разблокируем нормальный двухпроводный отладочный интерфейс навсегда
    // (записывается в опции-байты при первой прошивке)
    // SystemCoreClockUpdate();

    init();

    EXTI_INT_INIT();

    // USART_Printf_Init (115200);
    USART_Printf_Init (460800);

    printf ("\r\n---------------------------------------\r\n");
    printf ("Привет SystemClk:%d\r\n", SystemCoreClock);

    userEEPROM();

    //----
    printf ("-------------------------\r\n");
    printf ("CONFIG Power        : %d %%\r\n", eeprom_power.get());
    printf ("CONFIG Boost Enable : %d\r\n", eeprom_boostEnable.get());
    printf ("CONFIG Boost Power   : %d %%\r\n", eeprom_boostPower.get());
    printf ("CONFIG Boost Time   : %d ms\r\n", eeprom_boostTime.get());

    printf ("-------------------------\r\n");
    //----

    // RCC_ClocksTypeDef RCC_ClocksStatus={0};

    // RCC_LSICmd (ENABLE);
    // while (RCC_GetFlagStatus (RCC_FLAG_LSIRDY) == RESET);
    // PWR_AWU_SetPrescaler (PWR_AWU_Prescaler_128);
    // PWR_AWU_SetWindowValue (10);
    // PWR_AutoWakeUpCmd (ENABLE);
    // PWR_EnterSTANDBYMode (PWR_STANDBYEntry_WFE);

    NVIC_EnableIRQ (SysTick_IRQn);

    SysTick->SR &= ~(1 << 0);
    SysTick->CMP = 1000 - 1;
    SysTick->CNT = 0;
    SysTick->CTLR = 0xb;

    Motor_Init();

    // Восходящая трель - "данные сохранены"
    // Двойной тон с акцентом на втором
    tone1_vol (1000, 40, 70);
    tone1_vol (1500, 80, 100);

    gotoDeepSleep();

    printf ("Go...\r\n");

    static uint8_t currentADCChannel = 2;
    static uint16_t lastADCValue = 0;

    VOLUME_ON;
    while(ADC_Read_Single(currentADCChannel, &lastADCValue)==0){}
    printf("lastADCValue %d", lastADCValue);

    motor_volume = map(lastADCValue, 0, 1023, 30, 50);
 
    while (1) {

        VOLUME_ON;
        ADC_Read_Single(currentADCChannel, &lastADCValue);
        //printf("lastADCValue %d\r\n", lastADCValue);

        motor_volume = map(lastADCValue, 0, 1023, 40, 55);
        //printf("motor_volume %d\r\n", motor_volume);

        Motor_Tick();

        b.tick();

        if (screen == Screen::NORMAL) {
            ScreenNormal();
        }

        if (screen == Screen::SET_POWER) {  // 1
            screen = Screen::NORMAL;
            buzzer_shutdown();
            b.reset();
            LED_OFF;
        }

        if (screen == Screen::SET_BOOST_ENABLE) {  // 2
            ScreenBoostEnable();
            comandMotorOff = 1;
        }

        if (screen == Screen::SET_BOOST_POWER) {  // 3

            comandMotorOff = 1;
            ScreenBoostPower();
        }

        if (screen == Screen::SET_BOOST_TIME) {  // 4
            comandMotorOff = 1;
            ScreenBoostTime();
        }

    }
}

void gotoDeepSleep (void) {

    VOLUME_OFF;

    GPIO_InitTypeDef GPIO_InitStructure = {0};

    // === КРИТИЧЕСКИ ВАЖНО: Отключить отладку ===
    // Это ДОЛЖНО быть первым!
    RCC_APB2PeriphClockCmd (RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig (GPIO_Remap_SDI_Disable, ENABLE);  // Отключить SWD

    // Включить GPIO и PWR
    // RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE);
    // RCC_APB1PeriphClockCmd (RCC_APB1Periph_PWR, ENABLE);

    // === ВСЕ GPIO В АНАЛОГОВЫЙ РЕЖИМ (минимум тока) ===
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;  // ANALOG INPUT - минимум!

                                                   //  GPIO_Init (GPIOA, &GPIO_InitStructure);
    GPIO_Init (GPIOD, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All & ~GPIO_Pin_0;
    GPIO_Init (GPIOC, &GPIO_InitStructure);
    // // === ОТКЛЮЧИТЬ ВСЕ ПЕРИФЕРИЙНЫЕ БЛОКИ ===
    // // APB2
    // RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOA |
    //                             RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO |
    //                             RCC_APB2Periph_ADC1 | RCC_APB2Periph_TIM1 |
    //                             RCC_APB2Periph_SPI1 | RCC_APB2Periph_USART1,
    //                         DISABLE);

    // // APB1
    RCC_APB1PeriphClockCmd (RCC_APB1Periph_TIM2 | RCC_APB1Periph_WWDG | RCC_APB1Periph_I2C1, DISABLE);

    // === ОТКЛЮЧИТЬ HSI (внутренний генератор 24 МГц) ===
    // Это даст основную экономию!
    // RCC_HSICmd (DISABLE);

    printf ("Заснули\r\n");
    // // === ВОЙТИ В STANDBY БЕЗ ВОЗВРАТА ===
    PWR_EnterSTANDBYMode (PWR_STANDBYEntry_WFI);

    printf ("Проснулись\r\n");
    // Возврат
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init (GPIOC, &GPIO_InitStructure);

    // Возврат
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init (GPIOD, &GPIO_InitStructure);

    //GPIO_PinRemapConfig (GPIO_Remap_SDI_Disable, DISABLE);  // Включить SWD
}



// Установка канала
void ADC_SelectChannel(uint8_t channel) {
    ADC_RegularChannelConfig(ADC1, channel, 1, ADC_SampleTime_241Cycles);
}

//Для неблокирующего измерения каналов АЦП
// Запустить замер и забрать результат
// вернет true, если замер готов. Или false, если не готов
uint8_t ADC_Read_Single(uint8_t channel, uint16_t * value) {
    static bool adcConversionStarted = false; // Флаг того, что замер уже начался

    if (adcConversionStarted) {
        if (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET) { // Замер еще не окончен
            return 0;
        }
        else { // Замер окончен, предоставить результат
            *value = ADC_GetConversionValue(ADC1);
            adcConversionStarted = false;
            return 1;
        }
    }
    else { // Замер еще не стартовал
        ADC_SelectChannel(channel); // Выбор канала
        ADC_SoftwareStartConvCmd(ADC1, ENABLE); // Запуск преобразования
        adcConversionStarted = true;
        return 0;
    }
}
