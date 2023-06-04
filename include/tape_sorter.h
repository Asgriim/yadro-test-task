//
// Created by asgrim on 04.06.23.
//

#ifndef YADRO_TEST_TASK_TAPE_SORTER_H
#define YADRO_TEST_TASK_TAPE_SORTER_H
#include <cstdint>
#include <tape_device_f.h>
#include <vector>

enum SortOrder
{
    ASCENDING = 0,
    DESCENDING
};


struct TempTape;

class TapeSorter
{
public:
    bool sortTapes(ITapeDevice &inTape, ITapeDevice &outTape);
    void mergSortInMemory(std::vector<int32_t> &v);
    bool isRealLatency();
    int64_t getByteMemoryLimit();
    int16_t getTempTapeLimit();

    void setRealLAtency(bool realLatency);
    void setByteMemoryLimit(int64_t limit);
    void setTempTapeLimit(int64_t limit);
private:
    TempTape mergeTempTapes(TempTape left, TempTape right);
    bool m_realLatency = false;
    //16 MB by default
    int64_t m_byteMemoryLimit = 16777216;
    int16_t m_tempTapeLimit = 256;
};


#endif //YADRO_TEST_TASK_TAPE_SORTER_H
