#include "contact.h"
#include "validator.h"
#include <sstream>

Contact::Contact(const std::wstring& fn, const std::wstring& ln, const std::wstring& p,
    const std::wstring& addr, const std::wstring& bd, const std::wstring& em,
    const std::vector<PhoneNumber>& phs)
    : firstName(fn), lastName(ln), patronymic(p), address(addr),
    birthDate(bd), email(em), phones(phs) {
}

bool Contact::operator==(const Contact& other) const {
    return firstName == other.firstName &&
        lastName == other.lastName &&
        patronymic == other.patronymic &&
        address == other.address &&
        birthDate == other.birthDate &&
        email == other.email &&
        phones.size() == other.phones.size();
}

bool Contact::operator!=(const Contact& other) const {
    return !(*this == other);
}

void Contact::print() const {
    std::wcout << L"Имя: " << firstName << L"\n";
    std::wcout << L"Фамилия: " << lastName << L"\n";
    std::wcout << L"Отчество: " << patronymic << L"\n";
    std::wcout << L"Адрес: " << address << L"\n";
    std::wcout << L"Дата рождения: " << birthDate << L"\n";
    std::wcout << L"Email: " << email << L"\n";
    std::wcout << L"Телефоны:\n";
    for (const auto& phone : phones) {
        std::wcout << L"  " << phoneTypeToString(phone.type) << L": " << phone.number << L"\n";
    }
    std::wcout << L"-------------------------\n";
}

std::wstring Contact::toString() const {
    std::wstring result = firstName + L";" + lastName + L";" + patronymic + L";" +
        address + L";" + birthDate + L";" + email + L";";

    for (size_t i = 0; i < phones.size(); ++i) {
        if (i > 0) result += L",";
        result += phones[i].number + L":" + phoneTypeToString(phones[i].type);
    }
    return result;
}

Contact Contact::fromString(const std::wstring& line) {
    Contact c;
    std::wstringstream ss(line);
    std::wstring token;
    std::vector<std::wstring> tokens;

    while (std::getline(ss, token, L';')) {
        tokens.push_back(token);
    }

    if (tokens.size() < 6) return c;

    c.firstName = tokens[0];
    c.lastName = tokens[1];
    c.patronymic = tokens[2];
    c.address = tokens[3];
    c.birthDate = tokens[4];
    c.email = tokens[5];

    if (tokens.size() > 6 && !tokens[6].empty()) {
        std::wstringstream phoneStream(tokens[6]);
        std::wstring phoneEntry;
        while (std::getline(phoneStream, phoneEntry, L',')) {
            size_t colonPos = phoneEntry.find(L':');
            if (colonPos != std::wstring::npos) {
                std::wstring number = phoneEntry.substr(0, colonPos);
                std::wstring typeStr = phoneEntry.substr(colonPos + 1);
                c.phones.push_back(PhoneNumber(number, c.stringToPhoneType(typeStr)));
            }
            else {
              
                c.phones.push_back(PhoneNumber(phoneEntry));
            }
        }
    }

    return c;
}

bool Contact::hasRequiredFields() const {
  
    return !firstName.empty() &&
        !lastName.empty() &&
        !email.empty() &&
        !phones.empty();
}

std::wstring Contact::phoneTypeToString(PhoneType type) const {
    switch (type) {
    case PhoneType::HOME: return L"home";
    case PhoneType::WORK: return L"work";
    case PhoneType::OFFICE: return L"office";
    default: return L"other";
    }
}

PhoneType Contact::stringToPhoneType(const std::wstring& typeStr) const {
    if (typeStr == L"home") return PhoneType::HOME;
    if (typeStr == L"work") return PhoneType::WORK;
    if (typeStr == L"office") return PhoneType::OFFICE;
    return PhoneType::OTHER;
}