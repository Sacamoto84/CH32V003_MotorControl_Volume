#include <debug.h>
#include "pwm.hpp"
#include "eeprom.hpp"

//extern uEeprom eeprom_power;
extern uEeprom eeprom_boostEnable;
extern uEeprom eeprom_boostPower;
extern uEeprom eeprom_boostTime;

extern int motor_volume;

Pwm motorPwm;

// Состояния автомата
typedef enum {
    STATE_IDLE = 0,
    STATE_BOOST,
    STATE_RUNNING
} motor_state_t;

// Команды для мотора
typedef enum {
    CMD_NONE = 0,
    CMD_START,
    CMD_STOP
} motor_cmd_t;

// Внутренние переменные состояния
static motor_state_t motor_state = STATE_IDLE;
static volatile motor_cmd_t motor_cmd = CMD_NONE;
static uint64_t boost_start_time = 0;

// ============================================================================
// PUBLIC API - вызывается извне (обработчики кнопок, UART команды и т.д.)
// ============================================================================

void Motor_Init (void) {
    motorPwm.init (100, 7, 50);
    motorPwm.setDutyPercent (0);
    motorPwm.setFrequency (50);
    motorPwm.disable();
    motor_state = STATE_IDLE;
    motor_cmd = CMD_NONE;
}

// Установить команду START (неблокирующая)
void Motor_Start (void) {
    motor_cmd = CMD_START;
}

// Установить команду STOP (неблокирующая)
void Motor_Stop (void) {
    motor_cmd = CMD_STOP;
}

// Переключить состояние мотора (вкл/выкл)
void Motor_Toggle (void) {
    if (motor_state == STATE_IDLE) {
        motor_cmd = CMD_START;
    } else {
        motor_cmd = CMD_STOP;
    }
}

// Получить текущее состояние
motor_state_t Motor_GetState (void) {
    return motor_state;
}

int Motor_isStop (void) {
    return motor_state == STATE_IDLE;
}

// ============================================================================
// TICK FUNCTION - вызывается в главном цикле
// ============================================================================

void Motor_Tick (void) {
    // Обработка команд в первую очередь
    if (motor_cmd != CMD_NONE) {
        motor_cmd_t cmd = motor_cmd;
        motor_cmd = CMD_NONE;  // Сбрасываем команду сразу

        switch (cmd) {
        case CMD_START:
            if (motor_state == STATE_IDLE) {
                motorPwm.enable();

                if (eeprom_boostEnable.get() == 0) {
                    // Буст выключен - сразу на рабочую мощность
                    motorPwm.setFrequency (1000);
                    int p = motor_volume;
                    if (p > 100)
                        p = 100;
                    motorPwm.setDutyPercent (p);
                    motor_state = STATE_RUNNING;
                } else {
                    // Буст включен - стартуем с boost мощности
                    motorPwm.setFrequency (50);
                    int pp = motor_volume + eeprom_boostPower.get();
                    if (pp > 100)
                        pp = 100;
                    motorPwm.setDutyPercent (pp);
                    boost_start_time = millisec;
                    motor_state = STATE_BOOST;
                }
            }
            break;

        case CMD_STOP:
            // STOP работает из любого состояния
            motorPwm.setDutyPercent (0);
            motorPwm.disable();
            motor_state = STATE_IDLE;
            break;

        default:
            break;
        }
    }

    // Обработка состояний
    switch (motor_state) {
    case STATE_IDLE:
        // Ничего не делаем
        break;

    case STATE_BOOST:
        // Проверяем, истекло ли время буста
        if ((millisec - boost_start_time) >= eeprom_boostTime.get()) {
            // Переходим на рабочую частоту и мощность
            motorPwm.setFrequency (5000);
            motorPwm.setDutyPercent (motor_volume);
            motor_state = STATE_RUNNING;
        }
        break;

    case STATE_RUNNING:
        // Мотор работает на рабочей мощности
        // Можно добавить мониторинг или другую логику
        int p = motor_volume;
        if (p > 100)  p = 100;
        motorPwm.setDutyPercent (p);
        break;
    }
}

// ============================================================================
// OPTIONAL: Изменение рабочей мощности на лету
// ============================================================================

void Motor_SetPower (uint8_t power_percent) {
    if (motor_state == STATE_RUNNING) {
        motorPwm.setDutyPercent (power_percent);
    }
}