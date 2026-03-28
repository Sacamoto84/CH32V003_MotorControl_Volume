#ifndef __DEBUG_H
#define __DEBUG_H


#ifdef __cplusplus
extern "C" {
#endif

#include <ch32v00x.h>
#include <stdio.h>


#include "buzzer_tunes.h"

/* UART Printf Definition */
#define DEBUG_UART1_NoRemap 1  // Tx-PD5
#define DEBUG_UART1_Remap1 2   // Tx-PD0
#define DEBUG_UART1_Remap2 3   // Tx-PD6
#define DEBUG_UART1_Remap3 4   // Tx-PC0

/* DEBUG UATR Definition */
#ifndef DEBUG
#define DEBUG DEBUG_UART1_Remap2  // DEBUG_UART1_Remap2
#endif

/* SDI Printf Definition */
#define SDI_PR_CLOSE 0
#define SDI_PR_OPEN 1

#ifndef SDI_PRINT
#define SDI_PRINT SDI_PR_CLOSE
#endif

#define LED_ON GPIO_WriteBit (GPIOC, GPIO_Pin_2, Bit_SET)
#define LED_OFF GPIO_WriteBit (GPIOC, GPIO_Pin_2, Bit_RESET)

#define BUZZER_ON GPIO_WriteBit (GPIOC, GPIO_Pin_1, Bit_SET)
#define BUZZER_OFF GPIO_WriteBit (GPIOC, GPIO_Pin_1, Bit_RESET)

#define VOLUME_ON GPIO_WriteBit (GPIOC, GPIO_Pin_3, Bit_SET)
#define VOLUME_OFF GPIO_WriteBit (GPIOC, GPIO_Pin_3, Bit_RESET)

#define KEY GPIO_ReadInputDataBit (GPIOC, GPIO_Pin_4)

void Delay_Init (void);
void Delay_Us (uint32_t n);
void Delay_Ms (uint32_t n);
void USART_Printf_Init (uint32_t baudrate);
void SDI_Printf_Enable (void);

enum Screen {
    NORMAL,  // 0

    SET_POWER,         // 1
    SET_BOOST_ENABLE,  // 2
    SET_BOOST_POWER,   // 3
    SET_BOOST_TIME,    // 4
};

extern uint64_t millisec;

// main.c
extern uint16_t comandMotorOn;       // Признак того что мотор должен работать

extern uint16_t configBoostEnable;   // Настройка того что будет использоваться буст
extern uint16_t configBoostTime;     // Время буста в ms
extern uint16_t configCurrentPower;  // Текущая мощность 0..100

extern void gotoDeepSleep (void);

extern void delay (int time);

// init.c
extern void init (void);


// pwm.cpp
extern void PWM_SetDuty (uint16_t duty);
extern void PWM_Enable (void);
extern void PWM_Disable (void);


// screen.c

extern void ScreenNormal (void);
extern void ScreenPower (void);
extern void ScreenBoostEnable (void);
extern void ScreenBoostPower (void);
extern void ScreenBoostTime (void);


//motor.cpp
extern void Motor_Start(void);
extern void Motor_Stop(void);
extern void Motor_Tick(void);
extern void Motor_Toggle();
extern int  Motor_isStop(void);

#ifdef __cplusplus
}
#endif

#endif /* __DEBUG_H */
