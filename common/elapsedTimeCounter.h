#pragma once

class ElapsedTimeCounter
{
    DWORD m_ticker;
public:
    ElapsedTimeCounter() { m_ticker = GetTickCount(); }
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
};
