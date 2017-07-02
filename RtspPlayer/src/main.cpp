#include "PlayerView.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CPlayerView w;
    w.show();
    return a.exec();
}
