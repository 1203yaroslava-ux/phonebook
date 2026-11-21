#pragma once
#include <string>
#include "contact.h"
class Validator
{
public:
static std::string trim(const std::string &s);
static bool checkName(std::string &value);

// имя, фамилия, отчество
static bool checkEmail(std::string &value);
static bool checkPhone(std::string &value);
static bool checkBirthDate(std::string &value);   // пустая строка допустима
// проверка всего контакта; в error текст первой найденной ошибки
static bool checkContact(Contact &c, std::string &error);
};
