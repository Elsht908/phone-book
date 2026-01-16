#include "contactdialog.h"
#include "ui_contactdialog.h"
#include "qtvalidator.h"
#include <QMessageBox>
#include <QListWidgetItem>
#include <QToolTip>
#include <QMenu>        
#include <QAction>     
#include <QDebug>

ContactDialog::ContactDialog(const Contact& contact, int index, QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::ContactDialog())
    , contact(contact)
    , contactIndex(index) {
    ui->setupUi(this);

    setWindowTitle(index == -1 ? "Add Contact" : "Edit Contact");
    setupUI();
    loadContactData();

    connect(ui->firstNameEdit, &QLineEdit::textChanged, this, &ContactDialog::validateForm);
    connect(ui->lastNameEdit, &QLineEdit::textChanged, this, &ContactDialog::validateForm);
    connect(ui->emailEdit, &QLineEdit::textChanged, this, &ContactDialog::validateForm);
    connect(ui->phoneListWidget, &QListWidget::itemChanged, this, &ContactDialog::validateForm);
    connect(ui->birthDateEdit, &QDateEdit::dateChanged, this, &ContactDialog::validateForm);

    connect(ui->addPhoneButton, &QPushButton::clicked, this, &ContactDialog::onAddPhone);
    connect(ui->removePhoneButton, &QPushButton::clicked, this, &ContactDialog::onRemovePhone);

    // Кнопки OK/Cancel
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &ContactDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &ContactDialog::reject);

    // Включаем контекстное меню для списка телефонов
    ui->phoneListWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->phoneListWidget, &QWidget::customContextMenuRequested,
        this, &ContactDialog::showPhoneContextMenu);

    // Подключаем двойной клик для редактирования телефона - УПРОЩЕННАЯ ВЕРСИЯ
    connect(ui->phoneListWidget, &QListWidget::itemDoubleClicked,
        this, &ContactDialog::onEditPhone);

    // Изначальная валидация
    validateForm();
}

ContactDialog::~ContactDialog() {
    delete ui;
}

void ContactDialog::setupUI() {
    // Настройка DateEdit
    ui->birthDateEdit->setCalendarPopup(true);
    ui->birthDateEdit->setMaximumDate(QDate::currentDate().addDays(-1));
    ui->birthDateEdit->setMinimumDate(QDate(1900, 1, 1));
    ui->birthDateEdit->setDate(QDate::currentDate().addYears(-30));
    ui->birthDateEdit->setDisplayFormat("dd.MM.yyyy");

    // Настройка ComboBox для типов телефонов
    ui->phoneTypeCombo->addItem("Home", static_cast<int>(PhoneType::HOME));
    ui->phoneTypeCombo->addItem("Work", static_cast<int>(PhoneType::WORK));
    ui->phoneTypeCombo->addItem("Office", static_cast<int>(PhoneType::OFFICE));
}

void ContactDialog::loadContactData() {
    if (contactIndex != -1) {
        ui->firstNameEdit->setText(contact.getFirstName());
        ui->lastNameEdit->setText(contact.getLastName());
        ui->patronymicEdit->setText(contact.getPatronymic());
        ui->addressEdit->setText(contact.getAddress());
        ui->birthDateEdit->setDate(contact.getBirthDate());
        ui->emailEdit->setText(contact.getEmail());

        // Загрузка телефонов
        QVector<PhoneNumber> phones = contact.getPhones();
        for (const auto& phone : phones) {
            QString phoneText = QtValidator::formatPhoneForDisplay(phone.number);
            QString typeText;

            switch (phone.type) {
            case PhoneType::HOME: typeText = "Home"; break;
            case PhoneType::WORK: typeText = "Work"; break;
            case PhoneType::OFFICE: typeText = "Office"; break;
            }

            ui->phoneListWidget->addItem(phoneText + " (" + typeText + ")");
        }
    }
}

Contact ContactDialog::getContact() const {
    Contact result;

    result.setFirstName(QtValidator::normalizeName(ui->firstNameEdit->text()));
    result.setLastName(QtValidator::normalizeName(ui->lastNameEdit->text()));
    result.setPatronymic(QtValidator::normalizeName(ui->patronymicEdit->text()));
    result.setAddress(ui->addressEdit->text().trimmed());
    result.setBirthDate(ui->birthDateEdit->date());
    result.setEmail(QtValidator::normalizeEmail(ui->emailEdit->text()));

    // Сохраняем телефоны из списка - УПРОЩЕННАЯ ВЕРСИЯ
    QVector<PhoneNumber> phones;
    for (int i = 0; i < ui->phoneListWidget->count(); ++i) {
        QListWidgetItem* item = ui->phoneListWidget->item(i);
        QString itemText = item->text();

        // Отладочный вывод
        qDebug() << "Parsing phone item:" << itemText;

        // Формат: "+7(812)123-45-67 (Home)"
        // Находим последнюю открывающую скобку для типа
        int typeStart = itemText.lastIndexOf('(');
        int typeEnd = itemText.lastIndexOf(')');

        if (typeStart != -1 && typeEnd != -1 && typeEnd > typeStart) {
            QString phonePart = itemText.left(typeStart).trimmed();
            QString typePart = itemText.mid(typeStart + 1, typeEnd - typeStart - 1);

            qDebug() << "Phone part:" << phonePart << "Type part:" << typePart;

            PhoneType type = PhoneType::HOME;
            if (typePart == "Work") type = PhoneType::WORK;
            else if (typePart == "Office") type = PhoneType::OFFICE;

            QString normalized = QtValidator::normalizePhone(phonePart);
            qDebug() << "Normalized phone:" << normalized;

            phones.append(PhoneNumber(normalized, type));
        }
        else {
            qDebug() << "Failed to parse phone item:" << itemText;
        }
    }

    qDebug() << "Total phones parsed:" << phones.size();

    result.setPhones(phones);
    return result;
}

void ContactDialog::onAddPhone() {
    QString phoneNumber = ui->phoneEdit->text().trimmed();

    if (!QtValidator::isValidPhone(phoneNumber)) {
        QMessageBox::warning(this, "Invalid Phone Number",
            "Please enter a valid phone number in one of these formats:\n\n"
            "• +78121234567\n"
            "• 88121234567\n"
            "• +7(812)1234567\n"
            "• 8(812)1234567\n"
            "• +7(812)123-45-67\n"
            "• 8(812)123-45-67");
        return;
    }

    QString normalized = QtValidator::normalizePhone(phoneNumber);
    PhoneType type = static_cast<PhoneType>(ui->phoneTypeCombo->currentData().toInt());
    QString phoneText = QtValidator::formatPhoneForDisplay(normalized);
    QString typeText = ui->phoneTypeCombo->currentText();

    ui->phoneListWidget->addItem(phoneText + " (" + typeText + ")");
    ui->phoneEdit->clear();
    validateForm();
}

void ContactDialog::onRemovePhone() {
    int currentRow = ui->phoneListWidget->currentRow();
    if (currentRow >= 0) {
        delete ui->phoneListWidget->takeItem(currentRow);
        validateForm();
    }
}

void ContactDialog::onEditPhone() {
    int currentRow = ui->phoneListWidget->currentRow();
    if (currentRow >= 0) {
        editPhoneItem(currentRow);
    }
}

void ContactDialog::editPhoneItem(int row) {
    QListWidgetItem* item = ui->phoneListWidget->item(row);
    if (!item) return;

    QString itemText = item->text();
    QRegularExpression regex(R"((\+?\d[^\(]+\([^\)]+\)[^\s]+)\s+\(([^\)]+)\))");
    QRegularExpressionMatch match = regex.match(itemText);

    if (match.hasMatch()) {
        QString phoneNumber = match.captured(1);
        QString typeStr = match.captured(2);
        QString normalized = QtValidator::normalizePhone(phoneNumber);
        ui->phoneEdit->setText(QtValidator::formatPhoneForDisplay(normalized));

        int index = 0;
        if (typeStr == "Work") index = 1;
        else if (typeStr == "Office") index = 2;
        ui->phoneTypeCombo->setCurrentIndex(index);

        delete ui->phoneListWidget->takeItem(row);

        ui->phoneEdit->setFocus();
    }
}

void ContactDialog::showPhoneContextMenu(const QPoint& pos) {
    QMenu contextMenu("Phone menu", this);

    QAction* editAction = contextMenu.addAction("Edit");
    QAction* removeAction = contextMenu.addAction("Remove");

    // Проверяем, есть ли выбранный элемент
    QListWidgetItem* item = ui->phoneListWidget->itemAt(pos);

    if (!item) {
        editAction->setEnabled(false);
        removeAction->setEnabled(false);
    }

    QAction* selectedAction = contextMenu.exec(ui->phoneListWidget->mapToGlobal(pos));

    if (selectedAction == editAction && item) {
        int row = ui->phoneListWidget->row(item);
        editPhoneItem(row);
    }
    else if (selectedAction == removeAction && item) {
        int row = ui->phoneListWidget->row(item);
        delete ui->phoneListWidget->takeItem(row);
        validateForm();
    }
}

void ContactDialog::validateForm() {
    bool isValid = true;
    QString styleError = "border: 1px solid red;";
    QString styleNormal = "";

    // Проверка имени
    QString firstName = ui->firstNameEdit->text().trimmed();
    if (!QtValidator::isValidName(firstName) || firstName.isEmpty()) {
        ui->firstNameEdit->setStyleSheet(styleError);
        isValid = false;
    }
    else {
        ui->firstNameEdit->setStyleSheet(styleNormal);
    }

    // Проверка фамилии
    QString lastName = ui->lastNameEdit->text().trimmed();
    if (!QtValidator::isValidName(lastName) || lastName.isEmpty()) {
        ui->lastNameEdit->setStyleSheet(styleError);
        isValid = false;
    }
    else {
        ui->lastNameEdit->setStyleSheet(styleNormal);
    }

    // Проверка отчества
    QString patronymic = ui->patronymicEdit->text().trimmed();
    if (!patronymic.isEmpty() && !QtValidator::isValidName(patronymic)) {
        ui->patronymicEdit->setStyleSheet(styleError);
        isValid = false;
    }
    else {
        ui->patronymicEdit->setStyleSheet(styleNormal);
    }

    // Проверка email
    QString email = ui->emailEdit->text().trimmed();
    if (!QtValidator::isValidEmail(email) || email.isEmpty()) {
        ui->emailEdit->setStyleSheet(styleError);
        isValid = false;
    }
    else {
        ui->emailEdit->setStyleSheet(styleNormal);
    }

    // Проверка даты рождения
    QDate birthDate = ui->birthDateEdit->date();
    if (!QtValidator::isValidDate(birthDate)) {
        ui->birthDateEdit->setStyleSheet(styleError);
        isValid = false;

        QToolTip::showText(ui->birthDateEdit->mapToGlobal(QPoint(0, 0)),
            "Date must be valid and before today",
            ui->birthDateEdit);
    }
    else {
        ui->birthDateEdit->setStyleSheet(styleNormal);
        QToolTip::hideText();
    }

    // Проверка телефонов
    if (ui->phoneListWidget->count() == 0) {
        ui->phoneListWidget->setStyleSheet(styleError);
        isValid = false;
    }
    else {
        ui->phoneListWidget->setStyleSheet(styleNormal);
    }

    // Включаем/выключаем кнопку OK
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(isValid);
}

void ContactDialog::saveContactData() {
    contact.setFirstName(ui->firstNameEdit->text().trimmed());
    contact.setLastName(ui->lastNameEdit->text().trimmed());
    contact.setPatronymic(ui->patronymicEdit->text().trimmed());
    contact.setAddress(ui->addressEdit->text().trimmed());
    contact.setBirthDate(ui->birthDateEdit->date());
    contact.setEmail(ui->emailEdit->text().trimmed());
}