#include "manager.h"
#include <iostream>
#include <locale>
#include <codecvt>
#include <windows.h>

int main() {
  
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    setlocale(LC_ALL, "ru_RU.UTF-8");
    std::wcout.imbue(std::locale("ru_RU.UTF-8"));

    Manager manager;

    std::wstring choice;
    while (true) {
        std::wcout << L"\n=== ТЕЛEФОННЫЙ СПРАВОЧНИК ===\n";
        std::wcout << L"1. Добавить контакт\n";
        std::wcout << L"2. Просмотреть все контакты\n";
        std::wcout << L"3. Редактировать контакт\n";
        std::wcout << L"4. Удалить контакт\n";
        std::wcout << L"5. Поиск контакта\n";
        std::wcout << L"6. Сортировать контакты\n";
        std::wcout << L"7. Выход\n";
        std::wcout << L"Выберите действие: ";

        std::getline(std::wcin, choice);

        if (choice == L"1") {
            manager.addContact();
        }
        else if (choice == L"2") {
            manager.listContacts();
        }
        else if (choice == L"3") {
            std::wcout << L"Введите номер контакта для редактирования: ";
            std::wstring input;
            std::getline(std::wcin, input);
            try {
                size_t index = std::stoul(input) - 1;
                if (index < manager.getContactCount()) {
                    manager.editContact(index);
                }
                else {
                    std::wcout << L"Неверный номер.\n";
                }
            }
            catch (...) {
                std::wcout << L"Неверный ввод.\n";
            }
        }
        else if (choice == L"4") {
            std::wcout << L"Введите номер контакта для удаления: ";
            std::wstring input;
            std::getline(std::wcin, input);
            try {
                size_t index = std::stoul(input) - 1;
                if (index < manager.getContactCount()) {
                    manager.deleteContact(index);
                }
                else {
                    std::wcout << L"Неверный номер.\n";
                }
            }
            catch (...) {
                std::wcout << L"Неверный ввод.\n";
            }
        }
        else if (choice == L"5") {
            manager.searchContacts();
        }
        else if (choice == L"6") {
            manager.sortContacts();
        }
        else if (choice == L"7") {
            std::wcout << L"До свидания!\n";
            break;
        }
        else {
            std::wcout << L"Неверный выбор. Попробуйте снова.\n";
        }
    }

    return 0;
}