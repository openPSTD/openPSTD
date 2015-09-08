//
// Created by michiel on 8-9-2015.
//

#include "InvalidationData.h"

bool InvalidationData::IsChanged()
{
    if(changed)
        return true;

    auto i = std::begin(items);

    while (i != std::end(items))
    {
        if (auto observe = i->lock())
        {
            if(observe->changed)
                return true;
            i++;
        }
        else
        {
            items.erase(i);
        }

    }
    return false;

}
void InvalidationData::Change()
{
    changed = true;
}
void InvalidationData::Reset()
{
    changed = false;

    auto i = std::begin(items);

    while (i != std::end(items))
    {
        if (auto observe = i->lock())
        {
            observe->Reset();
            i++;
        }
        else
        {
            items.erase(i);
        }
    }
}

void InvalidationData::Register(std::weak_ptr <InvalidationData> item)
{
    this->items.push_back(item);
}