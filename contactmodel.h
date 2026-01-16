#pragma once
#ifndef CONTACTMODEL_H
#define CONTACTMODEL_H

#include "contact.h"
#include <QObject>
#include <QVector>

class ContactModel : public QObject {
    Q_OBJECT

public:
    ContactModel(QObject* parent = nullptr) : QObject(parent) {}
    virtual ~ContactModel() = default;

    // Основные операции
    virtual bool addContact(const Contact& contact) = 0;
    virtual bool deleteContact(int index) = 0;
    virtual bool updateContact(int index, const Contact& contact) = 0;
    virtual QVector<Contact> getAllContacts() const = 0;
    virtual Contact getContact(int index) const = 0;
    virtual int contactCount() const = 0;

    // Поиск и сортировка
    virtual QVector<Contact> searchContacts(const QString& query) const = 0;
    virtual void sortByField(const QString& field, bool ascending = true) = 0;

signals:
    void dataChanged();
    void contactAdded(int index);
    void contactRemoved(int index);
    void contactUpdated(int index);
};

#endif // CONTACTMODEL_H