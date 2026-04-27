#ifndef __FORMINFO_H__
#define __FORMINFO_H__

//   в начале MainDoc.cpp пишем:
//   FORMINFO(TFormularPribor);
//

/* ПРИМЕР:
struct TFormularPribor{
   int i;
   float f;          // никаких DOUBLE! только те,что FITYPE_xxx
   BOOL b;
   float arr[3];
   //****************************************
   FI_BEGIN 
      FI_VAR(int   ,"целое i"                ,0)
      FI_VAR(float ,"флоатное f"             ,0)
      FI_VAR(BOOL  ,"булево b"               ,0)
      FI_VAR(float ,"массив дистанций arr[3]",3)
      // дальше будет список команд:
   FI_COMMAND("управление", 'UPR')
      FI_VAR(int   ,"параметр команды управление", 0)
   FI_END;
};
*/

struct TFormInfo {
   UINT    type;  
   LPCTSTR name;
   UINT    ncmd;// Теперь команда. Длина убирается повторителем.
};


#define FITYPE_float 1
#define FITYPE_int   2
#define FITYPE_long  2
#define FITYPE_LONG  2
#define FITYPE_BOOL  3
#define FITYPE_DWORD 4
#define FITYPE_REGIONS 5
#define FITYPE_COMMAND 10


#define FI_BEGIN  static const TFormInfo * __cdecl GetFormInfo() { \
                  static const TFormInfo tfi[]={

#define FI_COMMAND(name,cmdid) {FITYPE_COMMAND,name,cmdid},

#define FI_COMMAND_CASE(name,cmdid) (cmdid)

#define FI_END {0,0,0}}; return tfi; };

#define FI_VAL(type,desc)      {FITYPE_##type,desc,0},
#define FI_IDX(type,desc,i)    {FITYPE_##type,desc "[" #i "]",0},

#define FI_VAL0(type,desc)    FI_VAL(type,desc)
#define FI_VAL1(type,desc)    FI_VAL(type,desc)
#define FI_VAL2(type,desc)    FI_IDX(type,desc,0)FI_IDX(type,desc,1)
#define FI_VAL3(type,desc)    FI_VAL2(type,desc)FI_IDX(type,desc,2)
#define FI_VAL4(type,desc)    FI_VAL3(type,desc)FI_IDX(type,desc,3)
#define FI_VAL5(type,desc)    FI_VAL4(type,desc)FI_IDX(type,desc,4)
#define FI_VAL6(type,desc)    FI_VAL5(type,desc)FI_IDX(type,desc,5)
#define FI_VAL7(type,desc)    FI_VAL6(type,desc)FI_IDX(type,desc,6)
#define FI_VAL8(type,desc)    FI_VAL7(type,desc)FI_IDX(type,desc,7)
#define FI_VAL9(type,desc)    FI_VAL8(type,desc)FI_IDX(type,desc,8)
#define FI_VAL10(type,desc)   FI_VAL9(type,desc)FI_IDX(type,desc,9)
#define FI_VAL11(type,desc)   FI_VAL10(type,desc)FI_IDX(type,desc,10)
#define FI_VAL12(type,desc)   FI_VAL11(type,desc)FI_IDX(type,desc,11)
#define FI_VAL13(type,desc)   FI_VAL12(type,desc)FI_IDX(type,desc,12)
#define FI_VAL14(type,desc)   FI_VAL13(type,desc)FI_IDX(type,desc,13)
#define FI_VAL15(type,desc)   FI_VAL14(type,desc)FI_IDX(type,desc,14)
#define FI_VAL16(type,desc)   FI_VAL15(type,desc)FI_IDX(type,desc,15)

#define FI_VAR(type,desc,len)  FI_VAL##len(type,desc)

namespace NFormInfo {           
   extern const TFormInfo *pFormInfo; 
   extern DWORD dwFormLen;
};

#define FORMINFO(a)             \
namespace NFormInfo {              \
   extern const TFormInfo *pFormInfo;    \
   extern DWORD dwFormLen;    \
   BOOL SetFormInfo(const TFormInfo *p,DWORD len)  \
   {                               \
      pFormInfo = p;               \
      dwFormLen = len;             \
      return TRUE;                 \
   }                               \
   BOOL b=SetFormInfo(a::GetFormInfo(),sizeof(a));       \
};



// TODO : параметры формуляра по умолчанию
#if(0)

class TFIParam
{
   union {
      UINT ncmd;
      float fdef;
   };
public:
   TFIParam(UINT n):ncmd(n){}
   TFIParam(int n):ncmd((UINT)n){}
   TFIParam(float f):fdef(f){}
   TFIParam(double f):fdef((float)f){}
   
};

struct TFormInfo2 {
   UINT    type;  
   LPCTSTR name;
   const TFIParam par;

//   можно просто float !!! преобразовывать к нужному.
};

const TFormInfo2 __test = {6,"aa", 3.};
#define FI_DEF(type,desc,def)  {FITYPE_##type,desc,def},
static const TFormInfo2 __testtfi[]={
   FI_DEF(BOOL,"aa", 3.)
   FI_DEF(int,"bb", 3)
      
};
#endif

#endif