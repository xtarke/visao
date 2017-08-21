#include "inmoov_qt.h"
#include <QApplication>

int main(int argc, char *argv[])
{    
    QApplication app(argc, argv);
    inmoov_qt w;
    w.show();

    return app.exec();
}

