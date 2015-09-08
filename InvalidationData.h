//
// Created by michiel on 8-9-2015.
//

#ifndef OPENPSTD_INVALIDATIONDATA_H
#define OPENPSTD_INVALIDATIONDATA_H

#include <memory>
#include <vector>

class InvalidationData
{
private:
    std::vector<std::weak_ptr<InvalidationData>> items;

public:
    InvalidationData() : changed(true), items()
    {

    }

    void Register(std::weak_ptr<InvalidationData> item);
    bool changed;
    bool IsChanged();
    void Change();
    void Reset();
};

template<typename T>
class InvalidationDataItem: public InvalidationData
{
public:
    T value;

    InvalidationDataItem()
    {

    }

    InvalidationDataItem(T internalValue): InvalidationData()
    {
        this->value = internalValue;
    }
    T Get()
    {
        return value;
    }

    void Set(T value)
    {
        this->value = value;
    }

    operator T&() { return value; }
    operator T() const { return value; }
};

#endif //OPENPSTD_INVALIDATIONDATA_H
