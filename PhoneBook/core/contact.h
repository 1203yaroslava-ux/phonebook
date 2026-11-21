#pragma once
#include <string>
#include <vector>
enum class PhoneType { Work = 0, Home = 1, Official = 2 };
struct Phone {
    PhoneType type;
    std::string number;
};
struct Contact {
    int id = 0;
    std::string firstName;
    std::string lastName;
    std::string middleName;     // необязательно
    std::string address;        // необязательно
    std::string birthDate;      // "ДД.ММ.ГГГГ" или пустая строка
    std::string email;
    std::vector<Phone> phones;
};
inline std::string phoneTypeToString(PhoneType t)
{
    switch (t) {
    case PhoneType::Work: return "work";
    case PhoneType::Home: return "home";
    default:              return "official";
    }
}
inline PhoneType phoneTypeFromString(const std::string &s)
{
    if (s == "work") return PhoneType::Work;
    if (s == "home") return PhoneType::Home;
    return PhoneType::Official;
}
