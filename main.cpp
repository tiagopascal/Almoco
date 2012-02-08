#include <QtGui/QApplication>
#include <QTextCodec>
#include "almoco.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qApp->addLibraryPath("/data/data/org.kde.necessitas.ministro/files/qt/plugins");

    QTextCodec::setCodecForCStrings( QTextCodec::codecForName("UTF-8") );

    Almoco w;
#if defined(Q_WS_S60)
    w.showMaximized();
#else
    w.show();
#endif

    return a.exec();
}
