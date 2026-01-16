#pragma once
#include <QtWidgets/QMainWindow>
#include "contactmodel.h"
#include "filecontactmodel.h"
#include <QItemSelectionModel>
#include <QStandardItemModel>
#include <QStandardPaths>

namespace Ui {
    class phone_book_qtClass;
}

class phone_book_qt : public QMainWindow {
    Q_OBJECT

public:
    phone_book_qt(QWidget* parent = nullptr);
    ~phone_book_qt();

private slots:
    void onAddContact();
    void onEditContact();
    void onDeleteContact();
    void onSearchChanged();
    void onSortClicked();
    void refreshTable();
    void updateButtons();
    void showContactDetails(const QModelIndex& index);

private:
    Ui::phone_book_qtClass* ui;
    ContactModel* model;
    QStandardItemModel* tableModel;

    void setupTable();
    void loadContacts();
    QString getCurrentSortField() const;
};