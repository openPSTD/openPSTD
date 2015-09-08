//
// Created by michiel on 8-9-2015.
//

#ifndef OPENPSTD_INVALIDATIONDATA_H
#define OPENPSTD_INVALIDATIONDATA_H

#include <memory>
#include <vector>

/**
 * A class that keeps track if something has changed. If it changes than Change has to be called. To check call
 * IsChanged. To reset the change, Reset has to be called. It also keeps children that individually can be changed.
 */
class InvalidationData
{
private:
    std::vector<std::weak_ptr<InvalidationData>> items;
    virtual bool changed;

public:
    /**
     * Creates a the invalidation class. This initilizes the class that the value has changed.
     */
    InvalidationData() : changed(true), items()
    {

    }

    /**
     * Registers a child that is checked when calling IsChanged. This is a weak_ptr, so that if the object is destroyed,
     * then it will deregister.
     */
    virtual void Register(std::weak_ptr<InvalidationData> child);
    /**
     * checks if it has changed or any of it's children.
     */
    virtual bool IsChanged();
    /**
     * Specify that there is a change.
     */
    virtual void Change();
    /**
     * Resets the change.
     */
    virtual void Reset();
};

/**
 * A template class that can hold a single value.
 */
template<typename T>
class InvalidationDataItem: public InvalidationData
{
public:
    T value;

    /**
     * Initiates the class
     */
    InvalidationDataItem()
    {

    }

    /**
     * Initiates the class with a specific value
     */
    InvalidationDataItem(T internalValue): InvalidationData()
    {
        this->value = internalValue;
    }
    /**
     * returns the value
     */
    T Get()
    {
        return value;
    }

    /**
     * Sets the value. With this function Change is also called from the base class.
     */
    void Set(T value)
    {
        this->value = value;
        this->Change();
    }

    /**
     * implicit conversion to the value of type T
     */
    operator T&() { return value; }
    /**
     * implicit conversion to the value of type T
     */
    operator T() const { return value; }
};

#endif //OPENPSTD_INVALIDATIONDATA_H
