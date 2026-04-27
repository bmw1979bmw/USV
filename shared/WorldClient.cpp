#include "stdafx.h"

#include "structures.h"
#include "WorldClient.h"
#include "GoodFunc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#ifndef NO_DEVELOPER_CHECK
CDeveloper g_Developer;
#endif

LPCTSTR StandGetDefDir_() { return g_StandDefDir.GetDefDir(); }

CWorldClient::CWorldClient() :
   m_nCarrierID((DWORD)-1),
   a_xcmd(0),
   a_xcmd_last(0),
   a_arc(NULL),
   m_pTarg(NULL),
   m_bInitClientOK(0),
   m_dwMyXReq(0),
   m_nScenNumber(0),
   m_nScenBeginNum(0),
   m_nScenEndNum(0),
   m_bWinterFlag(0),
   m_nBettaType(0),
   m_nSerializeID(0),
   m_FresnelZone(0)
{
   *m_szRegion = 0;
   g_Developer;
}

CWorldClient::~CWorldClient()
{
   Destroy();
}

void CWorldClient::Destroy()
{
   CFMExchange::Destroy();
   m_bInitClientOK = 0;
   m_Images.DeleteImageList();
}

BOOL CWorldClient::Create 
   (
      CCmdTarget *pTarg,     // чей OnCmdMsg
      UINT nSerializeID,     // параметр OnCmdMsg
      DWORD dwReq,           // запрашиваемые данные XREQ_xxx
      DWORD nCarrierID,      // ид своего носителя, 
                             // if _bCarrier -> -1 для создания объекта с новым
      UINT     iAlias,
//       LPCTSTR  szAlias,      // alias - его файлы
//       LPCTSTR  szName,       // имя для отображения
      BOOL     bCarrier,     // 1-плавает 0-система
      UINT     nIdBitmap,    // битмапка для отображения 34x16
      UINT     nIdBitmapSel, // селектир. битмапка -if bCarrier 
      DWORD    dwQuant,      // квант работы
      float    fBumpDist,    // дистанция столкновения, км  
        /* если ноль - команда не приходит 
           иначе приходит XREQ_BUMP с параметрами TBumpStruct
           независимо от знака дистанции столкновения */
      UINT     nLength,     // длина объекта -if bCarrier 
      UINT     nClass,      // класс объекта носителя/ для систем- разрешение запуска дубля
      float    MagMoment,  // магнитный момент -if _bCarrier кА*м^2 - вертикальный
      float    fWidth      // ширина объекта (корма), м  для кильв.следа -if bCarrier 
   )
{
   Destroy(); // удалим, если вызов - второй раз

   m_dwMyXReq = dwReq;

   m_nCarrierID = nCarrierID;
//    sAlias       = szAlias;

//    sAlias.MakeUpper();
//    if ( !CheckLatAlias(sAlias) ) return FALSE;

//    sName        = szName;
   nType        = bCarrier?1:0;
//   dwSomeFlags  = 0;


// 	DWORD d = GetModuleFileName(
//       NULL, sExeName.GetBuffer(_MAX_PATH), _MAX_PATH);
// 	sExeName.ReleaseBuffer();
//    int t = sExeName.ReverseFind('\\');
//    if ( !d || t==-1 ) {
//       sExeName = AfxGetAppName();
//    } else {
//       sExeName = sExeName.Right(sExeName.GetLength()-t-1);
//    }
   this->iAlias     = iAlias;
   this->dwQuant    = dwQuant;
   this->fBumpDist  = fBumpDist;  
   this->nLength    = nLength;
   this->fWidth     = fWidth;
   this->nClass     = nClass;
   this->fMagMoment = MagMoment;
   nIconIdx = -1;
   nIconCnt = 0;

   //m_dwFormId = dwFormId;

   ASSERT( !(dwReq&XREQ_BUMP) || ((dwReq&XREQ_BUMP) && fabs(fBumpDist) < 1));

   m_Images.Create(34, 16, ILC_COLOR|ILC_MASK, 2, 1);

   CBitmap bmp;
   VERIFY( bmp.LoadBitmap(nIdBitmap) );
#ifdef _DEBUG
   { // 16.10.2018 09:31
      BITMAP bm;
      GetObject(bmp.m_hObject, sizeof BITMAP, &bm);
      ASSERT( (bm.bmWidth%34) == 0 ); // размер картинки д.б. кратен 34
      if ( bm.bmWidth > 34 ) ASSERT( !nIdBitmapSel ); // используйте одну длинную картинку или 2 коротких
   }
#endif
   m_Images.Add(&bmp, 0xFF00FF);

   if ( nIdBitmapSel /*bCarrier */) {       // 16.10.2018 09:34
      //ASSERT( nIdBitmapSel != 0 );
      bmp.DeleteObject();
      //if ( nIdBitmapSel ) {
         VERIFY(bmp.LoadBitmap(nIdBitmapSel));
         m_Images.Add(&bmp, 0xFF00FF);
      //}
   //} else {
    //  ASSERT( nIdBitmapSel == 0 );
   }
   
   ASSERT_VALID(pTarg);

   m_pTarg = pTarg;

   m_nSerializeID = nSerializeID;

   ASSERT(dwReq < XREQ_MAX); // для запрашиваемых команд

   const CString &base = g_StandDefDir.GetName();
   int nLen = base.GetLength();
   if ( nLen == 0 ) {
      m_sCause = ("CWorldClient::Create - нет имени папки стенда!");
      return FALSE;
   }
   if ( nLen > MAX_STAND_NAMELEN ) {
      m_sCause = ("CWorldClient::Create - слишком длинное имя папки!");
      return FALSE;
   }
   
   if ( !CFMExchange::CreateClient(base) ) {
      CString s = "Невозможно создать клиента!\n" + m_sCause;
      if ( g_Developer ) {
         AfxMessageBox(s, MB_ICONSTOP|MB_SETFOREGROUND|MB_TOPMOST);
      } else {
         CString name(AfxGetApp()->m_pszHelpFilePath);
         name = name.Left(name.GetLength()-3);
         name += "log";
         CString Time;
         SYSTEMTIME st;
         GetLocalTime(&st);
         Time.Format("%04d.%02d.%02d  %02d:%02d: ", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute);
         Time += s;
         Time += "\r\n";
         CFile file;
         if ( file.Open(name, CFile::modeCreate|CFile::modeWrite|CFile::modeNoTruncate) ) {
            file.SeekToEnd();
            file.Write( (LPCTSTR)Time, Time.GetLength()*sizeof TCHAR);
         }
      }
      return FALSE;
   }
   /////////////////////////////////////////////////////////////
   //   Send серверу начальные данные
   if ( IsDebuggerPresent() ) { // 09.09.2020 09:17
      dwReq |= xCMD_DEBUGGER_PRESENT;
   }

   CArcBuf ab;
   ab << (DWORD)XCMD_NEWCLIENT
      << nCarrierID
      << dwReq;

   CWorldClientData::LoadFormInfo();
   CWorldClientData::Send(ab);

   CFMExchange::SendToServer(ab);

   return TRUE;
}

void CWorldClient::OnClose(HANDLE)
{
   if ( m_app ) m_app->m_pxchg = NULL;

   if ( a_xcmd  ) { // in notify
      ExitApp();
   }

   a_arc = NULL;
   a_xcmd = xCMD_CLOSE;
   try {
      NotifyCmdTarget();   // уведомляем
   } catch(...) {
   }
   a_xcmd_last = a_xcmd;
   a_xcmd = 0;

   PostWmClose();
}

void CWorldClientData::Receive(CArchive &ar)
{
   ASSERT(ar.IsLoading());
   Serialize(ar);
}

void CWorldClientData::Send(CArchive &ar)
{
   ASSERT(ar.IsStoring());
   Serialize(ar);
}

BOOL CWorldClientData::LoadFormInfo(const TFormInfo *pFormInfo, DWORD dwFormLen)
{
   if ( !pFormInfo ) return FALSE;
   
   try {
      const TFormInfo *p = pFormInfo;
      int n; // ndatalen
      for ( n=0; p[n].type > 0 && p[n].type < FITYPE_COMMAND; n++ ) {
         if ( p[n].ncmd ) {
            AfxMessageBox("Ошибка в формуляре объекта: неразвернутые массивы.");
            return FALSE;
         }
      }

      if ( n && (DWORD)n*sizeof(DWORD) != dwFormLen ) {
         AfxMessageBox("Ошибка в длине описания формуляра объекта!");
         return FALSE;
      }

      Frm.Create(n);

      int i;
      for ( i=0; i<n ;i++ ) {
         Frm[i].type = p[i].type;
         Frm[i].ncmd = 0;
         Frm[i].name = p[i].name;
      }
      
      int t;
      for ( t=0; p[i].type ;i++ ) t++;
      Cmd.Create(t);
      for ( i=0; i<t ;i++ ) {
         Cmd[i].type = p[n+i].type;
         Cmd[i].ncmd = p[n+i].ncmd;
         Cmd[i].name = p[n+i].name;
      }
   } catch(...) {
      Frm.Destroy();
      return FALSE;
   }
   
   return TRUE;
}

BOOL CWorldClient::CheckLatAlias(LPCTSTR szAlias)
{               
   if ( *szAlias == '<' ) return FALSE;
   while ( *szAlias ) {
      if ( *(BYTE *)szAlias > 127 ) return FALSE;
      szAlias++;
   }
   return TRUE; 
}

void CWorldClient::OnReceive()
{
   if ( m_InputBuf.IsEmpty() ) return;

   CArcBuf ab(m_InputBuf.GetData(), m_InputBuf.GetSize());

   ab >> a_xcmd;
   a_arc = &ab;

//    m_pSituation = NULL;
//    m_nSituation = 0;

   switch ( a_xcmd ) {
   case XCMD_INITCLIENT:
      ab >> m_sAlias
         >> m_nCarrierID
         >> m_sCarAlias
         >> m_sFileName
         >> m_dwDepth // глубина моря для заданного региона, м
         >> m_bWinterFlag;
      ab.Read(m_szRegion, 64); // имя региона
      ab >> m_FresnelZone
         >> m_nBettaType
         >> m_nScenNumber
         >> m_ScenTitle0    // WSS_ +  m_ScenTitle0
         >> m_sSavePath     // (отличный от %stand%)
         >> m_nScenBeginNum // номер первого сценария       
         >> m_nScenEndNum   // последний сценарий
         >> m_nUseScenList; // использование списка экспериментов
      m_bInitClientOK = 1;
      
      // был запущен минимизированно - отключить отрисовку
      if ( AfxGetMainWnd()->IsIconic()/*m_bRunMin*/ ) {
         g_dwDebugFlags.Set(DEBUGFLAG_NOUPDATE);
      }
     // nobreak!
   default:
      NotifyCmdTarget();
      break;
   }

   a_arc = NULL;
   a_xcmd_last = a_xcmd;
   a_xcmd = 0;

}

UINT CWorldClient::GetSituation(const TSituation **ts)
{  // валидно только при поступлении XREQ_SITUATION
   if ( m_InputBuf.GetSize() < 8 ) return 0;
   DWORD *pData = (DWORD *)m_InputBuf.GetData();
   if ( *pData != XREQ_SITUATION ) return 0;
   UINT *p = (UINT*)(pData+1);
   if ( *p ) {
      if ( ts ) *ts = (TSituation*)(p+1); // структуры объектов ситуации
   }
   return *p;
}

const TBumpStruct *CWorldClient::GetBumpStruct()
{  // валидно только при поступлении XREQ_BUMP
   if ( m_InputBuf.GetSize() < sizeof(TBumpStruct) ) return NULL;
   const TBumpStruct *pbs = (const TBumpStruct *)m_InputBuf.GetData();
   if ( pbs->XBUMP != XREQ_BUMP ) return NULL;
   return pbs;
}

DWORD *CWorldClient::GetXCmdData()
{  // валидно только при поступлении XCMD_CMD
   // XCMD_CMD, cmd, len
   if ( m_InputBuf.GetSize() < 12 ) return NULL;
   DWORD *pData = (DWORD *)m_InputBuf.GetData();
   if ( *pData != XCMD_CMD ) return NULL;
   return pData + 1;
}

void CWorldClient::PostWmClose()
{
   HWND hWnd = AfxGetMainWnd()->GetSafeHwnd(); // м.б. уже закрылось
   if ( hWnd ) {
      HWND hWnd2 = ::GetLastActivePopup(hWnd);
      if ( hWnd2 != hWnd && hWnd2 ) {
         EndDialog(hWnd2, -1);
         ::PostMessage(hWnd2, WM_CLOSE, 0, 0);
//#pragma chMSG("отладить. SendMsg?")
      } 
      ::PostMessage(hWnd, WM_CLOSE, 0, 0);
   }
}

void CWorldClient::ErrorStream()
{
   if ( AfxMessageBox(
      MB_SYSTEMMODAL|MB_ICONSTOP|MB_ABORTRETRYIGNORE|MB_TOPMOST,
      "Нарушен поток данных!\nПоследняя команда: %X", a_xcmd_last ) == IDRETRY )
   {
      _asm int 3;
   }
   PostWmClose();
}

class CExitException : public CException
{
	DECLARE_DYNAMIC(CExitException)
public:
};

IMPLEMENT_DYNAMIC(CExitException, CException);

void CWorldClient::ExitApp()
{
   THROW(new CExitException());
}

void CWorldClient::ExitAppSmart()
{
   if ( a_xcmd  ) { // in notify
      ExitApp();
   }    
   PostWmClose();
}

void CWorldClient::NotifyCmdTarget()
{  
   ASSERT_VALID(m_pTarg);
   TRY {
      m_pTarg->OnCmdMsg(m_nSerializeID, 0, NULL, NULL);
   } CATCH ( CExitException, e ) {
      PostWmClose();
   } AND_CATCH ( CFileException, e ) {
      TRACE("CFileException fired\n");
      PostWmClose();
   } AND_CATCH ( CArchiveException, e ) {
      TRACE("CArchiveException fired\n");
      PostWmClose();
   } END_CATCH;
}


BOOL CWorldClient::SendToServer(DWORD xcmd, DWORD param)
{ 
//    CArcBuf ab;
//    ab << xcmd
//       << param;
   // для ускорения, без CArcBuf
   DWORD buf[2] = {
      xcmd,
      param
   };
   return SendToServer(buf, 2*sizeof(DWORD));
//    return SendToServer(ab);
}

BOOL CWorldClient::SendToServer2(DWORD xcmd, DWORD param1, DWORD param2)
{ 
//    CArcBuf ab;
//    ab << xcmd
//       << param1
//       << param2;
   // для ускорения, без CArcBuf
   DWORD buf[3] = {
      xcmd,
      param1,
      param2
   };
   return SendToServer(buf, 3*sizeof(DWORD));
//    return SendToServer(ab);
}

// void CWorldClient::SendResponse(DWORD rcmd)
// {
//    SendToServer(XCMD_RESPONSE, rcmd);
// }

void CWorldClient::SetBufferEmpty(DWORD nID, int nBufNum)
{
//    CArcBuf ab;
//    ab << (DWORD)XCMD_BUFFER
//       << nID
//       << nBufNum
//       << XBUF_SETEX
//       << 0ul
//       << 0ul;
   // для ускорения, без CArcBuf
   DWORD buf[6] = {
      XCMD_BUFFER,
          nID,
         nBufNum,
         XBUF_SETEX,
         0,
         0
   };
   SendToServer(buf, 6*sizeof(DWORD));
//    SendToServer(ab);
}

void CWorldClient::SetBuffer(DWORD nID, int nBufNum, void *pdata, DWORD nlen, DWORD nCount)
{
   CArcBuf ab;
   ab << (DWORD)XCMD_BUFFER
      << nID
      << nBufNum
      << XBUF_SETEX
      << nCount
      << nlen;
   if ( nlen ) ab.Write(pdata, nlen);
   SendToServer(ab);
}

void CWorldClient::AppendBuffer(DWORD nID, int nBufNum, void *pdata, DWORD nlen, DWORD nCount)
{
   ASSERT(nlen);
   CArcBuf ab;
   ab << (DWORD)XCMD_BUFFER
      << nID
      << nBufNum
      << XBUF_APPENDEX
      << nCount
      << nlen;
   ab.Write(pdata, nlen);
   SendToServer(ab);
}

void CWorldClient::SendCommand(UINT iAlias, DWORD nID, DWORD cmd, void *pdata, DWORD nlen)
{
   CArcBuf ab;
   ab << (DWORD)XCMD_SENDCOMMAND
      << iAlias
      << nID
      << cmd
      << nlen;
   if ( nlen ) {
      ab.Write(pdata, nlen);
   }
   SendToServer(ab);
}

void CWorldClient::SendCommand(UINT iAlias, DWORD nID, DWORD cmd, CArcBuf &ab)
{
   CArcBuf ar;
   ar << (DWORD)(XCMD_SENDCOMMAND);
   DWORD nlen = ab.GetLen();
   ar << iAlias   // кому: имя
      << nID                // кому: ид
      << cmd                // команда
      << nlen;              // параметры
   if ( nlen ) {
      ar.Write(ab.GetBuf(), nlen);
   }
   SendToServer(ar);
}

// void CWorldClient::RunProgram(UINT iAlias,             // номер алиаса, кого запускать
//                               DWORD dwFormLen /*= 0*/,     // длина формуляра 
//                               void *pForm /*= NULL*/,      // формуляр
//                               DWORD nID /*= DWORD(-1)*/,   // опциональный ID
//                               LPCTSTR szArguments /*= NULL*/) // опциональная строка запуска
// {
//    CArcBuf ab;
//    ab << (DWORD)xCMD_RUN;
//    
//    CString sArguments(szArguments);
// 
//    ab << iAlias
//       << nID
//       << sArguments
//       << dwFormLen;
//    if ( dwFormLen ) {
//       ab.Write(pForm, dwFormLen);
//    }
//    SendToServer(ab);
// }

void CWorldClient::RunProgram(UINT iAlias,             // номер алиаса, кого запускать
                              DWORD dwFormLen /*= 0*/,     // длина формуляра 
                              void *pForm /*= NULL*/,      // формуляр
                              DWORD nID /*= DWORD(-1)*/,   // опциональный ID
                              LPCTSTR szArguments /*= NULL*/, // опциональная строка запуска
                              DWORD dwDebugFlags /*= 0*/ )         // отладка
{
   CArcBuf ab;
   ab << (DWORD)XCMD_CMD;
   ab << (DWORD)XWORLD_CMD_RUN;
   
   // допустимы только флаги XRUN_FLAG_DEBUG, XRUN_ASK_FORMULAR
   ASSERT( (dwDebugFlags & ~(XRUN_FLAG_DEBUG|XRUN_ASK_FORMULAR)) == 0 );

//    DWORD dwFlags = 0;
//    if ( bDebug ) dwFlags = XRUN_FLAG_DEBUG;
//    ab << dwFlags;
   ab << dwDebugFlags;
   
   CString sArguments(szArguments);
   
   ab << iAlias
      << nID
      << sArguments
      << dwFormLen;
   if ( dwFormLen ) {
      ab.Write(pForm, dwFormLen);
   }
   SendToServer(ab);
}


void CWorldClient::RunDecoy(UINT iAlias, // номер алиаса, кого запускать
                            DWORD nID,
                            TXParams *pTX)
{
   CArcBuf ab;
   ab << (DWORD)XCMD_CMD;
   ab << (DWORD)XWORLD_CMD_RUNDECOY;
   ab << iAlias
      << nID;
   ab.Write(pTX, sizeof TXParams);
   SendToServer(ab);
}

void CWorldClient::Docs(DWORD nMsgID, LPCTSTR szFmt, ...)
{
   CString s;
   va_list args;
   va_start(args, szFmt);
   s.FormatV(szFmt, args);
   va_end(args);

   CArcBuf ab;
   ab << m_nCarrierID
      << nMsgID
      << iAlias
      << s;
   SendCommand(IALIAS_DOCS, (DWORD)-1, XDOC_CMD_MSG , ab);
}

void CWorldClient::DocsDontSaveWss()
{
   SendCommand(IALIAS_DOCS, (DWORD)-1, XDOC_CMD_DONTSAVE , NULL, 0);
}

// void CWorldClient::EndExperiment(BOOL bDontSave)
// {
//    //SendCommand(_T("DOCS"), (DWORD)-1, XDOC_CMD_DONTSAVE , NULL, 0);
// //    CArcBuf ab; 
// //    ab << (DWORD)XCMD_CMD
// //       << XWORLD_CMD_ENDEXPERIMENT
// //       << bDontSave;
//    // для ускорения, без CArcBuf
//    DWORD buf[3] = {
//       XCMD_CMD,
//       XWORLD_CMD_ENDEXPERIMENT,
//       bDontSave
//    };
//    SendToServer(buf, 3*sizeof(DWORD));
// //   SendToServer(ab);
// 
// }

// void CWorldClient::SetQuantSize(DWORD dwQuant)
// {
// //    CArcBuf ab; 
// //    ab << (DWORD)XCMD_CMD
// //       << XWORLD_CMD_SETQUANT
// //       << dwQuant;
//    // для ускорения, без CArcBuf
//    DWORD buf[3] = {
//       XCMD_CMD,
//       XWORLD_CMD_SETQUANT,
//       dwQuant
//    };
//    SendToServer(buf, 3*sizeof(DWORD));
// //    SendToServer(ab);
// }

void CWorldClient::ChangeXReq(DWORD dwClear, DWORD dwSet)// изменить m_dwReq для сокета
{
//  dwClear - биты, что снять (XReq &= ~dwClear)
//  dwSet   - биты, что установить (XReq |= dwSet)
//    CArcBuf ab; 
//    ab << (DWORD)XCMD_CMD
//       << XWORLD_CMD_CHANGEXREQ
//       << dwClear
//       << dwSet;
   // для ускорения, без CArcBuf
   DWORD buf[4] = {
      XCMD_CMD,
      XWORLD_CMD_CHANGEXREQ,
      dwClear,
      dwSet
   };
   SendToServer(buf, 4*sizeof(DWORD));
//    SendToServer(ab);
}

namespace NFormInfo {
   const TFormInfo *pFormInfo = NULL;
   DWORD dwFormLen = 0;
};

#pragma hdrstop
#include "WorldCommon.cpp"

void CWorldClientData::Serialize(CArchive &ar)
{
   if ( ar.IsStoring() ) {
      dwVersion = GetExeTimeDateStamp();
      //if ( Cmd.GetCount() ) dwSomeFlags = 0x100;
   }
   
   TWorldSystem::Serialize(ar);
   
   if ( ar.IsStoring() ) {
      //ar << (DWORD)0; // m_dwFormId; // TODO убрать в новой версии обмена  
      m_Images.Write(&ar);
   } else {
//       DWORD dummy; // TODO убрать в новой версии обмена  
//       ar >> dummy; //  m_dwFormId;
      m_Images.Read(&ar);
      nIconIdx = -1;
      nIconCnt = 0;
//   if ( dwSomeFlags_x & 0x100 ) Cmd.Serialize(ar);
   }
   Cmd.Serialize(ar);
}



