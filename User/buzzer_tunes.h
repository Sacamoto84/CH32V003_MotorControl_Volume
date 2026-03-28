#ifndef BUZZER_TUNES_H
#define BUZZER_TUNES_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "debug.h"

extern void melody_Nokia(void);
extern void melody_iPhoneMarimba(void);
extern void melody_SamsungWhistle(void);

// §ª§Ô§â§í
extern void melody_SuperMario(void);
extern void melody_Tetris(void);
extern void melody_StarWars(void);
extern void melody_ImperialMarch(void);
extern void melody_ZeldaSecret(void);
extern void melody_Pacman(void);

// §±§â§Ñ§Ù§Õ§ß§Ú§é§ß§í§Ö
extern void melody_HappyBirthday(void);
extern void melody_JingleBells(void);
extern void melody_MerryChristmas(void);

// §±§à§á§å§Ý§ñ§â§ß§í§Ö
extern void melody_PinkPanther(void);
extern void melody_MissionImpossible(void);
extern void melody_TakeOnMe(void);
extern void melody_FurElise(void);

// §¬§Ý§Ñ§ã§ã§Ú§Ü§Ñ
extern void melody_CanonInD(void);
extern void melody_OdeToJoy(void);




// --- §³§­§µ§¨§¦§¢§¯§½§¦ §©§£§µ§¬§ª ---
extern void buzzer_ok(void);
extern void buzzer_ok2(void);
extern void buzzer_error(void);
extern void buzzer_error_strong(void);
extern void buzzer_warning(void);
extern void buzzer_warning_double(void);
extern void buzzer_click(void);
extern void buzzer_success_long(void);
extern void buzzer_critical(void);
extern void buzzer_beepboop(void);
extern void buzzer_access_denied(void);
extern void buzzer_notify(void);

// §¥§à§á§à§Ý§ß§Ú§ä§Ö§Ý§î§ß§í§Ö §ã§ä§Ú§Ý§Ú§Ù§à§Ó§Ñ§ß§ß§í§Ö:
extern void buzzer_ios_click(void);
extern void buzzer_android_notify(void);
extern void buzzer_robot(void);
extern void buzzer_microwave_done(void);
extern void buzzer_winxp_msg(void);


// --- §¥§°§¢§¡§£§­§¦§¯§¯§½§¦ ---
extern void buzzer_startup(void);
extern void buzzer_shutdown(void);
extern void buzzer_charging(void);
extern void buzzer_button_hold(void);





extern void beep_Increment_Max(void);
extern void beep_Decrement_Min(void);
extern void beep_Save(void);



extern void tone1_vol(uint16_t frequency, uint16_t duration_ms, uint8_t volume);



// §¶§à§â§Þ§Ñ§ß§ä§í §à§ã§ß§à§Ó§ß§í§ç §Ô§Ý§Ñ§ã§ß§í§ç §Ù§Ó§å§Ü§à§Ó (§å§á§â§à§ë§Ö§ß§ß§à)
// §¥§Ý§ñ §â§Ö§é§Ú §ß§å§Ø§ß§í 2-3 §æ§à§â§Þ§Ñ§ß§ä§í, §ß§à §ß§Ñ buzzer §Ú§ã§á§à§Ý§î§Ù§å§Ö§Þ §à§ã§ß§à§Ó§ß§à§Û (F1)

// §¤§Ý§Ñ§ã§ß§í§Ö (§à§ã§ß§à§Ó§ß§Ñ§ñ §é§Ñ§ã§ä§à§ä§Ñ F1)
#define VOWEL_A  700   // §Ñ (§Ü§Ñ§Ü §Ó "§â§Ñ§Ù")
#define VOWEL_O  500   // §à (§Ü§Ñ§Ü §Ó "§ß§à§Ý§î")
#define VOWEL_U  300   // §å (§Ü§Ñ§Ü §Ó "§ã§ä§à")
#define VOWEL_E  600   // §Ö (§Ü§Ñ§Ü §Ó "§ã§Ö§Þ§î")
#define VOWEL_I  400   // §Ú (§Ü§Ñ§Ü §Ó "§ä§â§Ú")
#define VOWEL_Y  450   // §í (§Ü§Ñ§Ü §Ó "§ä§í")

// §³§à§Ô§Ý§Ñ§ã§ß§í§Ö (§ê§å§Þ§à§Ó§í§Ö, §Ú§ã§á§à§Ý§î§Ù§å§Ö§Þ §Ó§í§ã§à§Ü§Ú§Ö §é§Ñ§ã§ä§à§ä§í)
#define CONS_S   4000  // §ã
#define CONS_SH  3000  // §ê
#define CONS_T   2500  // §ä
#define CONS_K   2000  // §Ü
#define CONS_P   1800  // §á
#define CONS_M   250   // §Þ (§ß§à§ã§à§Ó§à§Û)
#define CONS_N   300   // §ß (§ß§à§ã§à§Ó§à§Û)
#define CONS_V   200   // §Ó (§Ó§Ú§Ò§â§Ñ§è§Ú§ñ)
#define CONS_D   1500  // §Õ
#define CONS_R   350   // §â (§Ó§Ú§Ò§â§Ñ§è§Ú§ñ)


#ifdef __cplusplus
}
#endif

#endif // BUZZER_TUNES_H