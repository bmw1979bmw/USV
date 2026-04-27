#pragma once

class CByteArrayEx : public CByteArray 
{
public:
   BOOL IsEmpty() { return GetSize() == 0; }

   void Copy(const void *data, INT_PTR nDataLength)
   {
      ASSERT_VALID(this);
      SetSize(nDataLength);
      if ( nDataLength ) {
         ASSERT(data);
         memcpy(m_pData, data, nDataLength);
      }
   }

   INT_PTR Append(const void *data, INT_PTR nDataLength)
   {
      ASSERT_VALID(this);
      
      INT_PTR nOldSize = m_nSize;
      SetSize(m_nSize + nDataLength);
      memcpy(m_pData + nOldSize, data, nDataLength);
      
      return nOldSize;
   }

};
