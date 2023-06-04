//
// Created by asgrim on 03.06.23.
//
#ifndef YADRO_TEST_TASK_I_TAPE_DEVICE_H
#define YADRO_TEST_TASK_I_TAPE_DEVICE_H
#include <cstdint>
class ITapeDevice
{
public:
    ITapeDevice() = default;
    virtual ~ITapeDevice()= default;

    virtual bool rewindLeft(int64_t offset) noexcept = 0 ;
    virtual bool rewindRight(int64_t offset) noexcept = 0;
    virtual void rewindToStart() noexcept = 0;
    virtual void rewindToEnd() noexcept = 0;
    virtual bool goNext() noexcept = 0;
    virtual bool goPrev() noexcept = 0;
    virtual bool write(int32_t elem) noexcept = 0;
    virtual bool read(int32_t &elem) noexcept = 0;
    virtual uint64_t getLen() noexcept = 0;
    virtual void setRealLatency(bool realLatency) noexcept = 0;
    virtual bool atEnd() noexcept = 0;
    virtual bool atStart() noexcept = 0;
    virtual uint32_t getReadLatency() noexcept = 0;
    virtual uint32_t getWriteLatency() noexcept = 0;
    virtual uint32_t getRewindLatency() noexcept = 0;

};
#endif //YADRO_TEST_TASK_I_TAPE_DEVICE_H
