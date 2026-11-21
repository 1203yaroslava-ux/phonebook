#pragma once
#include <string>
#include <vector>
#include "contact.h"
#include "storage/istorage.h"
enum class SortField { LastName, FirstName, MiddleName, Email, BirthDate };
class PhoneBook
{
public:
explicit PhoneBook(IContactStorage *storage);
bool load();
bool add(Contact c, std::string &error);
bool update(int id, Contact c, std::string &error);
bool remove(int id);
void sortBy(SortField field);
std::vector<Contact> search(const std::string &text) const;
const std::vector<Contact> &contacts() const { return m_contacts; }
const Contact *find(int id) const;
private:
IContactStorage *m_storage;
std::vector<Contact> m_contacts;
int m_nextId = 1;
};
