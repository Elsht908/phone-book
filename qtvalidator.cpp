#include "qtvalidator.h"
#include <QRegularExpression>
#include <QDebug>

// Инициализация статических переменных

// Регулярное выражение для имени/фамилии/отчества
QRegularExpression QtValidator::nameRegex(R"(^[A-Za-z][A-Za-z0-9]*(?:[\s-][A-Za-z0-9]+)*$)");

// Регулярное выражение для телефона (все форматы из ТЗ)
QRegularExpression QtValidator::phoneRegex(R"(^(\+7|8)[\s\-]?\(?\d{3}\)?[\s\-]?\d{3}[\s\-]?\d{2}[\s\-]?\d{2}$)");

// Регулярное выражение для нормализованного телефона
QRegularExpression QtValidator::normalizedPhoneRegex(R"(^\+7\d{10}$)");

// Регулярное выражение для email
QRegularExpression QtValidator::emailRegex(R"(^[A-Za-z0-9]+@[A-Za-z0-9]+(\.[A-Za-z0-9]+)*$)");

// Реализация методов

bool QtValidator::isValidName(const QString& name) {
    QString trimmed = name.trimmed();

    if (trimmed.isEmpty()) {
        return true; // Пустое имя допустимо для необязательных полей
    }

    QRegularExpressionMatch match = nameRegex.match(trimmed);
    if (!match.hasMatch()) {
        return false;
    }

    // Дополнительные проверки
    if (trimmed.startsWith('-') || trimmed.endsWith('-')) {
        return false;
    }

    if (trimmed.contains("--") || trimmed.contains("  ")) {
        return false;
    }

    return true;
}

QString QtValidator::normalizeName(const QString& name) {
    QString result = name.trimmed();

    // Удаляем лишние пробелы (оставляем только один пробел между словами)
    result.replace(QRegularExpression(R"(\s+)"), " ");

    // Удаляем пробелы вокруг дефисов
    result.replace(QRegularExpression(R"(\s*-\s*)"), "-");

    return result;
}

bool QtValidator::isValidPhone(const QString& phone) {
    QString trimmed = phone.trimmed();

    if (trimmed.isEmpty()) {
        return false;
    }

    // Проверяем базовое регулярное выражение
    QRegularExpressionMatch match = phoneRegex.match(trimmed);
    if (!match.hasMatch()) {
        return false;
    }

    // Дополнительная проверка после нормализации
    QString normalized = normalizePhone(trimmed);

    // Проверяем длину: +7 + 10 цифр = 12 символов
    if (normalized.length() != 12) {
        return false;
    }

    // Проверяем формат: начинается с +7
    if (!normalized.startsWith("+7")) {
        return false;
    }

    // Проверяем, что все символы после +7 - цифры
    for (int i = 2; i < normalized.length(); ++i) {
        if (!normalized[i].isDigit()) {
            return false;
        }
    }

    return true;
}

QString QtValidator::normalizePhone(const QString& phone) {
    QString result = phone.trimmed();

    // Удаляем все нецифровые символы, кроме + в начале
    if (result.startsWith('+')) {
        QString plus = "+";
        result.remove(0, 1); // Удаляем +
        result.remove(QRegularExpression(R"([^\d])")); // Удаляем всё, кроме цифр
        result = plus + result; // Возвращаем + в начало
    }
    else {
        result.remove(QRegularExpression(R"([^\d])")); // Удаляем всё, кроме цифр
    }

    // Если номер начинается с 8, заменяем на +7
    if (result.startsWith("8")) {
        result = "+7" + result.mid(1);
    }
    // Если номер начинается с 7 (без +), добавляем +
    else if (result.startsWith("7") && !result.startsWith("+7")) {
        result = "+" + result;
    }
    // Если номер не начинается с +7, добавляем
    else if (!result.startsWith("+7")) {
        result = "+7" + result;
    }

    // Проверяем длину: +7 (2 символа) + 10 цифр = 12 символов
    if (result.length() < 12) {
        // Добавляем нули в конце если номер короткий
        while (result.length() < 12) {
            result += "0";
        }
    }
    else if (result.length() > 12) {
        // Обрезаем если номер длинный
        result = result.left(12);
    }

    return result;
}

QString QtValidator::formatPhoneForDisplay(const QString& phone) {
    QString normalized = normalizePhone(phone);

    // Проверяем, что номер в правильном формате
    if (normalized.length() == 12 && normalized.startsWith("+7")) {
        QString code = normalized.mid(2, 3);
        QString part1 = normalized.mid(5, 3);
        QString part2 = normalized.mid(8, 2);
        QString part3 = normalized.mid(10, 2);

        return QString("+7(%1)%2-%3-%4").arg(code).arg(part1).arg(part2).arg(part3);
    }

    // Если не удалось отформатировать, возвращаем исходный номер
    return phone;
}

bool QtValidator::isValidEmail(const QString& email) {
    QString normalized = normalizeEmail(email);
    QRegularExpressionMatch match = emailRegex.match(normalized);

    if (!match.hasMatch()) {
        return false;
    }

    // Дополнительная проверка: после последней точки должно быть минимум 2 символа
    QStringList parts = normalized.split('@');
    if (parts.size() != 2) return false;

    QString domain = parts[1];
    QStringList domainParts = domain.split('.');
    if (domainParts.isEmpty()) return false;

    QString lastPart = domainParts.last();
    if (lastPart.length() < 2) {
        return false;
    }

    return true;
}

QString QtValidator::normalizeEmail(const QString& email) {
    QString result = email.trimmed();

    // Удаляем ВСЕ пробелы в строке email
    result.remove(QRegularExpression(R"(\s+)"));

    // Приводим к нижнему регистру
    result = result.toLower();

    return result;
}

bool QtValidator::isValidDate(const QDate& date) {
    // 1. Проверяем, что дата валидна (включая проверку дней в месяце)
    if (!date.isValid()) {
        return false;
    }

    // 2. Проверяем, что дата не в будущем
    if (date >= QDate::currentDate()) {
        return false;
    }

    // 3. Дополнительные проверки (хотя QDate уже делает их)
    if (date.year() < 1900 || date.year() > QDate::currentDate().year()) {
        return false;  // Ограничиваем диапазон лет
    }

    // 4. Явная проверка дней в месяце (для соответствия ТЗ)
    int day = date.day();
    int month = date.month();
    int year = date.year();

    if (month < 1 || month > 12) {
        return false;
    }

    if (day < 1 || day > date.daysInMonth()) {
        return false;
    }

    // 5. Проверяем, что это реальная дата, а не 31 февраля и т.п.
    QDate checkDate(year, month, day);
    if (!checkDate.isValid() || checkDate != date) {
        return false;
    }

    return true;
}

// Опциональная тестовая функция
void QtValidator::testPhoneFormats() {
    QStringList testNumbers = {
        "+78121234567",
        "88121234567",
        "+7(812)1234567",
        "8(812)1234567",
        "+7(812)123-45-67",
        "8(812)123-45-67",
        "+7 812 123 45 67",
        "8-812-123-45-67"
    };

    QStringList invalidNumbers = {
        "8121234567",
        "+7123",
        "+7(812)123-45-678",
        "+7(81)123-45-67",
        "abc"
    };

    qDebug() << "=== Testing VALID phone numbers ===";
    for (const QString& number : testNumbers) {
        bool valid = isValidPhone(number);
        QString normalized = normalizePhone(number);
        QString formatted = formatPhoneForDisplay(number);

        qDebug() << number << "-> Valid:" << valid
            << "Normalized:" << normalized
            << "Formatted:" << formatted;
    }

    qDebug() << "\n=== Testing INVALID phone numbers ===";
    for (const QString& number : invalidNumbers) {
        bool valid = isValidPhone(number);
        qDebug() << number << "-> Valid:" << valid;
    }
}