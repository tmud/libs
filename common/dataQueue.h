#pragma once

#include "memorybuffer.h"

class DataQueue
{
public:
    DataQueue() : m_dataSize(0), m_trucatedSize(0) {}
    DataQueue(int datasize) : m_dataSize(0), m_trucatedSize(0) { m_data.alloc(datasize); }
    ~DataQueue() {}

    void truncate(int len)
    {
        int newTruncate = m_trucatedSize + len;
        assert (len >= 0 && newTruncate <= m_dataSize);
        if (newTruncate > m_dataSize)
            newTruncate = m_dataSize;
        m_trucatedSize = newTruncate;
    }

    void write(const MemoryBuffer& mb)
    {
        write(mb.getData(), mb.getSize());
    }

    void write(const DataQueue& dq)
    {
        write(dq.getData(), dq.getSize());
    }

    void write(const void *data, int len)
    {
        assert(data && len >= 0);
        int free_space = m_data.getSize() - m_dataSize;
        if (len > free_space)
        {
            if (m_trucatedSize > 0)
            {
                int not_truncated = m_dataSize - m_trucatedSize;
                if (not_truncated > 0)
                {
                    char *out = (char*)m_data.getData();
                    const char* in = out + m_trucatedSize;
                    memcpy(out, in, not_truncated);
                }
                m_dataSize = not_truncated;
                m_trucatedSize = 0;
                free_space = m_data.getSize() - m_dataSize;
            }

            int new_size = len + m_dataSize;
            if (new_size > free_space)
                m_data.keepalloc(new_size);
        }
        char *out = m_data.getData();
        out = out + m_dataSize;
        memcpy(out, data, len);
        m_dataSize = m_dataSize + len;
    }

    int read(MemoryBuffer* mb)
    {
        int size = min(getSize(), mb->getSize());
        memcpy(mb->getData(), getData(), size);
        return size;
    }

    int read(DataQueue* dq)
    {
        int data_size = getSize();
        dq->write(getData(), data_size);
        return data_size;
    }

    bool read(void *data, int len)
    {
        assert(data && len >= 0);
        if (len > getSize()) return false;
        memcpy(data, getData(), len);
        truncate(len);
        return true;
    }

    void* getData() const
    {
        char *data = m_data.getData();
        data += m_trucatedSize;
        return data;
    }

    int getSize() const 
    {
        return m_dataSize - m_trucatedSize; 
    }

    void clear()
    {
        m_dataSize = 0;
        m_trucatedSize = 0;
    }

    void setBufferSize(int size)
    {
        m_data.keepalloc(size);
    }

    const MemoryBuffer& getMemoryBuffer() 
    {
        if (m_trucatedSize != 0)
        {
            memcpy(m_data.getData(), getData(), m_dataSize);
            m_trucatedSize = 0;
        }
        m_data.keepalloc(m_dataSize);
        return m_data;
    }

private:
    int m_dataSize;
    int m_trucatedSize;
    MemoryBuffer m_data;
};
