#pragma once
#include <string>
#include "istorage.h"
class FileStorage : public IContactStorage
{
public:
    explicit FileStorage(const std::string &fileName);
    bool load(std::vector<Contact> &contacts) override;
    bool save(const std::vector<Contact> &contacts) override;

private:
    std::string m_fileName;
};
