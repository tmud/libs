#pragma once

// Class for temporary memory buffers with autoclean
class MemoryBuffer
{
public:
    MemoryBuffer() : m_pData(NULL), m_size(0), m_allocated(0) {}
    MemoryBuffer(int size) : m_pData(NULL), m_size(0), m_allocated(0) { alloc(size); }
    ~MemoryBuffer() { delete []m_pData; }
    char* getData() const { return m_pData; }
    int getSize() const { return m_size; }
    void alloc(int size) { allocf(size, false); }
    void keepalloc(int size) { allocf(size, true); }
    void copy(void *data, int size) { allocf(size, false); memcpy(m_pData, data, size); }

private:
    void allocf(int size, bool keepdata)
    {
        if (m_allocated >= size)
            { m_size = size; return; }
        char *new_data = new char[size];
        if (m_size != 0 && keepdata)
            memcpy(new_data, m_pData, m_size);
        delete []m_pData;
        m_pData = new_data;
        m_size = size;
        m_allocated = size;
    }

private:
    char* m_pData;
    int m_size;
    int m_allocated;
};
