#include "phonebook.h"
#include "validator.h"
#include <algorithm>
namespace {
// "ДД.ММ.ГГГГ" -> "ГГГГММДД", чтобы даты сравнивались как строки
std::string dateKey(const std::string &d)
{
if (d.size() != 10)
return "";
return d.substr(6, 4) + d.substr(3, 2) + d.substr(0, 2);
}
bool contains(const std::string &text, const std::string &part)
{
    return text.find(part) != std::string::npos;
}
} // namespace
PhoneBook::PhoneBook(IContactStorage *storage) : m_storage(storage) {}
bool PhoneBook::load()
{
    if (!m_storage->load(m_contacts))
        return false;
    m_nextId = 1;
    for (const Contact &c : m_contacts)
        if (c.id >= m_nextId)
            m_nextId = c.id + 1;
    return true;
}
bool PhoneBook::add(Contact c, std::string &error)
{
    if (!Validator::checkContact(c, error))
        return false;
    c.id = m_nextId++;
    m_contacts.push_back(c);
    if (!m_storage->save(m_contacts)) {
        m_contacts.pop_back();          // откатываем изменение
        --m_nextId;
        error = "Не удалось сохранить данные";
        return false;
    }
    return true;
}
bool PhoneBook::update(int id, Contact c, std::string &error)
{
    if (!Validator::checkContact(c, error))
        return false;
    for (Contact &old : m_contacts) {
        if (old.id != id)
            continue;
        Contact backup = old;
        c.id = id;
        old = c;
        if (!m_storage->save(m_contacts)) {
            old = backup;
            error = "Не удалось сохранить данные";
            return false;
        }
        return true;
    }
    error = "Контакт не найден";
    return false;
}
bool PhoneBook::remove(int id)
{
    std::vector<Contact>::iterator it = std::remove_if(
        m_contacts.begin(), m_contacts.end(),
        [id](const Contact &c) { return c.id == id; });
    if (it == m_contacts.end())
        return false;
    m_contacts.erase(it, m_contacts.end());
    return m_storage->save(m_contacts);
}
void PhoneBook::sortBy(SortField field)
{
    std::sort(m_contacts.begin(), m_contacts.end(),
              [field](const Contact &a, const Contact &b) {
        switch (field) {
        case SortField::LastName:   return a.lastName   < b.lastName;
        case SortField::FirstName:  return a.firstName  < b.firstName;
        case SortField::MiddleName: return a.middleName < b.middleName;
        case SortField::Email:      return a.email      < b.email;
        default:                    return dateKey(a.birthDate) <
dateKey(b.birthDate);
        }
    });
}
std::vector<Contact> PhoneBook::search(const std::string &text) const
{
    if (text.empty())
        return m_contacts;
    std::vector<Contact> result;
    for (const Contact &c : m_contacts) {
        bool found = contains(c.firstName, text) || contains(c.lastName,
text)
                  || contains(c.middleName, text) || contains(c.email, text)
                  || contains(c.address, text);
for (const Phone &p : c.phones)
if (contains(p.number, text))
found = true;
if (found)
result.push_back(c);
}
return result;
}
const Contact *PhoneBook::find(int id) const
{
for (const Contact &c : m_contacts)
if (c.id == id)
return &c;
return nullptr;
};
