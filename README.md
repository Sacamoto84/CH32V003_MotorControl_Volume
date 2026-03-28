### Параметры настроек

Управление мощностью идет через потенциометр, который запитывается при установке PC3, который сбрасывается во сне.

| Параметр       | Диапазон        | Шаг   | Ед.  |
|----------------|-----------------|-------|------|
| Power          | Не используется | 5     | %    |
| Boost Enable   | Off / On        | —     | —    |
| Boost Power    | 5 – 100         | 5     | %    |
| Boost Time     | 50 – 1000       | 50    | мс   |

Motor Power = Power + Boost Power

```cpp
PD4 SWIO
PD6 TX  

PA2 PWM

PC0 KEY
PC1 BUZZER
PC2 LED
PC3 OUT Volume
PC4 AIN2
```
