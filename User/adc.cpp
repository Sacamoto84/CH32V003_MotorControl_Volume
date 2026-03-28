#include <debug.h>


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