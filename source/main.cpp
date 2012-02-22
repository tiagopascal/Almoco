#include <QtGui/QApplication>
#include <QTextCodec>
#include <QIcon>

#include "header/almoco.h"
#include "header/menu.h"
#include "header/ConexaoBancoAtendimento.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //Para o sqlite funcionar.
    qApp->addLibraryPath("/data/data/org.kde.necessitas.ministro/files/qt/plugins");

    QTextCodec::setCodecForCStrings( QTextCodec::codecForName("UTF-8") );

    QApplication::setWindowIcon( QIcon( QPixmap(":imagens/Comida.png") ) );

//    QFont fonte;

//    fonte.setPointSize( 7 );

//    QApplication::setFont( fonte );

    //Almoco w;
    Menu w;

    w.setWindowModality( Qt::ApplicationModal );

#if defined(Q_WS_S60)
    w.showMaximized();
#else
    w.show();
#endif

    return a.exec();
}
