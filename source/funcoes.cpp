#include "header/funcoes.h"

//Banco
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlRecord>
#include <QDebug>
//Internet
#include <QtNetwork/QNetworkAddressEntry>
#include <QtNetwork/QFtp>
#include <QtNetwork/QHostInfo>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

#include <QFileDialog>
#include <QFileInfo>
#include <QDir>
#include <QProcess>
#include <QSettings>
#include <QEventLoop>
#include <QTime>
#include <QTextStream>

#include "header/ConexaoBancoAtendimento.h"

QFtp *ftp;
QFile *arquivo;

QString Funcoes::NovaVersao()
{
    QString versao;
    static bool verificou = false;

    if( verificou )
        return QString();

    if( arquivo != 0 && !ftp->hasPendingCommands() )//Se fiver apontado para algum arquivo
    {
        if( arquivo->isOpen() )
            arquivo->close();

        QFile fArquivo( QCoreApplication::applicationDirPath() + "/versao.txt" );

        fArquivo.open( QIODevice::ReadOnly );

        QTextStream texto( &fArquivo );

        do
        {
            versao += texto.readLine();
        }while( !texto.atEnd() );

        if( ftp->errorString() != "Unknown error")
        {
            QMessageBox::information(0 , "", "Erro ao verificar por novas versões.\n"
                                     "\nRotina: Funcoes::NovaVersao()\nDescrição: "
                                     + ftp->errorString() );
        }
        /*
        else if(  ftp->currentCommand() != QFtp::None )
        {
            QMessageBox::information(0 , "", "Executando comando:\n"
                                     + QString::number( ftp->currentCommand() ) );
        }*/
//        else
//        {
//            QMessageBox::information(0 , "", "Stado ftp:\n"
//                                     + QString::number( ftp->state() ) );
//        }

        arquivo = 0;
        ftp = 0;
        verificou = true;

        delete ftp;
        delete arquivo;
    }

    QString v( versao );

    return v;
}

void Funcoes::TemNovaVersao()
{
    static bool verificou = false;

    if( !verificou )
    {
        ftp = new QFtp();

        arquivo = new QFile( QCoreApplication::applicationDirPath() + "/versao.txt");
        arquivo->open(QIODevice::WriteOnly);
        ftp->connectToHost("ftp.xpg.com.br");
        ftp->login("escrifonife1", "zero3um");
        //ftp->cd("Atendimento");
        ftp->get("versao.txt", arquivo);

        verificou = true;
    }

//    QFile *arquivo = new QFile("versao.txt");

//    if( !arquivo->open(QIODevice::WriteOnly) )
//        return false;

//    Ftp ftp;

//    ftp.conectarNoHost( "ftp.xpg.com.br" );
//    ftp.login("escrifonife1", "zero3um");
//    ftp.baixar( "versao.txt", arquivo );


//    QEventLoop loop;


//    ftp->connect( ftp, SIGNAL( readyRead() ), &loop, SLOT( quit() ) );
    //loop.processEvents( QEventLoop::AllEvents, 10000000 );
    //loop.exec();


//        QProgressDialog download;

//        ftp->connect( ftp, SIGNAL(done(bool)), &download, SLOT( done(int) ));
//        download.exec();

//    if( ftp->errorString() != "Unknown error")
//        QMessageBox::information(0 , "", "Descrição do erro:\n"
//                                 + ftp->errorString() );
//    else if(  ftp->currentCommand() != QFtp::None )
//    {
//        QMessageBox::information(0 , "", "Executando comando:\n"
//                                 + QString::number( ftp->currentCommand() ) );
//    }

//    delete ftp;
//    delete arquivo;
//    QTime horaFinal = QTime::currentTime();
//    QTime horaCorrente = horaFinal;

//    horaFinal = horaFinal.addSecs( 30 );

//    while( horaFinal > horaCorrente )
//    {
//        horaCorrente = QTime::currentTime();
//        QCoreApplication::processEvents();
//    }

//    QNetworkAccessManager *manager = new QNetworkAccessManager;
////    QUrl url;

////    url.setHost("ftp://ftp.trolltech.com/qt/source/INSTALL");
////    url.setUserName("escrifonife1");
////    url.setPassword("zero3um");
//    QString s ="ftp://ftp.trolltech.com/qt/source/m.txt";

//     s ="ftp://escrifonife1:zero3um@ftp.xpg.com.br/versao.txt";
//     s = "ftp://escrifonife1:zero3um@ftp.xpg.com.br/Atendimento/Dlls.zip";
//     //s = "ftp://ftp.trolltech.com/qt/source/INSTALL";
//    QUrl url( s );

//    //url.setAuthority("ftp://escrifonife1:zero3um@ftp.xpg.com.br/Atendimento/versao.txt");
//    QNetworkReply *reply = manager->get( QNetworkRequest( url ) );
//    QEventLoop loop;

//    reply->connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
//    loop.exec();

//    QString errosDownload = reply->errorString();

//    if( errosDownload != "Unknown error")
//        QMessageBox::information(0 , "", "Descrição do erro:\n"
//                                 + reply->errorString() );

//    QFile file( QCoreApplication::applicationDirPath() + "/versao.txt" );

//    file.open(QIODevice::WriteOnly);
//    file.write(reply->readAll());

//    delete reply;
//    delete manager;
}

void Funcoes::LimparPonteiros()
{
    if( arquivo != 0 )
    {
        arquivo = 0;
        delete arquivo;
    }

    if( ftp != 0)
    {
        ftp = 0;
        delete ftp;
    }
}

void Funcoes::RenomearArquivoAtualizacao()
{
    QString nomeArquivoNovo("FtpAplicativoNovo");
    QFile arquivoNovo( QCoreApplication::applicationDirPath() + "/" + nomeArquivoNovo + ".exe" );

    if( arquivoNovo.exists() )
    {
        QDir versoes( QCoreApplication::applicationDirPath() );
        QString pasta("VersoesAntigas");

        if( !versoes.exists( pasta ) )
        {
            versoes.mkdir( pasta );
        }

        QString nomeArquivoAntigo("FtpAplicativo");
        QFile arquivoAntigo( QCoreApplication::applicationDirPath() + "/"
                             + nomeArquivoAntigo + ".exe" );
        if( arquivoAntigo.exists() )
        {

            arquivoAntigo.copy( QCoreApplication::applicationDirPath()
                                + "/" + pasta + "/" + nomeArquivoAntigo + " "
                                + QDate::currentDate().toString("dd-MM-yyyy")
                                + " " + QTime::currentTime().toString("hh_mm_ss") + ".exe" );

            arquivoAntigo.remove();
        }

        arquivoNovo.rename( QCoreApplication::applicationDirPath()
                            + "/" + nomeArquivoAntigo + ".exe" );
    }
}

void Funcoes::copyFolder(QString sourceFolder, QString destFolder)
{
    QDir sourceDir(sourceFolder);
    if(!sourceDir.exists())
        return;
    QDir destDir(destFolder);
    if(!destDir.exists())
    {
        destDir.mkdir(destFolder);
    }
    QStringList files = sourceDir.entryList(QDir::Files);
    for(int i = 0; i< files.count(); i++)
    {
        QString srcName = sourceFolder + "/" + files[i];
        QString destName = destFolder + "/" + files[i];
        QFile::copy(srcName, destName);
    }
    files.clear();
    files = sourceDir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
    for(int i = 0; i< files.count(); i++)
    {
        QString srcName = sourceFolder + "/" + files[i];
        QString destName = destFolder + "/" + files[i];
        copyFolder(srcName, destName);
    }
}

void Funcoes::IconeAplicativo()
{
    if( QCoreApplication::applicationDirPath().contains("Cronograma", Qt::CaseInsensitive) )
        QApplication::setWindowIcon( QIcon( QPixmap(":image/verde.png") ) );
    else
        QApplication::setWindowIcon( QIcon( QPixmap(":image/iconePrincipal.png") ) );
}

void Funcoes::ErroSQL(QSqlQuery sql, QString rotina, QWidget *tela)
{
//    QMessageBox mensagem( tela );
//    QFont fonte;

//    fonte.setPointSize( 7 );
//    mensagem.setFont( fonte );

//    //mensagem.setParent( tela );
//    mensagem.setText( rotina );
//    mensagem.setInformativeText( "<br><br>Descrição:\n" + sql.lastError().text() + "</br></br>"
//                       "<br><b><br>SQL  " + sql.lastQuery() + "</br><b></br>");

//    mensagem.exec();
//    QMessageBox::warning( tela, "Erro SQL" ,rotina +
//                         + "<br><br>Descrição:\n" + sql.lastError().text() + "</br></br>"
//                          "<br><b><br>SQL  " + sql.lastQuery() + "</br><b></br>"
//                         , "ok");
    MensagemAndroid( "Erro SQL:\n" + rotina, "Descrição: " + sql.lastError().text() + "\nQuery: " + sql.lastQuery(), "Ok", ( QDialog *) tela  );
}

bool Funcoes::AtualizaSistema()
{
    QProcess *executavel = new QProcess( );
    QFile arquivo( QCoreApplication::applicationDirPath() + "/FtpAplicativo.exe");

    if( arquivo.exists() )
    {
        executavel->start( QCoreApplication::applicationDirPath() + "/FtpAplicativo.exe" );
    }
    else
    {
        QMessageBox::information( 0, "Atualização", "Programa de atualização não encontrado.", "&Ok" );

        return false;
    }

    return true;
}

QString Funcoes::SubstituirBarraInvertida(const QByteArray &string)
{
    QString tmpString;

    int qtd = string.size();
    tmpString = "";

    for( int i = 0; i < qtd ; ++i)
    {
        char caractere = string.at( i );

        switch( caractere )
        {
        case '\a':
            tmpString.append( "/a" );
            break;
        case '\b':
            tmpString.append( "/b" );
            break;
        case '\e':
            tmpString.append( "/e" );
            break;
        case '\f':
            tmpString.append( "/f" );
            break;
        case '\n':
            tmpString.append( "/n" );
            break;
        case '\r':
            tmpString.append( "/r" );
            break;
        case '\t':
            tmpString.append( "/t" );
            break;
        case '\v':
            tmpString.append( "/v" );
            break;
        default:
            tmpString.append( caractere );
        };
    }
    QString retornoString( tmpString );

    return retornoString;
}

//esta função centraliza o widget na tela
void Funcoes::centralizarWidget(QWidget *widget, bool deixarRedimensionar)
{
    QRect rect = QApplication::desktop()->availableGeometry(widget);
    widget->move(rect.center() - widget->rect().center());

    if( !deixarRedimensionar )
    {
        QRect tamanhoAtual =widget->geometry();

        widget->setMaximumHeight( tamanhoAtual.height() );
        widget->setMinimumHeight( tamanhoAtual.height() );
        widget->setMaximumWidth( tamanhoAtual.width() );
        widget->setMinimumWidth( tamanhoAtual.width() );
    }
}

//esta função centraliza o widget na tela
void Funcoes::centralizarWidget(QWidget &widget, bool deixarRedimensionar)
{
    QRect rect = QApplication::desktop()->availableGeometry( &widget );
    widget.move( rect.center() - widget.rect().center() );

    if( !deixarRedimensionar )
    {
        QRect tamanhoAtual = widget.geometry();

        widget.setMaximumHeight( tamanhoAtual.height() );
        widget.setMinimumHeight( tamanhoAtual.height() );
        widget.setMaximumWidth( tamanhoAtual.width() );
        widget.setMinimumWidth( tamanhoAtual.width() );
    }
}

QString Funcoes::SistemaOperacional()
{
    #ifdef Q_WS_X11
        return QString("Linux");
    #endif

    #ifdef Q_WS_MAC
        return QString("Mac");
    #endif

    #ifdef Q_WS_QWS
        return QString("Embedded Linux");
    #endif

    #ifdef Q_WS_WIN
        return QString("Windows");
    #endif
        return "";
}//Fim SistemaOperacional

QString Funcoes::FormataCodigo(QString codigo, int qtdZeros)
{
    QString novoCodigo;
    int qtdCaracterCodigo = qtdZeros - codigo.length();

    for(int i = 0; i < qtdCaracterCodigo; ++i)
    {
        novoCodigo+= "0";
    }

    novoCodigo+= codigo;
    return novoCodigo;
}

QDate Funcoes::FormataData(QString data, QString tipoBanco)
{
    QString dia, mes, ano;
    QDate novaData;
    if(tipoBanco == "QSQLITE")
    {
        dia = data.left(2);
        mes = data.mid(3, 2);
        ano = data.right(4);
        novaData.setDate(ano.toInt(), mes.toInt(), dia.toInt());
    }
    else if(tipoBanco == "QMYSQL")
    {
        dia = data.right(2);
        mes = data.mid(5, 2);
        ano = data.left(4);
        novaData.setDate(ano.toInt(), mes.toInt(), dia.toInt());
    }
    //QMessageBox::information(0, "data", "Dia: " + dia + " Mes: " + mes + " ano: " + ano);

    return novaData;
}

QString Funcoes::IP_LocalHost()
{

    QHostInfo info = QHostInfo::fromName( QHostInfo::localHostName() );
    QString retorno, IPs;
    QList<QHostAddress> l = info.addresses();
    int qtd = l.count();

    for(int i = 0; i < qtd; ++i)
    {
        QString tempIp = l[i].toString();
        if(tempIp != "127.0.0.1" && tempIp != "127.0.1.1")
        {
            IPs+= tempIp;
            break;
        }
    }
//    QNetworkAddressEntry endereco;
//    QHostAddress ip;
//    QNetworkInterface iface;
//    //iface = QNetworkInterface::interfaceFromName(  iface.name() );
//            QList<QNetworkAddressEntry> entries = iface.addressEntries();
//            if (!entries.isEmpty())
//            {
//                    QNetworkAddressEntry entry = entries.first();
//                    ip = entry.ip();
//            }

    retorno = IPs;
    return retorno;
}

QTime Funcoes::DiferencaEntreHoras(QTime hora1, QTime hora2)
{
    QTime diferenca(0, 0, 0);

    if( hora1 < hora2 )
        diferenca = diferenca.addSecs( hora1.secsTo( hora2 ) );

    return diferenca;
}

QString Funcoes::LeLog(QString caminhoArquivo, QString sessao, QString chave, QString valorDefault)
{
    QString valorChave;
    //Se conseguir criar o arquivo
    if( CriarArquivo(caminhoArquivo) )
    {
        QSettings arquivoINI(caminhoArquivo, QSettings::IniFormat);

        arquivoINI.beginGroup(sessao);
            valorChave = arquivoINI.value( chave, valorDefault ).toString();
        arquivoINI.endGroup();
    }

    QString retorno = valorChave;

    return retorno;
}

bool Funcoes::GravaLog(QString caminhoArquivo, QString sessao, QString chave, QString valor)
{
    //Se conseguir criar o arquivo
    if( CriarArquivo(caminhoArquivo) )
    {
        QSettings arquivoINI(caminhoArquivo, QSettings::IniFormat);

        if( chave.isEmpty() )
            chave = "chave" + QTime::currentTime().toString("hh_mm_ss_zzz");
        arquivoINI.beginGroup(sessao);
            arquivoINI.setValue(chave, valor);
        arquivoINI.endGroup();
        return true;
    }
    //Caso não consiga criar o arquivo
    return false;
}

bool Funcoes::CriarArquivo(QString caminhoArquivo)
{
    QFile criarArquivo(caminhoArquivo);

    if(!criarArquivo.exists())//Se o arquivo não existe
    {
        if(criarArquivo.open(QIODevice::WriteOnly)) //Cria o arquivo
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    //Caso não entrar no if o arquivo ja existe
    return true;
}

QString Funcoes::MontaQueryInsert(QString nomeTabela, const QVector<QVector<QString> > query)
{
    QString sql;

    int qtdCampos = query.size();

    sql = "INSERT INTO " + nomeTabela;

    QStringList campos;
    QStringList valores;

    for(int i = 0; i < qtdCampos; ++i)
    {
        campos.append( query[i][0] );
        valores.append( query[i][1] );
    }

    sql += " (" + campos.join(",") + ") values (" + valores.join(",") + ")";

    return sql;
}

QString Funcoes::MontaQueryUpdate(QString nomeTabela, QVector< QVector<QString> > query, QString condicao)
{
    QString sql;

    int qtdCampos = query.size();

    sql = "UPDATE " + nomeTabela + " SET ";

    QString valores;


    for(int i = 0; i < qtdCampos; ++i)
    {
        valores.append( query[i][0] );
        valores.append( " = " + query[i][1] + ", " );
    }

                            //Retira a ultima virgula
    valores = valores.left( valores.size() - 2);

    if( !condicao.isEmpty() )
        sql += valores + " WHERE " + condicao;
    else
        sql += valores;

    return sql;
}

void Funcoes::CarregaCombo(QComboBox &combo, QString tabela, QString campo, QString valorPrimeiroIndice
                  , QString campoItemData, bool limparComboAntes, QString condicao)
{
    QSqlQuery sql( ConexaoBanco::Banco() );
    QString consulta;
    bool ok;

    consulta = "SELECT " + campo + ( campoItemData.isEmpty() ? "" : ", " + campoItemData )
            + " FROM " + tabela + ( condicao.isEmpty() ? "" : " WHERE " + condicao ) ;
    ok = sql.exec( consulta );

    if( !ok )
    {
        ErroSQL( sql, "funcoes.CarregaCombo<br><b>" + consulta + "</b></br>");
        return ;
    }

    if( limparComboAntes )
        combo.clear();

    if( !valorPrimeiroIndice.isEmpty() )
    {
        combo.clear();
        combo.addItem( valorPrimeiroIndice );
    }

    if( campoItemData.isEmpty() )
    {
        while( sql.next() )
        {
            combo.addItem( sql.record().value( campo ).toString() );
        }
    }
    else
    {
        while( sql.next() )
        {
            combo.addItem( sql.record().value( campo ).toString() );
            combo.setItemData( combo.count() - 1, sql.record().value( campoItemData ) );
        }
    }
}

int Funcoes::PedeSegundo(int minuto)
{
    int m = minuto -10;//pega um emprestado, que na verdade são dez

    if(m > 0)
    {
        int retorno = m;//Na depuração, quando incosta o mouse encima da variavel que esta no return,
                        //o Qt não mostra seu valor, por isso foi criada outra variavel
        return retorno;
    }
    else
    {
        return 0;
    }
}

int Funcoes::PedeMinuto(int hora)
{
    int h = hora - 10;//pega um emprestado, que na verdade são dez

    if(h > 0)
    {
        int retorno = h;//Na depuração, quando incosta o mouse encima da variavel que esta no return,
                        //o Qt não mostra seu valor, por isso foi criada outra variavel
        return retorno;
    }
    else
    {
        return 0;
    }
}

QString Funcoes::CriptografarSenha(const QString senha)
{
    const QString AlfabetoeMaiusculoNumeros = "ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
    const QString AlfabetoeMinusculoNumeros = "abcdefghijklmnopqrstuvwxyz1234567890";
    const QString AlfabetoCriptMaiusculo = "DEFGHIJKLMNOPQRSTUVWXYZ1234567890ABC";
    const QString AlfabetoCriptMinusculo = "defghijklmnopqrstuvwxyz1234567890abc";
    QString senhaCriptografada = "";
    int contador = senha.size();


    for( int i = 0; i < contador; ++i)//Percorre a senha atual
    {
        int qtd = AlfabetoeMaiusculoNumeros.size();
        char caracter = senha.at( i ).toAscii();
        int posicao;
        bool bMaiusculo;

        posicao = -1;
        bMaiusculo = false;

        for(int j = 0; j < qtd; ++j )//Percorre cada posição que vai ser criptografada
        {
            char caracterMaiusculo = AlfabetoeMaiusculoNumeros.at( j ).toAscii();
            char caracterMinusculo = AlfabetoeMinusculoNumeros.at( j ).toAscii();

            if( caracter == caracterMaiusculo )
            {
                posicao = j;
                bMaiusculo = true;
                break;
            }
            else if( caracter == caracterMinusculo )
            {
                bMaiusculo = false;
                posicao = j;
                break;
            }
        }

        if( posicao > -1)//Se achou o caractere na posição que vai criptografar
        {
            if( bMaiusculo )
            {
                senhaCriptografada.append( AlfabetoCriptMaiusculo.at( posicao ) );
            }
            else
            {
                senhaCriptografada.append( AlfabetoCriptMinusculo.at( posicao ) );
            }
        }
        else
        {
            senhaCriptografada.append( senha.at( i ) );
        }
    }//for( int i = 0; i < contador; ++i)

    QString retorno = senhaCriptografada;

    return retorno;
}

QString Funcoes::MoedaParaNumero(QString sValor)
{
    QString numero( sValor );
    //Ex: 3.000,33 vai ficar 3000.33
    numero = numero.replace(".", "").replace(",", ".");

    return QString( numero );
}

QString Funcoes::NumeroParaMoeda(QString sValor)
{
    //QString sValor( QString::number( dValor, 'g', 8 ) );

    if( sValor.contains( "." ) && !sValor.contains( "," ) )
    {
        sValor = sValor.replace( ".", "," );
    }

    QString moeda;
    //Formata o numero com duas casas apos a virgula
    moeda = QString("%L1").arg( sValor.replace(".", "").replace(",", ".").toDouble(), 0, 'f', 2);

    //moeda = FormataCodigo( moeda, 5 );

    return QString( moeda );
}


int Funcoes::MensagemAndroid(QString texto, QString informativeTexto, QString botao1, QString botao2, QString botao3, QDialog *parent)
{
    QMessageBox box;

    //Coloca o titulo em italico e negrito
    if( !texto.contains( "<b>" ) )
        texto = "<i><b>" + texto + "</b></i>";

    box.setText( texto );
    box.setInformativeText( informativeTexto );
    box.addButton( botao1, QMessageBox::AcceptRole );

    if( !botao2.isEmpty() )
    {
        box.addButton( botao2, QMessageBox::AcceptRole );
    }

    if( !botao3.isEmpty() )
    {
        box.addButton( botao3, QMessageBox::AcceptRole );
    }


    //box.setDefaultButton(  );
    //box.setIconPixmap( QPixmap( ":imagens/Comida.png" ) );
    //box.setDetailedText( "Teste de testo detalhado\nmais teste\noutro\nblablabla" );
    box.setWindowModality( Qt::WindowModal );
    box.setWindowFlags( Qt::ToolTip );

    QString sStyleSheet;

    //sStyleSheet = "QDialog{background-image: url(:/imagens/imagens/madeira1.png);color: rgb(170, 170, 127); border: 1px solid black; border-style: solid; border-radius: 4px; font-size: 7pt;}; "
    if( parent )
    {
        sStyleSheet = "QDialog{" + parent->styleSheet() + "}"
                "QDialog{background-image: url(:/imagens/imagens/madeira1.png);color: rgb(170, 170, 127); border: 1px solid black; border-style: solid; border-radius: 4px; font-size: 7pt;}; ";
    }

    //        "QPushButton{background-image: url(:/imagens/imagens/madeira1.png);color: rgb(170, 170, 127); border: 1px solid black; border-style: solid; border-radius: 4px;font-size: 10pt;};";

    box.setStyleSheet( sStyleSheet );


    //box.setParent( parent );

    return box.exec();
}

int Funcoes::MensagemAndroid(QString texto, QString informativeTexto, QString botao1, QString botao2, QDialog *parent)
{
    return MensagemAndroid( texto, informativeTexto, botao1, botao2, QString("") , parent );
}

int Funcoes::MensagemAndroid(QString texto, QString informativeTexto, QString botao1, QDialog *parent)
{
    return MensagemAndroid( texto, informativeTexto, botao1, QString(""), QString("") , parent );
}
