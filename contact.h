#pragma once
#ifndef CONTACT_H
#define CONTACT_H

#include <QString>
#include <QDate>
#include <QVector>
#include <QMetaType>

enum class PhoneType {
    HOME,
    WORK,
    OFFICE
};

struct PhoneNumber {
    QString number;
    PhoneType type;

    PhoneNumber() = default;
    PhoneNumber(const QString& num, PhoneType t = PhoneType::HOME)
        : number(num), type(t) {
    }

    QString typeToString() const {
        switch (type) {
        case PhoneType::HOME: return "home";
        case PhoneType::WORK: return "work";
        case PhoneType::OFFICE: return "office";
        default: return "home";
        }
    }

    static PhoneType stringToType(const QString& typeStr) {
        if (typeStr == "home") return PhoneType::HOME;
        if (typeStr == "work") return PhoneType::WORK;
        if (typeStr == "office") return PhoneType::OFFICE;
        return PhoneType::HOME;
    }

    bool operator==(const PhoneNumber& other) const {
        return number == other.number && type == other.type;
    }
};

class Contact {
public:
    Contact() = default;
    Contact(const QString& fn, const QString& ln, const QString& p,
        const QString& addr, const QDate& bd, const QString& em,
        const QVector<PhoneNumber>& phs);

    QString getFirstName() const { return firstName; }
    void setFirstName(const QString& fn) { firstName = fn; }

    QString getLastName() const { return lastName; }
    void setLastName(const QString& ln) { lastName = ln; }

    QString getPatronymic() const { return patronymic; }
    void setPatronymic(const QString& p) { patronymic = p; }

    QString getAddress() const { return address; }
    void setAddress(const QString& addr) { address = addr; }

    QDate getBirthDate() const { return birthDate; }
    void setBirthDate(const QDate& bd) { birthDate = bd; }

    QString getEmail() const { return email; }
    void setEmail(const QString& em) { email = em; }

    QVector<PhoneNumber> getPhones() const { return phones; }
    void setPhones(const QVector<PhoneNumber>& phs) { phones = phs; }
    void addPhone(const PhoneNumber& phone) { phones.append(phone); }

    bool isValid() const;
    QString toString() const;
    static Contact fromString(const QString& line);

    bool operator==(const Contact& other) const;
    bool operator!=(const Contact& other) const { return !(*this == other); }

private:
    QString firstName;
    QString lastName;
    QString patronymic;
    QString address;
    QDate birthDate;
    QString email;
    QVector<PhoneNumber> phones;
};

Q_DECLARE_METATYPE(Contact)

#endif 