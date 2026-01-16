#pragma once
#ifndef CONTACTDIALOG_H
#define CONTACTDIALOG_H

#include <QDialog>
#include "contact.h"

namespace Ui {
    class ContactDialog;
}

class ContactDialog : public QDialog {
    Q_OBJECT

public:
    explicit ContactDialog(const Contact& contact = Contact(), int index = -1, QWidget* parent = nullptr);
    ~ContactDialog();

    Contact getContact() const;

private slots:
    void onAddPhone();
    void onRemovePhone();
    void validateForm();
    void onEditPhone();
    void showPhoneContextMenu(const QPoint& pos);  // днаюбкемн

private:
    Ui::ContactDialog* ui;
    Contact contact;
    int contactIndex;

    void setupUI();
    void loadContactData();
    void saveContactData();
    void editPhoneItem(int row);  // днаюбкемн
};

#endif // CONTACTDIALOG_H