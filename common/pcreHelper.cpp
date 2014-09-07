#include "stdafx.h"
#include "pcreHelper.h"
#include "elapsedTimeCounter.h"

#ifdef _UNICODE
#ifdef _DEBUG
#pragma comment(lib, "pcred.lib")
#else
#pragma comment(lib, "pcre.lib")
#endif
#endif

Pcre16::Pcre16(): m_ph(NULL), m_pe(NULL)
{
}

Pcre16::~Pcre16()
{
    clear();
}

bool Pcre16::setRegExp(const tstring& regexp, bool extra)
{
    clear();
    if (regexp.empty())
        return false;
    const unsigned short *reg = (unsigned short*)regexp.c_str();
    const char *error = NULL;
    int error_offset = 0;
    pcre16 *p = pcre16_compile(reg, 0, &error, &error_offset, NULL);
    if (p)
    {
        m_ph = p;
        if (extra)
        {
            const char *error = NULL;
            m_pe = pcre16_study(m_ph, 0, &error);
        }
        m_regexp = regexp;
        return true;
    }    
    return false;
}

void Pcre16::find(const std::wstring& string)
{ 
    if (!m_ph)
        return;

    m_str = string;
    m_indexes.clear();
    int params[33];
    const unsigned short *src = (unsigned short*)string.c_str();
    int src_len = string.length();

#ifdef SHOW_ELAPSED_PCRE_TIME
    ElapsedTimeCounter tc;
#endif
    int count = pcre16_exec(m_ph, m_pe, src, src_len, 0, 0, params, 33);
#ifdef SHOW_ELAPSED_PCRE_TIME
    double dt = tc.getTime();
    char buffer[32];
    sprintf(buffer, "%f :", dt);
    OutputDebugStringA(buffer);
    OutputDebugStringW(m_regexp.c_str());
    OutputDebugStringA("\r\n");
#endif
    for (int i=0; i<count; i++)
    {
        m_indexes.push_back(params[2*i]);
        m_indexes.push_back(params[2*i+1]);
    }
}

void Pcre16::findAllMatches(const std::wstring& string)
{
    if (!m_ph)
        return;
 
    m_str = string;
    m_indexes.clear();
    int params[33];
    const unsigned short *src = (unsigned short*)string.c_str();
    int src_len = string.length();
    int pos = 0;

    while(1)
    {
        int count = pcre16_exec(m_ph, m_pe, src, src_len, pos, 0, params, 33);
        if (count <= 0)
           break;
        m_indexes.push_back(params[0]);
        m_indexes.push_back(params[1]);
        pos = params[1];
    }
}

int Pcre16::getSize() const
{
    return m_indexes.size() / 2; 
}

int Pcre16::getFirst(int index) const
{
    assert(index>=0 && index<getSize());
    return m_indexes[index*2];
}

int Pcre16::getLast(int index) const
{
    assert(index>=0 && index<getSize());
    return m_indexes[index*2+1];
}

void Pcre16::getString(int index, std::wstring* str) const
{
    assert(index>=0 && index<getSize());
    int b = m_indexes[index*2];
    int e = m_indexes[index*2+1];
    str->assign(m_str.substr(b, e-b));    
}

void Pcre16::clear()
{
    m_regexp.clear();
    if (m_pe)
        pcre16_free_study(m_pe);
    m_pe = NULL;
    if (m_ph)
        pcre16_free(m_ph);
    m_ph = NULL;    
}
