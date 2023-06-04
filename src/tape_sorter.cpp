//
// Created by asgrim on 04.06.23.
//
#include <iostream>
#include <algorithm>
#include <queue>
#include "tape_sorter.h"
#include "util.h"

int64_t fileIndex = 0;

bool calculate(int32_t a, int32_t b, SortOrder order)
{
    bool t = a < b;
    if (order == SortOrder::ASCENDING)
    {
        return t;
    } else
    {
        return !t;
    }
}

constexpr char* tmpDirPath = "/tmp/tempTape";

//I don't want to use this struct outside of this file
struct TempTape
{
    int64_t size;
    int64_t index;
    ITapeDevice *tapeDevice = nullptr;
    SortOrder sortOrder;

    bool operator<(const TempTape &rhs) const {
        return size < rhs.size;
    }

    bool operator>(const TempTape &rhs) const {
        return rhs < *this;
    }

    bool operator<=(const TempTape &rhs) const {
        return !(rhs < *this);
    }

    bool operator>=(const TempTape &rhs) const {
        return !(*this < rhs);
    }
};

bool TapeSorter::sortTapes(ITapeDevice &inTape, ITapeDevice &outTape)
{
    int32_t inElem = 0;
    inTape.setRealLatency(m_realLatency);
    outTape.setRealLatency(m_realLatency);
    int64_t elemCap = m_byteMemoryLimit / sizeof(inElem);
    uint32_t rLate, rewLate, wLate;
    rLate = inTape.getReadLatency();
    rewLate = inTape.getRewindLatency();
    wLate = inTape.getWriteLatency();
    std::priority_queue<TempTape,std::vector<TempTape>, std::greater<>> tmpQueue;
    while (!inTape.atEnd())
    {
        if (tmpQueue.size() < m_tempTapeLimit)
        {
            std::vector<int32_t> v;
            for (int i = 0; i < elemCap; ++i)
            {
                if (inTape.read(inElem))
                {
                    v.push_back(inElem);
                    inTape.goNext();
                } else
                {
                    break;
                }

            }
            mergSortInMemory(v);

            std::string filePath = tmpDirPath + std::to_string(fileIndex);
            if(createEmptyBinaryFile(filePath,sizeof(inElem),v.size()))
            {
                int64_t sz = v.size();
                TempTape tmpTape = {.size = sz, .index = fileIndex};
                ITapeDevice *tmpDev = new TapeDeviceF(filePath,
                                                      rLate,
                                                      wLate,
                                                      rewLate,
                                                      sz * sizeof(inElem));
                tmpTape.tapeDevice = tmpDev;
                tmpDev->setRealLatency(m_realLatency);
                //decending because we stop pointer in the und of tape
                tmpTape.sortOrder = SortOrder::DESCENDING;
                for (auto a : v)
                {
                    if (!tmpDev->write(a))
                    {

                        return false;
                    }
                    tmpDev->goNext();
                }
                fileIndex++;
                tmpQueue.push(tmpTape);
            }

        } else
        {
            if (tmpQueue.size() < 2)
            {
                continue;
            }
            auto left = tmpQueue.top();
            tmpQueue.pop();
            auto right = tmpQueue.top();
            tmpQueue.pop();

            //this is really bad
            //refactor later
            auto newTape = mergeTempTapes(left, right);
            std::string fileToDelete = tmpDirPath + std::to_string(left.index);
            delete left.tapeDevice;
            std::remove(fileToDelete.data());
            std::string fileToDelete2 = tmpDirPath + std::to_string(right.index);
            delete right.tapeDevice;
            std::remove(fileToDelete2.data());

            tmpQueue.push(newTape);
        }
    }

    while (tmpQueue.size() > 1)
    {
        auto left = tmpQueue.top();
        tmpQueue.pop();
        auto right = tmpQueue.top();
        tmpQueue.pop();

        //this is really bad
        //refactor later
        auto newTape = mergeTempTapes(left, right);
        std::string fileToDelete = tmpDirPath + std::to_string(left.index);
        delete left.tapeDevice;
        std::remove(fileToDelete.data());
        std::string fileToDelete2 = tmpDirPath + std::to_string(right.index);
        delete right.tapeDevice;
        std::remove(fileToDelete2.data());

        tmpQueue.push(newTape);
    }

    auto last = tmpQueue.top();
    tmpQueue.pop();
    int32_t out = 0;

    //damn
    if (last.sortOrder == SortOrder::ASCENDING)
    {
        while (true)
        {

            if (!last.tapeDevice->goPrev())
            {
                break;
            }
            last.tapeDevice->read(out);
            outTape.write(out);
            outTape.goNext();
        }
    } else
    {
        last.tapeDevice->rewindToStart();
        while (true)
        {

            if (!last.tapeDevice->read(out))
            {
                break;
            }
            outTape.write(out);
            outTape.goNext();
            if (!last.tapeDevice->goNext())
            {
                break;
            }
        }
    }
    delete last.tapeDevice;
    std::string lF = tmpDirPath + std::to_string(last.index);
    std::remove(lF.data());
    return true;
}

void TapeSorter::mergSortInMemory(std::vector<int32_t> &v)
{
    int64_t n  = v.size();
    for (int i = 1; i < n; i *= 2)
    {
        for (int j = 0; j < n -i ; j += 2 * i) {
            auto beg = v.begin();
            auto left = beg + j;
            auto mid = beg + (j + i);
            int64_t right =j + 2 * i;
            auto last = beg + std::min(right, n);
            std::inplace_merge(left,mid, last);
        }
    }
}


//merging temp tapes
//we reading temp tape from end to improve performance
//if tapes have different sorted types we need to rewind one of them to merge

TempTape TapeSorter::mergeTempTapes(TempTape left, TempTape right)
{
    if (left > right)
    {
        auto t = right;
        right = left;
        left = t;

    }
    if (fileIndex == 49)
    {
        std::cout << "";
    }
    SortOrder oldOrder = right.sortOrder;
    int32_t leftElem = 0;
    int32_t rightElem = 0;
    int64_t sz = left.size + right.size;
    std::string filePath = tmpDirPath + std::to_string(fileIndex);
    TempTape out;
    createEmptyBinaryFile(filePath,sizeof(int32_t),sz);
    out.index = fileIndex;
    fileIndex++;
    out.size = sz;
    if (oldOrder == SortOrder::DESCENDING)
    {
        out.sortOrder = SortOrder::ASCENDING;
    } else
    {
        out.sortOrder = SortOrder::DESCENDING;
    }

    ITapeDevice *tmpDev = new TapeDeviceF(filePath,
                                          left.tapeDevice->getReadLatency(),
                                          left.tapeDevice->getWriteLatency(),
                                          left.tapeDevice->getRewindLatency(),
                                          sz * sizeof(int32_t));
    tmpDev->setRealLatency(m_realLatency);
    out.tapeDevice = tmpDev;
    bool leftEnded;
    if (left.sortOrder != right.sortOrder)
    {
        left.tapeDevice->rewindToStart();
        right.tapeDevice->goPrev();
        left.tapeDevice->read(leftElem);
        right.tapeDevice->read(rightElem);

        while (true)
        {
            if(calculate(leftElem,rightElem, oldOrder))
            {
                tmpDev->write(leftElem);
                if (!left.tapeDevice->goNext())
                {
                    leftEnded = true;
                    break;
                }
                if (!left.tapeDevice->read(leftElem))
                {
                    leftEnded = true;
                    break;
                }

            } else
            {
                tmpDev->write(rightElem);
                if (!right.tapeDevice->goPrev())
                {
                    leftEnded = false;
                    break;
                }
                right.tapeDevice->read(rightElem);
            }
            tmpDev->goNext();
        }
        tmpDev->goNext();

        if (leftEnded)
        {
            tmpDev->write(rightElem);

            while (right.tapeDevice->goPrev())
            {
                if (!right.tapeDevice->read(rightElem))
                {
                    break;
                }

                tmpDev->goNext();
                tmpDev->write(rightElem);
            }


        } else{

            tmpDev->write(leftElem);
            while (left.tapeDevice->goNext())
            {
                if (!left.tapeDevice->read(leftElem))
                {
                    break;
                }
                tmpDev->goNext();
                tmpDev->write(leftElem);

            }

        }

    }
    else
    {
        left.tapeDevice->goPrev();
        right.tapeDevice->goPrev();
        left.tapeDevice->read(leftElem);
        right.tapeDevice->read(rightElem);


        while (true)
        {
            if(calculate(leftElem,rightElem, oldOrder))
            {
                tmpDev->write(leftElem);
                if (!left.tapeDevice->goPrev())
                {
                    leftEnded = true;
                    break;
                }
                left.tapeDevice->read(leftElem);
            } else
            {
                tmpDev->write(rightElem);
                if (!right.tapeDevice->goPrev())
                {
                    leftEnded = false;
                    break;
                }
                right.tapeDevice->read(rightElem);
            }
            tmpDev->goNext();
        }

        if (!leftEnded)
        {
            right = left;
            rightElem = leftElem;
        }

        tmpDev->goNext();
        tmpDev->write(rightElem);

        while (right.tapeDevice->goPrev())
        {
            if (!right.tapeDevice->read(rightElem))
            {
                break;
            }
            tmpDev->goNext();
            tmpDev->write(rightElem);

        }
    }

    tmpDev->rewindToEnd();
    return out;

}

bool TapeSorter::isRealLatency()
{
    return m_realLatency;
}

int64_t TapeSorter::getByteMemoryLimit()
{
    return m_byteMemoryLimit;
}

int16_t TapeSorter::getTempTapeLimit()
{
    return m_tempTapeLimit;
}

void TapeSorter::setByteMemoryLimit(int64_t limit)
{
    m_byteMemoryLimit = limit;
}

void TapeSorter::setRealLAtency(bool realLatency)
{
    m_realLatency = realLatency;
}

void TapeSorter::setTempTapeLimit(int64_t limit)
{
    m_tempTapeLimit = limit;
}


