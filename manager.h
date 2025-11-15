#pragma once
#ifndef MANAGER_H
#define MANAGER_H

#include "contact.h"
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <locale>

class Manager {
private:
    std::vector<Contact> contacts;
    std::wstring filename = L"contact.txt";

    void saveToFile();
    void loadFromFile();

public:
    Manager();
    ~Manager();

    void addContact();
    void deleteContact(size_t index);
    void editContact(size_t index);
    void listContacts() const;
    void searchContacts();
    void sortContacts();

    size_t getContactCount() const { return contacts.size(); }
    Contact& getContact(size_t index) { return contacts[index]; }
    const Contact& getContact(size_t index) const { return contacts[index]; }
};

#endif