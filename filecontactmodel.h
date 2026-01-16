#pragma once
#ifndef FILECONTACTMODEL_H
#define FILECONTACTMODEL_H

#include "contactmodel.h"
#include <QFile>
#include <QVector>
#include <QString>

class FileContactModel : public ContactModel {
    Q_OBJECT

public:
    explicit FileContactModel(const QString& filename = "contacts.txt", QObject* parent = nullptr);
    ~FileContactModel();

    // ContactModel interface
    bool addContact(const Contact& contact) override;
    bool deleteContact(int index) override;
    bool updateContact(int index, const Contact& contact) override;
    QVector<Contact> getAllContacts() const override;
    Contact getContact(int index) const override;
    int contactCount() const override;

    QVector<Contact> searchContacts(const QString& query) const override;
    void sortByField(const QString& field, bool ascending = true) override;

    // Файловые операции
    bool loadFromFile();
    bool saveToFile();

private:
    QVector<Contact> contacts;
    QString filename;
    mutable QVector<Contact> filteredContacts; // Для поиска/сортировки

    // Вспомогательные методы
    bool isValidIndex(int index) const;
    void updateFilteredList();
};

#endif // FILECONTACTMODEL_H