#include <iostream>
#include <string>
#include <cstdlib>
#include "core/phonebook.h"
#include "core/validator.h"
#include "storage/filestorage.h"
#ifdef _WIN32
#include <windows.h>
#endif
static int g_failed = 0;
static void check(bool ok, const std::string &what)
{
std::cout << (ok ? "OK    " : "FAIL  ") << what << "\n";
if (!ok)
        ++g_failed;
}
static void runSelfTest()
{
    std::string s;
    s = "  Иван-Петров  ";
    check(Validator::checkName(s) && s == "Иван-Петров", "имя с дефисом, пробелы по краям убраны");
    s = "Anna Maria 2";   check(Validator::checkName(s),  "имя из латиницы, пробела и цифры");
    s = "-Иван";          check(!Validator::checkName(s), "имя не может начинаться с дефиса");
    s = "Иван-";          check(!Validator::checkName(s), "имя не может заканчиваться дефисом");
    s = "1Иван";          check(!Validator::checkName(s), "имя не может начинаться с цифры");
    s = "Иван_";          check(!Validator::checkName(s), "в имени нет подчёркивания");
    const char *goodPhones[] = {"+78121234567", "88121234567",
"+7(812)1234567",
                                "8(812)1234567", "+7(812)123-45-67",
"8(812)123-45-67"};
    for (const char *p : goodPhones) {
        s = p;
        check(Validator::checkPhone(s), std::string("телефон ") + p);
    }
    const char *badPhones[] = {"78121234567", "+7 812 1234567", "8(812)1234567"};
    for (const char *p : badPhones) {
        s = p;
        check(!Validator::checkPhone(s), std::string("неверный телефон ") +
p);
    }
    s = " user1 @ mail.ru ";
    check(Validator::checkEmail(s) && s == "user1@mail.ru", "e-mail: пробелы вокруг @ убраны");
    s = "user@mail";       check(!Validator::checkEmail(s), "e-mail без точки в домене");
    s = "юзер@mail.ru";    check(!Validator::checkEmail(s), "e-mail с русскими буквами");
    s = "29.02.2000";      check(Validator::checkBirthDate(s),  "29.02.2000 (високосный год)");
    s = "29.02.1900";      check(!Validator::checkBirthDate(s), "29.02.1900 (1900 не високосный)");
    s = "31.04.2001";      check(!Validator::checkBirthDate(s), "31.04.2001 (в апреле 30 дней)");
    s = "15.13.2000";      check(!Validator::checkBirthDate(s), "месяц 13");
    s = "01.01.2999";      check(!Validator::checkBirthDate(s), "дата в будущем");
    s = "5.3.1999";
    check(Validator::checkBirthDate(s) && s == "05.03.1999", "5.3.1999 приводится к 05.03.1999");
    std::cout << (g_failed == 0 ? "\nВсе проверки пройдены\n" : "\nЕсть ошибки\n");
}
//консольный ввод и вывод
static const char *typeName(PhoneType t)
{
    switch (t) {
    case PhoneType::Work: return "рабочий";
    case PhoneType::Home: return "домашний";
    default:              return "служебный";
    }
}
static void printContact(const Contact &c)
{
    std::cout << "[" << c.id << "] " << c.lastName << " " << c.firstName
              << " " << c.middleName << "\n";
    if (!c.birthDate.empty())
        std::cout << "    дата рождения: " << c.birthDate << "\n";
    if (!c.address.empty())
        std::cout << "    адрес: " << c.address << "\n";
    std::cout << "    e-mail: " << c.email << "\n";
    for (const Phone &p : c.phones)
        std::cout << "    " << typeName(p.type) << ": " << p.number << "\n";
}
// Enter оставляет текущее значение, "-" очищает поле
static std::string ask(const std::string &title, const std::string &current =
"")
{
    std::cout << title;
    if (!current.empty())
        std::cout << " [" << current << "]";
    std::cout << ": ";
    std::string s;
    std::getline(std::cin, s);
    s = Validator::trim(s);
    if (s.empty())
        return current;
    if (s == "-")
        return "";
    return s;
}
static Contact readContact(const Contact &old)
{
    Contact c;
    c.id         = old.id;
    c.lastName   = ask("Фамилия", old.lastName);
    c.firstName  = ask("Имя", old.firstName);
    c.middleName = ask("Отчество (необязательно)", old.middleName);
    c.address    = ask("Адрес (необязательно)", old.address);
    c.birthDate  = ask("Дата рождения ДД.ММ.ГГГГ (необязательно)",
old.birthDate);
    c.email      = ask("E-mail", old.email);
    bool keepPhones = false;
    if (!old.phones.empty())
        keepPhones = (ask("Оставить телефоны без изменений? (y/n)", "y") !=
"n");
    if (keepPhones) {
        c.phones = old.phones;
    } else {
        std::cout << "Телефоны. Тип: 1 - рабочий, 2 - домашний, 3 - служебный. "
                     "Пустой тип - конец ввода.\n";
        while (true) {
            std::cout << "Тип (1/2/3): ";
            std::string type;
            std::getline(std::cin, type);
            type = Validator::trim(type);
            if (type.empty())
                break;
            if (type != "1" && type != "2" && type != "3") {
                std::cout << "Нужно 1, 2 или 3\n";
                continue;
            }
            Phone p;
            p.type   = static_cast<PhoneType>(std::atoi(type.c_str()) - 1);
            p.number = ask("Номер");
            c.phones.push_back(p);
        }
    }
    return c;
}
static int askId()
{
    return std::atoi(ask("Номер контакта (id)").c_str());
}
static void printAll(const std::vector<Contact> &list)
{
    if (list.empty())
        std::cout << "Ничего нет\n";
    for (const Contact &c : list)
        printContact(c);
}
int main(int argc, char *argv[])
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
    if (argc > 1 && std::string(argv[1]) == "test") {
        runSelfTest();
        return g_failed == 0 ? 0 : 1;
    }
    FileStorage storage("contacts.txt");
    PhoneBook book(&storage);
    book.load();
    while (true) {
        std::cout << "\n1 - показать все\n2 - добавить\n3 - изменить\n4 - удалить\n"
                     "5 - сортировать\n6 - поиск\n0 - выход\n> ";
        std::string cmd;
        if (!std::getline(std::cin, cmd))
            break;
        cmd = Validator::trim(cmd);
        std::string error;
        if (cmd == "0") {
            break;
        } else if (cmd == "1") {
            printAll(book.contacts());
        } else if (cmd == "2") {
            if (book.add(readContact(Contact()), error))
                std::cout << "Добавлено\n";
            else
                std::cout << "Ошибка: " << error << "\n";
        } else if (cmd == "3") {
            int id = askId();
            const Contact *old = book.find(id);
            if (!old) {
                std::cout << "Нет такого контакта\n";
            } else if (book.update(id, readContact(*old), error)) {
                std::cout << "Изменено\n";
            } else {
                std::cout << "Ошибка: " << error << "\n";
            }
        } else if (cmd == "4") {
            std::cout << (book.remove(askId()) ? "Удалено\n" : "Нет такого контакта\n");
        } else if (cmd == "5") {
            std::string f = ask("Поле: 1 - фамилия, 2 - имя, 3 - отчество, 4 - e-mail, 5 - дата рождения");
            SortField field = SortField::LastName;
            if (f == "2") field = SortField::FirstName;
            else if (f == "3") field = SortField::MiddleName;
            else if (f == "4") field = SortField::Email;
            else if (f == "5") field = SortField::BirthDate;
            book.sortBy(field);
            printAll(book.contacts());
        } else if (cmd == "6") {
            printAll(book.search(ask("Что искать (имя, фамилия, e-mail, адрес, телефон)")));
        }
    }
    return 0;
};
