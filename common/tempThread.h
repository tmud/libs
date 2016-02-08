#pragma once

template<bool indll>
class TempThread
{
    volatile int m_stop;
    HANDLE  m_thread;
protected:
    virtual void threadProc() = 0;
    virtual void threadStop() {}
    bool needStop() const {  return (m_stop == 0) ? false : true; }

public:
    TempThread() : m_thread(NULL), m_stop(0) {}
    virtual ~TempThread() { clear(); }

   bool isFinished()
   {
       if (!m_thread)
           return true;
       DWORD code = 0;
       bool result = (GetExitCodeThread(m_thread, &code) && code == STILL_ACTIVE) ? false : true;
       if (result)
           clear();
       return result;
   }

   void terminate()
   {
       TerminateThread(m_thread, 0);
       clear();
   }

   void stop()
   {
       m_stop = 1;
       threadStop();
   }

   void wait()
   {
       if(!m_thread)
       {
           clear();
           return;
       }
       if (!indll)
       {
           WaitForSingleObject(m_thread, INFINITE);   // doesn't working in dll ((
       }
       else
       {
           while (!isFinished())                      // bad, but working in dll
               Sleep(100);
       }
       clear();
   }

   bool run()
   {
       if (!isFinished())
           return false;
        m_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)TempThread::threadProc, 
          (void*)this, 0, NULL);
       return true;
   }

private:
    void clear()
    {
        if (m_thread)
            CloseHandle(m_thread);
        m_thread = NULL;
        m_stop = 0;
    }

    static DWORD threadProc(void* lpParameter)
    {
        if (lpParameter) 
        {
           TempThread* _this = (TempThread*)(lpParameter);
           _this->threadProc();
           _this->clear();
        }
        return 0;
    }
};
