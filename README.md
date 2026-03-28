### Параметры настроек

| Параметр       | Диапазон      | Шаг   | Ед.  |
|----------------|---------------|-------|------|
| Power          | 5 – 100       | 5     | %    |
| Boost Enable   | Off / On      | —     | —    |
| Boost Power    | 5 – 100       | 5     | %    |
| Boost Time     | 50 – 1000     | 50    | мс   |

Motor Power = Power + Boost Power

```cpp
TX  [PD6   PD4] SWIO
GNG [VSS   PC4] KEY
PWM [PA2   PC2] LED
VDD [VDD   PC1] BUZZER
```
