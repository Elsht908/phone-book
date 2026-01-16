#include "phone_book_qt.h"
#include "ui_phone_book_qt.h"
#include "contactdialog.h"
#include <QMessageBox>
#include <QStandardItemModel>
#include <QHeaderView>
#include <QPushButton>
#include "qtvalidator.h"
#include <QDebug>
#include <QStandardPaths>


phone_book_qt::phone_book_qt(QWidget* parent)
    : QMainWindow(parent)
    , model(nullptr)
    , tableModel(new QStandardItemModel(this)) {

    ui = new Ui::phone_book_qtClass();
    ui->setupUi(this);

    setupTable();

    // Определяем путь к файлу контактов
    QString fileName = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)
        + "/contacts.txt";

    // Создаем модель
    FileContactModel* fileModel = new FileContactModel(fileName, this);
    model = fileModel;  // Присваиваем базовому указателю

    // Пытаемся загрузить контакты
    if (!fileModel->loadFromFile()) {
        QMessageBox::warning(this, "File Error",
            QString("Cannot load contacts from file '%1'.\n"
                "A new file will be created when you add contacts.")
            .arg(fileName));
    }
    else {
        loadContacts();
    }

    // ╧юфъы■іхэшх ёшуэрыют
    connect(ui->addButton, &QPushButton::clicked, this, &phone_book_qt::onAddContact);
    connect(ui->editButton, &QPushButton::clicked, this, &phone_book_qt::onEditContact);
    connect(ui->deleteButton, &QPushButton::clicked, this, &phone_book_qt::onDeleteContact);
    connect(ui->refreshButton, &QPushButton::clicked, this, &phone_book_qt::refreshTable);
    connect(ui->sortButton, &QPushButton::clicked, this, &phone_book_qt::onSortClicked);
    connect(ui->searchEdit, &QLineEdit::textChanged, this, &phone_book_qt::onSearchChanged);

    connect(ui->contactsTable->selectionModel(), &QItemSelectionModel::selectionChanged,
        this, &phone_book_qt::updateButtons);
    connect(ui->contactsTable, &QTableView::doubleClicked,
        this, &phone_book_qt::showContactDetails);

    // ╤шуэры√ юҐ ьюфхыш
    connect(model, &ContactModel::dataChanged, this, &phone_book_qt::loadContacts);

    updateButtons();
}
phone_book_qt::~phone_book_qt() {
    delete ui;
}

void phone_book_qt::setupTable() {
    tableModel->setColumnCount(7);
    tableModel->setHorizontalHeaderLabels({
        "Last Name", "First Name", "Patronymic", "Birth Date",
        "Email", "Address", "Phones"
        });

    ui->contactsTable->setModel(tableModel);
    ui->contactsTable->horizontalHeader()->setStretchLastSection(true);
    ui->contactsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->contactsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void phone_book_qt::loadContacts() {
    tableModel->removeRows(0, tableModel->rowCount());

    QVector<Contact> contacts = model->getAllContacts();

    for (const Contact& contact : contacts) {
        QList<QStandardItem*> row;

        row.append(new QStandardItem(contact.getLastName()));
        row.append(new QStandardItem(contact.getFirstName()));
        row.append(new QStandardItem(contact.getPatronymic()));
        row.append(new QStandardItem(contact.getBirthDate().toString("dd.MM.yyyy")));
        row.append(new QStandardItem(contact.getEmail()));
        row.append(new QStandardItem(contact.getAddress()));

        // Форматируем телефоны
        QString phonesStr;
        QVector<PhoneNumber> phones = contact.getPhones();
        for (const PhoneNumber& phone : phones) {
            if (!phonesStr.isEmpty()) phonesStr += "\n";
            phonesStr += QtValidator::formatPhoneForDisplay(phone.number) +
                " (" + phone.typeToString() + ")";
        }
        row.append(new QStandardItem(phonesStr));

        tableModel->appendRow(row);
    }
}

void phone_book_qt::onAddContact() {
    ContactDialog dialog(Contact(), -1, this);
    if (dialog.exec() == QDialog::Accepted) {
        Contact newContact = dialog.getContact();

        // Подробная проверка контакта
        if (!newContact.isValid()) {
            QString errorMsg = "Contact is not valid:\n";

            if (newContact.getFirstName().isEmpty()) errorMsg += "• First name is required\n";
            if (newContact.getLastName().isEmpty()) errorMsg += "• Last name is required\n";
            if (newContact.getEmail().isEmpty()) errorMsg += "• Email is required\n";
            else if (!QtValidator::isValidEmail(newContact.getEmail())) errorMsg += "• Email is invalid\n";

            if (newContact.getPhones().isEmpty()) errorMsg += "• At least one phone number is required\n";
            else {
                for (const PhoneNumber& phone : newContact.getPhones()) {
                    if (!QtValidator::isValidPhone(phone.number)) {
                        errorMsg += "• Phone number " + phone.number + " is invalid\n";
                    }
                }
            }

            QMessageBox::critical(this, "Invalid Contact", errorMsg);
            return;
        }

        if (model->addContact(newContact)) {
            QMessageBox::information(this, "Success", "Contact added successfully");
            loadContacts();
        }
        else {
            QMessageBox::warning(this, "Error",
                "Failed to add contact.\n"
                "Possible reasons:\n"
                "• Contact with this email already exists\n"
                "• Invalid contact data");
        }
    }
}

void phone_book_qt::onEditContact() {
    int row = ui->contactsTable->currentIndex().row();
    if (row >= 0 && row < model->contactCount()) {
        Contact contact = model->getContact(row);
        ContactDialog dialog(contact, row, this);
        if (dialog.exec() == QDialog::Accepted) {
            Contact updatedContact = dialog.getContact();

            // ПРОВЕРЯЕМ валидность контакта
            if (!updatedContact.isValid()) {
                QString errorMsg = "Contact is not valid:\n";

                if (updatedContact.getFirstName().isEmpty()) errorMsg += "• First name is required\n";
                if (updatedContact.getLastName().isEmpty()) errorMsg += "• Last name is required\n";
                if (updatedContact.getEmail().isEmpty()) errorMsg += "• Email is required\n";
                else if (!QtValidator::isValidEmail(updatedContact.getEmail())) errorMsg += "• Email is invalid\n";

                if (updatedContact.getPhones().isEmpty()) errorMsg += "• At least one phone number is required\n";
                else {
                    for (const PhoneNumber& phone : updatedContact.getPhones()) {
                        if (!QtValidator::isValidPhone(phone.number)) {
                            errorMsg += "• Phone number " + phone.number + " is invalid\n";
                        }
                    }
                }

                // Проверка даты рождения
                if (!QtValidator::isValidDate(updatedContact.getBirthDate())) {
                    errorMsg += "• Invalid birth date\n";
                }

                QMessageBox::critical(this, "Invalid Contact", errorMsg);
                return;
            }

            if (model->updateContact(row, updatedContact)) {
                QMessageBox::information(this, "Success", "Contact updated successfully");
                loadContacts();
            }
            else {
                QMessageBox::warning(this, "Error",
                    "Failed to update contact.\n"
                    "Possible reasons:\n"
                    "• Contact with this email already exists\n"
                    "• Invalid contact data");
            }
        }
    }
}

void phone_book_qt::onDeleteContact() {
    int row = ui->contactsTable->currentIndex().row();
    if (row >= 0 && row < model->contactCount()) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Confirmation",
            "Are you sure you want to delete this contact?",
            QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            if (model->deleteContact(row)) {
                QMessageBox::information(this, "Success", "Contact deleted");
                loadContacts();
            }
            else {
                QMessageBox::warning(this, "Error", "Failed to delete contact");
            }
        }
    }
}

void phone_book_qt::onSearchChanged() {
    QString query = ui->searchEdit->text();
    model->searchContacts(query);
    loadContacts();
}

void phone_book_qt::onSortClicked() {
    QString field = getCurrentSortField();
    bool ascending = true;
    model->sortByField(field, ascending);
    loadContacts();
}

void phone_book_qt::refreshTable() {
    loadContacts();
}

void phone_book_qt::updateButtons() {
    bool hasSelection = ui->contactsTable->selectionModel()->hasSelection();
    ui->editButton->setEnabled(hasSelection);
    ui->deleteButton->setEnabled(hasSelection);
}

void phone_book_qt::showContactDetails(const QModelIndex& index) {
    // Получаем номер строки из индекса
    int row = index.row();
    if (row >= 0 && row < model->contactCount()) {
        onEditContact();  // Вызываем редактирование
    }
}

QString phone_book_qt::getCurrentSortField() const {
    switch (ui->sortCombo->currentIndex()) {
    case 0: return "lastName";
    case 1: return "firstName";
    case 2: return "patronymic";    // ДОБАВЛЕНО
    case 3: return "birthDate";
    case 4: return "email";
    case 5: return "address";       // ДОБАВЛЕНО
    case 6: return "phone";         // ДОБАВЛЕНО
    default: return "lastName";
    }
}