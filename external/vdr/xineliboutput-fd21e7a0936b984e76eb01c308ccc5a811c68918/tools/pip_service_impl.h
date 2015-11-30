/*
 * pip_service_impl.h:
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: pip_service_impl.h,v 1.1 2010-03-12 21:15:04 phintuka Exp $
 *
 */

#ifndef PIP_SERVICE_IMPL_H
#define PIP_SERVICE_IMPL_H

#include "../include/vdr-xineliboutput/service_pip.h"

#ifndef TS_SIZE
#  define TS_SIZE 188
#endif

#define PIP_TS_CHUNK_SIZE (8*TS_SIZE)

class cPipServiceImpl : public cXineliboutputPipService
{
  private:

    cXinelibDevice *m_Dev;
    int             m_Id;

    uint8_t         m_Data[PIP_TS_CHUNK_SIZE + TS_SIZE];
    uint            m_DataLen;

  public:

    cPipServiceImpl(int Index, cXinelibDevice *Device)
    {
      m_Dev     = Device;
      m_Id      = Index;
      m_DataLen = 0;

      int X = 5 + 22 * (Index % 4);
      int Y = 5 + 22 * (Index / 4);
      int W = 20;
      int H = 20;

      m_Dev->Pip_Config(m_Id, X, Y, W, H);
    }

  private:

    virtual void SetPosition(uint X, uint Y, uint W, uint H)
    {
      m_Dev->Pip_Config(m_Id, X, Y, W, H);
    }

    virtual int PlayTs(const uint8_t *Data)
    {
      if (Data) {
        /* accumulate data */
        memcpy(m_Data + m_DataLen, Data, TS_SIZE);
        m_DataLen += TS_SIZE;

        if (m_DataLen < PIP_TS_CHUNK_SIZE)
          return TS_SIZE;
      }

      /* flush to device */
      m_Dev->Pip_Play(m_Id, m_Data, m_DataLen);
      m_DataLen = 0;

      return TS_SIZE;
    }

    virtual ~cPipServiceImpl()
    {
      m_Dev->Pip_Close(m_Id);
    }
};


#endif /* PIP_SERVICE_IMPL_H */
