#pragma once
#ifndef QTVALIDATOR_H
#define QTVALIDATOR_H

#include <QString>
#include <QDate>
#include <QRegularExpression>

class QtValidator {
public:
    // Валидация
    static bool isValidName(const QString& name);
    static bool isValidPhone(const QString& phone);
    static bool isValidEmail(const QString& email);
    static bool isValidDate(const QDate& date);

    // Нормализация
    static QString normalizeName(const QString& name);
    static QString normalizePhone(const QString& phone);
    static QString normalizeEmail(const QString& email);

    // Форматирование для отображения
    static QString formatPhoneForDisplay(const QString& phone);

    // Тестирование (опционально)
    static void testPhoneFormats();

private:
    static QRegularExpression nameRegex;
    static QRegularExpression phoneRegex;
    static QRegularExpression normalizedPhoneRegex;
    static QRegularExpression emailRegex;
};

#endif 