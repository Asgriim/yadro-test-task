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
    if(m_tape == nullptr)
    {
        fclose(fopen(path.data(),"w"));
        m_tape = std::fopen(path.data(),"r+b");
    }
    std::fseek(m_tape,0,SEEK_SET);

  }

TapeDeviceF::TapeDeviceF(TapeDeviceF &&tapeDeviceF) noexcept
{
m_tape = tapeDeviceF.m_tape;
tapeDeviceF.m_tape = nullptr;
m_readLatency = tapeDeviceF.m_readLatency;
m_writeLatency = tapeDeviceF.m_writeLatency;
m_curPos = tapeDeviceF.m_curPos;
m_length = tapeDeviceF.m_length;
m_rewindLatency = tapeDeviceF.m_rewindLatency;
    m_emulateLatency = tapeDeviceF.m_emulateLatency;
}


TapeDeviceF& TapeDeviceF::operator=(TapeDeviceF &&tapeDeviceF) noexcept
{
    if(this == &tapeDeviceF)
    {
        return *this;
    }

    if (m_tape != nullptr)
    {
        std::fclose(m_tape);
    }
    m_tape = tapeDeviceF.m_tape;
    tapeDeviceF.m_tape = nullptr;
    m_readLatency = tapeDeviceF.m_readLatency;
    m_writeLatency = tapeDeviceF.m_writeLatency;
    m_curPos = tapeDeviceF.m_curPos;
    m_length = tapeDeviceF.m_length;
    m_rewindLatency = tapeDeviceF.m_rewindLatency;
    m_emulateLatency = tapeDeviceF.m_emulateLatency;
    return *this;
}


TapeDeviceF::~TapeDeviceF()
{
    if(m_tape != nullptr)
    {
        std::fclose(m_tape);
    }

}

//todo обавить слип потока на задержках
bool TapeDeviceF::rewindLeft(int64_t offset) noexcept
{
    if (m_curPos == 0 || m_tape == nullptr)
    {
        return false;
    }
    int64_t tmpOff = offset * elementSize;
    if (!std::fseek(m_tape, -tmpOff, SEEK_CUR))
    {
        m_curPos -= tmpOff;
        if (m_emulateLatency)
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
    if (m_curPos == m_length || m_tape == nullptr)
    {
        return false;
    }
    if (!std::fseek(m_tape, elementSize, SEEK_CUR))
    {
        m_curPos += elementSize;
        if (m_emulateLatency)
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
    if (m_tape == nullptr)
    {
        return false;
    }
    int64_t tmpOff = offset * elementSize;
    if (!std::fseek(m_tape, tmpOff, SEEK_CUR))
    {
        m_curPos += tmpOff;
        if(m_emulateLatency)
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
    if (m_tape == nullptr)
    {
        return;
    }
    std::rewind(m_tape);
    if(m_emulateLatency)
    {
        std::this_thread::sleep_for(std::chrono::microseconds(m_rewindLatency * m_curPos));
    }
    m_curPos = 0;
}

void TapeDeviceF::rewindToEnd() noexcept
{
    if (m_tape == nullptr)
    {
        return;
    }
    fseek(m_tape,0,SEEK_END);
    m_curPos = ftell(m_tape);
    if (m_emulateLatency)
    {
        std::this_thread::sleep_for(std::chrono::microseconds(m_rewindLatency * (m_length - m_curPos)));
    }
}

bool TapeDeviceF::goPrev() noexcept
{
    if (m_curPos == 0 || m_tape == nullptr)
    {
        return false;
    }
    if(!std::fseek(m_tape, -elementSize, SEEK_CUR))
    {
        if (m_emulateLatency)
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
    if (m_curPos == m_length || m_tape == nullptr)
    {
        return false;
    }
    if(std::fwrite(&elem, elementSize, 1, m_tape) == 1)
    {
        std::fseek(m_tape,-elementSize, SEEK_CUR);
        if (m_emulateLatency)
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
    if (m_curPos == m_length || m_tape == nullptr)
    {
        return false;
    }
    if(std::fread(&elem, elementSize, 1, m_tape) == 1)
    {
        std::fseek(m_tape,-elementSize, SEEK_CUR);
        if (m_emulateLatency)
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
    m_emulateLatency = realLatency;
}
bool TapeDeviceF::atEnd() noexcept
{
    return (m_curPos) == m_length;
}

bool TapeDeviceF::atStart() noexcept
{
    return m_curPos == 0;
}

//restore file ptr if error occurred
void TapeDeviceF::goToCurrPos()
{
    if (m_tape == nullptr)
    {
        return;
    }
    fseek(m_tape,m_curPos,SEEK_SET);
}

uint32_t TapeDeviceF::getReadLatency() noexcept
{
    return m_readLatency;
}

uint32_t TapeDeviceF::getRewindLatency() noexcept
{
    return m_rewindLatency;
}

uint32_t TapeDeviceF::getWriteLatency() noexcept
{
    return m_writeLatency;
}