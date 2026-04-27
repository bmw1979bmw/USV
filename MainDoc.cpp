// MainDoc.cpp : реализация класса CMainDoc (документ приложения, содержащий основную логику модели БЭК)

#include "stdafx.h"                 // Прекомпилированный заголовок, включает стандартные MFC и Windows заголовки
#include "USV.h"                    // Главный заголовочный файл приложения USV

#include "MainDoc.h"                // Заголовок данного класса CMainDoc
#include "TextArchive.h"            // Класс для текстовой сериализации (используется для формуляра)
#include "FormularDlg.h"            // Диалог формуляра управления БЭК (ввод команд)
#include "goodfunc.h"               // Вспомогательные функции общего назначения (DDX_Hex, AfxMessageBox и т.д.)
#include "Functions.h"              // Математические функции (нормировка углов, пересчёт координат и др.)
#include "math2.h"                  // Расширенные математические функции (интерполяция, генераторы случайных чисел)

#ifdef _DEBUG                       // Если сборка в отладочном режиме
#define new DEBUG_NEW               // Подмена оператора new для отслеживания утечек памяти
#undef THIS_FILE                    // Снимаем предыдущее определение THIS_FILE
static char THIS_FILE[] = __FILE__; // Имя текущего файла для отладочных сообщений
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainDoc

FORMINFO(TFormularUSV); // Макрос, регистрирующий структуру TFormularUSV для сериализации/обмена с сервером

IMPLEMENT_DYNCREATE(CMainDoc, CDocument) // MFC-макрос, позволяющий динамическое создание объектов класса CMainDoc

BEGIN_MESSAGE_MAP(CMainDoc, CDocument)   // Начало карты сообщений документа
   ON_COMMAND(ID_PORT_WORLD, OnPortWorld) // Обработка команды ID_PORT_WORLD (сообщения от модельного сервера через файловый маппинг)
   ON_COMMAND_RANGE(ID_DEBUG_FIRST, ID_DEBUG_LAST, OnDebugFlags) // Обработка отладочных команд в заданном диапазоне
   ON_UPDATE_COMMAND_UI_RANGE(ID_DEBUG_FIRST, ID_DEBUG_LAST, OnUpdateDebugFlags) // Обновление состояния UI для отладочных флагов
END_MESSAGE_MAP()                  // Конец карты сообщений

/////////////////////////////////////////////////////////////////////////////


const float CMainDoc::ANGLE_EPSILON = 0.01f;  //bmw Допуск сравнения углов в градусах (0.01° - ок. 0.6 морской мили на 100 км) 

struct MK                // Структура для хранения коэффициентов модели поворота по курсу (матрица MATR_KURS)
{
   float MATR_KURS[9];      // Массив из 9 значений коэффициентов для разных скоростей и углов закладки рулей
} mk[8] =                // Массив из 8 структур MK, индексированных по скорости (каждые 5 узлов, V/5)
{
   { 0.0f, 0.0f,     0.0f,     0.0f,     0.0f,     0.0f,     0.0f,     0.0f,     0.0f },       // Скорость 0 узлов
   { 0.0f, 0.002445f, 0.004790f, 0.007340f, 0.009830f, 0.012300f, 0.014560f, 0.016630f },      // Скорость 5 узлов
   { 0.0f, 0.003450f, 0.006930f, 0.010290f, 0.013460f, 0.016850f, 0.019970f, 0.023290f },      // 10 узлов
   { 0.0f, 0.004290f, 0.008910f, 0.014030f, 0.019380f, 0.025640f, 0.031980f, 0.038170f },      // 15 узлов
   { 0.0f, 0.007410f, 0.014510f, 0.021390f, 0.028860f, 0.034520f, 0.040209f, 0.044219f },      // 20 узлов
   { 0.0f, 0.009000f, 0.018390f, 0.028310f, 0.036910f, 0.047418f, 0.055047f, 0.065596f },      // 25 узлов
   { 0.0f, 0.011970f, 0.024450f, 0.035710f, 0.049628f, 0.058287f, 0.072545f, 0.083924f },      // 30 узлов
   { 0.0f, 0.014970f, 0.031445f, 0.042710f, 0.063628f, 0.074287f, 0.095545f, 0.10424f  }       // 35 узлов
};

static const struct MV   // Структура для хранения коэффициентов модели изменения скорости на циркуляции (матрица MATR_V)
{
   float MATR_V[9];      // Массив из 9 значений для разных скоростей и углов закладки рулей
} mv[8] =                // Массив из 8 структур MV, индексированных по скорости
{
   { 0.0f, 0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f },  // 0 узлов
   { 0.0f, 2.597f, 2.587f, 2.57f,  2.55f,  2.525f, 2.49f,  2.45f,  2.4f },  // 5 узлов
   { 0.0f, 5.19f,  5.16f,  5.115f, 5.05f,  4.98f,  4.9f,   4.8f,   4.7f },  // 10 узлов
   { 0.0f, 7.78f,  7.72f,  7.64f,  7.56f,  7.41f,  7.3f,   7.17f,  7.05f }, // 15 узлов
   { 0.0f, 10.38f, 10.32f, 10.21f, 10.1f,  9.9f,   9.65f,  9.38f,  9.0f },  // 20 узлов
   { 0.0f, 12.98f, 12.92f, 12.81f, 12.65f, 12.49f, 12.27f, 12.01f, 11.8f }, // 25 узлов
   { 0.0f, 14.44f, 14.37f, 14.25f, 14.1f,  13.9f,  13.7f,  13.4f,  12.0f }, // 30 узлов
   { 0.0f, 17.12f, 17.05f, 16.95f, 16.8f,  16.6f,  16.4f,  16.1f,  15.7f }  // 35 узлов
};

static const struct MT   // Структура для хранения коэффициентов времени переходного процесса по курсу (матрица MATR_T)
{
   float MATR_T[9];      // Массив из 9 значений времени (в секундах?) для разных скоростей и углов закладки
} mt[8] =                // Массив из 8 структур MT
{
   { 0.0f, 0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f },   // 0 узлов
   { 0.0f, 250.0f, 250.0f, 250.0f, 250.0f, 200.0f, 200.0f, 200.0f, 200.0f }, // 5 узлов
   { 0.0f, 150.0f, 150.0f, 150.0f, 100.0f, 100.0f, 100.0f, 100.0f, 100.0f }, // 10 узлов
   { 0.0f, 120.0f, 120.0f, 100.0f, 80.0f,  80.0f,  80.0f,  80.0f,  80.0f },  // 15 узлов
   { 0.0f, 120.0f, 100.0f, 80.0f,  60.0f,  60.0f,  60.0f,  60.0f,  60.0f },  // 20 узлов
   { 0.0f, 100.0f, 80.0f,  60.0f,  50.0f,  50.0f,  50.0f,  50.0f,  50.0f },  // 25 узлов
   { 0.0f, 100.0f, 80.0f,  60.0f,  50.0f,  50.0f,  50.0f,  50.0f,  50.0f },  // 30 узлов
   { 0.0f, 100.0f, 80.0f,  60.0f,  50.0f,  50.0f,  50.0f,  50.0f,  50.0f },  // 35 узлов
};

/////////////////////////////////////////////////////////////////////////////
const CMainDoc::KURS_CLASS_PL CMainDoc::kk_kurs = // Инициализация статической константы с коэффициентами модели курса
{
   0.5f,    0.8f,        // K_S_1_KURS[2]  - коэффициенты для расчёта падения скорости на циркуляции (тип управления 1 и 2)
   0.016f,  0.016f,      // K_S_2_KURS[2]  - ...
   0.3f,    0.4f,        // K_S_3_KURS[2]  - ...
   0.1013f, 0.23f,       // K_TG_1_KURS[2] - ...
   0.571f,  0.2f,        // K_TG_2_KURS[2] - ...
   12.3f,   6.25f,       // K1_KURS[2]     - ...
   85.0f,   90.0f,       // K2_KURS[2]     - ...
   -1.1f,   0.097f,      // K3_KURS[2]     - ...
   0.025f,  0.095f, 0.5f // RK1, RK2, RK3  - коэффициенты для экспоненциальной модели изменения угловой скорости
};

const CMainDoc::SPEED_CLASS_PL CMainDoc::kk_speed = // Инициализация статической константы с коэффициентами модели скорости
{
   { 0.0017f, 0.0024f,  0.0033f,  0.0048f,  0.005f,   0.0085f,  0.0085f,  0.0085f }, // sk1[8] - коэффициенты полинома разгона (зависят от скорости)
   { 0.0004f, 0.00077f, 0.00131f, 0.00229f, 0.00204f, 0.00204f, 0.00204f, 0.00204f }, // sk2[8] - коэффициенты
   1.1f,          // sk3
   2.03f,         // sk4
   0.51f,         // sk5
   0.068f,        // sk6
   0.18f,         // sk7
   2.2f,          // sk8
   0.00001f,      // sk9  - коэффициенты для торможения (режим 2)
   0.0008f,       // sk10
   0.0358f,       // sk11
   -0.33f,        // sk12
   0.00002f,      // sk13 - коэффициенты для торможения (режим 3)
   -0.0009f,      // sk14
   0.0228f,       // sk15
   -0.114f        // sk16
};

BOOL bZERORETURN = 1; // Флаг, определяющий, будет ли при запуске показан диалог начальных параметров (1 - показать, 0 - нет)

CMainDoc::CMainDoc() // Конструктор документа
{
   CONTROL87();             // Установка режима работы FPU (маскирование исключений плавающей точки)
#ifdef ZERORETURN           // Если определён макрос ZERORETURN
   bZERORETURN = 0;         // то отключаем показ диалога (для автоматического запуска)
#endif
   b_PGN = 0;               // Признак преследования
   b_1Contact_PL = 0;       // Был ли первый контакт с ПЛ
   b_1Contact_OPL = 0;      // Был ли первый контакт с помехой (ОПЛ)
   b_END_OPL = 0;           // Признак завершения работы всех помех
   SPEED_DGN = 0;           // Скорость догона (преследования)

   Time_Out_Contact = 0;    // Таймер отсутствия контакта
   bSetPror = 0;            // Признак выполнения манёвра прорыва/наведения
   KURS_NULL = 0;           // Исходный курс (для возврата)
   m_pTFM = NULL;           // Указатель на массив формуляров целей (от сервера)
   m_nTFM = 0;              // Количество записей в m_pTFM
   time_observ = 0;         // Время наблюдения

   Time_Reset = 0;          // Таймер для сброса состояния
   SPEED_SEARCH = 0;        // Скорость поиска
   b_Class_VP = 0;          // Признак классификации помехи
   type_moving = 1;         // Текущий тип движения (1 - поиск, 3 - наведение)

   m_pTR = NULL;            // Указатель на массив отчётов о трассах (цели от ГАК)
   m_nTrassReport = 0;      // Количество отчётов

   bCMD_K = 0;              // Признак, что команда по курсу уже выполнена по времени (из формуляра)
   bCMD_V = 0;              // Признак, что команда по скорости уже выполнена по времени

   T_S = 0;                 // Таймер стабилизации
   SPEED_KURS_CMD = 0;      // Скорость в начале манёвра курса
   delta_speed_r = 0;       // Накопленное изменение скорости при антициркуляции
   delta_speed = 0;         
   m_bBump = FALSE;         // Флаг столкновения
   usk0 = 0;                // Начальное ускорение
   ostatok = 0;             // Оставшееся время переходного процесса
   k0 = 0;
   d_SPEED_KURS = 0;        // Коэффициент изменения скорости на циркуляции
   time_speed_kurs_cmd = 0; // Расчётное время переходного процесса по скорости на циркуляции
   SPEED_MIN = 4;           // Минимальная скорость хода
   D_TIME = 1.0f;           // Шаг интегрирования по времени (будет переопределён)
   CLASS_PL = 1;            // Класс ПЛ (проект) - здесь используется для БЭК, но сохранено имя
   SPEED_MAX = 30.0f;       // Максимальная скорость
   m_Me.V = 5.0f;           // Начальная скорость объекта
   KURS_TEC = m_Me.K = KURS_CMD = 0.0f; // Текущий, заданный и командный курс
   KURS_UP = 0.0f;          // Текущая угловая скорость поворота
   speed_kurs_exit = speed_kurs_antre = speed_kurs = FALSE; // Флаги состояний изменения скорости на циркуляции
   avto_exit = perechet = rachet_exp = rachet_pow = avto_exit_extr = FALSE; // Флаги автоматического выхода из циркуляции, пересчёта и т.д.
   SPEED_CMD_MAX = SPEED_MAX;  // Максимально допустимая скорость по состоянию ГЭУ (здесь равна максимальной)
   f_new_cmd = TRUE;
   time_speed_kurs = 0.0f;
   SPEED_K_FIX = 0.0f;      // Зафиксированная скорость на начало манёвра
   speed_tec = 0.0f;
   avto_exit_speed = FALSE;
   flag_1 = FALSE;
   time_speed_exit = 0.0f;
   A_ZACLAD_TEC = A_ZACLAD_s = A_ZACLAD = 0.0f; // Угол закладки рулей (текущий, сохранённый, командный)
   SPEED_CMD_OLD = 0.0f;
   ZIRC = ZIRC_TEC = 0;     // Направление циркуляции (1 - вправо, 0 - влево)

   CAppKey key;             // объект key , настроенный на корневой ключ реестра приложения
}

CMainDoc::~CMainDoc() // Деструктор
{
   CAppKey key;
   if (m_pTFM)              // Если был выделен массив формуляров целей
   {
      delete[] m_pTFM;      // Освобождаем память
      m_pTFM = NULL;
   }
   m_nTFM = 0;
}

float DISTANCE(float X1, float Y1, float X2, float Y2) // Вспомогательная функция вычисления расстояния на плоскости
{
   float d_x = -X1 + X2;     // Разность по X
   float d_y = -Y1 + Y2;     // Разность по Y
   return float(sqrt(d_x * d_x + d_y * d_y)); // Евклидово расстояние
}
//определение полной  дистанции
float FULL_DISTANCE(float X1, float Y1, float H1, float X2, float Y2, float H2) // Расстояние в 3D
{
   float d_x = -X1 + X2;
   float d_y = -Y1 + Y2;
   float d_h = -H1 + H2;
   return float(sqrt(d_x * d_x + d_y * d_y + d_h * d_h));
}
//определение вертикального курсового угла
//отрицательный - против часовой стрелки
float VERT_ANG_KOORD(float X1, float Y1, float H1, float X2, float Y2, float H2, float diff) // Вертикальный угол между объектами
{
   float d = DISTANCE(X1, Y1, X2, Y2); // Горизонтальная дистанция
   float dz = H1 - H2;             // Разность глубин
   return (float)(atan2(dz, d) * RdGr - diff); // Угол в градусах с поправкой на дифферент
}
//определение пеленга
float PELENG(float X1, float Y1, float X2, float Y2) // Пеленг (азимут) из точки 1 на точку 2
{
   float d = DISTANCE(X1, Y1, X2, Y2); // Горизонтальная дистанция
   float dy_d = (-Y1 + Y2) / d;      // Косинус угла? Здесь вычисляется через арккосинус
   if (fabs(dy_d) > 1.0f)           // Защита от выхода за [-1,1]
   {
      if (dy_d > 0.0f) dy_d = 1.0f;
      else dy_d = -1.0f;
   }
   float out_angl = 180.0f * acos(dy_d) / M_PI; // Угол в градусах от оси Y (север?)
   if (-X1 + X2 >= 0.0f)            // Учёт знака проекции на X для определения четверти
      return out_angl;
   else
      return float(360.0f - out_angl);
}
//Определение курсового угла
float KURS_ANG_KOORD(float X1, float Y1, float X2, float Y2, float K2) // Курсовой угол на цель (от -180 до 180)
{																			  // (угол между направлением движения носителя и направлением на цель)
   float d_k = PELENG(X1, Y1, X2, Y2) - K2;                // Разность пеленга и курса носителя
   if (d_k > 0.0f && d_k <= 180.0f) return d_k;            // возвращается d_k - в (0, 180] (справа)
   if (d_k > 0.0f && d_k > 180.0f) return d_k - 360.0f;    // возвращается (d_k - 360) -  в (-180, 0) (слева)
   if (d_k < 0.0f && d_k >= -180.0f) return d_k;           // возвращается d_k - в [-180, 0) (слева)
   if (d_k < 0.0f && d_k < -180.0f) return d_k + 360.0f;   // возвращается (d_k + 360) -  в (0, 180] (справа)
   return 0.0f;														  // возвращается 0 (прямо по курсу)
}

BOOL CMainDoc::OnNewDocument() // Вызывается при создании нового документа
{
   if (!CDocument::OnNewDocument()) // Вызов базового класса
      return FALSE;

   // Создание клиента для связи с модельным сервером "World"
   if (!xworld.Create(this,                   // Указатель на целевой объект для обработки сообщений
                  ID_PORT_WORLD,              // Идентификатор команды, по которой будет вызываться OnPortWorld
                  XREQ_Q_MOVE | XREQ_TARGETS, // Запрашиваемые данные: кванты перемещения ИЛИ цели
                  g_cmdInfo.m_nID,            // Идентификатор носителя из командной строки
                  110,                        // Неиспользуемый параметр (в старой версии)
                  1,                          // Признак "носитель" (bCarrier = TRUE)
                  IDB_SYSID,                  // Идентификатор битмапа для отображения (обычный)
                  0,                          // Идентификатор битмапа для выделенного состояния (не используется)
                  0,                          // Квант работы (0 - по умолчанию)
                  0,                          // Дистанция столкновения (не задана)
                  40,                         // Длина объекта, м
                  31,                         // Класс объекта
                  48,                         // Магнитный момент
                  10                          // Ширина объекта, м
                 )
      )
   {
      return FALSE; // Не удалось подключиться к серверу
   }

   return TRUE;
}

void CMainDoc::Serialize(CArchive& ar) // Сериализация документа (не используется в данной модели)
{
   if (ar.IsStoring())
   {
      // Сохранение (пусто)
   }
   else
   {
      // Загрузка (пусто)
   }
}

#ifdef _DEBUG
void CMainDoc::AssertValid() const // Проверка целостности объекта в отладке
{
   CDocument::AssertValid();
}

void CMainDoc::Dump(CDumpContext& dc) const // Вывод отладочной информации
{
   CDocument::Dump(dc);
}
#endif

void CMainDoc::OnPortWorld() // Основной обработчик сообщений от модельного сервера (через маппинг)
{
   DWORD xcmd = xworld.GetXCmd();     // Получаем код команды от сервера
   CArchive& ar = xworld.GetArchive(); // Получаем архив с данными

   if (ar.IsStoring())             // Если архив - для записи (сейчас не используется)
   {
   }
   else                        // Архив - для чтения
   {
      m_bBump = FALSE;            // Сброс флага столкновения
      switch (xcmd)               // Обработка в зависимости от кода команды xcmd
      {
      case XCMD_INITCLIENT:         // Инициализация клиента (первое подключение)
         ar >> m_Time;            // Читаем текущее модельное время

         DWORD dwFormLen;         // Длина формуляра объекта
         ar >> dwFormLen;

         TCHAR buf[8];
         wsprintf(buf, _T("a%d"), xworld.m_nCarrierID); // Формируем заголовок окна с ID носителя
         SetTitle(buf);

         ASSERT(dwFormLen == 0 ||                  // Проверка длины формуляра
               dwFormLen == sizeof(m_Me) ||
               dwFormLen == sizeof(m_Me) + sizeof(m_form)
              );

         m_form.bDemo = FALSE;                     // Сброс флага демо-режима
         if (dwFormLen > 0)								// если данные есть, считываем из буфера
         {
            ar.Read(&m_Me, sizeof(m_Me));            // Читаем начальные параметры движения (координаты, курс, скорость)
            if (dwFormLen > sizeof(m_Me))
            {
               ar.Read(&m_form, sizeof(m_form));      // Читаем формуляр с командами управления
            }
            else
            {
               m_form.bUpr = FALSE;               // Если формуляр не передан, управление отключено
            }
         }
         else													// Если данных нет - берем из реестра/файла формуляра
         {
            CAppKey key;	// Получаем объект key, настроенный на корневой ключ реестра приложения
            key[_T("m_Me")].Read(&m_Me, sizeof(m_Me));  // Читаем параметры из реестра/файла
            FromFileRead("USV.frm", &m_form, sizeof(m_form)); // Читаем формуляр из файла
            m_form.bDemo = TRUE;                  // Включаем демо-режим (показ диалогов)
         }

         if (m_form.bDemo && bZERORETURN)            // Если демо и разрешён диалог
         {
            CTXParamsDlg dlg(&m_Me);               // Диалог ввода начальных координат
            CFormularDlg dlg2(&m_form);             // Диалог ввода команд управления
            if (dlg.DoModal() != IDOK || dlg2.DoModal() != IDOK)
            {
               xworld.ExitApp();                  // Выход при отмене
            }
            CAppKey key;	// Получаем объект key, настроенный на корневой ключ реестра приложения
            key[_T("m_Me")].Write(&m_Me, sizeof(m_Me)); // Сохраняем параметры
         }

         SPEED_CMD = SPEED_SEARCH = m_Me.V;           // Устанавливаем текущую скорость как командную и скорость поиска
         KURS_CMD = KURS_TEC = KURS_NULL = m_Me.K;      // Устанавливаем курс

         xworld.SetXParams(&m_Me);                  // Отправляем начальные параметры на сервер

         xworld.SendResponse(XCMD_INITCLIENT);         // Подтверждаем инициализацию
         UpdateAllViews(NULL);                     // Обновляем представление
         break;

      case XREQ_WORLD_Q_MOVE:                      // Квант "перемещение"
         ar >> m_Time;                           // Текущее время
         ar >> m_TimeStep;                        // Шаг времени в мс
         Move();                               // Выполняем расчёт перемещения и динамики
         xworld.SendResponse(XREQ_WORLD_Q_MOVE);       // Отправляем подтверждение
         UpdateAllViews(NULL);
         break;

      case XCMD_WORLD_BUMP:                        // Столкновение
      {
         DWORD nClass;
         ar >> nClass;                           // Класс объекта, с которым столкнулись
         if (nClass == 1 || nClass / 10 == 1)         // Если это носитель (класс 1x - торпеда?)
         {
            xworld.ExitApp();                     // Завершаем приложение
         }
         break;
      }

      case XREQ_TARGETS:                          // Получение списка обнаруженных целей (от ГАК)
      {
         DWORD len;
         DWORD cnt;
         ar >> len;                             // Длина данных

         m_nTrassReport = len / sizeof(TTrassReport);   // Количество записей
         if (m_nTrassReport > 0)						// Если есть новые записи
         {
            delete m_pTR;                        // Удаляем старый массив
            m_pTR = new TTrassReport[m_nTrassReport];   // Выделяем новый
            ar >> cnt;                          // Читаем количество (избыточно)
            for (int i = 0; i < m_nTrassReport; i++)
            {
               ar.Read(&m_pTR[i], sizeof(TTrassReport)); // Заполняем массив
            }
         }
      }

      Step(1000.0);                              // Обработка целей (шаг 1000 мс)
      break;

      case XREQ_SITUATION:                        // Получение полной ситуации от сервера (все объекты)
      {
         TXParams mo;
         float pel;					// Пеленг
         float dist;					// Дистанция
         DWORD t;
         UINT nObj;

         ar >> nObj;             // Количество объектов
         if (m_pTFM)
         {
            delete[] m_pTFM;
            m_pTFM = NULL;
            m_nTFM = 0;
         }
         if (nObj > 0)
         {
            m_nTFM = nObj - 1;   // Минус собственный объект
            m_pTFM = new TFM[nObj];
            for (UINT i = 0, j = 0; i < nObj; i++)
            {
               ar >> m_pTFM[j].nNum;               // Номер объекта
               ar >> m_pTFM[j].sName;              // Имя объекта
               ar.Read(&mo, sizeof(TXParams));       // Параметры движения
               //bmw ar >> t;                        // Время? (пропускаем)
               ar >> t;
               if (m_pTFM[j].nNum == xworld.m_nCarrierID) // Пропускаем собственный объект
               {
                  continue;
               }
               pel = KURS_ANG_KOORD(m_Me.X, m_Me.Y, mo.X, mo.Y, m_Me.K); // Курсовой угол на объект
               dist = distance(&m_Me, &mo);     // Дистанция
               m_pTFM[j].Y = mo.Y;              // Сохраняем координату Y
               m_pTFM[j].p = pel;               // Пеленг
               m_pTFM[j].dp = 0.007f;           // СКО пеленга (задано константой)
               m_pTFM[j].sp = dist;             // Сигнал/помеха (заглушка)
               if (j == 0)                      // Первый объект считаем целью
               {
                  m_En = mo;
               }
               j++;
            }
         }
      }
      break;

      case XCMD_CMD:                             // Команда управления от внешней системы
      {
         DWORD cmd, len;
         ar >> cmd >> len;                        // Код команды и длина данных
         switch (cmd)
         {
         case 'PRV':                            // Команда "начать прорыв/преследование"
         {
            int type;
            float s;

            ar >> type;                         // Тип логики (1 - прорыв, 2 - слежение)
            ar >> s;                           // Скорость

            SPEED_DGN = SPEED_CMD = s;              // Устанавливаем скорость
            SU_SPEED = 1;                        // Тип управления скоростью

            xworld.Docs(1, "Команда на изменение скорости=%f, R=%d", s, type);

            rachet_pow = FALSE;                   // Сброс флага расчёта мощности
            Comand_V();                         // Применить команду скорости

            m_form.Type_Logic = type;               // Сохраняем тип логики
            break;
         }

         case 'KURS':                           // Команда изменения курса
         {
            float k, ak;
            int su, z, bOtn;

            ar >> k     // Заданный курс
               >> ak    // Угол закладки рулей
               >> su    // Тип управления по курсу (1,2,3)
               >> z     // Направление циркуляции
               >> bOtn; // Признак относительности (если 1, курс задаётся относительно текущего)

            // Проверки корректности
            if (k == KURS_CMD && ak == A_ZACLAD && su == SU_KURS && z == ZIRC)
            {
               AfxMessageBox("Выдано сообщение? Проверьте кодировку");
               break;
            }
            if (k < 0.0f || k >= 360.0f)
            {
               AfxMessageBox("Курс должен быть от 0 до 359 градусов");
               break;
            }
            if (ak <= 0.0f)
            {
               AfxMessageBox("Угол перекладки должен быть больше 0");
               break;
            }
            if (ak > 35.0f)
            {
               AfxMessageBox("Угол перекладки должен быть не более 35 градусов");
               break;
            }
            if (!(su == 1 || su == 2 || su == 3))
            {
               AfxMessageBox("Неправильный тип управления? Или кодировка");
               break;
            }

            if (bOtn)                           // Относительный курс
            {
               KURS_CMD = k + m_Me.K;              // Складываем с текущим
               if (KURS_CMD > 360.0f) KURS_CMD -= 360.0f;
               if (KURS_CMD < 0.0f) KURS_CMD += 360.0f;
            }
            else
            {
               KURS_CMD = k;                     // Абсолютный курс
            }

            A_ZACLAD = ak;
            SU_KURS = su;
            if (bOtn)                           // Для относительного курса направление циркуляции определяется знаком k
            {
               if (k < 0.0f) ZIRC = 0;
               else ZIRC = 1;
            }
            else
            {
               ZIRC = z;
            }

            avto_exit = perechet = FALSE;
            xworld.Docs(1, "Команда на изменение курса=%f, R=%f, SU=%d, Z=%d", k, ak, su, z);

            dv = SPEED_CMD - m_Me.V;               // Разность скоростей (используется в Comand_K)
            Comand_K();                         // Применить команду курса

            break;
         }

         case 'SPD':                            // Команда изменения скорости
         {
				float s;
            int su;

            ar >> s   // Заданная скорость
               >> su;  // Тип управления

            if (s > SPEED_MAX)
            {
               AfxMessageBox("Заданная скорость больше максимально возможной");
               break;
            }
            if (s < SPEED_MIN)
            {
               AfxMessageBox("Скорость не может быть менее минимальной");
               break;
            }

            SPEED_CMD = s;
            SU_SPEED = su;

            rachet_pow = FALSE;
				xworld.Docs(1, "Команда на изменение скорости=%f", s); //(1, "Команда на изменение скорости=%f, R=%d", s, type)
            Comand_V();
            break;
         }

         default: ;
         }
      }
      break;
      }
   }
}

void CMainDoc::Move() // Главный такт расчёта динамики и логики управления
{
   ASSERT(this);

   // --- Логика работы в зависимости от Type_Logic (из формуляра) ---
   if (m_form.Type_Logic == 1) // Режим "прорыв"
   {
      if (!bSetPror) // Если ещё не выполняем манёвр
      {
         float pel_R = 0.0f;
         // Ищем помеху (объекты с номерами 6, 7, 8) среди известных целей
         for (UINT i = 0; i < m_nTFM; i++)
         {
            if (m_pTFM[i].nNum == 6 || m_pTFM[i].nNum == 7 || m_pTFM[i].nNum == 8)
            {
               pel_R = m_pTFM[i].p; // Берём пеленг на первую найденную помеху
               break;
            }
         }

         if (pel_R != 0.0f && time_observ > 100000) // Если помеха есть и прошло достаточно времени наблюдения
         {
            bSetPror = 1;                     // Входим в режим прорыва
            float newKurs = m_Me.K + pel_R;   // Курс на помеху
            // Нормализация курса
            while (newKurs >= 360.0f) newKurs -= 360.0f;
            while (newKurs < 0.0f)    newKurs += 360.0f;

            if (fabs(newKurs - KURS_CMD) > ANGLE_EPSILON || !avto_exit) //bmw 
            {
                KURS_CMD = newKurs;

                A_ZACLAD = 25.0f;                  // Угол закладки 25°
                SU_KURS = 3;                     // Тип управления 3 (резкий)
                if (pel_R > 0.0f) ZIRC = 1; else ZIRC = 0; // Направление циркуляции

                avto_exit = perechet = FALSE;
                time_observ = 0;
                dv = SPEED_CMD - m_Me.V;
                Comand_K();                      // Выполнить поворот
            }
         }
      }
      else // Уже в режиме прорыва
      {
         bSetPror = 0;
         // Проверяем, есть ли ещё помехи
         for (UINT i = 0; i < m_nTFM; i++)
         {
            if (m_pTFM[i].nNum == 6 || m_pTFM[i].nNum == 7 || m_pTFM[i].nNum == 8)
            {
               bSetPror = 1;
               break;
            }
         }

         if (!bSetPror) // Помех больше нет - возвращаемся на исходный курс и скорость поиска
         {
            if (KURS_CMD != KURS_NULL)
            {
               KURS_CMD = KURS_NULL;
               A_ZACLAD = 25.0f;
               SU_KURS = 3;
               ZIRC = 0;
               avto_exit = perechet = FALSE;
               dv = SPEED_CMD - m_Me.V;
               Comand_K();
            }

            SPEED_CMD = SPEED_SEARCH;
            SU_SPEED = 1;
            rachet_pow = FALSE;
            Comand_V();
         }
      }
   }

   if (m_form.Type_Logic == 2) // Режим "слежение" (преследование)
   {
      b_END_OPL = 1;
      if (b_1Contact_OPL == 0) b_END_OPL = 0;

      for (UINT k = 0; k < m_nTFM; k++)
      {
         if ((b_1Contact_OPL == 0) && (m_pTFM[k].nNum == 6 || m_pTFM[k].nNum == 7 || m_pTFM[k].nNum == 8))
            b_1Contact_OPL = 1;
         if ((b_1Contact_OPL == 1) && (m_pTFM[k].nNum == 6 || m_pTFM[k].nNum == 7 || m_pTFM[k].nNum == 8))
            b_END_OPL = 0;
      }

      // Обработка обнаруженных трасс от ГАК
      if (m_nTrassReport > 0)
      {
         float teta_SR = 0.0f;
         for (int i = 0; i < m_nTrassReport; i++)
         {
            b_PGN = 0;
            if (m_pTR[i].nNum == 2) // Цель класса ПЛ (номер 2)
            {
               for (UINT j = 0; j < m_nTFM; j++)
               {
                  if (m_pTFM[j].nNum == 2)
                  {
                     teta_SR = m_pTFM[j].p;
                     if (m_pTFM[j].Y > m_Me.Y)
                     {
                        xworld.Docs(2, "Обнаружение ПЛ противника");
                     }
                     break;
                  }
               }
               Time_Out_Contact = 0;
               b_PGN = 1;
               if (b_END_OPL == 1)
                  xworld.Docs(33, "Обнаружение ПЛ D =%f", m_pTR[i].dist);
               else
                  xworld.Docs(5, "Обнаружение ПЛ D =%f", m_pTR[i].dist);

               // Поворот на цель
               float newKurs = m_Me.K + teta_SR;
               if (teta_SR < 0.0f) ZIRC = 0; else ZIRC = 1;
               while (newKurs >= 360.0f) newKurs -= 360.0f;
               while (newKurs < 0.0f)    newKurs += 360.0f;

               if (fabs(newKurs - KURS_CMD) > ANGLE_EPSILON || !avto_exit)
               {
                   KURS_CMD = newKurs;
                   A_ZACLAD = 25.0f;
                   SU_KURS = 3;
                   avto_exit = perechet = FALSE;
                   dv = SPEED_CMD - m_Me.V;
                   Comand_K();
               }

               SPEED_CMD = SPEED_MAX; // Максимальная скорость для преследования
               SU_SPEED = 1;
               rachet_pow = FALSE;
               Comand_V();

               goto m14; // Выходим из обработки ситуации
            }
         }
      }

      // Если цель не обнаружена, работаем по помехам аналогично режиму 1
      if (!bSetPror)
      {
         float pel_R = 0.0f;
         for (UINT i = 0; i < m_nTFM; i++)
         {
            if (m_pTFM[i].nNum == 6 || m_pTFM[i].nNum == 7 || m_pTFM[i].nNum == 8)
            {
               pel_R = m_pTFM[i].p;
               if (m_pTFM[i].Y > m_Me.Y)      // Помеха впереди? Выбор скорости
               {
                  SPEED_CMD = SPEED_SEARCH;
               }
               else
               {
                  SPEED_CMD = SPEED_DGN;
               }
               SU_SPEED = 1;
               rachet_pow = FALSE;
               Comand_V();
               break;
            }
         }

         if (pel_R != 0.0f)
         {
            bSetPror = 1;
            float newKurs = m_Me.K + pel_R;
            while (newKurs >= 360.0f) newKurs -= 360.0f;
            while (newKurs < 0.0f)    newKurs += 360.0f;

            if (fabs(newKurs - KURS_CMD) > ANGLE_EPSILON || !avto_exit)
            {
                KURS_CMD = newKurs;
                A_ZACLAD = 25.0f;
                SU_KURS = 3;
                if (pel_R > 0.0f) ZIRC = 1; else ZIRC = 0;
                avto_exit = perechet = FALSE;
                time_observ = 0;
                dv = SPEED_CMD - m_Me.V;
                Comand_K();
            }
         }
      }
      else
      {
         // Аналогично проверке наличия помех и возврату
         bSetPror = 0;
         for (UINT i = 0; i < m_nTFM; i++)
         {
            if (m_pTFM[i].nNum == 6 || m_pTFM[i].nNum == 7 || m_pTFM[i].nNum == 8)
            {
               bSetPror = 1;
               if (m_pTFM[i].Y > m_Me.Y)
                  SPEED_CMD = SPEED_SEARCH;
               else
                  SPEED_CMD = SPEED_DGN;
               SU_SPEED = 1;
               rachet_pow = FALSE;
               Comand_V();
               break;
            }
         }
         if (!bSetPror)
         {
            if (KURS_CMD != KURS_NULL)
            {
               KURS_CMD = KURS_NULL;
               A_ZACLAD = 25.0f;
               SU_KURS = 3;
               ZIRC = 0;
               avto_exit = perechet = FALSE;
               dv = SPEED_CMD - m_Me.V;
               Comand_K();
            }
            SPEED_CMD = SPEED_SEARCH;
            SU_SPEED = 1;
            rachet_pow = FALSE;
            Comand_V();
         }
      }
   m14: ;
   }

   if (m_form.Type_Logic == 3) // Режим "наведение" (упрощённый)
   {
      // Аналогично установке флагов b_END_OPL
      b_END_OPL = 1;
      if (b_1Contact_OPL == 0) b_END_OPL = 0;
      for (UINT k = 0; k < m_nTFM; k++)
      {
         if ((b_1Contact_OPL == 0) && (m_pTFM[k].nNum == 6 || m_pTFM[k].nNum == 7 || m_pTFM[k].nNum == 8))
            b_1Contact_OPL = 1;
         if ((b_1Contact_OPL == 1) && (m_pTFM[k].nNum == 6 || m_pTFM[k].nNum == 7 || m_pTFM[k].nNum == 8))
            b_END_OPL = 0;
      }

      if (m_nTrassReport > 0)
      {
         float teta_SR = 0.0f;
         for (int i = 0; i < m_nTrassReport; i++)
         {
            b_PGN = 0;
            if (m_pTR[i].nNum == 2) //Обнаружена ПЛ
            {
               for (UINT j = 0; j < m_nTFM; j++)
               {
                  if (m_pTFM[j].nNum == 2)
                  {
                     teta_SR = m_pTFM[j].p;
                     if (m_pTFM[j].Y > m_Me.Y)
                        xworld.Docs(2, "Обнаружение ПЛ противника");
                     break;
                  }
               }
               Time_Out_Contact = 0;
               b_PGN = 1;
               if (b_END_OPL == 1)
                  xworld.Docs(33, "Обнаружение ПЛ D =%f", m_pTR[i].dist);
               else
                  xworld.Docs(5, "Обнаружение ПЛ D =%f", m_pTR[i].dist);
            }
         }
      }
   }

   // --- Обработка подрежимов движения (type_moving) ---
   switch (type_moving)
   {
   case 0: break;
   case 1: // Поиск
      if (m_form.Type_Logic == 1) _1_Searching();
      break;
   case 2: break;
   case 3: // Наведение
      if (m_form.Type_Logic == 3/* ==1 ???*/) _3_Pointing();
      break;
	default: //Обработка значений вне диапазона
        break; 
   }

   D_TIME = float(m_TimeStep) / 1000.0f; // Шаг времени в секундах

   dv = SPEED_CMD - m_Me.V;

   OnSPEED();  // Моделирование динамики скорости
   OnKURS();   // Моделирование динамики курса
   OnTRAEKT(); // Интегрирование координат
   dv = SPEED_CMD - m_Me.V;

   // --- Обработка команд из формуляра по времени ---
   if (m_form.bUpr)
   {
      if (!bCMD_K)
      {
         if (m_Time > m_form.T_CMD_K) // Если наступило время подачи команды курса
         {
            bCMD_K = 1;
            if (m_form.bOtn)
            {
               KURS_CMD = m_Me.K + m_form.KURS_CMD;
               if (KURS_CMD > 360.0f) KURS_CMD -= 360.0f;
               if (KURS_CMD < 0.0f) KURS_CMD += 360.0f;
            }
            else
            {
               KURS_CMD = m_form.KURS_CMD;
            }
            // Нормализация...
            A_ZACLAD = m_form.A_ZACLAD;
            SU_KURS = m_form.SU_KURS;
            ZIRC = m_form.ZIRC;
            avto_exit = perechet = FALSE;
            dv = SPEED_CMD - m_Me.V;
            Comand_K();
         }
      }
      if (!bCMD_V)
      {
         if (m_Time > m_form.T_CMD_V) // Время команды скорости
         {
            bCMD_V = 1;
            SPEED_CMD = m_form.SPEED_CMD;
            SU_SPEED = m_form.SU_SPEED;
            rachet_pow = FALSE;
            Comand_V();
         }
      }
   }

   xworld.SetXParams(&m_Me); // Отправляем обновлённые параметры на сервер
}


// Вспомогательная функция для расчёта изменения скорости во время циркуляции (замедление)
void CMainDoc::SPEED_KURS_ANTRE()
{
   if (!avto_exit && time_speed_kurs < time_speed_kurs_cmd)
   {
      if (!rachet_pow)
      {
         if (speed_kurs_antre)
         {
            SPEED_K_FIX = m_Me.V;
            speed_kurs_antre = FALSE;
         }
         // Параболическое уменьшение скорости
         float x = d_SPEED_KURS * (time_speed_kurs_cmd / 2.0f - fabs(-time_speed_kurs_cmd / 2.0f + time_speed_kurs)) * D_TIME;
         SPEED_CMD = m_Me.V -= x;
         delta_speed_r += x;
      }
      else
      {
         SPEED_CMD_OLD = -1.0f;
         m_Me.V -= d_SPEED_KURS * (time_speed_kurs_cmd / 2.0f - fabs(-time_speed_kurs_cmd / 2.0f + time_speed_kurs)) * D_TIME;
         speed_kurs_antre = TRUE;
      }
      time_speed_kurs += D_TIME;
      ostatok = time_speed_kurs_cmd - time_speed_kurs;
   }
}

// Вспомогательная функция для восстановления скорости после циркуляции
void CMainDoc::SPEED_KURS_EXIT()
{
   if (avto_exit_speed)
   {
      if (!flag_1)
      {
         time_speed_exit = ostatok;
         SPEED_K_FIX = m_Me.V;
         speed_tec = m_Me.V;
         flag_1 = TRUE;
         float v_cmd = 0.0f;
         int V1 = int(m_Me.V / 5.0f);
         int V2 = V1 + 1;
         int X1 = int(A_ZACLAD_s / 5.0f);
         int X2 = X1 + 1;

         if (delta_speed_r > 0.0f)
         {
            v_cmd = m_Me.V + delta_speed_r;
         }
         else
         {
            // Интерполяция из таблицы MV для определения ожидаемой скорости в конце циркуляции
            float Y1 = MeKn * mv[V1].MATR_V[X1] + (A_ZACLAD_s - 5.0f * X1) * (MeKn * mv[V1].MATR_V[X2] - MeKn * mv[V1].MATR_V[X1]) / (5.0f * (X2 - X1));
            float Y2 = MeKn * mv[V2].MATR_V[X1] + (A_ZACLAD_s - 5.0f * X1) * (MeKn * mv[V2].MATR_V[X2] - MeKn * mv[V2].MATR_V[X1]) / (5.0f * (X2 - X1));
            v_cmd = 2.0f * m_Me.V - (Y1 + (Y2 - Y1) * (m_Me.V - 5.0f * V1) / (5.0f * (V2 - V1))) * 0.9893f; // Было * 0.98f
         }																							// 0.9893f - коэфф. гидродинамич. сопротивл.?

         // Интерполяция времени из таблицы MT
         float Y1 = mt[V1].MATR_T[X1] + (A_ZACLAD_s - 5.0f * X1) * (mt[V1].MATR_T[X2] - mt[V1].MATR_T[X1]) / (5.0f * (X2 - X1));
         float Y2 = mt[V2].MATR_T[X1] + (A_ZACLAD_s - 5.0f * X1) * (mt[V2].MATR_T[X2] - mt[V2].MATR_T[X1]) / (5.0f * (X2 - X1));
         time_speed_kurs_cmd = (Y1 + (Y2 - Y1) * (m_Me.V - 5.0f * V1) / (5.0f * (V2 - V1)));

         if (time_speed_kurs_cmd < 10.0f)
            d_SPEED_KURS = 4.0f * (-m_Me.V + v_cmd) / (time_speed_kurs_cmd * time_speed_kurs_cmd);
      }

      if (time_speed_exit >= time_speed_kurs_cmd)
      {
         avto_exit_speed = FALSE;
         d_SPEED_KURS = 0.0f;
         time_speed_kurs_cmd = 0.0f;
         time_speed_kurs = 0.0f;
         flag_1 = FALSE;
         delta_speed_r = 0.0f;
         time_speed_exit = 0.0f;
         return;
      }
      else
      {
         time_speed_exit += D_TIME;
      }

      if (!rachet_pow)
      {
         SPEED_CMD = m_Me.V += d_SPEED_KURS * (time_speed_kurs_cmd / 2.0f - fabs(-time_speed_kurs_cmd / 2.0f + time_speed_exit)) * D_TIME;
         if (SPEED_CMD > SPEED_MAX) SPEED_CMD = SPEED_MAX;
         if (m_Me.V > SPEED_MAX) m_Me.V = SPEED_MAX;
      }
      else
      {
         m_Me.V += d_SPEED_KURS * (time_speed_kurs_cmd / 2.0f - fabs(-time_speed_kurs_cmd / 2.0f + time_speed_exit)) * D_TIME;
         if (m_Me.V > SPEED_MAX) m_Me.V = SPEED_MAX;
      }
   }
}

// Подготовка параметров для выполнения команды курса
void CMainDoc::Comand_K()
{
	while (KURS_CMD >= 360.0f) KURS_CMD -= 360.0f; //bmw Нормализация командного курса,
   while (KURS_CMD < 0.0f)    KURS_CMD += 360.0f; //bmw если он вышел за пределы [0,360)
   
	if (A_ZACLAD_TEC != A_ZACLAD) // Если угол закладки изменился, пересчитываем коэффициенты
   {
      avto_exit = perechet = FALSE;
      {
         int V1 = int(m_Me.V / 5.0f);
         int V2 = V1 + 1;
         int X1 = int(A_ZACLAD / 5.0f);
         int X2 = X1 + 1;

         // Интерполяция KURS_UP_LIM (предельная угловая скорость) из таблицы mk
         float Y1 = mk[V1].MATR_KURS[X1] + (A_ZACLAD - 5.0f * X1) * (mk[V1].MATR_KURS[X2] - mk[V1].MATR_KURS[X1]) / (5.0f * (X2 - X1));
         float Y2 = mk[V2].MATR_KURS[X1] + (A_ZACLAD - 5.0f * X1) * (mk[V2].MATR_KURS[X2] - mk[V2].MATR_KURS[X1]) / (5.0f * (X2 - X1));
         KURS_UP_LIM = (Y1 + (Y2 - Y1) * (m_Me.V - 5.0f * V1) / (5.0f * (V2 - V1)));
      }

      {
         int V1 = int(m_Me.V / 5.0f);
         int V2 = V1 + 1;
         int X1 = int(A_ZACLAD / 5.0f);
         int X2 = X1 + 1;

         // Интерполяция времени переходного процесса из mt
         float Y1 = mt[V1].MATR_T[X1] + (A_ZACLAD - 5.0f * X1) * (mt[V1].MATR_T[X2] - mt[V1].MATR_T[X1]) / (5.0f * (X2 - X1));
         float Y2 = mt[V2].MATR_T[X1] + (A_ZACLAD - 5.0f * X1) * (mt[V2].MATR_T[X2] - mt[V2].MATR_T[X1]) / (5.0f * (X2 - X1));
         float t = Y1 + (Y2 - Y1) * (m_Me.V - 5.0f * V1) / (5.0f * (V2 - V1));
         if (t > time_speed_kurs_cmd) time_speed_kurs_cmd = t;
      }

      {
         float VV = 0.0f;
         if (!rachet_exp)
         {
            VV = SPEED_K_FIX = m_Me.V;
            if (SPEED_CMD == m_Me.V) SPEED_CMD_OLD = SPEED_CMD;
         }
         else
         {
            VV = SPEED_K_FIX;
         }

         int V1 = int(VV / 5.0f);
         int V2 = V1 + 1;
         int X1_V = int(A_ZACLAD / 5.0f);
         int X2_V = X1_V + 1;

         // Интерполяция конечной скорости на циркуляции из mv
         float Y1 = MeKn * mv[V1].MATR_V[X1_V] + (A_ZACLAD - 5.0f * X1_V) * (MeKn * mv[V1].MATR_V[X2_V] - MeKn * mv[V1].MATR_V[X1_V]) / (5.0f * (X2_V - X1_V));
         float Y2 = MeKn * mv[V2].MATR_V[X1_V] + (A_ZACLAD - 5.0f * X1_V) * (MeKn * mv[V2].MATR_V[X2_V] - MeKn * mv[V2].MATR_V[X1_V]) / (5.0f * (X2_V - X1_V));
         float v_cmd = (Y1 + (Y2 - Y1) * (VV - 5.0f * V1) / (5.0f * (V2 - V1))) * 0.9893f;

         // Коэффициент для параболического изменения скорости
         d_SPEED_KURS = 4.0f * (m_Me.V - v_cmd) / (time_speed_kurs_cmd * time_speed_kurs_cmd);
      }
      time_speed_kurs = 0.0f;
   }

   A_ZACLAD_TEC = A_ZACLAD;
   ZIRC_TEC = ZIRC;
   avto_exit = perechet = FALSE;
}

// Моделирование изменения курса (циркуляции)
void CMainDoc::OnKURS()
{
   dv = SPEED_CMD - m_Me.V;
   CString s;
   SPEED_KURS_EXIT(); // Обработка восстановления скорости после циркуляции

   if (A_ZACLAD_TEC != A_ZACLAD || ZIRC_TEC != ZIRC)
      avto_exit = FALSE;

//   if (KURS_TEC != KURS_CMD) // Если есть рассогласование по курсу
	if (fabs(KURS_TEC - KURS_CMD) > 0.01f) //bmw Если рассогласование по курсу не превышает погрешности float
   {
      if (!rachet_exp)
      {
         // Экспоненциальная модель для коэффициента x
         x = kk_kurs.RK1 * exp(-kk_kurs.RK2 * pow((SPEED_MAX - m_Me.V) / SPEED_MAX, kk_kurs.RK3));
         rachet_exp = TRUE;
      }

      if (!perechet) // Первый вызов - пересчёт командного курса в непрерывную шкалу с учётом направления циркуляции
      {
         float k_t_r = KURS_TEC;
         while (k_t_r >= 360.0f) k_t_r -= 360.0f;
         while (k_t_r < 0.0f) k_t_r += 360.0f;

         if (ZIRC)
         {
            if (k_t_r < KURS_CMD)
               KURS_CMD = KURS_TEC + (KURS_CMD - k_t_r);
            else
               KURS_CMD = KURS_TEC + 360.0f - (k_t_r - KURS_CMD);
         }
         else
         {
            if (k_t_r < KURS_CMD)
               KURS_CMD = KURS_TEC - 360.0f + (KURS_CMD - k_t_r);
            else
               KURS_CMD = KURS_TEC - (k_t_r - KURS_CMD);
         }
         perechet = TRUE;

         // Корректировка зафиксированной скорости с учётом типа управления
         if (avto_exit_speed)
         {
            if (A_ZACLAD_s >= 0.0f)
               SPEED_K_FIX = 0.97f * SPEED_K_FIX * (kk_kurs.K_S_1_KURS[SU_KURS - 1] * pow(kk_kurs.K_S_2_KURS[SU_KURS - 1] * A_ZACLAD_s, kk_kurs.K_S_3_KURS[SU_KURS - 1]) + 1.0f);
            else
               SPEED_K_FIX = 0.0f;

            time_speed_exit = 0.0f;
            flag_1 = FALSE;
            avto_exit_speed = FALSE;
            speed_kurs_exit = FALSE;
            speed_kurs = TRUE;
            speed_kurs_antre = FALSE;
         }
         A_ZACLAD_s = A_ZACLAD;
      }

      SPEED_KURS_ANTRE(); // Моделируем падение скорости на циркуляции

      // Изменение угловой скорости KURS_UP в зависимости от фазы (вход/выход)
      if (ZIRC)
      {
         if (!avto_exit)
         {
            if (KURS_UP < KURS_UP_LIM)
               KURS_UP = KURS_UP + KURS_UP_LIM * x * D_TIME;
            else
               KURS_UP = KURS_UP - KURS_UP_LIM * x * D_TIME;
         }
         else
         {
            if (KURS_UP > 0.0f)
               KURS_UP = KURS_UP - KURS_UP_LIM * x * D_TIME;
            else
               KURS_UP = 0.0f;
         }
      }
      else
      {
         if (!avto_exit)
         {
            if (KURS_UP > -KURS_UP_LIM)
               KURS_UP = KURS_UP - KURS_UP_LIM * x * D_TIME;
            else
               KURS_UP = KURS_UP + KURS_UP_LIM * x * D_TIME;
         }
         else
         {
            if (KURS_UP < 0.0f)
               KURS_UP = KURS_UP + KURS_UP_LIM * x * D_TIME;
            else
               KURS_UP = 0.0f;
         }
      }

      // Интегрирование курса и проверка условий автовыхода
      if (ZIRC) //bmw правая циркуляция
			//Здесь ниже вычисляется прогнозируемый курс KURS_EXIT с учетом торможения угловой скорости.
			//Если он превышает командный, то включается avto_exit. Затем в фазе avto_exit угловая скорость уменьшается,
			//и когда KURS_TEC >= KURS_CMD, маневр завершается.
      {
         KURS_TEC = KURS_TEC + KURS_UP * D_TIME * m_Me.V;
         if (!avto_exit)
         {
            KURS_EXIT = KURS_TEC;
            KURS_UP_EXIT = KURS_UP;
            ASSERT(KURS_UP_LIM > 0.0f);
            while (KURS_UP_EXIT > 0.0f)
            {
               KURS_UP_EXIT = KURS_UP_EXIT - KURS_UP_LIM * x * D_TIME;
               KURS_EXIT = KURS_EXIT + KURS_UP_EXIT * D_TIME * m_Me.V;
            }
            if (KURS_EXIT >= KURS_CMD)
            {
               avto_exit = TRUE;
               avto_exit_speed = TRUE;
               time_speed_kurs = 0.0f;
               time_speed_exit = 0.0f;
               speed_kurs = FALSE;
            }
            else
               avto_exit = FALSE;
         }
         else
         {
            if (KURS_TEC >= KURS_CMD)
            {
               KURS_TEC = KURS_CMD;
               A_ZACLAD_TEC = 0.0f;
               A_ZACLAD = 0.0f;
               KURS_UP = 0.0f;
               KURS_UP_LIM = 0.0f;
               avto_exit = FALSE;
               perechet = FALSE;
               rachet_exp = FALSE;
               speed_kurs = FALSE;
               float k_t_r = KURS_TEC;
               while (k_t_r >= 360.0f) k_t_r -= 360.0f;
               while (k_t_r < 0.0f) k_t_r += 360.0f;
               KURS_TEC = k_t_r;
               KURS_CMD = KURS_TEC; // Синхронизация курсов после правой циркуляции
            }
         }
      }
      else // ZIRC == 0 (левая циркуляция)
      {
         KURS_TEC = KURS_TEC + KURS_UP * D_TIME * m_Me.V;
         if (!avto_exit)
         {
            KURS_EXIT = KURS_TEC;
            KURS_UP_EXIT = KURS_UP;
            while (KURS_UP_EXIT < 0.0f)
            {
               KURS_UP_EXIT = KURS_UP_EXIT + KURS_UP_LIM * x * D_TIME;
               KURS_EXIT = KURS_EXIT + KURS_UP_EXIT * D_TIME * m_Me.V;
            }
            if (KURS_EXIT <= KURS_CMD)
            {
               avto_exit = TRUE;
               time_speed_kurs = 0.0f;
               time_speed_exit = 0.0f;
               speed_kurs = FALSE;
               avto_exit_speed = TRUE;
            }
            else
               avto_exit = FALSE;
         }
         if (KURS_TEC <= KURS_CMD)
         {
            KURS_TEC = KURS_CMD;
            A_ZACLAD_TEC = 0.0f;
            A_ZACLAD = 0.0f;
            KURS_UP = 0.0f;
            KURS_UP_LIM = 0.0f;
            avto_exit = FALSE;
            perechet = FALSE;
            rachet_exp = FALSE;
            speed_kurs = FALSE;
            float k_t_r = KURS_TEC;
            while (k_t_r >= 360.0f) k_t_r -= 360.0f;
            while (k_t_r < 0.0f) k_t_r += 360.0f;
            KURS_TEC = k_t_r;
            KURS_CMD = KURS_TEC; // Синхронизация курсов после левой циркуляции
         }
      }
      // Нормализация текущего курса в диапазон [0,360)
      float k_t_r = KURS_TEC;
      while (k_t_r >= 360.0f) k_t_r -= 360.0f;
      while (k_t_r < 0.0f) k_t_r += 360.0f;
      m_Me.K = k_t_r;
   }
}

// Подготовка параметров для выполнения команды скорости
void CMainDoc::Comand_V()
{
   if (avto_exit_speed)
   {
      // Сброс состояния выхода из циркуляции, если активен
      avto_exit_speed = FALSE;
      d_SPEED_KURS = 0.0f;
      time_speed_kurs_cmd = 0.0f;
      time_speed_kurs = 0.0f;
      flag_1 = FALSE;
      delta_speed_r = 0.0f;
      time_speed_exit = 0.0f;
      ostatok = 0.0f;
      return;
   }

   // Если мы в циркуляции и задана скорость больше текущей, ограничиваем её достижимой на циркуляции
   if (m_Me.K != KURS_CMD && SPEED_CMD > m_Me.V)
   {
      if (SPEED_CMD > MeKn * mv[6].MATR_V[7] * 0.9893f)
      {
         int V1 = int(SPEED_CMD / 5.0f);
         int V2 = V1 + 1;
         int X1 = int(A_ZACLAD / 5.0f);
         int X2 = X1 + 1;

         float Y1 = MeKn * mv[V1].MATR_V[X1] + (A_ZACLAD - 5.0f * X1) * (MeKn * mv[V1].MATR_V[X2] - MeKn * mv[V1].MATR_V[X1]) / (5.0f * (X2 - X1));
         float Y2 = MeKn * mv[V2].MATR_V[X1] + (A_ZACLAD - 5.0f * X1) * (MeKn * mv[V2].MATR_V[X2] - MeKn * mv[V2].MATR_V[X1]) / (5.0f * (X2 - X1));
         float v_cmd = (Y1 + (Y2 - Y1) * (SPEED_CMD - 5.0f * V1) / (5.0f * (V2 - V1))) * 0.9893f;
         SPEED_CMD = v_cmd; // Ограничиваем командную скорость
      }
      time_speed_kurs = time_speed_kurs_cmd = 0.0f;
      ostatok = 0.0f;
      delta_speed_r = 0.0f;
   }

   float v1 = m_Me.V - SPEED_CMD;
   float SC_S = -v1;
   if (SC_S <= 0.0f) SC_S = 0.01f;

   // Расчёт начального ускорения usk0 по эмпирической формуле с использованием kk_speed
   float x = (SC_S - 5.0f) / 5.0f;
   float x2 = (2.0f * (m_Me.V - 5.0f) / 5.0f - 1.0f);
   if (x2 < 0.0f) x2 = 0.0f;
   if (x < 0.0f) x = 0.0f;

   float kkn1 = 0.0f;
   float kkn2 = 0.0f;
   float kkn4 = kk_speed.sk4;

   int n_v = int(m_Me.V / 5.0f);
   // Интерполяция коэффициентов sk1 и sk2 по текущей скорости
   kkn1 = kk_speed.sk1[n_v] + (kk_speed.sk1[n_v + 1] - kk_speed.sk1[n_v]) * (m_Me.V - n_v * 5.0f) / 5.0f;
   kkn2 = kk_speed.sk2[n_v] + (kk_speed.sk2[n_v + 1] - kk_speed.sk2[n_v]) * (m_Me.V - n_v * 5.0f) / 5.0f;

   float pp1 = pow(x, kk_speed.sk5);
   float pp2 = pow(SC_S, kkn4);
   usk0 = (kkn1 - (kkn2) * pp1) * pp2;

   ASSERT(!_isnan(usk0));

   dV0 = SC_S;
   dV0MAX = SPEED_MAX - m_Me.V;
   rachet_pow = TRUE;
}

// Функция для расчёта торможения (используется в OnSPEED)
float SPEED_TORM(float s, float sk1, float sk2, float sk3, float sk4, float t)
{
   float dv = (sk1 * s * s * s + sk2 * s * s + sk3 * s + sk4);
   if (dv < 0.01f) dv = 0.01f;
   return (s - dv * t);
}

// Моделирование изменения скорости (разгон/торможение)
void CMainDoc::OnSPEED()
{
   if (SPEED_CMD < 1.0f) SPEED_CMD = 1.0f;
   if (m_Me.V != SPEED_CMD)
   {
      if (T_S > 10.0f) // Если прошло более 10 секунд с последней смены курса, можно корректировать команду курса
      {
         if (m_Me.K != KURS_CMD) Comand_K();
         T_S = 0.0f;
      }
      else
      {
         T_S += D_TIME;
      }

      SPEED_CMD_OLD = -1.0f;
      if (SPEED_CMD > SPEED_CMD_MAX) SPEED_CMD = SPEED_CMD_MAX;

      float v1 = m_Me.V - SPEED_CMD;
      float SC_S = -v1;
      if (SC_S == 0.0f) SC_S = 0.01f;

      rachet_exp = FALSE;

      if (m_Me.V > SPEED_CMD) // Торможение
      {
         if (SU_SPEED == 2)
            m_Me.V = SPEED_TORM(m_Me.V, kk_speed.sk9, kk_speed.sk10, kk_speed.sk11, kk_speed.sk12, D_TIME);
         else
            m_Me.V = SPEED_TORM(m_Me.V, kk_speed.sk13, kk_speed.sk14, kk_speed.sk15, kk_speed.sk16, D_TIME);
      }
      else // Разгон
      {
         float ar1 = (dV0 + v1) / dV0;
         if (ar1 < 0.0f) ar1 = 0.0f;

         float A_tec = static_cast<float>(
				(-kk_speed.sk3 * pow(static_cast<double>(ar1), static_cast<double>(kk_speed.sk8)) + 1.0f +
				kk_speed.sk6 * pow(static_cast<double>(fabs(dV0MAX / 2.3f - SPEED_CMD) * KnMe), static_cast<double>(kk_speed.sk7))
				) * usk0
         );

         if (A_tec < 0.002f) A_tec = 0.002f;

         m_Me.V += A_tec * MeKn * D_TIME;
         if (m_Me.V > SPEED_MAX) m_Me.V = SPEED_MAX;
      }

      // Если достигли заданной скорости
      if (v1 * (m_Me.V - SPEED_CMD) < 0.0f)
      {
         m_Me.V = SPEED_CMD;
         SPEED_K_FIX = m_Me.V;
         if (m_Me.K != KURS_CMD)
         {
            d_SPEED_KURS = 0.0f;
            delta_speed_r = 0.0f;
            Comand_K();
            T_S = 0.0f;
            d_SPEED_KURS = 0.0f;
            time_speed_kurs_cmd = time_speed_kurs = 0.0f;
            delta_speed_r = 0.0f;
         }
         rachet_pow = FALSE;
      }
   }
   else
   {
      m_Me.V = SPEED_CMD;
   }

   if (m_Me.V > SPEED_MAX) m_Me.V = SPEED_MAX;
   if (m_Me.V < 0.0f) m_Me.V = 0.0f;
}

// Интегрирование координат (перемещение)
void CMainDoc::OnTRAEKT()
{
   CString s;
   float dx = KnMe * m_Me.V * sin(m_Me.K * GrRd) * D_TIME; // Приращение X (в метрах)
   float dy = KnMe * m_Me.V * cos(m_Me.K * GrRd) * D_TIME; // Приращение Y (в метрах)

   m_Me.X += dx / 1000.0f; // Переводим в километры
   m_Me.Y += dy / 1000.0f;
   time_observ += m_TimeStep;     // Накапливаем время наблюдения
   Time_Out_Contact += m_TimeStep; // Таймер потери контакта

   if (Time_Out_Contact > 1800000 && b_END_OPL == 1) // 30 минут нет контакта
   {
      xworld.Docs(4, "Потерян контакт с ПЛ");
   }
}

// Обработка отладочных флагов (пункты меню Debug)
void CMainDoc::OnDebugFlags(UINT nID)
{
   ASSERT(nID >= ID_DEBUG_FIRST && nID <= ID_DEBUG_LAST);
   DWORD bit = 1 << (nID - ID_DEBUG_FIRST);
   g_dwDebugFlags ^= bit; // Переключение бита
}

void CMainDoc::OnUpdateDebugFlags(CCmdUI* pCmdUI)
{
   UINT nID = pCmdUI->m_nID;
   ASSERT(nID >= ID_DEBUG_FIRST && nID <= ID_DEBUG_LAST);
   DWORD bit = 1 << (nID - ID_DEBUG_FIRST);
   pCmdUI->SetCheck(!!(g_dwDebugFlags & bit)); // Установка галочки в меню
}

// Обработка данных о целях (вызывается при получении XREQ_TARGETS)
// Преобразует полученные от сервера данные о трассах целей в другой внутренний формат (TGakForm),
// попутно вычисляя абсолютные пеленги.
void CMainDoc::Step(double /*r*/)
{
   int i;
   CString s;
   //bmw double dt;

	// === ИСПРАВЛЕНИЕ №1: УТЕЧКА ПАМЯТИ ===
   // БЫЛО: TGakForm* pTGF[25];  // Массив указателей -> new без delete -> УТЕЧКА
   // СТАЛО: Массив структур на стеке. Память очистится автоматически при return.
   TGakForm pTGF[25];             // Выделено на стеке (около 25 * ~40 байт = 1 Кб)
   
	//bmw dt = 1.0;  // Артефакт рефакторинга (раньше r мог означть шаг времени в миллисекундах)?
   m_Me.dwAcFlags = 5; // Устанавливаем акустические флаги (приёмник и отражатель) [биты 0 и 2]
   double q;
   if (m_nTrassReport > 0)   // Если массив отчётов от ГАК не пуст
   {
      // === ИСПРАВЛЕНИЕ №2: Защита от выхода за границы массива ===
      // БЫЛО: if (m_nTrassReport > 25) m_nTrassReport = 25;
      // СТАЛО: вместо вызова функции min в nCount = min(m_nTrassReport, 25),
      // используется ручная проверка с присваиванием:
      int nCount = m_nTrassReport; // m_nTrassReport остаётся НЕИЗМЕННЫМ, если где-то позже код снова обратится к m_nTrassReport
      if (nCount > 25) 
      {
         nCount = 25;          // Количество обрабатываемых трасс ограничивается 25 (только локальную переменную)
      }
      
		for (i = 0; i < nCount; i++) // Безопасный проход по массиву pTGF[25]
      {
         // === ИСПРАВЛЕНИЕ №3: работаем с адресом элемента массива структур ===
         // БЫЛО: pTGF[i] = new TGakForm; // УТЕЧКА
         // СТАЛО: TGakForm* pItem = &pTGF[i]; // Получаем указатель на готовую память в стеке
         TGakForm* pItem = &pTGF[i]; // Нет выхода за границы стека
      			
         pItem->nNum = m_pTR[i].nNum;                         // Номер цели
         pItem->nObj = 0;
         pItem->bUse = FALSE;
         pItem->nClass = m_pTR[i].nClass;                     // Класс цели
         pItem->nClassOk = (int)(m_pTR[i].ClassProb * 100.0); // Достоверность классификации (в процентах)
         pItem->osp = m_pTR[i].snr;                           // Отношение сигнал/помеха
			
         // Пересчёт относительного пеленга в абсолютный
         q = m_pTR[i].teta + m_Me.K;          // Абсолютный пеленг на цель в градусах (из относительного курса носителя)
         while (q < 0.0) q += 360.0;
         while (q >= 360.0) q -= 360.0;
         pItem->p = GrRd * q;                  // Перевод абс. пеленга в радианы
         pItem->dp = 0.007;                    // Среднеквадратическая ошибка пеленга
         pItem->d = -1.0;                      // Дистанция
         pItem->dd = -1.0;                     // и её производная (не определены)
         pItem->time = m_pTR[i].time / 1000.0; // Время обнаружения (в секундах)
         pItem->vip = m_pTR[i].vip;            // Величины изменения: пеленга
         pItem->vis = m_pTR[i].vis;            // и скорости (?)
      }
   }
}

// Логика поиска (вызывается из Move при type_moving == 1)
void CMainDoc::_1_Searching()
{
   if (m_nTrassReport > 0)
   {
      float teta_SR = 0.0f;
      for (int i = 0; i < m_nTrassReport; i++)
      {
         if (m_pTR[i].nNum == 2) // Обнаружена ПЛ
         {
            xworld.Docs(3, "Обнаружена ПЛ D =%f", m_pTR[i].dist);
         }
         if (m_pTR[i].nNum > 5) // Обнаружена помеха (ВП)
         {
            b_Class_VP = 1;
            teta_SR += m_pTR[i].teta;
         }
      }

      if (b_Class_VP) // Если есть помеха, переходим в наведение
      {
         xworld.Docs(5, "Пеленг обнаруженной ВП %f", teta_SR);

         float newKurs = m_Me.K + teta_SR / m_nTrassReport; // Усреднённый пеленг на помехи
         while (newKurs >= 360.0f) newKurs -= 360.0f;
         while (newKurs < 0.0f)    newKurs += 360.0f;

         if (fabs(newKurs - KURS_CMD) > ANGLE_EPSILON || !avto_exit)
         {
            KURS_CMD = newKurs;
            if (teta_SR < 0.0f) ZIRC = 0; else ZIRC = 1;

            A_ZACLAD = 25.0f;
            SU_KURS = 3;
            avto_exit = perechet = FALSE;
            dv = SPEED_CMD - m_Me.V;
            Comand_K();
         }

         type_moving = 3; // Переключаемся в режим наведения

         {
            SPEED_CMD = m_form.SPEED_PR_CMD; // Скорость прорыва
            SU_SPEED = m_form.SU_SPEED;
            rachet_pow = FALSE;
            Comand_V();
         }
         return;
      }
   }
}

// Логика наведения (type_moving == 3)
void CMainDoc::_3_Pointing()
{
   BOOL m_b_Class_VP = 0;
   if (m_nTrassReport > 0)
   {
      float teta_SR = 0.0f;
      for (int i = 0; i < m_nTrassReport; i++)
      {
         if (m_pTR[i].nNum > 5)
         {
            b_Class_VP = 1;
            teta_SR += m_pTR[i].teta;
         }
      }

      if (b_Class_VP)
      {
         xworld.Docs(6, "Пеленг ВП %f", teta_SR);
         if (Time_Reset > 180000) // Каждые 3 минуты можно корректировать курс
         {
            Time_Reset = 0;
            if (fabs(KURS_CMD - m_Me.K) <= ANGLE_EPSILON && teta_SR < 110.0f)
            {
               float newKurs = m_Me.K + teta_SR / m_nTrassReport;
               while (newKurs >= 360.0f) newKurs -= 360.0f;
               while (newKurs < 0.0f)    newKurs += 360.0f;

               if (fabs(newKurs - KURS_CMD) > ANGLE_EPSILON || !avto_exit)
               {
                  KURS_CMD = newKurs;
                  if (teta_SR < 0.0f) ZIRC = 0; else ZIRC = 1;

                  A_ZACLAD = 25.0f;
                  SU_KURS = 3;
                  avto_exit = perechet = FALSE;
                  dv = SPEED_CMD - m_Me.V;
                  Comand_K();
               }
            }
         }
         else
         {
            Time_Reset += m_TimeStep;
         }
         return;
      }
   }
   if (m_b_Class_VP == 0) // Помеха потеряна
   {
      b_Class_VP = 0;
      type_moving = 1; // Возврат в поиск
      SPEED_CMD = SPEED_SEARCH;
      SU_SPEED = 1;
      rachet_pow = FALSE;
      Comand_V();
      xworld.Docs(7, "Потерян контакт с ВП");
      Time_Reset = 0;
   }
}

void CMainDoc::DeleteContents()
{
   delete m_pTR;
   m_pTR = NULL;
   CDocument::DeleteContents();
}

// Заглушки для неиспользуемых функций
void CMainDoc::_1_Searching_PR() {}
void CMainDoc::_3_Pointing_PR() {}