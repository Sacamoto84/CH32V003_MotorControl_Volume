/**
 * EEPROM Emulation Implementation for CH32V003
 * Based on ST AN3969 algorithm
 * With compact English logging
 */

#include "eeprom_ch32v.h"
#include "ch32v00x_flash.h"

/* Helper to print page status */
static const char* EE_StatusToString(uint16_t status) {
    switch(status) {
        case EEPROM_ERASED: return "ERASED";
        case EEPROM_RECEIVE_DATA: return "RECEIVE_DATA";
        case EEPROM_VALID_PAGE: return "VALID";
        default: return "UNKNOWN";
    }
}

/* Internal helper functions */
static uint16_t EE_CheckPage(uint32_t pageBase, uint16_t status, uint32_t pageSize);
static uint16_t EE_ErasePage(uint32_t pageBase, uint32_t pageSize);
static uint16_t EE_CheckErasePage(uint32_t pageBase, uint16_t status, uint32_t pageSize);
static uint32_t EE_FindValidPage(EEPROM_HandleTypeDef *heeprom);
static uint16_t EE_GetVariablesCount(uint32_t pageBase, uint16_t skipAddress, uint32_t pageSize);
static uint16_t EE_PageTransfer(EEPROM_HandleTypeDef *heeprom, uint32_t newPage, uint32_t oldPage, uint16_t skipAddress);
static uint16_t EE_VerifyPageFullWriteVariable(EEPROM_HandleTypeDef *heeprom, uint16_t Address, uint16_t Data);

/**
  * @brief  Check page for blank
  */
static uint16_t EE_CheckPage(uint32_t pageBase, uint16_t status, uint32_t pageSize) {
    uint32_t pageEnd = pageBase + pageSize;
    
    EEPROM_LOG("🔍 Page 0x%08X", (unsigned)pageBase);
    
    uint16_t pageStatus = (*(__IO uint16_t*)pageBase);
    EEPROM_LOG("   St:0x%04X(%s) exp:0x%04X(%s)", pageStatus, EE_StatusToString(pageStatus), status, EE_StatusToString(status));
    
    if (pageStatus != EEPROM_ERASED && pageStatus != status) {
        EEPROM_LOG("   ❌ EE_CheckPage Bad status! ps:%04X s:%04X adr:%04X", pageStatus , status, pageBase);
        return EEPROM_BAD_FLASH;
    }
    
    for (uint32_t addr = pageBase + 4; addr < pageEnd; addr += 4) {
        if ((*(__IO uint32_t*)addr) != 0xFFFFFFFF) {
            EEPROM_LOG("   ❌ Not empty @0x%08X:0x%08X", 
                       (unsigned)addr, (unsigned)(*(__IO uint32_t*)addr));
            return EEPROM_BAD_FLASH;
        }
    }
    
    EEPROM_LOG("   ✅ OK");
    return EEPROM_OK;
}

/**
  * @brief  Erase page and increment erase counter
  */
static uint16_t EE_ErasePage(uint32_t pageBase, uint32_t pageSize) {
    FLASH_Status status;
    uint16_t eraseCount;
    
    EEPROM_LOG("🗑️  Erase 0x%08X", (unsigned)pageBase);
    
    uint16_t pageStatus = (*(__IO uint16_t*)pageBase);
    if ((pageStatus == EEPROM_ERASED) || 
        (pageStatus == EEPROM_VALID_PAGE) || 
        (pageStatus == EEPROM_RECEIVE_DATA)) {
        eraseCount = (*(__IO uint16_t*)(pageBase + 2)) + 1;
        EEPROM_LOG("   Erases:%d→%d", 
                   (*(__IO uint16_t*)(pageBase + 2)), eraseCount);
    } else {
        eraseCount = 0;
        EEPROM_LOG("   Reset cnt→0");
    }
    
    FLASH_Unlock();
    
    EEPROM_LOG("   ⏳ Erasing...");
    status = FLASH_ErasePage(pageBase);
    if (status != FLASH_COMPLETE) {
        FLASH_Lock();
        EEPROM_LOG("   ❌ Err:%d", status);
        return EEPROM_FLASH_ERROR;
    }
    EEPROM_LOG("   ✅ Done");
    
    status = FLASH_ProgramHalfWord(pageBase + 2, eraseCount);
    FLASH_Lock();
    
    if (status != FLASH_COMPLETE) {
        EEPROM_LOG("   ❌ Write cnt err:%d", status);
        return EEPROM_FLASH_ERROR;
    }
    
    EEPROM_LOG("   ✅ Cnt written");
    return EEPROM_OK;
}

/**
  * @brief  Check page and erase if needed
  */
static uint16_t EE_CheckErasePage(uint32_t pageBase, uint16_t status, uint32_t pageSize) {
    uint16_t result;
    
    result = EE_CheckPage(pageBase, status, pageSize);
    if (result != EEPROM_OK) {
        result = EE_ErasePage(pageBase, pageSize);
        if (result != EEPROM_OK)
            return result;
        return EE_CheckPage(pageBase, status, pageSize);
    }
    
    return EEPROM_OK;
}

/**
  * @brief  Find valid page for operation
  */
static uint32_t EE_FindValidPage(EEPROM_HandleTypeDef *heeprom) {
    uint16_t status0 = (*(__IO uint16_t*)heeprom->PageBase0);
    uint16_t status1 = (*(__IO uint16_t*)heeprom->PageBase1);
    
    if (status0 == EEPROM_VALID_PAGE && status1 == EEPROM_ERASED)
        return heeprom->PageBase0;
    if (status1 == EEPROM_VALID_PAGE && status0 == EEPROM_ERASED)
        return heeprom->PageBase1;
    
    return 0;
}

/**
  * @brief  Count unique variables in page
  */
static uint16_t EE_GetVariablesCount(uint32_t pageBase, uint16_t skipAddress, uint32_t pageSize) {
    uint16_t varAddress, nextAddress;
    uint32_t pageEnd = pageBase + pageSize;
    uint16_t count = 0;
    
    EEPROM_LOG("📊 Count @0x%08X", (unsigned)pageBase);
    
    for (uint32_t addr = pageBase + 6; addr < pageEnd; addr += 4) {
        varAddress = (*(__IO uint16_t*)addr);
        if (varAddress == 0xFFFF || varAddress == skipAddress)
            continue;
        
        count++;
        
        for (uint32_t idx = addr + 4; idx < pageEnd; idx += 4) {
            nextAddress = (*(__IO uint16_t*)idx);
            if (nextAddress == varAddress) {
                count--;
                break;
            }
        }
    }
    
    EEPROM_LOG("   Unique:%d", count);
    return count;
}

/**
  * @brief  Transfer data from old page to new page
  */
static uint16_t EE_PageTransfer(EEPROM_HandleTypeDef *heeprom, uint32_t newPage, uint32_t oldPage, uint16_t skipAddress) {
    FLASH_Status flashStatus;
    uint32_t newIdx, oldIdx;
    uint16_t address, data;
    bool found;
    uint16_t copiedCount = 0;
    
    EEPROM_LOG("🔄 XFER:0x%08X→0x%08X", 
               (unsigned)oldPage, (unsigned)newPage);
    
    uint32_t newEnd = newPage + heeprom->PageSize;
    uint32_t oldEnd = oldPage + 4;
    
    for (newIdx = newPage + 4; newIdx < newEnd; newIdx += 4) {
        if ((*(__IO uint32_t*)newIdx) == 0xFFFFFFFF)
            break;
    }
    
    if (newIdx >= newEnd) {
        EEPROM_LOG("   ❌ New page full!");
        return EEPROM_OUT_SIZE;
    }
    
    EEPROM_LOG("   Free@0x%08X", (unsigned)newIdx);
    
    FLASH_Unlock();
    
    for (oldIdx = oldPage + (heeprom->PageSize - 2); oldIdx > oldEnd; oldIdx -= 4) {
        address = (*(__IO uint16_t*)oldIdx);
        
        if (address == 0xFFFF || address == skipAddress)
            continue;
        
        found = false;
        for (uint32_t idx = newPage + 6; idx < newIdx; idx += 4) {
            if ((*(__IO uint16_t*)idx) == address) {
                found = true;
                break;
            }
        }
        
        if (found)
            continue;
        
        if (newIdx < newEnd) {
            data = (*(__IO uint16_t*)(oldIdx - 2));
            
            flashStatus = FLASH_ProgramHalfWord(newIdx, data);
            if (flashStatus != FLASH_COMPLETE) {
                FLASH_Lock();
                EEPROM_LOG("   ❌ Data write err!");
                return EEPROM_FLASH_ERROR;
            }
            
            flashStatus = FLASH_ProgramHalfWord(newIdx + 2, address);
            if (flashStatus != FLASH_COMPLETE) {
                FLASH_Lock();
                EEPROM_LOG("   ❌ Addr write err!");
                return EEPROM_FLASH_ERROR;
            }
            
            copiedCount++;
            EEPROM_LOG("   ✅ %d=%d @0x%08X", 
                       address, data, (unsigned)newIdx);
            
            newIdx += 4;
        } else {
            FLASH_Lock();
            EEPROM_LOG("   ❌ Overflow!");
            return EEPROM_OUT_SIZE;
        }
    }
    
    FLASH_Lock();
    
    EEPROM_LOG("   📦 Copied:%d", copiedCount);
    
    EEPROM_LOG("   🗑️  Erase old...");
    uint16_t result = EE_CheckErasePage(oldPage, EEPROM_ERASED, heeprom->PageSize);
    if (result != EEPROM_OK) {
        EEPROM_LOG("   ❌ Erase err!");
        return result;
    }
    
    EEPROM_LOG("   ✏️  Mark active...");
    FLASH_Unlock();
    flashStatus = FLASH_ProgramHalfWord(newPage, EEPROM_VALID_PAGE);
    FLASH_Lock();
    
    if (flashStatus != FLASH_COMPLETE) {
        EEPROM_LOG("   ❌ Mark err!");
        return EEPROM_FLASH_ERROR;
    }
    
    EEPROM_LOG("✅ XFER OK!");
    return EEPROM_OK;
}

/**
  * @brief  Write variable with page full check
  */
static uint16_t EE_VerifyPageFullWriteVariable(EEPROM_HandleTypeDef *heeprom, uint16_t Address, uint16_t Data) {
    FLASH_Status flashStatus;
    uint32_t pageBase, pageEnd, newPage;
    uint16_t count;
    
    pageBase = EE_FindValidPage(heeprom);
    if (pageBase == 0)
        return EEPROM_NO_VALID_PAGE;
    
    pageEnd = pageBase + heeprom->PageSize;
    
    for (uint32_t idx = pageEnd - 2; idx > pageBase; idx -= 4) {
        if ((*(__IO uint16_t*)idx) == Address) {
            count = (*(__IO uint16_t*)(idx - 2));
            if (count == Data)
                return EEPROM_OK;
            if (count == 0xFFFF) {
                FLASH_Unlock();
                flashStatus = FLASH_ProgramHalfWord(idx - 2, Data);
                FLASH_Lock();
                return (flashStatus == FLASH_COMPLETE) ? EEPROM_OK : EEPROM_FLASH_ERROR;
            }
            break;
        }
    }
    
    for (uint32_t idx = pageBase + 4; idx < pageEnd; idx += 4) {
        if ((*(__IO uint32_t*)idx) == 0xFFFFFFFF) {
            FLASH_Unlock();
            flashStatus = FLASH_ProgramHalfWord(idx, Data);
            if (flashStatus != FLASH_COMPLETE) {
                FLASH_Lock();
                return EEPROM_FLASH_ERROR;
            }
            flashStatus = FLASH_ProgramHalfWord(idx + 2, Address);
            FLASH_Lock();
            return (flashStatus == FLASH_COMPLETE) ? EEPROM_OK : EEPROM_FLASH_ERROR;
        }
    }
    
    count = EE_GetVariablesCount(pageBase, Address, heeprom->PageSize) + 1;
    if (count >= (heeprom->PageSize / 4 - 1))
        return EEPROM_OUT_SIZE;
    
    newPage = (pageBase == heeprom->PageBase1) ? heeprom->PageBase0 : heeprom->PageBase1;
    
    FLASH_Unlock();
    flashStatus = FLASH_ProgramHalfWord(newPage, EEPROM_RECEIVE_DATA);
    if (flashStatus != FLASH_COMPLETE) {
        FLASH_Lock();
        return EEPROM_FLASH_ERROR;
    }
    
    flashStatus = FLASH_ProgramHalfWord(newPage + 4, Data);
    if (flashStatus != FLASH_COMPLETE) {
        FLASH_Lock();
        return EEPROM_FLASH_ERROR;
    }
    
    flashStatus = FLASH_ProgramHalfWord(newPage + 6, Address);
    FLASH_Lock();
    
    if (flashStatus != FLASH_COMPLETE)
        return EEPROM_FLASH_ERROR;
    
    return EE_PageTransfer(heeprom, newPage, pageBase, Address);
}

/* ==================== Public API ==================== */

/**
  * @brief  Initialize EEPROM emulation
  */
uint16_t EEPROM_Init(EEPROM_HandleTypeDef *heeprom) {
    uint16_t status0, status1;
    FLASH_Status flashStatus;
    
    if (!heeprom)
        return EEPROM_BAD_ADDRESS;
    
    EEPROM_LOG("🚀 INIT EEPROM");
    EEPROM_LOG("   P0:0x%08X P1:0x%08X Size:%d", 
               (unsigned)heeprom->PageBase0, 
               (unsigned)heeprom->PageBase1, 
               heeprom->PageSize);
    
    heeprom->Status = EEPROM_NO_VALID_PAGE;
    
    status0 = (*(__IO uint16_t*)heeprom->PageBase0);
    status1 = (*(__IO uint16_t*)heeprom->PageBase1);
    
    EEPROM_LOG("   P0:0x%04X(%s) P1:0x%04X(%s)", 
               status0, EE_StatusToString(status0),
               status1, EE_StatusToString(status1));
    
    switch (status0) {
        case EEPROM_ERASED:
            EEPROM_LOG("📄 P0 erased");
            if (status1 == EEPROM_VALID_PAGE) {
                EEPROM_LOG("   ✅ P1 active-check P0");
                heeprom->Status = EE_CheckErasePage(heeprom->PageBase0, EEPROM_ERASED, heeprom->PageSize);
            } else if (status1 == EEPROM_RECEIVE_DATA) {
                EEPROM_LOG("   📥 P1 recv-activate");
                FLASH_Unlock();
                flashStatus = FLASH_ProgramHalfWord(heeprom->PageBase1, EEPROM_VALID_PAGE);
                FLASH_Lock();
                heeprom->Status = (flashStatus == FLASH_COMPLETE) ? 
                    EE_CheckErasePage(heeprom->PageBase0, EEPROM_ERASED, heeprom->PageSize) : EEPROM_FLASH_ERROR;
            } else if (status1 == EEPROM_ERASED) {
                EEPROM_LOG("   ⚠️  Both erased-format");
                heeprom->Status = EEPROM_Format(heeprom);
            }
            break;
            
        case EEPROM_RECEIVE_DATA:
            EEPROM_LOG("📥 P0 recv");
            if (status1 == EEPROM_VALID_PAGE) {
                EEPROM_LOG("   🔄 P1 active-xfer to P0");
                heeprom->Status = EE_PageTransfer(heeprom, heeprom->PageBase0, heeprom->PageBase1, 0xFFFF);
            } else if (status1 == EEPROM_ERASED) {
                EEPROM_LOG("   ✅ P1 erased-activate P0");
                heeprom->Status = EE_CheckErasePage(heeprom->PageBase1, EEPROM_ERASED, heeprom->PageSize);
                if (heeprom->Status == EEPROM_OK) {
                    FLASH_Unlock();
                    flashStatus = FLASH_ProgramHalfWord(heeprom->PageBase0, EEPROM_VALID_PAGE);
                    FLASH_Lock();
                    heeprom->Status = (flashStatus == FLASH_COMPLETE) ? EEPROM_OK : EEPROM_FLASH_ERROR;
                }
            }
            break;
            
        case EEPROM_VALID_PAGE:
            EEPROM_LOG("✅ P0 active");
            if (status1 == EEPROM_VALID_PAGE) {
                EEPROM_LOG("   ❌ ERROR:Both active!");
                heeprom->Status = EEPROM_NO_VALID_PAGE;
            } else if (status1 == EEPROM_RECEIVE_DATA) {
                EEPROM_LOG("   🔄 P1 recv-xfer from P0");
                heeprom->Status = EE_PageTransfer(heeprom, heeprom->PageBase1, heeprom->PageBase0, 0xFFFF);
            } else {
                EEPROM_LOG("   ✅ P1 erased-OK");
                heeprom->Status = EE_CheckErasePage(heeprom->PageBase1, EEPROM_ERASED, heeprom->PageSize);
            }
            break;
            
        default:
            EEPROM_LOG("⚠️  P0 unknown:0x%04X", status0);
            if (status1 == EEPROM_VALID_PAGE) {
                EEPROM_LOG("   ✅ P1 active-check P0");
                heeprom->Status = EE_CheckErasePage(heeprom->PageBase0, EEPROM_ERASED, heeprom->PageSize);
            } else if (status1 == EEPROM_RECEIVE_DATA) {
                EEPROM_LOG("   📥 P1 recv-activate");
                FLASH_Unlock();
                flashStatus = FLASH_ProgramHalfWord(heeprom->PageBase1, EEPROM_VALID_PAGE);
                FLASH_Lock();
                heeprom->Status = (flashStatus == FLASH_COMPLETE) ? 
                    EE_CheckErasePage(heeprom->PageBase0, EEPROM_ERASED, heeprom->PageSize) : EEPROM_FLASH_ERROR;
            }
            break;
    }
    
    if (heeprom->Status == EEPROM_OK) {
        EEPROM_LOG("✅ Init OK\r\n");
    } else {
        EEPROM_LOG("❌ Init err 0x%02X\r\n", heeprom->Status);
    }
    
    return heeprom->Status;
}

/**
  * @brief  Format EEPROM (erase both pages)
  */
uint16_t EEPROM_Format(EEPROM_HandleTypeDef *heeprom) {
    FLASH_Status flashStatus;
    uint16_t status;
    
    if (!heeprom)
        return EEPROM_BAD_ADDRESS;
    
    EEPROM_LOG("🔥FORMAT");
    EEPROM_LOG(" Clear P0...");
    status = EE_CheckErasePage(heeprom->PageBase0, EEPROM_VALID_PAGE, heeprom->PageSize);
    if (status != EEPROM_OK) {
        EEPROM_LOG("   ❌ P0 err:0x%02X", status);
        return status;
    }
    
    if ((*(__IO uint16_t*)heeprom->PageBase0) == EEPROM_ERASED) {
        EEPROM_LOG("   Mark P0 active...");
        FLASH_Unlock();
        flashStatus = FLASH_ProgramHalfWord(heeprom->PageBase0, EEPROM_VALID_PAGE);
        FLASH_Lock();
        if (flashStatus != FLASH_COMPLETE) {
            EEPROM_LOG("   ❌ Mark P0 err");
            return EEPROM_FLASH_ERROR;
        }
    }
    
    EEPROM_LOG("   Erase P1...");
    status = EE_CheckErasePage(heeprom->PageBase1, EEPROM_ERASED, heeprom->PageSize);
    if (status == EEPROM_OK) {
        EEPROM_LOG("✅ FORMAT OK\r\n");
    } else {
        EEPROM_LOG("❌ P1 err:0x%02X\r\n", status);
    }
    return status;
}

/**
  * @brief  Read variable from EEPROM
  */
uint16_t EEPROM_Read(EEPROM_HandleTypeDef *heeprom, uint16_t Address, uint16_t *Data) {
    uint32_t pageBase, pageEnd;
    
    if (!heeprom || !Data)
        return EEPROM_BAD_ADDRESS;
    
    *Data = EEPROM_DEFAULT_DATA;
    
    if (heeprom->Status == EEPROM_NOT_INIT) {
        if (EEPROM_Init(heeprom) != EEPROM_OK)
            return heeprom->Status;
    }
    
    pageBase = EE_FindValidPage(heeprom);
    if (pageBase == 0)
        return EEPROM_NO_VALID_PAGE;
    
    pageEnd = pageBase + heeprom->PageSize - 2;
    
    for (uint32_t addr = pageEnd; addr >= pageBase + 6; addr -= 4) {
        if ((*(__IO uint16_t*)addr) == Address) {
            *Data = (*(__IO uint16_t*)(addr - 2));
            EEPROM_LOG("📖 Rd %d→%d", Address, *Data);
            return EEPROM_OK;
        }
    }
    
    EEPROM_LOG("❓ Rd %d NF", Address);
    return EEPROM_BAD_ADDRESS;
}

/**
  * @brief  Write variable to EEPROM
  */
uint16_t EEPROM_Write(EEPROM_HandleTypeDef *heeprom, uint16_t Address, uint16_t Data) {
    if (!heeprom) return EEPROM_BAD_ADDRESS;
    
    // if (heeprom->Status == EEPROM_NOT_INIT) {
    //     if (EEPROM_Init(heeprom) != EEPROM_OK)
    //         return heeprom->Status;
    // }
    
    if (Address == 0xFFFF) return EEPROM_BAD_ADDRESS;
    
    EEPROM_LOG("Wr id:%d:%d", Address, Data);
    uint16_t result = EE_VerifyPageFullWriteVariable(heeprom, Address, Data);
    if (result == EEPROM_OK) {
        EEPROM_LOG("✅ OK");
    } else {
        EEPROM_LOG("❌ Err:0x%02X", result);
    }
    return result;
}

/**
  * @brief  Update variable (write only if different)
  */
uint16_t EEPROM_Update(EEPROM_HandleTypeDef *heeprom, uint16_t Address, uint16_t Data) {
    uint16_t currentData;
    uint16_t status;
    
    status = EEPROM_Read(heeprom, Address, &currentData);
    if (status == EEPROM_OK && currentData == Data)
        return EEPROM_SAME_VALUE;
    
    return EEPROM_Write(heeprom, Address, Data);
}

/**
  * @brief  Count variables in EEPROM
  */
uint16_t EEPROM_Count(EEPROM_HandleTypeDef *heeprom, uint16_t *Count) {
    uint32_t pageBase;
    
    if (!heeprom || !Count)
        return EEPROM_BAD_ADDRESS;
    
    if (heeprom->Status == EEPROM_NOT_INIT) {
        if (EEPROM_Init(heeprom) != EEPROM_OK)
            return heeprom->Status;
    }
    
    pageBase = EE_FindValidPage(heeprom);
    if (pageBase == 0)
        return EEPROM_NO_VALID_PAGE;
    
    *Count = EE_GetVariablesCount(pageBase, 0xFFFF, heeprom->PageSize);
    EEPROM_LOG("📊 Vars:%d/%d", *Count, EEPROM_MaxCount(heeprom));
    return EEPROM_OK;
}

/**
  * @brief  Get maximum variable count
  */
uint16_t EEPROM_MaxCount(EEPROM_HandleTypeDef *heeprom) {
    if (!heeprom)
        return 0;
    return (heeprom->PageSize / 4) - 1;
}

/**
  * @brief  Get erase counter
  */
uint16_t EEPROM_Erases(EEPROM_HandleTypeDef *heeprom, uint16_t *Erases) {
    uint32_t pageBase;
    
    if (!heeprom || !Erases)
        return EEPROM_BAD_ADDRESS;
    
    if (heeprom->Status == EEPROM_NOT_INIT) {
        if (EEPROM_Init(heeprom) != EEPROM_OK)
            return heeprom->Status;
    }
    
    pageBase = EE_FindValidPage(heeprom);
    if (pageBase == 0)
        return EEPROM_NO_VALID_PAGE;
    
    *Erases = (*(__IO uint16_t*)(pageBase + 2));
    EEPROM_LOG("🔢 Erases:%d", *Erases);
    return EEPROM_OK;
}