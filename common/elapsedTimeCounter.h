#pragma once
/*class ElapsedTimeCounter
{
    std::wstring m_label;
    DWORD m_ticker;    
public:
    ElapsedTimeCounter(const wchar_t* label) : m_label(label) { m_ticker = GetTickCount(); }
    double getTime() 
    {
        DWORD diff = -1;
        DWORD tick = GetTickCount();
        if (tick >= m_ticker)
        {
            diff = tick - m_ticker;
        }
        else
        {   // overflow 49.7 days (MSDN GetTickCount)
            diff = diff - m_ticker;
            diff = diff + tick + 1;
        }
        m_ticker = tick;
        double dt = (double)diff / 1000.0f;
        return dt;
    }
    void debugOutput()
    {
        std::wstring t(m_label);
        wchar_t buffer[32];
        swprintf(buffer, L": Elapsed time %.2f\r\n", getTime());
        t.append(buffer);  
        OutputDebugString(t.c_str());
    }
};*/
