#include "filecontactmodel.h"
#include <QTextStream>
#include <QRegularExpression>
#include <algorithm>
#include <QDebug>
#include "qtvalidator.h" 

FileContactModel::FileContactModel(const QString& filename, QObject* parent)
    : ContactModel(parent), filename(filename) {
    loadFromFile();
}

FileContactModel::~FileContactModel() {
    saveToFile();
}

bool FileContactModel::addContact(const Contact& contact) {
    // ПРОВЕРЯЕМ валидность контакта перед добавлением
    if (!contact.isValid()) {
        qDebug() << "Cannot add invalid contact";
        return false;
    }

    // Проверяем дубликаты по email
    QString newEmail = contact.getEmail().toLower();
    for (const Contact& existing : contacts) {
        if (existing.getEmail().toLower() == newEmail) {
            qDebug() << "Contact with email" << contact.getEmail() << "already exists";
            return false;
        }
    }

    contacts.append(contact);
    updateFilteredList();
    saveToFile();
    emit contactAdded(contacts.size() - 1);
    emit dataChanged();
    return true;
}


bool FileContactModel::deleteContact(int index) {
    if (!isValidIndex(index)) {
        return false;
    }

    contacts.removeAt(index);
    updateFilteredList();
    saveToFile();
    emit contactRemoved(index);
    emit dataChanged();
    return true;
}

bool FileContactModel::updateContact(int index, const Contact& contact) {
    if (!isValidIndex(index) || !contact.isValid()) {
        return false;
    }

    // Проверяем, не дублирует ли email другого контакта (кроме текущего)
    QString newEmail = contact.getEmail().toLower();  // Используем toLower вместо normalizeEmail
    for (int i = 0; i < contacts.size(); ++i) {
        if (i != index && contacts[i].getEmail().toLower() == newEmail) {
            qDebug() << "Email" << newEmail << "already used by another contact";
            return false;
        }
    }

    contacts[index] = contact;
    updateFilteredList();
    saveToFile();
    emit contactUpdated(index);
    emit dataChanged();
    return true;
}

QVector<Contact> FileContactModel::getAllContacts() const {
    return filteredContacts.isEmpty() ? contacts : filteredContacts;
}

Contact FileContactModel::getContact(int index) const {
    if (isValidIndex(index)) {
        return contacts.at(index);
    }
    return Contact();
}

int FileContactModel::contactCount() const {
    return filteredContacts.isEmpty() ? contacts.size() : filteredContacts.size();
}

bool FileContactModel::loadFromFile() {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream in(&file);
    in.setCodec("UTF-8");

    contacts.clear();
    int loadedCount = 0;
    int invalidCount = 0;

    while (!in.atEnd()) {
        QString line = in.readLine();

        if (!line.isEmpty() && line[0] == QChar(0xFEFF)) {
            line = line.mid(1);
        }

        Contact contact = Contact::fromString(line);
        if (contact.isValid()) {
            contacts.append(contact);
            loadedCount++;
        }
        else {
            invalidCount++;
            qDebug() << "Invalid contact skipped:" << line;
        }
    }

    file.close();
    updateFilteredList();

    if (invalidCount > 0) {
        qDebug() << "Loaded" << loadedCount << "contacts," << invalidCount << "invalid contacts skipped";
    }

    return true;
}

bool FileContactModel::saveToFile() {
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream out(&file);
    out.setCodec("UTF-8");
    out.setGenerateByteOrderMark(true);

    for (const auto& contact : contacts) {
        out << contact.toString() << "\n";
    }

    file.close();
    return true;
}

QVector<Contact> FileContactModel::searchContacts(const QString& query) const {
    if (query.isEmpty()) {
        const_cast<FileContactModel*>(this)->filteredContacts.clear();
        return contacts;
    }

    QVector<Contact> result;
    QString lowerQuery = query.toLower();

    for (const auto& contact : contacts) {
        if (contact.getFirstName().toLower().contains(lowerQuery) ||
            contact.getLastName().toLower().contains(lowerQuery) ||
            contact.getPatronymic().toLower().contains(lowerQuery) ||
            contact.getEmail().toLower().contains(lowerQuery) ||
            contact.getAddress().toLower().contains(lowerQuery)) {
            result.append(contact);
        }
    }

    const_cast<FileContactModel*>(this)->filteredContacts = result;
    return result;
}

void FileContactModel::sortByField(const QString& field, bool ascending) {
    auto sortFunc = [field, ascending](const Contact& a, const Contact& b) {
        auto getValue = [field](const Contact& c) -> QVariant {
            if (field == "firstName") return c.getFirstName();
            else if (field == "lastName") return c.getLastName();
            else if (field == "patronymic") return c.getPatronymic();
            else if (field == "birthDate") return c.getBirthDate();
            else if (field == "email") return c.getEmail();
            else if (field == "address") return c.getAddress();
            else if (field == "phone") {
                return c.getPhones().isEmpty() ? "" : c.getPhones().first().number;
            }
            return c.getLastName();
            };

        QVariant valueA = getValue(a);
        QVariant valueB = getValue(b);

        // Обработка пустых значений
        bool aIsEmpty = valueA.toString().isEmpty();
        bool bIsEmpty = valueB.toString().isEmpty();

        if (aIsEmpty && !bIsEmpty) return false; // Пустые в конец при ascending
        if (!aIsEmpty && bIsEmpty) return true;  // Непустые в начало при ascending

        if (aIsEmpty && bIsEmpty) return false; // Оба пустые - порядок неважен

        // Для дат
        if (field == "birthDate") {
            bool lessThan = valueA.toDate() < valueB.toDate();
            return ascending ? lessThan : !lessThan;
        }

        // Для строк
        bool lessThan = valueA.toString().compare(valueB.toString(), Qt::CaseInsensitive) < 0;
        return ascending ? lessThan : !lessThan;
        };

    if (filteredContacts.isEmpty()) {
        std::sort(contacts.begin(), contacts.end(), sortFunc);
    }
    else {
        std::sort(filteredContacts.begin(), filteredContacts.end(), sortFunc);
    }

    emit dataChanged();
}
     

bool FileContactModel::isValidIndex(int index) const {
    return index >= 0 && index < contacts.size();
}

void FileContactModel::updateFilteredList() {
    filteredContacts.clear();
}