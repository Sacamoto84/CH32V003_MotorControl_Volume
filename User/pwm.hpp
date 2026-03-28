#pragma once

#ifdef __cplusplus

/* PWM Output Mode Definition */
#define PWM_MODE1 0
#define PWM_MODE2 1

/* PWM Output Mode Selection */
#define PWM_MODE PWM_MODE2

class Pwm {
  public:
    Pwm() { }

    uint16_t arr = 100;
    uint16_t ccp = 50;
    uint16_t psc = 0;

    /*********************************************************************
     * @fn      TIM1_PWMOut_Init
     *
     * @brief   Initializes TIM1 Channel 2N PWM Output on PA2
     *
     * @param   _arr - период (ARR). Например: 100
     *          psc  - предделитель. Например: 479 для ~1kHz при 48MHz
     *          _ccp - заполнение (CCR2). Диапазон: 0..arr
     *
     * @return  none
     */
    void init (uint16_t _arr, uint16_t _psc, uint16_t _ccp) {
        // Защита от нулевого периода
        arr = (_arr == 0) ? 1 : _arr;
        psc = _psc;

        // Ограничение duty cycle
        ccp = (_ccp > arr) ? arr : _ccp;

        GPIO_InitTypeDef GPIO_InitStructure = {0};
        TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = {0};
        TIM_OCInitTypeDef TIM_OCInitStructure = {0};

        /* 1) Включение тактирования */
        RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOA |
                                    RCC_APB2Periph_TIM1,
                                ENABLE);

        /* 2) Настройка PA2 как Alternate Function Push-Pull для TIM1_CH2N */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;  // PA2 = TIM1_CH2N
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init (GPIOA, &GPIO_InitStructure);

        /* 3) Базовая настройка таймера */
        TIM_TimeBaseInitStructure.TIM_Period = arr - 1;  // ARR (0-based)
        TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
        TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
        TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
        TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
        TIM_TimeBaseInit (TIM1, &TIM_TimeBaseInitStructure);

        /* 4) Настройка канала 2 в режиме PWM Mode 1 с N-выходом */
        TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
        TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable;   // Основной выход НЕ используем
        TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;  // Используем N-выход (PA2)
        TIM_OCInitStructure.TIM_Pulse = ccp;
        TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
        TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;  // Прямая полярность для CH2N
        TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
        TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
        TIM_OC2Init (TIM1, &TIM_OCInitStructure);

        /* 5) Включение предзагрузки */
        TIM_OC2PreloadConfig (TIM1, TIM_OCPreload_Enable);
        TIM_ARRPreloadConfig (TIM1, ENABLE);

        /* 6) ВАЖНО: Включаем главный выход MOE для TIM1 */
        TIM_CtrlPWMOutputs (TIM1, ENABLE);

        /* 7) Запуск таймера */
        TIM_Cmd (TIM1, ENABLE);
    }

    /*********************************************************************
     * @fn      setDuty
     *
     * @brief   Изменение скважности PWM (на лету, без остановки таймера)
     *
     * @param   duty - значение CCR2 (0 = 0%, arr = 100%)
     *
     * @return  none
     */
    void setDuty (uint16_t duty) {
        if (duty > arr)
            duty = arr;
        ccp = duty;
        TIM_SetCompare2 (TIM1, duty);  // Применится на следующем обновлении
    }

    /*********************************************************************
     * @fn      setDutyPercent
     *
     * @brief   Установка скважности в процентах
     *
     * @param   percent - 0..100
     *
     * @return  none
     */
    void setDutyPercent (uint8_t percent) {
        if (percent > 100)
            percent = 100;
        uint16_t duty = (arr * percent) / 100;
        setDuty (duty);
    }

    /*********************************************************************
     * @fn      setPrescaler
     *
     * @brief   Изменение предделителя (на лету, без остановки таймера)
     *
     * @param   prescaler - новый предделитель (0-65535)
     *                      Частота PWM = F_CPU / ((prescaler+1) * arr)
     *                      Например: 48MHz / (479+1 * 100) = 1kHz
     *
     * @return  none
     *
     * @note    Изменения применяются мгновенно (TIM_PSCReloadMode_Immediate)
     *          или при обновлении (TIM_PSCReloadMode_Update)
     */
    void setPrescaler (uint16_t prescaler) {
        psc = prescaler;
        // TIM_PSCReloadMode_Immediate - применить сразу (может быть глитч)
        // TIM_PSCReloadMode_Update - применить при след. обновлении (плавно)
        TIM_PrescalerConfig (TIM1, prescaler, TIM_PSCReloadMode_Update);
    }

    /*********************************************************************
     * @fn      setFrequency
     *
     * @brief   Установка частоты PWM (на лету, без остановки таймера)
     *
     * @param   freq_hz - желаемая частота в Герцах (например: 1000 = 1kHz)
     *          f_cpu   - частота процессора в Герцах (по умолчанию 8000000)
     *
     * @return  none
     *
     * @note    Примеры для 8 MHz:
     *          setFrequency(1000)     -> 1 kHz
     *          setFrequency(10000)    -> 10 kHz
     *          setFrequency(80000)    -> 80 kHz (максимум при ARR=100)
     *          setFrequency(20)       -> 20 Hz (низкая частота)
     */
    void setFrequency (uint32_t freq_hz, uint32_t f_cpu = 8000000) {
        if (freq_hz == 0)
            freq_hz = 1;

        // Формула: freq = F_CPU / ((PSC+1) * ARR)
        // PSC = (F_CPU / (freq * ARR)) - 1
        uint32_t prescaler = (f_cpu / (freq_hz * arr)) - 1;

        // Ограничение диапазона
        if (prescaler > 65535)
            prescaler = 65535;

        setPrescaler ((uint16_t)prescaler);
    }

    /*********************************************************************
     * @fn      setPeriod
     *
     * @brief   Изменение периода ARR (на лету, без остановки таймера)
     *
     * @param   period - новое значение ARR (1-65535)
     *
     * @return  none
     *
     * @note    При изменении ARR автоматически корректируется duty cycle,
     *          чтобы сохранить процентное соотношение
     */
    void setPeriod (uint16_t period) {
        if (period == 0)
            period = 1;

        // Сохраняем процентное соотношение duty cycle
        uint8_t duty_percent = (ccp * 100) / arr;

        arr = period;
        TIM1->ATRLR = arr - 1;  // Записываем новый ARR

        // Пересчитываем duty с учётом нового периода
        uint16_t new_duty = (arr * duty_percent) / 100;
        setDuty (new_duty);
    }

    /*********************************************************************
     * @fn      setFrequencyWithPeriod
     *
     * @brief   Изменение частоты через ARR и PSC (максимальное разрешение)
     *
     * @param   freq_hz - желаемая частота в Герцах
     *          resolution - желаемое разрешение duty cycle (100, 1000, 10000)
     *          f_cpu - частота процессора (по умолчанию 48000000)
     *
     * @return  none
     *
     * @note    Автоматически подбирает оптимальные ARR и PSC для заданной частоты
     *          Примеры:
     *          setFrequencyWithPeriod(1000, 100)   -> 1kHz с разрешением 100 шагов
     *          setFrequencyWithPeriod(50, 20000)   -> 50Hz с разрешением 20000 (для серво)
     */
    void setFrequencyWithPeriod (uint32_t freq_hz, uint16_t resolution = 100, uint32_t f_cpu = 48000000) {
        if (freq_hz == 0)
            freq_hz = 1;
        if (resolution == 0)
            resolution = 1;

        // freq = F_CPU / ((PSC+1) * ARR)
        // PSC = (F_CPU / (freq * ARR)) - 1
        uint32_t prescaler = (f_cpu / (freq_hz * resolution)) - 1;

        if (prescaler > 65535) {
            // Если prescaler слишком большой, уменьшаем разрешение
            prescaler = 65535;
            resolution = f_cpu / (freq_hz * (prescaler + 1));
            if (resolution == 0)
                resolution = 1;
        }

        setPeriod (resolution);
        setPrescaler ((uint16_t)prescaler);
    }

    /*********************************************************************
     * @fn      getFrequency
     *
     * @brief   Получить текущую частоту PWM
     *
     * @param   f_cpu - частота процессора в Герцах (по умолчанию 48000000)
     *
     * @return  Частота в Герцах
     */
    uint32_t getFrequency (uint32_t f_cpu = 48000000) {
        return f_cpu / ((psc + 1) * arr);
    }

    void enable() {
       
        if (TIM1->CTLR1 & TIM_CEN) return;

        // 1) Вернуть PA2 в режим AF_PP (для TIM1_CH2N)
        GPIO_InitTypeDef gpio;
        gpio.GPIO_Pin = GPIO_Pin_2;
        gpio.GPIO_Mode = GPIO_Mode_AF_PP;
        gpio.GPIO_Speed = GPIO_Speed_30MHz;
        GPIO_Init (GPIOA, &gpio);

        TIM_CtrlPWMOutputs (TIM1, ENABLE);
        TIM_Cmd (TIM1, ENABLE);
    }

    void disable() {

         if ((TIM1->CTLR1 & TIM_CEN) == 0) return;

        TIM_CtrlPWMOutputs (TIM1, DISABLE);
        TIM_Cmd (TIM1, DISABLE);

        // Переводим PA2 в обычный GPIO и устанавливаем LOW
        GPIO_InitTypeDef gpio;
        gpio.GPIO_Pin = GPIO_Pin_2;
        gpio.GPIO_Mode = GPIO_Mode_Out_PP;
        gpio.GPIO_Speed = GPIO_Speed_10MHz;
        GPIO_Init (GPIOA, &gpio);
        GPIO_WriteBit (GPIOA, GPIO_Pin_2, Bit_RESET);
    }
};

#endif  // __cplusplus