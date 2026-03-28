#include <debug.h>

void init (void) {

    ADC_InitTypeDef  ADC_InitStructure = {0};
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    NVIC_PriorityGroupConfig (NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();
    Delay_Init();
    Delay_Ms (5000);

    RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE);
    RCC_APB1PeriphClockCmd (RCC_APB1Periph_PWR, ENABLE);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_ADCCLKConfig(RCC_PCLK2_Div8);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init (GPIOA, &GPIO_InitStructure);

    // GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 |  GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    // GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All & ~GPIO_Pin_4;
    // GPIO_Init (GPIOD, &GPIO_InitStructure);

    //GPIO_Init (GPIOC, &GPIO_InitStructure);

    // LED
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init (GPIOC, &GPIO_InitStructure);

    // BUZZER
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init (GPIOC, &GPIO_InitStructure);

    // VOLUME
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init (GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // KEY
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init (GPIOC, &GPIO_InitStructure);

    // BUZZER_ON;
    LED_ON;
    Delay_Ms (5);
    LED_OFF;


    ADC_DeInit(ADC1);
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;                   // Независимый режим
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;                        // Одиночное преобразование
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;                  // Одиночный режим (не непрерывный)
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;  // Запуск по софту
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;               // Выравнивание по правому краю
    ADC_InitStructure.ADC_NbrOfChannel = 1;                              // 1 канал
    ADC_Init(ADC1, &ADC_InitStructure);

    // Включение АЦП
    ADC_Cmd(ADC1, ENABLE);
    // Калибровка АЦП (обязательно!)
    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1); // И еще раз (так делают)
    while(ADC_GetCalibrationStatus(ADC1));

}