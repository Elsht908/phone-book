#include "manager.h"
#include "validator.h"
#include <iostream>
#include <algorithm>
#include <cwchar>
#include <ctime>
#include <fstream>
#include <string>
#include <sstream>
#include <locale>
#include <codecvt>


Manager::Manager() {
    loadFromFile();
}

Manager::~Manager() {
    saveToFile();
}

void Manager::saveToFile() {
    std::wofstream file(L"contact.txt");
    
    std::locale utf8_locale(std::locale(), new std::codecvt_utf8<wchar_t>);
    file.imbue(utf8_locale);

    if (!file.is_open()) {
        std::wcerr << L"Ошибка открытия файла для записи.\n";
        return;
    }

    for (const auto& contact : contacts) {
        file << contact.toString() << L"\n";
    }
    file.close();
}

void Manager::loadFromFile() {
    std::wifstream file(L"contact.txt");
    std::locale utf8_locale(std::locale(), new std::codecvt_utf8<wchar_t>);
    file.imbue(utf8_locale);

    if (!file.is_open()) {
        return;
    }

    std::wstring line;
    while (std::getline(file, line)) {
        Contact c = Contact::fromString(line);
        if (!c.firstName.empty()) {
            contacts.push_back(c);
        }
    }
    file.close();
}

void Manager::addContact() {
    std::wstring firstName, lastName, patronymic, address, birthDate, email;
    std::vector<PhoneNumber> phones;
    
    while (true) {
        std::wcout << L"Введите имя: ";
        std::getline(std::wcin, firstName);
        firstName = Validator::trim(firstName);

        if (firstName == L"0" || firstName == L"exit") {
            std::wcout << L"Отмена добавления контакта.\n";
            return;
        }

        if (Validator::isValidName(firstName)) {
            break;
        }
        std::wcout << L"Некорректное имя! Должно содержать только буквы, цифры, дефис и пробел, начинаться с буквы.\n";
    }

    while (true) {
        std::wcout << L"Введите фамилию: ";
        std::getline(std::wcin, lastName);
        lastName = Validator::trim(lastName);

        if (lastName == L"0" || lastName == L"exit") {
            std::wcout << L"Отмена добавления контакта.\n";
            return;
        }

        if (Validator::isValidName(lastName)) {
            break;
        }
        std::wcout << L"Некорректная фамилия! Должна содержать только буквы, цифры, дефис и пробел, начинаться с буквы.\n";
    }

    std::wcout << L"Введите отчество (необязательно): ";
    std::getline(std::wcin, patronymic);
    patronymic = Validator::trim(patronymic);
    if (!patronymic.empty()) {
        while (!Validator::isValidName(patronymic)) {
            std::wcout << L"Некорректное отчество! Попробуйте снова (или Enter для пропуска): ";
            std::getline(std::wcin, patronymic);
            patronymic = Validator::trim(patronymic);
            if (patronymic.empty()) break;
        }
    }

    std::wcout << L"Введите адрес (необязательно): ";
    std::getline(std::wcin, address);

    while (true) {
        std::wcout << L"Введите дату рождения (YYYY-MM-DD): ";
        std::getline(std::wcin, birthDate);
        birthDate = Validator::trim(birthDate);
        if (Validator::isValidDate(birthDate) && Validator::isDateBeforeToday(birthDate)) {
            break;
        }
        std::wcout << L"Некорректная дата рождения! Проверьте формат и что дата не в будущем.\n";
    }

    while (true) {
        std::wcout << L"Введите email: ";
        std::getline(std::wcin, email);
        email = Validator::trim(email);
        if (Validator::isValidEmail(email)) {
            break;
        }
        std::wcout << L"Некорректный email! Пример: user@domain.com\n";
    }

    std::wstring phone;
    PhoneType phoneType = PhoneType::HOME;

    while (true) {
        std::wcout << L"Введите телефон (минимум один): ";
        std::getline(std::wcin, phone);
        phone = Validator::trim(phone);
        if (Validator::isValidPhone(phone)) {
            phones.push_back(PhoneNumber(phone, phoneType));
            break;
        }
        std::wcout << L"Некорректный телефон! Допустимые форматы:\n";
        std::wcout << L"+78121234567\n";
        std::wcout << L"88121234567\n";
        std::wcout << L"+7(812)1234567\n";
        std::wcout << L"8(812)1234567\n";
        std::wcout << L"+7(812)123-45-67\n";
        std::wcout << L"8(812)123-45-67\n";
    }

    std::wstring more;
    do {
        std::wcout << L"Добавить еще телефон? (y/n): ";
        std::getline(std::wcin, more);
        if (more == L"y" || more == L"Y") {
            while (true) {
                std::wcout << L"Введите телефон: ";
                std::getline(std::wcin, phone);
                phone = Validator::trim(phone);

                std::wcout << L"Выберите тип телефона (1-дом, 2-работа, 3-служебный, 4-другой): ";
                std::wstring typeChoice;
                std::getline(std::wcin, typeChoice);

                PhoneType type = PhoneType::OTHER;
                if (typeChoice == L"1") type = PhoneType::HOME;
                else if (typeChoice == L"2") type = PhoneType::WORK;
                else if (typeChoice == L"3") type = PhoneType::OFFICE;

                if (Validator::isValidPhone(phone)) {
                    phones.push_back(PhoneNumber(phone, type));
                    std::wcout << L"Телефон добавлен.\n";
                    break;
                }
                std::wcout << L"Некорректный телефон! Попробуйте снова.\n";
            }
        }
    } while (more == L"y" || more == L"Y");

    Contact newContact(firstName, lastName, patronymic, address, birthDate, email, phones);
    contacts.push_back(newContact);
    std::wcout << L"Контакт успешно добавлен!\n";
    saveToFile();
}
void Manager::deleteContact(size_t index) {
    if (index >= contacts.size()) {
        std::wcout << L"Неверный индекс!\n";
        return;
    }
    contacts.erase(contacts.begin() + index);
    std::wcout << L"Контакт удален.\n";
    saveToFile();
}

void Manager::editContact(size_t index) {
    if (index >= contacts.size()) {
        std::wcout << L"Неверный индекс!\n";
        return;
    }

    Contact& c = contacts[index];
    std::wstring input;

    std::wcout << L"Редактирование контакта:\n";
    c.print();

    std::wcout << L"Новое имя (Enter - оставить текущее): ";
    std::getline(std::wcin, input);
    if (!input.empty()) {
        input = Validator::trim(input);
        while (!Validator::isValidName(input)) {
            std::wcout << L"Некорректное имя! Попробуйте снова: ";
            std::getline(std::wcin, input);
            input = Validator::trim(input);
        }
        c.firstName = input;
    }

    std::wcout << L"Новая фамилия (Enter - оставить текущую): ";
    std::getline(std::wcin, input);
    if (!input.empty()) {
        input = Validator::trim(input);
        while (!Validator::isValidName(input)) {
            std::wcout << L"Некорректная фамилия! Попробуйте снова: ";
            std::getline(std::wcin, input);
            input = Validator::trim(input);
        }
        c.lastName = input;
    }

    std::wcout << L"Новое отчество (Enter - оставить текущее): ";
    std::getline(std::wcin, input);
    if (!input.empty()) {
        input = Validator::trim(input);
        while (!Validator::isValidName(input)) {
            std::wcout << L"Некорректное отчество! Попробуйте снова: ";
            std::getline(std::wcin, input);
            input = Validator::trim(input);
        }
        c.patronymic = input;
    }

    std::wcout << L"Новый адрес (Enter - оставить текущий): ";
    std::getline(std::wcin, input);
    if (!input.empty()) {
        c.address = input;
    }

    std::wcout << L"Новая дата рождения (YYYY-MM-DD, Enter - оставить текущую): ";
    std::getline(std::wcin, input);
    if (!input.empty()) {
        input = Validator::trim(input);
        while (!Validator::isValidDate(input) || !Validator::isDateBeforeToday(input)) {
            std::wcout << L"Некорректная дата рождения! Попробуйте снова: ";
            std::getline(std::wcin, input);
            input = Validator::trim(input);
        }
        c.birthDate = input;
    }

    std::wcout << L"Новый email (Enter - оставить текущий): ";
    std::getline(std::wcin, input);
    if (!input.empty()) {
        input = Validator::trim(input);
        while (!Validator::isValidEmail(input)) {
            std::wcout << L"Некорректный email! Попробуйте снова: ";
            std::getline(std::wcin, input);
            input = Validator::trim(input);
        }
        c.email = input;
    }

    std::wcout << L"Редактировать телефоны? (y/n): ";
    std::getline(std::wcin, input);
    if (input == L"y" || input == L"Y") {
        std::wcout << L"Текущие телефоны:\n";
        for (size_t i = 0; i < c.phones.size(); ++i) {
            std::wcout << i + 1 << L": " << c.phoneTypeToString(c.phones[i].type)
                << L" - " << c.phones[i].number << L"\n";  
        }

        std::wcout << L"Удалить телефон? Введите номер (0 - отмена): ";
        std::wstring delInput;
        std::getline(std::wcin, delInput);
        try {
            int delIndex = std::stoi(delInput);
            if (delIndex > 0 && delIndex <= static_cast<int>(c.phones.size())) {
                c.phones.erase(c.phones.begin() + delIndex - 1);
                std::wcout << L"Телефон удален.\n";
            }
        }
        catch (...) {}

        std::wcout << L"Добавить телефон? (y/n): ";
        std::getline(std::wcin, input);
        while (input == L"y" || input == L"Y") {
            std::wstring phone;
            while (true) {
                std::wcout << L"Введите телефон: ";
                std::getline(std::wcin, phone);
                phone = Validator::trim(phone);
                if (Validator::isValidPhone(phone)) {
                    c.phones.push_back(phone);
                    std::wcout << L"Телефон добавлен.\n";
                    break;
                }
                std::wcout << L"Некорректный телефон! Попробуйте снова.\n";
            }
            std::wcout << L"Добавить еще? (y/n): ";
            std::getline(std::wcin, input);
        }
    }

    std::wcout << L"Контакт обновлен.\n";
    saveToFile();
}

void Manager::listContacts() const {
    if (contacts.empty()) {
        std::wcout << L"Список контактов пуст.\n";
        return;
    }

    std::wcout << L"=== СПИСОК КОНТАКТОВ ===\n";
    for (size_t i = 0; i < contacts.size(); ++i) {
        std::wcout << L"[" << i + 1 << L"] ";
        contacts[i].print();
    }
}

void Manager::searchContacts() {
    std::wstring query;
    std::wcout << L"Введите поисковый запрос: ";
    std::getline(std::wcin, query);
    query = Validator::trim(query);

    if (query.empty()) {
        std::wcout << L"Поисковый запрос пуст.\n";
        return;
    }

    std::wcout << L"=== РЕЗУЛЬТАТЫ ПОИСКА ===\n";
    bool found = false;
    for (size_t i = 0; i < contacts.size(); ++i) {
        const Contact& c = contacts[i];

        auto containsIgnoreCase = [](const std::wstring& str, const std::wstring& substr) {
            if (substr.empty()) return false;

            std::wstring strLower = str;
            std::wstring substrLower = substr;
            std::transform(strLower.begin(), strLower.end(), strLower.begin(), ::towlower);
            std::transform(substrLower.begin(), substrLower.end(), substrLower.begin(), ::towlower);

            return strLower.find(substrLower) != std::wstring::npos;
            };

        if (containsIgnoreCase(c.firstName, query) ||
            containsIgnoreCase(c.lastName, query) ||
            containsIgnoreCase(c.patronymic, query) ||
            containsIgnoreCase(c.email, query) ||
            containsIgnoreCase(c.address, query)) {

            std::wcout << L"[" << i + 1 << L"] ";
            c.print();
            found = true;
        }
    }

    if (!found) {
        std::wcout << L"Ничего не найдено.\n";
    }
}

void Manager::sortContacts() {
    if (contacts.empty()) {
        std::wcout << L"Список контактов пуст.\n";
        return;
    }

    std::wcout << L"Сортировать по:\n";
    std::wcout << L"1. Фамилии\n";
    std::wcout << L"2. Имени\n";
    std::wcout << L"3. Дате рождения\n";
    std::wcout << L"4. Email\n";
    std::wcout << L"Выберите: ";

    std::wstring choice;
    std::getline(std::wcin, choice);

    if (choice == L"1") {
        std::sort(contacts.begin(), contacts.end(),
            [](const Contact& a, const Contact& b) {
                return a.lastName < b.lastName;
            });
    }
    else if (choice == L"2") {
        std::sort(contacts.begin(), contacts.end(),
            [](const Contact& a, const Contact& b) {
                return a.firstName < b.firstName;
            });
    }
    else if (choice == L"3") {
        std::sort(contacts.begin(), contacts.end(),
            [](const Contact& a, const Contact& b) {
                return a.birthDate < b.birthDate;
            });
    }
    else if (choice == L"4") {
        std::sort(contacts.begin(), contacts.end(),
            [](const Contact& a, const Contact& b) {
                return a.email < b.email;
            });
    }
    else {
        std::wcout << L"Неверный выбор.\n";
        return;
    }

    std::wcout << L"Список отсортирован.\n";
    listContacts();
}