#include <QtGui/QApplication>
#include <QTextCodec>
#include <QIcon>

#include "header/almoco.h"
#include "header/menu.h"
#include "header/ConexaoBancoAtendimento.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //versão 1.1.0
    //QTextCodec::setCodecForCStrings( QTextCodec::codecForName("ISO 8859-1") );
    //QTextCodec::setCodecForCStrings( QTextCodec::codecForName("UTF-8") );

    //Para o sqlite funcionar.
    qApp->addLibraryPath("/data/data/org.kde.necessitas.ministro/files/qt/plugins");

    //QApplication::setWindowIcon( QIcon( QPixmap(":imagens/Comida.png") ) );

    Menu w;

    w.setWindowModality( Qt::ApplicationModal );

#if defined(Q_WS_S60)
    w.showMaximized();
#else
    w.show();
#endif

    return a.exec();
}
