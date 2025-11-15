#pragma once
#ifndef VALIDATOR_H
#define VALIDATOR_H

#include <string>
#include <regex>
#include <cwchar>
#include <ctime>

class Validator {
public:
    static bool isValidName(const std::wstring& name);
    static bool isValidEmail(const std::wstring& email);
    static bool isValidPhone(const std::wstring& phone);
    static bool isValidDate(const std::wstring& dateStr);
    static bool isDateBeforeToday(const std::wstring& dateStr);
    static std::wstring trim(const std::wstring& str);
    static bool isValidAddress(const std::wstring& addr);
};

#endif