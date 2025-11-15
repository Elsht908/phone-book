#include "validator.h"
#include <algorithm>
#include <cwchar>
#include <ctime>
#include <cctype>
#include <iomanip>
#include <sstream>
#include <regex>

std::wstring Validator::trim(const std::wstring& str) {
    if (str.empty()) return str;

    size_t start = 0, end = str.length() - 1;

    while (start <= end && ::iswspace(str[start])) ++start;
    while (end >= start && ::iswspace(str[end])) --end;

    return str.substr(start, end - start + 1);
}

bool Validator::isValidName(const std::wstring& name) {
    if (name.empty()) return false;

    std::wstring trimmed = trim(name);
    if (trimmed.empty()) return false;

    if (trimmed.front() == L'-' || trimmed.back() == L'-') {
        return false;
    }

    if (trimmed.find(L"--") != std::wstring::npos) {
        return false;
    }

    wchar_t firstChar = trimmed.front();
    if (!::iswalpha(firstChar)) {
        return false;
    }

    for (wchar_t c : trimmed) {
        if (!::iswalnum(c) && c != L'-' && c != L' ' && c != L'.') {
            return false;
        }
    }

    return true;
}

bool Validator::isValidEmail(const std::wstring& email) {
    if (email.empty()) return false;

    std::wstring trimmed = trim(email);
    if (trimmed.empty()) return false;

    size_t atPos = trimmed.find(L'@');
    if (atPos == std::wstring::npos) return false;

    std::wstring user = trim(trimmed.substr(0, atPos));
    std::wstring domain = trim(trimmed.substr(atPos + 1));

    if (user.empty() || domain.empty()) return false;

    for (wchar_t c : user) {
        if (!::iswalnum(c) && c != L'.' && c != L'_' && c != L'-') {
            return false;
        }
    }

    for (wchar_t c : domain) {
        if (!::iswalnum(c) && c != L'.' && c != L'-') {
            return false;
        }
    }

    if (domain.find(L'.') == std::wstring::npos) return false;

    if (domain.front() == L'.' || domain.front() == L'-' ||
        domain.back() == L'.' || domain.back() == L'-') {
        return false;
    }

    return true;
}

bool Validator::isValidPhone(const std::wstring& phone) {
    if (phone.empty()) return false;

    std::wstring cleaned = phone;


    std::wstring patterns[] = {
        L"^\\+7\\d{10}$",                            
        L"^8\\d{10}$",                                 
        L"^\\+7\\(\\d{3}\\)\\d{7}$",                  
        L"^8\\(\\d{3}\\)\\d{7}$",                      
        L"^\\+7\\(\\d{3}\\)\\d{3}-\\d{2}-\\d{2}$",     
        L"^8\\(\\d{3}\\)\\d{3}-\\d{2}-\\d{2}$"         
    };

    for (const auto& pattern : patterns) {
        std::wregex re(pattern);
        if (std::regex_match(cleaned, re)) {
            return true;
        }
    }

    return false;
}

bool Validator::isValidDate(const std::wstring& dateStr) {
    if (dateStr.length() != 10) return false;

    if (dateStr[4] != L'-' || dateStr[7] != L'-') return false;

    int year, month, day;
    try {
        year = std::stoi(dateStr.substr(0, 4));
        month = std::stoi(dateStr.substr(5, 2));
        day = std::stoi(dateStr.substr(8, 2));
    }
    catch (...) {
        return false;
    }

    if (year < 1900 || year > 2025) return false;
    if (month < 1 || month > 12) return false;

    int daysInMonth[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    
    if (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) {
        daysInMonth[1] = 29;
    }

    if (day < 1 || day > daysInMonth[month - 1]) return false;

    return true;
}

bool Validator::isDateBeforeToday(const std::wstring& dateStr) {
    if (!isValidDate(dateStr)) return false;

    int year, month, day;
    year = std::stoi(dateStr.substr(0, 4));
    month = std::stoi(dateStr.substr(5, 2));
    day = std::stoi(dateStr.substr(8, 2));

    time_t now = time(nullptr);
    tm local_tm;
    localtime_s(&local_tm, &now);

    int todayYear = local_tm.tm_year + 1900;
    int todayMonth = local_tm.tm_mon + 1;
    int todayDay = local_tm.tm_mday;

    if (year < todayYear) return true;
    if (year > todayYear) return false;

    if (month < todayMonth) return true;
    if (month > todayMonth) return false;

    return day < todayDay;
}

bool Validator::isValidAddress(const std::wstring& addr) {
    
    return addr.empty() || !trim(addr).empty();
}