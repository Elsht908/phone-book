#include <QtWidgets/QApplication>
#include "phone_book_qt.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // Установка стиля приложения
    app.setStyle("Fusion");

    phone_book_qt window;
    window.show();

    return app.exec();
}