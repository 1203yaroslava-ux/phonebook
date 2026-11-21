#include "filestorage.h"
#include <fstream>
#include <sstream>
#include <cstdlib>
namespace {
std::vector<std::string> split(const std::string &s, char sep)
{
    std::vector<std::string> parts;
    std::string part;
    std::istringstream in(s);
    while (std::getline(in, part, sep))
        parts.push_back(part);
    if (!s.empty() && s.back() == sep)
        parts.push_back("");            // getline не возвращает последнее пустое поле
    return parts;
}
} // namespace
FileStorage::FileStorage(const std::string &fileName) : m_fileName(fileName)
{}
bool FileStorage::load(std::vector<Contact> &contacts)
{
    contacts.clear();
    std::ifstream in(m_fileName.c_str());
    if (!in)
        return true;                    // файла ещё нет: справочник пуст
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty())
            continue;
        std::vector<std::string> f = split(line, '|');
        if (f.size() != 8)
            continue;                   // повреждённая строка пропускается
        Contact c;
        c.id         = std::atoi(f[0].c_str());
        c.firstName  = f[1];
        c.lastName   = f[2];
        c.middleName = f[3];
        c.address    = f[4];
        c.birthDate  = f[5];
        c.email      = f[6];
        for (const std::string &p : split(f[7], ';')) {
            std::string::size_type pos = p.find(':');
            if (pos == std::string::npos)
                continue;
            Phone ph;
            ph.type   = phoneTypeFromString(p.substr(0, pos));
            ph.number = p.substr(pos + 1);
            c.phones.push_back(ph);
        }
        contacts.push_back(c);
    }
    return true;
}
bool FileStorage::save(const std::vector<Contact> &contacts)
{
    std::ofstream out(m_fileName.c_str(), std::ios::trunc);
    if (!out)
        return false;
    for (const Contact &c : contacts) {
        out << c.id << '|' << c.firstName << '|' << c.lastName << '|'
            << c.middleName << '|' << c.address << '|' << c.birthDate << '|'
            << c.email << '|';
        for (std::size_t i = 0; i < c.phones.size(); ++i) {
            if (i > 0)
                out << ';';
            out << phoneTypeToString(c.phones[i].type) << ':' <<
c.phones[i].number;
        }
        out << '\n';
    }
    return out.good();
};
