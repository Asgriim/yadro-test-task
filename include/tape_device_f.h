//
// Created by asgrim on 03.06.23.
//

#ifndef YADRO_TEST_TASK_TAPEDEVICEFILE_H
#define YADRO_TEST_TASK_TAPEDEVICEFILE_H
#include "i_tape_device.h"
#include <fstream>
class  __attribute__((packed)) TapeDeviceF : public ITapeDevice
{
public:
    TapeDeviceF(const std::string &path,
                   uint32_t readLatency,
                   uint32_t writeLatency,
                   uint32_t rewindLatency,
                   uint64_t length) noexcept;
    ~TapeDeviceF() override;

    bool rewindLeft(int64_t offset) noexcept override;
    bool rewindRight(int64_t offset) noexcept override;
    void rewindToStart() noexcept override;
    void rewindToEnd() noexcept override;
    bool goNext() noexcept override;
    bool goPrev() noexcept override;
    bool write(int32_t elem) noexcept override;
    bool read(int32_t &elem) noexcept override;
    uint64_t getLen() noexcept override;
    void setRealLatency(bool realLatency) noexcept override;
    bool atEnd() noexcept override;
    bool atStart() noexcept override;
private:
    //milliseconds
    uint32_t m_readLatency;
    uint32_t m_writeLatency;
    uint32_t m_rewindLatency;
    //length of tape
    uint64_t m_length;
    //current position of tape
    int64_t m_curPos = 0;
    //todo удалитть curr elem
    uint64_t m_currElem = 0;
    // defines to emulate tape latency
    bool m_realLatency = false;
    //in this case easier to use file
    std::FILE *m_tape;

};
#endif //YADRO_TEST_TASK_TAPEDEVICEFILE_H
