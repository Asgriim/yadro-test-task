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
    if (m_curPos == 0)
    {
        return false;
    }
    int64_t tmpOff = offset * elementSize;
    if (!std::fseek(m_tape, -tmpOff, SEEK_CUR))
    {
        m_curPos -= tmpOff;
        if (m_realLatency)
        {
            std::this_thread::sleep_for(std::chrono::microseconds(m_rewindLatency * offset));
        }
        return true;
    }
    goToCurrPos();
    return false;
}

bool TapeDeviceF::goNext() noexcept
{
    if (m_curPos == m_length)
    {
        return false;
    }
    if (!std::fseek(m_tape, elementSize, SEEK_CUR))
    {
        m_curPos += elementSize;
        if (m_realLatency)
        {
            std::this_thread::sleep_for(std::chrono::microseconds(m_rewindLatency));
        }
        return true;
    }
    goToCurrPos();
    return false;
}

bool TapeDeviceF::rewindRight(int64_t offset) noexcept
{
    int64_t tmpOff = offset * elementSize;
    if (!std::fseek(m_tape, tmpOff, SEEK_CUR))
    {
        m_curPos += tmpOff;
        if(m_realLatency)
        {
            std::this_thread::sleep_for(std::chrono::microseconds(m_rewindLatency * offset));
        }
        return true;
    }
    goToCurrPos();
    return false;
}

void TapeDeviceF::rewindToStart() noexcept
{
    std::rewind(m_tape);
    if(m_realLatency)
    {
        std::this_thread::sleep_for(std::chrono::microseconds(m_rewindLatency * m_curPos));
    }
    m_curPos = 0;
}

void TapeDeviceF::rewindToEnd() noexcept
{
    fseek(m_tape,0,SEEK_END);
    m_curPos = ftell(m_tape);
    if (m_realLatency)
    {
        std::this_thread::sleep_for(std::chrono::microseconds(m_rewindLatency * (m_length - m_curPos)));
    }
}

bool TapeDeviceF::goPrev() noexcept
{
    if (m_curPos == 0)
    {
        return false;
    }
    if(!std::fseek(m_tape, -elementSize, SEEK_CUR))
    {
        if (m_realLatency)
        {
            std::this_thread::sleep_for(std::chrono::microseconds(m_rewindLatency));
        }
        m_curPos -= elementSize;
        return true;
    }
    goToCurrPos();
    return false;
}

bool TapeDeviceF::write(int32_t elem) noexcept
{
    if (m_curPos == m_length)
    {
        return false;
    }
    if(std::fwrite(&elem, elementSize, 1, m_tape) == 1)
    {
        std::fseek(m_tape,-elementSize, SEEK_CUR);
        if (m_realLatency)
        {
            std::this_thread::sleep_for(std::chrono::microseconds(m_writeLatency));
        }
        return true;
    }
    goToCurrPos();
    return false;
}

bool TapeDeviceF::read(int32_t &elem) noexcept
{
    if (m_curPos == m_length)
    {
        return false;
    }
    if(std::fread(&elem, elementSize, 1, m_tape) == 1)
    {
        std::fseek(m_tape,-elementSize, SEEK_CUR);
        if (m_realLatency)
        {
            std::this_thread::sleep_for(std::chrono::microseconds(m_readLatency));
        }
        return true;
    }
    goToCurrPos();
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
    return (m_curPos) == m_length;
}

bool TapeDeviceF::atStart() noexcept
{
    return m_curPos == 0;
}

void TapeDeviceF::goToCurrPos()
{
    fseek(m_tape,m_curPos,SEEK_SET);
}