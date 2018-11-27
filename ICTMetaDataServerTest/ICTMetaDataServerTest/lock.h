#ifndef __AUTO_LOCK_H__
#define __AUTO_LOCK_H__

class CLock
{
public:
    CLock()
    {
        InitializeCriticalSection(&m_CriticalSection);
    }
    ~CLock()
    {
        DeleteCriticalSection(&m_CriticalSection);
    }
    void Acquire()
    {
        EnterCriticalSection(&m_CriticalSection);
    }
    void Release()
    {
        LeaveCriticalSection(&m_CriticalSection);
    }
private:
    CLock(const CLock&);    // disabling copy constructor
    CRITICAL_SECTION m_CriticalSection;
};
 

// Using RAII to make unlocking cleaner and safer
class CAutoLock
{
public:
    CAutoLock(CLock& lock)
        : m_Lock(lock)
    {
        m_Lock.Acquire();
    }
    ~CAutoLock()
    {
        m_Lock.Release();
    }
private:
    CLock &m_Lock;
};


#endif