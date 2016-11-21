#pragma once

extern "C" {
#define PCRE_STATIC
#include "pcre.h"
}

#ifdef _DEBUG
//#define SHOW_ELAPSED_PCRE_TIME
#endif

class Pcre16
{
public:
    Pcre16();
    ~Pcre16();
    bool setRegExp(const std::wstring& regexp, bool extra = false);
    void find(const std::wstring& string);           // find first match + additional data(params indexes)
    void findAllMatches(const std::wstring& string); // find all matches
    int  getSize() const;
    int  getFirst(int index) const;
    int  getLast(int index) const;
    bool getString(int index, std::wstring* str) const;
    void getRegexp(std::wstring* regexp) const;
    void clear();
    const std::wstring& getRegexp() const { return m_regexp; }
    bool valid() const { return (m_ph) ? true : false; }
private:
    pcre16 *m_ph;
    pcre16_extra *m_pe;
    std::vector<int> m_indexes;
    std::wstring m_str;
    std::wstring m_regexp;
};
