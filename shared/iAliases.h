#pragma once

// Алиас это WORD, из них 12 младших битов - тип объекта, 4 старших - номер команды
// Всего типов может быть 4096
// Нулевой тип - это World. Его команда - Условие остановки - имеет алиас 4096.
// В закладке "Типы" iAlias отображается как 10тичное число X000Y,
// где X - номер команды, Y - тип объекта
// Модификатор алиаса хранится как верхние 16 бит DWORDа, поэтому не может быть больше 64К.

#define IALIAS_MASK     0xFFF
#define IALIAS_CMDMASK  0xF000
#define IALIAS_CMDSTEP  0x1000
#define IALIAS_SHIFT    12
#define IALIAS_MAX      0xFFFF

// зарезервированы
#define IALIAS_WORLD    0
#define IALIAS_ENDC     (1<<12)
#define IALIAS_DOCS     1
#define IALIAS_MONITOR  2
#define IALIAS_GRAF     3
#define IALIAS_PLOTTER  4
#define IALIAS_NETGATE  6
#define IALIAS_CAMERA   9
#define IALIAS_VIDEO    10
