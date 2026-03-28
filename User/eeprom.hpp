#pragma once

#ifdef __cplusplus

/* Includes ------------------------------------------------------------------*/
#include "ch32v00x_flash.h"

/* Debug Configuration ------------------------------------------------------*/
/* Main debug switch - enables/disables ALL logs */
#ifndef EEPROM_DEBUG
#define EEPROM_DEBUG 0  // 0 = disable, 1 = enable debug messages
#endif

/* Color macros */
#define CLRscr "\033[2J\033[H"
#define FG(color) "\033[38;5;" #color "m"
#define BG(color) "\033[48;5;" #color "m"
#define RESET1 "\033[0m"
#define BOLD "\033[1m"
#define UNDERLINE "\033[4m"

/* Logging macros */
#if EEPROM_DEBUG
#include <stdio.h>
#define EEPROM_LOG(fmt, ...) printf (FG (51) "[EE] " RESET1 fmt "\n", ##__VA_ARGS__)
#define EEPROM_LOG_OK(fmt, ...) printf (FG (46) "[EE] ✓ " RESET1 fmt "\n", ##__VA_ARGS__)
#define EEPROM_LOG_WARN(fmt, ...) printf (FG (226) "[EE] W " RESET1 fmt "\n", ##__VA_ARGS__)
#define EEPROM_LOG_ERROR(fmt, ...) printf (FG (196) "[EE] E " RESET1 fmt "\n", ##__VA_ARGS__)
#define EEPROM_LOG_INFO(fmt, ...) printf (FG (141) "[EE] I " RESET1 fmt "\n", ##__VA_ARGS__)
#define EEPROM_LOG_DEBUG(fmt, ...) printf (FG (93) "[EE] D " RESET1 fmt "\n", ##__VA_ARGS__)
#else
#define EEPROM_LOG(fmt, ...) ((void)0)
#define EEPROM_LOG_OK(fmt, ...) ((void)0)
#define EEPROM_LOG_WARN(fmt, ...) ((void)0)
#define EEPROM_LOG_ERROR(fmt, ...) ((void)0)
#define EEPROM_LOG_INFO(fmt, ...) ((void)0)
#define EEPROM_LOG_DEBUG(fmt, ...) ((void)0)
#endif

/* EEPROM start address in Flash memory */
#define EEPROM_START_ADDRESS ((uint32_t)0x08003C00) /* EEPROM emulation start: after 2KB used Flash */

class uEeprom {
  public:
    // Default constructor - creates empty object
    uEeprom()
        : index (0), min (0), max (0), define (0), title (nullptr), value (0), initialized (false) {
        // No actions - empty object
    }

    uint16_t index;
    uint16_t min;
    uint16_t max;
    uint16_t define;
    const char *title;
    bool initialized;

    void set (uint16_t i) {
        if (!initialized) {
            EEPROM_LOG_ERROR ("ERR: set() before init()!");
            return;
        }

        if (i < min || i > max) {
            EEPROM_LOG_WARN (FG (82) "\"%s\"" RESET1 ": Val " FG (196) "%u" RESET1
                                                                            " out of range [%u..%u], clamped",
                             title, i, min, max);
            i = (i < min) ? min : max;
        }

        EEPROM_LOG (FG (82) "\"%s\"" RESET1 ": Set: " FG (226) "idx=%u" RESET1
                                                                              ", old=%u → " FG (82) "new=%u" RESET1,
                    title, index, value, i);
        value = i;
    }

    uint16_t get (void) {
        if (!initialized) {
            EEPROM_LOG_ERROR ("ERR: get() before init()!");
            return 0;
        }

        EEPROM_LOG (FG (82) "\"%s\"" RESET1 ": Get: " FG (226) "idx=%u" RESET1
                                                                           ", val=" FG (82) "%u" RESET1,
                    title, index, value);
        return value;
    }

    FLASH_Status save() {
        if (!initialized) {
            EEPROM_LOG_ERROR ("ERR: save() before init()!");
            return FLASH_ERROR_PG;
        }

        uint32_t address = EEPROM_START_ADDRESS + (index * 64);

        EEPROM_LOG (BOLD FG (82) "\"%s\"" RESET1 BOLD ": Save to Flash" RESET1, title);
        EEPROM_LOG ("  " FG (226) "idx=%u" RESET1 ", adr=" FG (141) "0x%08X" RESET1
                                                                 ", val=" FG (82) "%u:0x%04X" RESET1,
                    index, address, value, value);

        uint16_t currentValue = *(volatile uint16_t *)address;
        if (currentValue == value) {
            EEPROM_LOG_OK (FG (82) "\"%s\"" RESET1 ": Already saved (0x%04X), skip",
                           title, currentValue);
            return FLASH_COMPLETE;
        }

        EEPROM_LOG_INFO (FG (82) "\"%s\"" RESET1 ": Unlock Flash...", title);
        FLASH_Unlock();
        FLASH_Unlock_Fast();

        EEPROM_LOG_INFO (FG (82) "\"%s\"" RESET1 ": Erase 64B @ " FG (141) "0x%08X" RESET1, title, address);
        FLASH_ErasePage_Fast (address);
        
        EEPROM_LOG_INFO (FG (82) "\"%s\"" RESET1 ": Lock fast mode", title);
        FLASH_Lock_Fast();
        FLASH_Lock();
        
        // Verify erase
        EEPROM_LOG_INFO ("Verify erase (64B)...");
        bool eraseOk = true;
        for (uint32_t i = 0; i < 32; i++) {
            uint16_t readVal = *(volatile uint16_t *)(address + (i * 2));
            if (readVal != 0xFFFF) {
                EEPROM_LOG_ERROR ("Erase err %08X:0x%04X", address + (i * 2), readVal);
                eraseOk = false;
            }
        }
        
        if (!eraseOk) {
            EEPROM_LOG_ERROR (FG (82) "\"%s\"" RESET1 ": Block not erased!", title);
            return FLASH_ERROR_PG;
        }
        
        EEPROM_LOG_OK ("Block erased (all 64B=0xFFFF)");

        // Write data
        EEPROM_LOG_INFO (FG (82) "\"%s\"" RESET1 ": Write data...", title);
        FLASH_Unlock();
        
        FLASH_Status res = FLASH_ProgramHalfWord (address, value);
        if (res != FLASH_COMPLETE) {
            EEPROM_LOG_ERROR (FG (82) "\"%s\"" RESET1 ": Write word1 err! St=%d", title, res);
            FLASH_Lock();
            return res;
        }

        res = FLASH_ProgramHalfWord (address + 2, value);
        if (res != FLASH_COMPLETE) {
            EEPROM_LOG_ERROR (FG (82) "\"%s\"" RESET1 ": Write word2 err! St=%d", title, res);
        }

        FLASH_Lock();

        // Verify write
        uint16_t verify1 = *(volatile uint16_t *)address;
        uint16_t verify2 = *(volatile uint16_t *)(address + 2);
        if (verify1 == value && verify2 == value) {
            EEPROM_LOG_OK (FG (82) "\"%s\"" RESET1 ": Save OK, w1=0x%04X w2=0x%04X",
                           title, verify1, verify2);
        } else {
            EEPROM_LOG_ERROR (FG (82) "\"%s\"" RESET1 ": Verify err! Exp=0x%04X, rd: w1=0x%04X w2=0x%04X",
                              title, value, verify1, verify2);
        }

        return res;
    }

    int16_t readEEPROM (void) {
        if (!initialized) {
            EEPROM_LOG_ERROR ("ERR: readEEPROM() before init()!");
            return 0;
        }

        uint32_t address = EEPROM_START_ADDRESS + (index * 64);
        int16_t val = *(volatile uint16_t *)address;
        EEPROM_LOG (FG (82) "\"%s\"" RESET1 ": Direct read: " FG (226) "idx=%u" RESET1
                                                                                ", adr=" FG (141) "0x%08X" RESET1 ", val=" FG (82) "%d" RESET1,
                    title, index, address, val);
        return val;
    }

    // MAIN METHOD - accepts all parameters and initializes object
    void init (uint16_t _index, uint16_t _min, uint16_t _max, uint16_t _define, const char *_title) {
        // Save parameters
        index = _index;
        min = _min;
        max = _max;
        define = _define;
        title = _title;

        uint32_t address = EEPROM_START_ADDRESS + (index * 64);

        EEPROM_LOG_INFO (BOLD "╔════════════════════════════════════════╗" RESET1);
        EEPROM_LOG_INFO (BOLD "║ Init EEPROM: " FG (82) "%-17s" RESET1 "         ║", title);
        EEPROM_LOG_INFO (BOLD "╠════════════════════════════════════════╣" RESET1);
        EEPROM_LOG_INFO ("║ " FG (226) "Idx: %3u" RESET1 "                               ║", index);
        EEPROM_LOG_INFO ("║ " FG (141) "Adr: 0x%08X" RESET1 "                        ║", address);
        EEPROM_LOG_INFO ("║ Range: [%3u .. %4u]                   ║", min, max);
        EEPROM_LOG_INFO ("║ " FG (82) "Def: %-3u" RESET1 "                               ║", define);
        EEPROM_LOG_INFO (BOLD "╚════════════════════════════════════════╝" RESET1);

        uint16_t a = (*(volatile uint16_t *)address);
        uint16_t b = (*(volatile uint16_t *)(address + 2));

        EEPROM_LOG ("Read from Flash: w1=" FG (141) "0x%04X" RESET1 " (%u), w2=" FG (141) "0x%04X" RESET1 " (%u)",
                    a, a, b, b);

        if ((a != b) || (a == 0xFFFF) || (b == 0xFFFF)) {
            EEPROM_LOG_WARN ("Invalid or empty data detected!");

            if (a != b) {
                EEPROM_LOG_WARN ("  Reason: " FG (196) "mismatch" RESET1 " (w1=0x%04X != w2=0x%04X)", a, b);
            }
            if (a == 0xFFFF) {
                EEPROM_LOG_WARN ("  Reason: " FG (196) "w1=0xFFFF" RESET1 " (unprogrammed)");
            }
            if (b == 0xFFFF) {
                EEPROM_LOG_WARN ("  Reason: " FG (196) "w2=0xFFFF" RESET1 " (unprogrammed)");
            }

            EEPROM_LOG_INFO ("Init with default: " FG (82) "%u" RESET1, define);

            EEPROM_LOG_INFO ("Unlock Flash...");
            FLASH_Unlock();
            
            EEPROM_LOG_INFO ("Unlock fast mode...");
            FLASH_Unlock_Fast();

            EEPROM_LOG_INFO ("Erase 64B block @ " FG (141) "0x%08X" RESET1, address);
            FLASH_ErasePage_Fast (address);
            
            EEPROM_LOG_INFO ("Lock fast mode...");
            FLASH_Lock_Fast();
            
            EEPROM_LOG_INFO ("Lock Flash...");
            FLASH_Lock();

            // Verify erase
            EEPROM_LOG_DEBUG ("Verify erase (64B)...");
            bool eraseOk = true;
            for (uint32_t i = 0; i < 32; i++) {
                uint16_t readVal = *(volatile uint16_t *)(address + (i * 2));
                if (readVal != 0xFFFF) {
                    EEPROM_LOG_ERROR ("Erase err %08X:0x%04X", address + (i * 2), readVal);
                    eraseOk = false;
                }
            }
            
            if (!eraseOk) {
                EEPROM_LOG_ERROR ("Block not erased! Init aborted.");
                initialized = false;
                return;
            }
            
            EEPROM_LOG_OK ("Block erased");

            // Write default value
            EEPROM_LOG_INFO ("Unlock Flash for write...");
            FLASH_Unlock();
            
            EEPROM_LOG_INFO ("Write w1 (def=" FG (82) "%u" RESET1 ")...", define);
            FLASH_Status st1 = FLASH_ProgramHalfWord (address, define);
            if (st1 != FLASH_COMPLETE) {
                EEPROM_LOG_ERROR ("Write w1 err! St=%d", st1);
            }
            
            EEPROM_LOG_INFO ("Write w2...");
            FLASH_Status st2 = FLASH_ProgramHalfWord (address + 2, define);
            if (st2 != FLASH_COMPLETE) {
                EEPROM_LOG_ERROR ("Write w2 err! St=%d", st2);
            }

            EEPROM_LOG_INFO ("Lock Flash...");
            FLASH_Lock();

            // Verify write
            uint16_t verify1 = *(volatile uint16_t *)address;
            uint16_t verify2 = *(volatile uint16_t *)(address + 2);
            EEPROM_LOG_DEBUG ("Verify: w1=0x%04X w2=0x%04X", verify1, verify2);

            if (verify1 == define && verify2 == define) {
                value = define;
                EEPROM_LOG_OK ("Init done, val=" FG (82) "%u" RESET1, value);
            } else {
                EEPROM_LOG_ERROR ("Verify err! Exp=0x%04X, rd: w1=0x%04X w2=0x%04X",
                                 define, verify1, verify2);
                value = define;  // Set default anyway
            }
        } else {
            // Data is valid - load it
            value = a;
            EEPROM_LOG_OK ("Init OK, loaded val=" FG (82) "%u" RESET1, value);
        }

        initialized = true;
    }

  private:
    int16_t value;
};

#endif /* __cplusplus */