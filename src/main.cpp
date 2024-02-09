#include "demo_window.h"

#include <QApplication>
#pragma comment(lib, "user32.lib")

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    demo_window w;
    w.show();
    return a.exec();
}