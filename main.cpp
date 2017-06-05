#include <QApplication>
#include "startup.h"
#include "Model/Tomasulo.h"
#include "Model/Tomasulo/Ins.h"

using namespace std;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Startup start_up;
    start_up.show();

    return a.exec();
}
