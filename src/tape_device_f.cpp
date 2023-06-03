//
// Created by asgrim on 03.06.23.
//
#include <thread>
#include <iostream>
#include "tape_device_f.h"

constexpr int64_t elementSize = sizeof(int32_t);

TapeDeviceF::TapeDeviceF(const std::string &path,
                         uint32_t readLatency,
                         uint32_t writeLatency,
                         uint32_t rewindLatency,
                         uint64_t length) noexcept :
                         m_readLatency(readLatency), m_writeLatency(writeLatency), m_rewindLatency(rewindLatency), m_length(length)

  {
    m_tape = std::fopen(path.data(),"r+b");
    if(m_tape == NULL)
    {
        fclose(fopen(path.data(),"w"));
        m_tape = std::fopen(path.data(),"r+b");
    }
    std::fseek(m_tape,0,SEEK_SET);

  }

TapeDeviceF::~TapeDeviceF()
{
    std::fclose(m_tape);
}
//todo обавить слип потока на задержках
bool TapeDeviceF::rewindLeft(int64_t offset) noexcept
{
    int64_t tmpOff = m_curPos - offset * elementSize;
    if (!std::fseek(m_tape, tmpOff, SEEK_CUR))
    {
        m_curPos = tmpOff;
        return true;
    }
    return false;
}

bool TapeDeviceF::goNext() noexcept
{
     int64_t tmpOff = m_curPos + elementSize;
        if(!std::fseek(m_tape, tmpOff, SEEK_CUR))
        {
            m_curPos = tmpOff;
            return true;
        }
    return false;
}

bool TapeDeviceF::rewindRight(int64_t offset) noexcept
{
    int64_t tmpOff = m_curPos + offset * elementSize;
    if (!std::fseek(m_tape, tmpOff, SEEK_CUR))
    {
        m_curPos = tmpOff;
        return true;
    }
    return false;
}

void TapeDeviceF::rewindToStart() noexcept
{
    std::rewind(m_tape);
}

void TapeDeviceF::rewindToEnd() noexcept
{
    fseek(m_tape,0,SEEK_END);
    m_curPos = ftell(m_tape);
    std::cout << m_curPos;
}

bool TapeDeviceF::goPrev() noexcept
{
    int64_t tmpOff = m_curPos - elementSize;
    if(!std::fseek(m_tape, tmpOff, SEEK_CUR))
    {
        m_curPos = tmpOff;
        return true;
    }
    return false;
}

bool TapeDeviceF::write(int32_t elem) noexcept
{
    if(std::fwrite(&elem, elementSize, 1, m_tape) == 1)
    {
        std::fseek(m_tape,m_curPos, SEEK_SET);
        return true;
    }
    return false;
}

bool TapeDeviceF::read(int32_t &elem) noexcept
{
    if(std::fread(&elem, elementSize, 1, m_tape) == 1)
    {
        std::fseek(m_tape,m_curPos, SEEK_SET);
        return true;
    }
    return false;
}

uint64_t TapeDeviceF::getLen() noexcept
{
    return m_length;
}

void TapeDeviceF::setRealLatency(bool realLatency) noexcept
{
    m_realLatency = realLatency;
}
bool TapeDeviceF::atEnd() noexcept
{
    return feof(m_tape);
}

bool TapeDeviceF::atStart() noexcept
{
    return m_curPos == 0;
}