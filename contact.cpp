#include "contact.h"
#include <QStringList>
#include <QDebug>
#include "qtvalidator.h"


Contact::Contact(const QString& fn, const QString& ln, const QString& p,
    const QString& addr, const QDate& bd, const QString& em,
    const QVector<PhoneNumber>& phs)
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
        phones == other.phones;
}

QString Contact::toString() const {
    QString result = firstName + ";" + lastName + ";" + patronymic + ";" +
        address + ";" + birthDate.toString(Qt::ISODate) + ";" + email + ";";

    QStringList phoneEntries;
    for (const auto& phone : phones) {
        phoneEntries.append(phone.number + ":" + phone.typeToString());
    }

    result += phoneEntries.join(",");
    return result;
}

Contact Contact::fromString(const QString& line) {
    Contact contact;
    QStringList tokens = line.split(';', Qt::KeepEmptyParts);

    if (tokens.size() < 6) return contact;

    // НОРМАЛИЗУЕМ имена при загрузке из файла
    contact.setFirstName(QtValidator::normalizeName(tokens[0]));
    contact.setLastName(QtValidator::normalizeName(tokens[1]));
    contact.setPatronymic(QtValidator::normalizeName(tokens[2]));
    contact.setAddress(tokens[3]);
    contact.setBirthDate(QDate::fromString(tokens[4], Qt::ISODate));
    contact.setEmail(QtValidator::normalizeEmail(tokens[5]));

    if (tokens.size() > 6 && !tokens[6].isEmpty()) {
        // ИСПРАВЛЕНО: используем QString::SkipEmptyParts
        QStringList phoneTokens = tokens[6].split(',', QString::SkipEmptyParts);
        for (const auto& phoneEntry : phoneTokens) {
            QStringList parts = phoneEntry.split(':');
            if (parts.size() == 2) {
                contact.addPhone(PhoneNumber(parts[0], PhoneNumber::stringToType(parts[1])));
            }
        }
    }

    return contact;
}

bool Contact::isValid() const {
    // Проверяем обязательные поля
    if (firstName.isEmpty() || lastName.isEmpty() || email.isEmpty()) {
        return false;
    }

    // Проверяем email через QtValidator
    if (!QtValidator::isValidEmail(email)) {
        return false;
    }

    // Проверяем имена через QtValidator
    if (!QtValidator::isValidName(firstName) || !QtValidator::isValidName(lastName)) {
        return false;
    }

    // Отчество может быть пустым, но если не пустое - должно быть валидным
    if (!patronymic.isEmpty() && !QtValidator::isValidName(patronymic)) {
        return false;
    }

    // Проверяем дату рождения (с улучшенной валидацией)
    if (!QtValidator::isValidDate(birthDate)) {
        return false;
    }

    // Должен быть хотя бы один телефон
    if (phones.isEmpty()) {
        return false;
    }

    // Проверяем все телефоны
    for (const PhoneNumber& phone : phones) {
        if (!QtValidator::isValidPhone(phone.number)) {
            return false;
        }
    }

    return true;
}