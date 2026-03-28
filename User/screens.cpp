#include "debug.h"
#include "uButton.h"

// #include "eeprom_ch32v.h"

#include "eeprom.hpp"

// extern EEPROM_HandleTypeDef heeprom;

extern uButton b;
extern Screen screen;

extern uEeprom eeprom_power;
extern uEeprom eeprom_boostEnable;
extern uEeprom eeprom_boostPower;
extern uEeprom eeprom_boostTime;

void status (int step);
void exit (void);

void beep (int value) {
    delay (1000);
    for (int i = 0; i < value; i++) {
        LED_ON;
        // buzzer_beepboop();
        // buzzer_access_denied();
        // buzzer_notify();
        // buzzer_critical();

        // buzzer_click();

        buzzer_warning();
        LED_OFF;
        delay (200);
    }
}

void ScreenNormal (void) {

    static int step = 0;

    if (Motor_isStop()) {
        LED_OFF;
    } else {
        LED_ON;
    }

    if (b.press()) {
        //printf ("Click\n");
        buzzer_ios_click();
        Motor_Toggle();
    }

    if (b.hold()) {
        printf ("Hold\n");
        buzzer_startup();
        step = 0;
        Motor_Stop();
    }

    if (b.step()) {
        step++;
        //printf ("Step %d\n", step);
        buzzer_ios_click();
        LED_ON;
        delay (5);
        LED_OFF;
    }


    if (b.releaseStep()) {
        //printf ("releaseStep\n");

        if (step == Screen::SET_POWER) {
            screen = Screen::SET_POWER;
            beep (Screen::SET_POWER);
        }

        if (step == Screen::SET_BOOST_ENABLE) {
            screen = Screen::SET_BOOST_ENABLE;
            beep (Screen::SET_BOOST_ENABLE);
        }

        if (step == Screen::SET_BOOST_POWER) {
            screen = Screen::SET_BOOST_POWER;
            beep (Screen::SET_BOOST_POWER);
        }

        if (step == Screen::SET_BOOST_TIME) {
            screen = Screen::SET_BOOST_TIME;
            beep (Screen::SET_BOOST_TIME);
        }
    }

    if (b.release()) {
        //printf ("Release\n");
        LED_OFF;
    }

    if (b.hasClicks()) {
        //printf ("Clicks: %d\n", b.getClicks());
        if (b.getClicks() == 5) {
            buzzer_shutdown();
            buzzer_shutdown();
            buzzer_shutdown();
            buzzer_shutdown();

            __disable_irq();  // отключаем все прерывания
            NVIC_SystemReset();
            while (1);
        }
    }

    if (b.timeout()) {
        //printf ("Timeout\n");
        // buzzer_robot();
        if (Motor_isStop()) {
            gotoDeepSleep();
        }
    }
}

void uniScreen (uEeprom *eeprom, char *title, uint8_t div) {
    int imp = eeprom->get() / div;
    if (b.click()) {
        printf ("Click\r\n");
        buzzer_ios_click();
    }

    if (b.hasClicks()) {
        printf ("%s Clicks: %d\r\n", title, b.getClicks());

        if (b.getClicks() == 1) {
            imp++;
            if (imp > 20) {
                imp = 20;
                beep_Increment_Max();
            } else {
                buzzer_click();
            }
            eeprom->set (imp * div);
            printf ("++ Clicks: %d imp:%d\r\n", b.getClicks(), imp);
        }

        if (b.getClicks() == 2) {
            screen = Screen::NORMAL;
            buzzer_shutdown();
            b.reset();
            LED_OFF;
        }

        if (b.getClicks() == 3) {

            if (imp > 1) {
                imp--;
                buzzer_click();
            } else {
                beep_Decrement_Min();
            }
            eeprom->set (imp * div);
            printf ("-- Clicks: %d imp:%d\r\n", b.getClicks(), imp);
        }

        // Status
        if (b.getClicks() == 4) {
            printf ("Status\r\n");
            delay (1000);
            for (int i = 0; i < imp; i++) {
                LED_ON;
                buzzer_beepboop();
                LED_OFF;
                delay (300);
            }
        }

        // Save
        if (b.getClicks() == 5) {
            beep_Save();
            beep_Save();
            eeprom->save();
        }
    }
}

void ScreenPower() {
    uniScreen (&eeprom_power, (char *)"ScreenPower", 5);
}

void ScreenBoostEnable (void) {

    if (eeprom_boostEnable.get())
        LED_ON;
    else
        LED_OFF;

    if (b.click()) {
        printf ("Click\r\n");
        buzzer_ios_click();
    }

    if (b.hasClicks()) {
        printf ("Clicks: %d\r\n", b.getClicks());

        if (b.getClicks() == 1) {

            if (eeprom_boostEnable.get())
                eeprom_boostEnable.set (0);
            else
                eeprom_boostEnable.set (1);

            buzzer_ok();
        }
        if (b.getClicks() == 2) {
            screen = Screen::NORMAL;
            buzzer_shutdown();
            b.reset();
            LED_OFF;
        }
        // Save
        if (b.getClicks() == 5) {
            beep_Save();
            beep_Save();
            eeprom_boostEnable.save();
        }
    }
}

void ScreenBoostPower (void) {
    uniScreen (&eeprom_boostPower, (char *)"ScreenBoostPower", 5);
}

// configBoostTime;     // Время буста в ms  1-импульс 50ms 1..20 50..1000
void ScreenBoostTime() {
    uniScreen (&eeprom_boostTime, (char *)"ScreenBoostTime", 50);
}

void status (int step) {
    for (int i = 0; i < step; i++) {
        buzzer_ios_click();
    }
}

void exit (void) {
    screen = Screen::NORMAL;
    buzzer_ios_click();
}