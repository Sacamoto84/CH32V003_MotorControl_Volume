#include "buzzer_tunes.h"

extern void delay (int time);

void delayUsTone(uint32_t n)
{
    uint32_t i;

    SysTick->SR &= ~(1 << 0);
    i = (uint32_t)n;

    SysTick->CMP = 9999999;
    SysTick->CNT = 0;
    //SysTick->CTLR |=(1 << 0);

    while(SysTick->CNT <= i);
    //SysTick->CTLR &= ~(1 << 0);
    //SysTick->CTLR = 0xB;
    
    SysTick->CNT = 0;
    SysTick->CMP = 999;
    
}

// §¤§Ö§ß§Ö§â§Ñ§è§Ú§ñ §ä§à§ß§Ñ §Ù§Ñ§Õ§Ñ§ß§ß§à§Û §é§Ñ§ã§ä§à§ä§í §Ú §Õ§Ý§Ú§ä§Ö§Ý§î§ß§à§ã§ä§Ú
void tone1(uint16_t frequency, uint16_t duration_ms) {
    if(frequency == 0) {
        BUZZER_OFF;
        delay(duration_ms);
        return;
    }
    
    uint32_t period_us = 1000000UL / frequency;
    uint32_t half_period_us = period_us / 2;
    uint32_t cycles = (uint32_t)duration_ms * 1000UL / period_us;
    
    for(uint32_t i = 0; i < cycles; i++) {
        BUZZER_ON;
        delayUsTone(half_period_us);
        BUZZER_OFF;
        delayUsTone(half_period_us);
    }
}




// §¤§Ö§ß§Ö§â§Ñ§è§Ú§ñ §ä§à§ß§Ñ §ã §Ô§â§à§Þ§Ü§à§ã§ä§î§ð (0-100%)
void tone1_vol(uint16_t frequency, uint16_t duration_ms, uint8_t volume) {
    if(frequency == 0 || volume == 0) {
        BUZZER_OFF;
        delay(duration_ms);
        return;
    }
    
    // §°§Ô§â§Ñ§ß§Ú§é§Ú§ä§î §Ô§â§à§Þ§Ü§à§ã§ä§î 0-100%
    if(volume > 100) volume = 100;
    
    uint32_t period_us = 1000000UL / frequency;
    uint32_t half_period_us = period_us / 2;
    uint32_t cycles = (uint32_t)duration_ms * 1000UL / period_us;
    
    // §£§â§Ö§Þ§ñ ON §Ó §á§â§à§è§Ö§ß§ä§Ñ§ç §à§ä §á§à§Ý§å§á§Ö§â§Ú§à§Õ§Ñ (duty cycle)
    uint32_t on_time_us = (half_period_us * volume) / 100;
    uint32_t off_time_us = half_period_us - on_time_us;
    
    for(uint32_t i = 0; i < cycles; i++) {
        // §±§à§Ý§à§Ø§Ú§ä§Ö§Ý§î§ß§Ñ§ñ §á§à§Ý§å§Ó§à§Ý§ß§Ñ
        BUZZER_ON;
        delayUsTone(on_time_us);
        BUZZER_OFF;
        delayUsTone(off_time_us);
        
        // §°§ä§â§Ú§è§Ñ§ä§Ö§Ý§î§ß§Ñ§ñ §á§à§Ý§å§Ó§à§Ý§ß§Ñ (§á§Ñ§å§Ù§Ñ)
        delayUsTone(half_period_us);
    }
}



















// ------------------------
//   §³§­§µ§¨§¦§¢§¯§½§¦ §©§£§µ§¬§ª
// ------------------------

void buzzer_ok(void) {
    tone1(1000, 80);
    tone1(1500, 80);
}


void buzzer_error(void) {
    tone1(400, 150);
    tone1(200, 200);
}

void buzzer_error_strong(void) {
    tone1(300, 200);
    tone1(0, 50);
    tone1(300, 200);
}

void buzzer_warning(void) {
    tone1(800, 150);
}

void buzzer_warning_double(void) {
    tone1(800, 120);
    tone1(0, 60);
    tone1(800, 120);
}

void buzzer_click(void) {
    tone1(1200, 40);
}

void buzzer_success_long(void) {
    tone1(800, 150);
    tone1(1000, 150);
    tone1(1200, 200);
}

void buzzer_critical(void) {
    for (int i = 0; i < 3; i++) {
        tone1(250, 200);
        tone1(0, 80);
    }
}

void buzzer_beepboop(void) {
    tone1(1000, 60);
    tone1(700, 60);
}

void buzzer_access_denied(void) {
    tone1(600, 70);
    tone1(400, 120);
}

void buzzer_notify(void) {
    tone1(1000, 100);
    tone1(800, 150);
}

// ------------------------
//   §³§´§ª§­§ª§©§°§£§¡§¯§¯§½§¦ §©§£§µ§¬§ª
// ------------------------

void buzzer_ios_click(void) {
    tone1(1800, 25);
}

void buzzer_android_notify(void) {
    tone1(1200, 70);
    tone1(1500, 120);
}

void buzzer_robot(void) {
    tone1(800, 70);
    tone1(1000, 70);
    tone1(600, 90);
}

void buzzer_microwave_done(void) {
    tone1(1000, 200);
    tone1(0, 100);
    tone1(1000, 200);
}

void buzzer_winxp_msg(void) {
    tone1(900, 80);
    tone1(1200, 80);
    tone1(1000, 120);
}


// ------------------------
//     STARTUP
// ------------------------
// §®§ñ§Ô§Ü§Ñ§ñ §Ó§à§ã§ç§à§Õ§ñ§ë§Ñ§ñ §Þ§Ö§Ý§à§Õ§Ú§ñ, §Ü§Ñ§Ü §Ó§Ü§Ý§ð§é§Ö§ß§Ú§Ö §å§ã§ä§â§à§Û§ã§ä§Ó§Ñ
void buzzer_startup(void) {
    tone1(800, 120);
    tone1(1000, 120);
    tone1(1300, 160);
}

// ------------------------
//     SHUTDOWN
// ------------------------
// §°§Ò§â§Ñ§ä§ß§Ñ§ñ, §ß§Ú§ã§ç§à§Õ§ñ§ë§Ñ§ñ
void buzzer_shutdown(void) {
    tone1(1200, 150);
    tone1(900, 150);
    tone1(600, 180);
}

// ------------------------
//     CHARGING
// ------------------------
// §©§Ó§å§Ü "§á§Ú§ß§Ô", §Ü§Ñ§Ü §á§à§Õ§Ü§Ý§ð§é§Ö§ß§Ú§Ö §á§Ú§ä§Ñ§ß§Ú§ñ
void buzzer_charging(void) {
    tone1(1500, 80);
    tone1(1800, 150);
}










// === §¥§°§³§´§ª§¤§¯§µ§´ §®§¡§¬§³§ª§®§µ§® ===
void beep_Increment_Max(void) {
    // §©§Ó§å§Ü "§Ò§à§Ý§î§ê§Ö §ß§Ö§Ý§î§Ù§ñ"
    tone1_vol(1500, 80, 90);
    delay(40);
    tone1_vol(1500, 80, 90);
    delay(40);
    tone1_vol(1200, 100, 70);  // §°§ä§ã§Ü§à§Ü §ß§Ñ§Ù§Ñ§Õ
}

// === §¥§°§³§´§ª§¤§¯§µ§´ §®§ª§¯§ª§®§µ§® ===
void beep_Decrement_Min(void) {
    // §©§Ó§å§Ü "§Þ§Ö§ß§î§ê§Ö §ß§Ö§Ý§î§Ù§ñ"
    tone1_vol(800, 80, 90);
    delay(40);
    tone1_vol(800, 80, 90);
    delay(40);
    tone1_vol(1000, 100, 70);  // §°§ä§ã§Ü§à§Ü §ß§Ñ§Ù§Ñ§Õ
}

void beep_Save(void) {
    // §¬§à§â§à§ä§Ü§Ú§Û §Ó§à§ã§ç§à§Õ§ñ§ë§Ú§Û §Ù§Ó§å§Ü "§Ù§Ñ§á§Ú§ã§Ñ§ß§à!"
    tone1_vol(1000, 40, 70);
    tone1_vol(1500, 80, 100);
}