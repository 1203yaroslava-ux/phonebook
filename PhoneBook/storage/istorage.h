#pragma once
#include <vector>
#include "core/contact.h"
class IContactStorage
{
public:
    virtual ~IContactStorage() {}
    virtual bool load(std::vector<Contact> &contacts) = 0;
    virtual bool save(const std::vector<Contact> &contacts) = 0;
};
