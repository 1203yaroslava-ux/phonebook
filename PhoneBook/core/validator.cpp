#include "validator.h"
#include <regex>
#include <locale>
#include <codecvt>
#include <ctime>
#include <cstdio>
#include <stdexcept>
namespace {
// строки в программе хранятся в UTF-8, а wregex работает с широкими символами
std::wstring toWide(const std::string &s)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t> > conv;
    return conv.from_bytes(s);
}
} // namespace
std::string Validator::trim(const std::string &s)
{
    const char *spaces = " \t\r\n";
    std::string::size_type b = s.find_first_not_of(spaces);
    if (b == std::string::npos)
        return "";
    std::string::size_type e = s.find_last_not_of(spaces);
    return s.substr(b, e - b + 1);
}
bool Validator::checkName(std::string &value)
{
    value = trim(value);
    try {
        static const std::wregex re(
            L"^[A-Za-z\u0410-\u044F\u0401\u0451]"
            L"([A-Za-z0-9\u0410-\u044F\u0401\u0451 \\-]*"
            L"[A-Za-z0-9\u0410-\u044F\u0401\u0451])?$");
        return std::regex_match(toWide(value), re);
    } catch (const std::exception &) {
        return false;               // например, некорректный UTF-8
    }
}
bool Validator::checkEmail(std::string &value)
{
    static const std::regex re(
        "^([A-Za-z0-9]+)\\s*@\\s*([A-Za-z0-9]+(\\.[A-Za-z0-9]+)+)$");
    std::string s = trim(value);
    std::smatch m;
    if (!std::regex_match(s, m, re))
        return false;
    value = m[1].str() + "@" + m[2].str();     // без пробелов вокруг @
    return true;
}
bool Validator::checkPhone(std::string &value)
{
    static const std::regex re(
        "^(\\+7|8)(\\d{10}|\\(\\d{3}\\)(\\d{7}|\\d{3}-\\d{2}-\\d{2}))$");
    value = trim(value);
    return std::regex_match(value, re);
}
bool Validator::checkBirthDate(std::string &value)
{
    value = trim(value);
    if (value.empty())
        return true;                // дата необязательна
    static const std::regex re("^(\\d{1,2})\\.(\\d{1,2})\\.(\\d{4})$");
    std::smatch m;
    if (!std::regex_match(value, m, re))
        return false;
    int d  = std::stoi(m[1].str());
    int mo = std::stoi(m[2].str());
    int y  = std::stoi(m[3].str());
    if (mo < 1 || mo > 12 || d < 1)
        return false;
    static const int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30,
31};
    bool leap = (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
    int maxDay = days[mo - 1] + ((mo == 2 && leap) ? 1 : 0);
    if (d > maxDay)
        return false;
    // дата рождения должна быть меньше текущей даты
    std::time_t now = std::time(nullptr);
    std::tm *t = std::localtime(&now);
    int cy = t->tm_year + 1900, cm = t->tm_mon + 1, cd = t->tm_mday;
    if (y > cy || (y == cy && (mo > cm || (mo == cm && d >= cd))))
        return false;
    char buf[16];
    std::snprintf(buf, sizeof(buf), "%02d.%02d.%04d", d, mo, y);
    value = buf;                    // приводим к виду ДД.ММ.ГГГГ
    return true;
}
bool Validator::checkContact(Contact &c, std::string &error)
{
    if (!checkName(c.firstName)) {
        error = "Некорректное имя";
        return false;
    }
    if (!checkName(c.lastName)) {
        error = "Некорректная фамилия";
        return false;
    }
    c.middleName = trim(c.middleName);
    if (!c.middleName.empty() && !checkName(c.middleName)) {
        error = "Некорректное отчество";
        return false;
    }
    c.address = trim(c.address);
    if (c.address.find_first_of("|\n\r") != std::string::npos) {
        error = "Адрес содержит недопустимые символы";
        return false;
    }
    if (!checkBirthDate(c.birthDate)) {
        error = "Некорректная дата рождения (нужна ДД.ММ.ГГГГ, раньше сегодняшней)";
        return false;
    }
    if (!checkEmail(c.email)) {
        error = "Некорректный e-mail";
        return false;
    }
    if (c.phones.empty()) {
        error = "Нужен хотя бы один номер телефона";
        return false;
    }
    for (Phone &p : c.phones) {
        if (!checkPhone(p.number)) {
            error = "Некорректный номер телефона: " + p.number;
            return false;
        }
    }
    return true;
};
