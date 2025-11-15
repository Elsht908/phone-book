#pragma once
#ifndef CONTACT_H
#define CONTACT_H

#include <string>
#include <vector>
#include <iostream>

enum class PhoneType {
    HOME,
    WORK,
    OFFICE,
    OTHER
};

struct PhoneNumber {
    std::wstring number;
    PhoneType type;

    PhoneNumber(const std::wstring& num, PhoneType t = PhoneType::HOME)
        : number(num), type(t) {
    }
};

class Contact {
public:
    std::wstring firstName;
    std::wstring lastName;
    std::wstring patronymic;
    std::wstring address;
    std::wstring birthDate;
    std::wstring email;
    std::vector<PhoneNumber> phones;  

    Contact() = default;
    Contact(const std::wstring& fn, const std::wstring& ln, const std::wstring& p,
        const std::wstring& addr, const std::wstring& bd, const std::wstring& em,
        const std::vector<PhoneNumber>& phs);

    bool operator==(const Contact& other) const;
    bool operator!=(const Contact& other) const;

    void print() const;
    std::wstring toString() const;
    static Contact fromString(const std::wstring& line);

    bool hasRequiredFields() const;
    std::wstring phoneTypeToString(PhoneType type) const;
    PhoneType stringToPhoneType(const std::wstring& typeStr) const;
};

#endif