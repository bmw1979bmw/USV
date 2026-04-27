#pragma once 

template<class TYPE>
class CArrayEx : public CArray<TYPE, TYPE&>
{
public:
   const TYPE *AddrOf(int nIndex) const {
      if ( nIndex >= 0 && nIndex < m_nSize ) return &m_pData[nIndex];
      return NULL;
   }
   TYPE *AddrOf(int nIndex) {
      if ( nIndex >= 0 && nIndex < m_nSize ) return &m_pData[nIndex];
      return NULL;
   }

   const TYPE& GetAt(INT_PTR nIndex) const
   { ASSERT(nIndex >= 0 && nIndex < m_nSize); return m_pData[nIndex]; }
   TYPE& GetAt(INT_PTR nIndex) 
   { ASSERT(nIndex >= 0 && nIndex < m_nSize); return m_pData[nIndex]; }
   const TYPE& ElementAt(INT_PTR nIndex) const
   { ASSERT(nIndex >= 0 && nIndex < m_nSize); return m_pData[nIndex]; }
   TYPE& ElementAt(INT_PTR nIndex)
   { ASSERT(nIndex >= 0 && nIndex < m_nSize); return m_pData[nIndex]; }
   const TYPE& operator[](INT_PTR nIndex) const
   { ASSERT(nIndex >= 0 && nIndex < m_nSize); return m_pData[nIndex]; }
   TYPE& operator[](INT_PTR nIndex)
   { ASSERT(nIndex >= 0 && nIndex < m_nSize); return m_pData[nIndex]; }

};
