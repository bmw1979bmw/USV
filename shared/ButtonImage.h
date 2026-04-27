#pragma once

//! Класс поддержки картинки на кнопке с текстом
class CButtonImage  
{
   HIMAGELIST m_hIL;  //!< Хранилище картинки
public:
   CButtonImage() : m_hIL(NULL) {}
   virtual ~CButtonImage() { ImageList_Destroy(m_hIL); }

   //! Взять признак загруженности картинки из ресурсов
   BOOL Initialized() { return (BOOL)m_hIL; }

   //! Загрузить указанную картинку из ресурсов и установить ее на кнопку
   void Setup(HWND hWnd, int nDlgID, int nBmpID ) {
      if ( !m_hIL ) {
         HBITMAP hBmp = ::LoadBitmap(
            AfxFindResourceHandle(MAKEINTRESOURCE(nBmpID), RT_BITMAP),
            MAKEINTRESOURCE(nBmpID));
         if ( !hBmp ) return;
         BITMAP Bmp;
         if ( !GetObject(hBmp, sizeof Bmp, &Bmp) ) return;
         // если width == height * 6 - то много изображений 
         int w = /*(Bmp.bmWidth == Bmp.bmHeight*6) ? Bmp.bmHeight : */ Bmp.bmWidth;
         m_hIL = ImageList_Create(w, Bmp.bmHeight, 
            ILC_COLORDDB|ILC_MASK, 0, 0);
         if ( m_hIL ) ImageList_AddMasked(m_hIL, hBmp, 0x00C0C0C0); // серый фон
         DeleteObject(hBmp);
         if ( !m_hIL ) return;
      }
      BUTTON_IMAGELIST BtnIL;
      ZeroMemory(&BtnIL, sizeof BtnIL);
      BtnIL.himl = m_hIL;
      BtnIL.margin.left = 4;
      SendDlgItemMessage(hWnd, nDlgID, BCM_SETIMAGELIST, 0, (LPARAM)&BtnIL);
   }
};

