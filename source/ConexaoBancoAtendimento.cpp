#include "header/ConexaoBancoAtendimento.h"
#include "header/funcoes.h"

#include <QString>
#include <QCoreApplication>
#include <QSettings>
//Banco
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlRecord>

#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QLineEdit>
#include <QSqlError>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QList>
#include <QHostInfo>
#include <QProgressBar>

QSqlDatabase db;
const QString tituloMensagem = "Atendimento";

QString ConexaoBanco::MontaQueryUpdate(const QString &nomeTabela, const QMap<QString, QString> &query, QString condicao)
{
    QString sql;

    sql = "UPDATE " + nomeTabela + " SET ";

    QString valores;

    QMap<QString, QString>::const_iterator i = query.constBegin();

    while( i != query.constEnd() )
    {
        valores.append( i.key() );
        valores.append( " = " + i.value() + ", " );
        ++i;
    }
                            //Retira a ultima virgula
    valores = valores.left( valores.size() - 2);

    if( !condicao.isEmpty() )
        sql += valores + " WHERE " + condicao;
    else
        sql += valores;

    return sql;
}

QString ConexaoBanco::MontaQueryInsert(const QString &nomeTabela, const QMap<QString, QString> &query)
{
    QString sql;

    sql = "INSERT INTO " + nomeTabela;

    QStringList campos;
    QStringList valores;

    QMap<QString, QString>::const_iterator i = query.constBegin();

    while( i != query.constEnd() )
    {
        campos.append( i.key() );
        valores.append( i.value() );
        ++i;
    }

    sql += " (" + campos.join(",") + ") values (" + valores.join(",") + ")";

    return sql;
}

void ConexaoBanco::CriaUsuarioPadrao()
{
    QSqlQuery sql( db );
    QString consulta;
    bool ok;

    if( db.driverName() == "QSQLITE" )
        ok = sql.exec("CREATE TABLE IF NOT EXISTS usuario( usuario_codigo integer primary key autoincrement, usuario_nome varchar(50), usuario_senha varchar(50) )");
    else
        ok = sql.exec("CREATE TABLE IF NOT EXISTS usuario( usuario_codigo integer primary key auto_increment, usuario_nome varchar(50), usuario_senha varchar(50) )");

    ok = sql.exec("SELECT * FROM usuario LIMIT 1");

    if( ok )
    {   //Se não tiver algum usuário cadastrado, cadastra um: usuario root, senha root
        if( !sql.next() )
        {
            consulta = "INSERT INTO usuario(usuario_codigo, usuario_nome, usuario_senha) VALUES (null, 'root', '"
                    +  Funcoes::CriptografarSenha( "root") + "')";
            ok = sql.exec(consulta);
        }
    }
}

QSqlDatabase ConexaoBanco::Banco()
{
    return db;
}

QString ConexaoBanco::ValorCampo(QSqlDatabase dbBanco, QString tabela, QString condicao, QString campoRetorno, QString alias, bool bLimit1)
{
    QSqlQuery sql( dbBanco );
    QString consulta;
    QString retorno;
    bool ok;

    consulta = "SELECT " + campoRetorno + " FROM " + tabela + " WHERE " + condicao
               + " ORDER BY ";

    if( alias == "" )
        consulta += campoRetorno;
    else
        consulta += alias;

    if( bLimit1 )
        consulta += " DESC LIMIT 1 ";

    ok = sql.exec( consulta );

    if( ok )
    {
        ok = sql.next();
        if( alias == "" )
        {
            retorno = sql.record().value( campoRetorno).toString();
        }
        else
        {
            retorno = sql.record().value( alias ).toString();
        }
    }
    else
    {
        Funcoes::ErroSQL( sql, "ConexaoBancoAtendimento.ValorCampo" );
        retorno = QString("");
    }

    //Funcoes::MensagemAndroid( "", sql.lastQuery() );

    QString r = retorno;
    return r;
}

bool ConexaoBanco::VerificaSeCaminhoExiste(QString caminho)
{
    QDir diretorio;

    if( diretorio.exists( caminho ) )
    {
        return true;
    }
    else
    {
        return false;
    }

}

bool ConexaoBanco::VerificaSeArquivoExiste(QString caminhoArquivo)
{
    QFileInfo arquivo( caminhoArquivo );

    if( arquivo.isFile() )
        return arquivo.exists();

    return false;
}

QString ConexaoBanco::CaminhoBanco()
{
    if( db.driverName() == "QSQLITE" )
        return db.databaseName();
    else
        return db.databaseName() + " Driver: " + db.driverName();
}

void ConexaoBanco::FecharBanco()
{
    QStringList conexoes = db.connectionNames();

    for( int i = 0; i < conexoes.size() ; ++i )
    {
        db.close();
        db.removeDatabase( conexoes.at( i ) );
    }
}

bool ConexaoBanco::BancoSqLite()
{

    const QString sBarra = QDir::separator();
    QString sCaminhoBanco;
    const QString sPastaBanco("Almoco");
    QDir diretorio;

    //Quando testado no computador esse caminho n�o existe
    if( diretorio.exists( sBarra + "mnt" + sBarra + "sdcard" ) )
    {
        sCaminhoBanco = sBarra + "mnt" + sBarra + "sdcard";
    }
    else
    {
        sCaminhoBanco = sBarra + "sdcard" + sBarra;
    }

    diretorio.setCurrent( sCaminhoBanco );

    if( !diretorio.exists( sPastaBanco ) )
    {
        diretorio.mkdir( sPastaBanco );
    }

    diretorio.setCurrent( sCaminhoBanco + sBarra + sPastaBanco );

    sCaminhoBanco.append( sBarra + sPastaBanco + sBarra + "almoco.db" );

    db = QSqlDatabase::addDatabase( "QSQLITE", "almoco" );

    db.setDatabaseName( sCaminhoBanco );

    if( !db.open() )
    {
        QMessageBox::warning( 0, "Erro", "Erro ao conectar no banco:\n" +
                              db.lastError().text(), "ok");
        return false;
    }

    bool conectou;
    QStringList Tabelas = db.tables();
    int iQtdTabelas = Tabelas.size();

    if( iQtdTabelas <= 1 )
    {
        conectou = CriarTabelas( db );
    }
    else
    {
        conectou = AtualizaEstruturaDoBanco();
    }

    return conectou;
}

QString ConexaoBanco::CriaPastaBanco()
{
    QDir diretorioBanco ;
    QString caminho = QCoreApplication::applicationDirPath() + "/banco";

    if( !diretorioBanco.exists(caminho))
    {
        bool ok = diretorioBanco.mkdir(caminho);

        if( ok )//Se conseguiu criar
        {
            return caminho;
        }
        else
        {
            return QString("");
        }
    }

    return caminho;
}

bool ConexaoBanco::AlterarTipoCampoTabela(QString tabela, QString campoNovoTipo, bool transacao)
{
    QSqlQuery sql( db );
    bool ok;
    QString sNovaTabelaTemp, consulta;
    size_t numero = 0;

    sql.finish();

    if( transacao )
        ok = db.transaction();
    //Cria tabela temporaria
    do
    {
        sNovaTabelaTemp = tabela + QString::number( numero );
        numero++;
        //Cria a nova tabela
        consulta = "CREATE TEMPORARY TABLE " + sNovaTabelaTemp + " (" + campoNovoTipo + ")";
        ok = sql.exec( consulta );

    }while( !ok && numero < 10 );

    if( !ok )
    {
        if( transacao )
            ok = db.rollback();
       return false;
   }

    QString campo;

    {
        QStringList campoTemp;

        campoTemp = campoNovoTipo.split(" ");

        for( int i = 0; i < campoNovoTipo.size(); ++i)
        {
            campo = campoTemp.at( i );

            if( !campo.isEmpty() && campo != " " )
                break;
        }
    }

    //Salva o codigo
    consulta = "INSERT INTO " + sNovaTabelaTemp +
               " SELECT " + campo  + " FROM " + tabela + " ORDER BY " + campo;
    ok = sql.exec( consulta );

    if( transacao )
        ok = db.commit();

    ok = RemoverCampoTabela(tabela ,campo);

    ok = AdicionarCampoTabela(tabela, QStringList( campoNovoTipo ) );

    if( transacao )
        ok = db.transaction();

    consulta = "SELECT " + campo + " FROM " + tabela + " ORDER BY " + campo;
    ok = sql.exec( consulta );

    QSqlQuery sqlUpdate;
    QSqlQuery sqlTemp;

    consulta = "SELECT " + campo + " FROM " + sNovaTabelaTemp + " ORDER BY " + campo;
    ok = sqlTemp.exec( consulta );

    //Atualiza a tabela com o valor da tabela temporaria
    while( sql.next() && sqlTemp.next() )
    {
        consulta = "UPDATE " + tabela + " SET " + campo + " = "
                   + sqlTemp.record().value( campo ).toString()
                   + " WHERE " + campo + " = "
                   + sql.record().value( campo ).toString();
        ok = sqlUpdate.exec( consulta );

    }

    if( transacao )
        ok = db.commit();

    bool retorno = ok;

    return retorno;
}

bool ConexaoBanco::VerificaSeCampoExiste(QString tabela, QString campoVerificar, bool transacao)
{
    QSqlQuery sql( db );
    QString consulta;

    bool ok;

    if( transacao )
        ok = db.transaction();

    consulta = "SELECT * FROM " + tabela + " LIMIT 1";
    ok = sql.exec( consulta );

    ok = sql.record().contains( campoVerificar );

    bool bCampoExist = ok;

    if( transacao )
        ok = db.commit();

    return bCampoExist;
}

bool ConexaoBanco::AdicionarCampoTabela(QString tabela, QStringList novosCamposEtipo, char separador, bool transacao)
{
    QSqlQuery sql( db );
    QString consulta;
    QString camposTabelaOriginal;
    QString criacao;
    QStringList camposComTipo;
    QString camposSemTipo;
    bool ok;    
    int parenteses1;

    {//Remove os campos que ja existem na tabela, para adicionar so os que faltam
        int iQtdCampos = novosCamposEtipo.size();
        int iPosicao = 0;

        for( int i = 0; i < iQtdCampos; ++i)
        {
            bool bExist = VerificaSeCampoExiste( tabela, novosCamposEtipo.at( iPosicao ) );

            if( bExist )
            {
                novosCamposEtipo.removeAt( iPosicao );
            }
            else
            {
                iPosicao++;
            }
        }

        if( novosCamposEtipo.size() == 0)
            return false;
    }

    sql.finish();

    if( transacao )
        ok = db.transaction();

    //Pega estrutura da tabela
    consulta = "SELECT sql FROM sqlite_master WHERE type = 'table' AND name = '" + tabela + "'";
    ok = sql.exec( consulta );
    ok = sql.next();

    criacao = sql.record().value("sql").toString();

    if( criacao.isEmpty() )//Provavelmente o nome da tabela esta errado
    {
        return false;
    }
    else
    {
        //Acha o primeiro e segundo parenteses
        parenteses1 = criacao.indexOf( "(" );
        parenteses1++;

        QString criacaoTmp;

        criacaoTmp = criacao.mid( parenteses1, criacao.size() - parenteses1 );
        //tira o ultimo parenteses
        camposTabelaOriginal = criacaoTmp.left( criacaoTmp.size() -1 );
    }
    camposComTipo = camposTabelaOriginal.split( separador );

    //Enquanto não conseguir renomear a tabela
//    do
//    {
//        tabelaAntiga = tabela + QString::number( numero );
//        consulta = "ALTER TABLE " + tabela + " RENAME TO " + tabelaAntiga;
//        ok = sql.exec( consulta );
//        numero++;
//    }while( !ok && numero < 10 );

//    //Se não conseguiu renomear a tabela, sai
//    if( !ok )
//        return false;

    //pega os campos para serem usados no select e no insert
    int qtdCampos = camposComTipo.size();

    for( int i = 0; i < qtdCampos; ++i)
    {
        QString item = camposComTipo.value( i );
        QStringList itemTmp = item.split(" ");
        QString campoTmp;

        for( int j = 0; j < itemTmp.size(); ++j)
        {
            campoTmp = itemTmp.at( j );

            if( campoTmp != " " and !campoTmp.isEmpty() )
                break;
        }

        camposSemTipo.append( campoTmp + ", " );
    }
    //Tira a ultima virgula
    camposSemTipo = camposSemTipo.left( camposSemTipo.size() -2 );

    QStringList novaEstruturaTabela;
    //A nova estrutura é os campos que a tabela ja tem mais os novos campos
    novaEstruturaTabela = camposComTipo + novosCamposEtipo;

    qtdCampos = novaEstruturaTabela.size();

    QString novosCampos;

    novosCampos = "";

    for( int i = 0; i < qtdCampos; ++i)
    {
        novosCampos +=  novaEstruturaTabela.at( i ) + ", ";
    }
    //tira a ultima virgula
    novosCampos = novosCampos.left( novosCampos.size() -2 );

    {
        QString sNovaTabelaTemp;
        size_t numero = 0;

        numero = 1;

        do
        {
            sNovaTabelaTemp = tabela + QString::number( numero );
            numero++;
            //Cria a nova tabela
            consulta = "CREATE TABLE IF NOT EXISTS " + sNovaTabelaTemp + " (" + novosCampos + ")";
            ok = sql.exec( consulta );

        }while( !ok && numero < 10 );

        if( ok )
        {
            //Inseri os dados da tabela antiga (tabelaTmp) na nova tabela(tabela)
            consulta = "INSERT INTO " + sNovaTabelaTemp + " (" + camposSemTipo + ") SELECT " + camposSemTipo
                       + " FROM " + tabela;
            ok = sql.exec( consulta );
            consulta = "DROP TABLE " + tabela;
            ok = sql.exec( consulta );
            consulta = "ALTER TABLE " + sNovaTabelaTemp + " RENAME TO " + tabela;
            ok = sql.exec( consulta );
        }
        else
        {
            if( transacao )
                db.rollback();
            return false;
        }
    }

    bool retorno = ok;    

    return retorno;
}

bool ConexaoBanco::RemoverCampoTabela(QString tabela, QString campoRemover, char separador, bool transacao)
{
    QSqlQuery sql( db );
    QString consulta;
    QString tabelaTmp;
    QString camposTabelaOriginal;
    QString criacao;
    QStringList camposComTipo;
    QStringList camposSemTipo;
    bool ok;
    size_t numero = 0;
    int parenteses1;


    {//Remove os campos que ja existem na tabela, para adicionar so os que faltam
        QStringList sCamposTemp = campoRemover.split( separador );
        int iQtdCampos = sCamposTemp.size();
        int iPosicao = 0;

        for( int i = 0; i < iQtdCampos; ++i)
        {
            bool bExist = VerificaSeCampoExiste(tabela, sCamposTemp.at( iPosicao ) );

            if( bExist == false)
            {
                sCamposTemp.removeAt( iPosicao );
            }
            else
            {
                iPosicao++;
            }
        }

        if( sCamposTemp.size() == 0)
            return false;

        campoRemover = sCamposTemp.join( ", " );
    }

    sql.finish();

    if( transacao )
        ok = db.transaction();

    //Pega estrutura da tabela
    consulta = "SELECT sql FROM sqlite_master WHERE type = 'table' AND name = '" + tabela + "'";
    ok = sql.exec( consulta );
    ok = sql.next();

    criacao = sql.record().value("sql").toString();

    if( criacao.isEmpty() )//Provavelmente o nome da tabela esta errado
    {
        return false;
    }
    else
    {
        //Acha o primeiro e segundo parenteses
        parenteses1 = criacao.indexOf( "(" );
        parenteses1++;

        QString criacaoTmp;

        criacaoTmp = criacao.mid( parenteses1, criacao.size() - parenteses1 );
        //tira o ultimo parenteses
        camposTabelaOriginal = criacaoTmp.left( criacaoTmp.size() -1 );
    }
    camposComTipo = camposTabelaOriginal.split( separador );



    //pega os campos para serem usados no select e no insert e a nova estrutura
    int qtdCampos = camposComTipo.size();
    QStringList novaEstruturaTabela;

    for( int i = 0; i < qtdCampos; ++i)
    {
        QString item = camposComTipo.value( i );
        QStringList itemTmp = item.split(" ");
        QString campoTmp;

        for( int j = 0; j < itemTmp.size(); ++j)
        {
            campoTmp = itemTmp.at( j );

            if( campoTmp != " " and !campoTmp.isEmpty() )
                break;
        }

        if( !campoRemover.contains( campoTmp,  Qt::CaseInsensitive ) )//Se tiver o campo não é para pegar ele
        {
            novaEstruturaTabela.append( item );//Nova estrutura com o campo e tipo
            camposSemTipo.append( campoTmp );
        }
    }

    do
    {
        //Cria a nova tabela com um nome temporario
        tabelaTmp = tabela + QString::number( numero );
        consulta = "CREATE TEMPORARY TABLE " + tabelaTmp + " (" + novaEstruturaTabela.join(", ") + ")";
        ok = sql.exec( consulta );
        numero++;
    }while( !ok && numero < 10  );

    bool retorno = ok ;

    if( ok )
    {
        //Inseri os dados da tabela antiga (tabela) na nova tabela(tabelaTmp)
        consulta = "INSERT INTO " + tabelaTmp + " (" + camposSemTipo.join(", ") + ") SELECT " + camposSemTipo.join(", ")
                   + " FROM " + tabela;
        ok = sql.exec( consulta );
    }

    if( ok )
    {
        consulta = "DROP TABLE " + tabela;
        ok = sql.exec( consulta );
        if( !ok )
             QMessageBox::warning(0, "erro", sql.lastError().text(), "ok");

        consulta = "CREATE TABLE IF NOT EXISTS " + tabela + " (" + novaEstruturaTabela.join(", ") + ")";
        ok = sql.exec( consulta );
        if( !ok )
            QMessageBox::warning(0, "erro", sql.lastError().text(), "ok");

        //Inseri os dados da tabela antiga (tabela) na nova tabela(tabelaTmp)
        consulta = "INSERT INTO " + tabela + " (" + camposSemTipo.join(", ") + ") SELECT "
                   + camposSemTipo.join(", ") + " FROM " + tabelaTmp;
        ok = sql.exec( consulta );
        if( !ok )
            QMessageBox::warning(0, "erro", sql.lastError().text(), "ok");
//        consulta = "DROP TABLE " + tabelaTmp;
//        ok = sql.exec( consulta );

//        if( !ok )
//            QMessageBox::warning(0, "erro", sql.lastError().text(), "ok");

        consulta = "ALTER TABLE " + tabelaTmp + " RENAME TO " + tabela;
        ok = sql.exec( consulta );

        if( transacao )
            ok = db.commit();

        return true;
    }
    else
    {
        QMessageBox::warning(0, "erro", sql.lastError().text(), "ok");
        //Se deu algum tipo de erro, deleta a nova tabela e renomeia a tabela antiga para o nome antigo
        consulta = "DROP TABLE " + tabelaTmp;
        ok = sql.exec( consulta );

        if( transacao )
            db.rollback();

        return false;
    }

    return retorno;
}

void ConexaoBanco::RemoverTabelasTemp(QString tabela, int qtd, bool transacao)
{
    QSqlQuery sql( db );
    QString consulta;
    bool ok;

    sql.finish();

    if( transacao )
        ok = db.transaction();

    if( tabela.isEmpty() )
    {
        QStringList tabelas =  db.tables();
        int iQtdTabelas = tabelas.size();

        for( int i = 0; i < iQtdTabelas; i++)
        {
            QString tmp = tabelas.at( i );
            int numero = tmp.right( 1 ).toInt();

            if( numero == 0 )
            {
                if( tmp.right( 1 ) != "0" )
                    continue;
            }

            consulta = "DROP TABLE " + tabelas.at( i );
            ok = sql.exec( consulta );

//            if( !ok )
//            {
//                QMessageBox::critical(0, "Remover Tabela", "Descrição: "
//                                      + sql.lastError().text() ,"Ok" );
//            }
        }
    }
    else
    {
        for( int i = 0; i < qtd; ++i)
        {
            consulta = "DROP TABLE " + tabela + QString::number( i );
            ok = sql.exec( consulta );
        }
    }
    if( transacao )
        ok = db.commit();

    bool retorno = ok;

    ok = retorno;
}

bool ConexaoBanco::CriarTabelas(QSqlDatabase dbBanco, bool transacao)
{
    QSqlQuery sql( dbBanco );
    QString consulta;
    QString sAutoIncrement;
    bool ok;
    //Se for verdade provavelmente � mysql
    if( dbBanco.driverName() == "QODBC" )
    {
        sAutoIncrement = "auto_increment ";
    }
    else
    {
        sAutoIncrement = "autoincrement ";
    }

    if( transacao )
        ok = dbBanco.transaction();

    consulta = "CREATE TABLE IF NOT EXISTS versao (versao_codigo INTEGER PRIMARY KEY " + sAutoIncrement + ", versao varchar(10), "
               "dataversao date not null default '0000-00-00', dataatualizacao date not null default '0000-00-00')";
    ok = sql.exec( consulta );

    consulta = "create table if not exists almoco( codigo integer primary key " + sAutoIncrement + ", valor double not null default 0.00, data date not null default '00-00-0000', restaurante integer not null default 0) ";
    ok = sql.exec( consulta );

    if( !ok )
        Funcoes::ErroSQL( sql, "ConexaoBanco::CriarTabelas()" );

    consulta = "create table if not exists restaurante( codigo integer primary key "  + sAutoIncrement + ", nome vachar(100) not null default '', valor double not null default 0.00 ) ";
    ok = sql.exec( consulta );

    if( !ok )
        Funcoes::ErroSQL( sql, "ConexaoBanco::CriarTabelas()" );

    if( !ok )
        Funcoes::ErroSQL( sql, "ConexaoBanco::CriarTabelas()" );

    AtualizaTabelaVersao( "0.0.3", QDate( 2012, 3, 18 ) );

    if( transacao )
        ok = dbBanco.commit();

    return true;
}

bool ConexaoBanco::AtualizaTabelaVersao(QString versao, QDate data)
{
    QSqlQuery sql( db );
    QString consulta;
    bool ok;

    consulta = "SELECT * FROM versao WHERE versao = '" + versao + "'";
    ok = sql.exec( consulta );

    if( !ok )
         QMessageBox::warning(0, "Erro AtualizaTabelaVersao", "Descrição:\n" + sql.lastError().text(), "ok");

    if( !sql.next() )//Se não encontrou
    {
        consulta = "INSERT INTO versao (versao, dataversao, dataatualizacao) VALUES ('" + versao + "', '" + data.toString("yyyy-MM-dd") + "', '" + QDate::currentDate().toString("yyyy-MM-dd") + "')";
        ok = sql.exec(consulta);

        if( !ok )
             QMessageBox::warning(0, "Erro AtualizaTabelaVersao", "Descrição:\n" + sql.lastError().text(), "ok");
    }

    return ok;
}

bool ConexaoBanco::AtualizaEstruturaDoBanco()
{
    QSqlQuery sql( db );
    QString consulta;    
    QString versao;

    //Verifica se tem algum usuário cadastrado    
    bool atualizaou = true;

    consulta = "SELECT versao FROM versao WHERE versao = ( SELECT MAX(versao) FROM versao )";
    sql.exec(consulta);

    //BackUpBancoSQLite( false );

    if( sql.next() )
        versao = sql.record().value("versao").toString();
    else
        versao = "0.0.1";

    if( versao == "0.0.4" )
    {

        //AtualizaTabelaVersao( "0.0.4", QDate( 2012, 2, 22 ) );
    }

     return atualizaou;
}

void ConexaoBanco::BackUpBancoSQLite(bool bMensagem)
{
//    QString caminho = QCoreApplication::applicationDirPath() + "/atendimento.ini";

//    /*
//      Chave BANCO:
//      TIPOBANCO = tipo do banco de dados: ORACLE , MYSQL, sql SERVER, POSTGRESQL
//      CAMINHOBANCO = IP ou nome da máquina em que esta o banco de dados
//      DATABASENAME = nome do banco de dados
//    */

//    Funcoes::CriarArquivo( caminho );

//    QSettings arquivoINI(caminho, QSettings::IniFormat, 0);


//    QString caminhoOrigem = arquivoINI.value("BANCO/CAMINHOBANCO").toString();

//    if( caminhoOrigem.isEmpty() )
//    {
//        return ;
//    }

//    QString nomeBanco;

//    {
//        //Cria um arquivo temporario somente para pegar o nome do banco
//        QFileInfo arquivoTmp( caminhoOrigem );
//        QString extenssao;

//        nomeBanco = arquivoTmp.fileName();

//        int ponto = nomeBanco.indexOf(".");

//        extenssao = nomeBanco.mid( ponto );
//        nomeBanco = nomeBanco.left( ponto );

//        //Coloca o nome do banco como exemplo: Atendimento_02_11_2010_18_07_10. Dia e Hora
//        nomeBanco += QDateTime::currentDateTime().toString("_dd_MM_yyyy_hh_mm_ss") + extenssao;
//    }

//    QString caminhoBackupLocal = arquivoINI.value("BANCO/CAMINHOBACKUPLOCAL").toString();

//    if( !caminhoBackupLocal.isEmpty() )
//    {
//        bool ok = QFile::copy( caminhoOrigem, caminhoBackupLocal + "/" + nomeBanco);

//        if( !ok )
//        {
//            QMessageBox::critical(0, tituloMensagem, "Não foi possivel fazer o backup em:\n" + caminhoBackupLocal, "Ok");
//        }
//    }

//    QString caminhoBackupServidor = arquivoINI.value("BANCO/CAMINHOBACKUPSERVIDOR").toString();

//    if( !caminhoBackupServidor.isEmpty() )
//    {
//        bool ok = QFile::copy(caminhoOrigem, caminhoBackupServidor + "/" + nomeBanco);

//        if( !ok )
//        {
//            QMessageBox::critical(0, tituloMensagem, "Não foi possivel fazer o backup em:\n" + caminhoBackupServidor, "Ok");
//        }
//    }

//    if( bMensagem )
//    {
//        QMessageBox::information(0, tituloMensagem, "Backup feito com sucesso\n"
//                             + ( caminhoOrigem.isEmpty() ? "" : "\nBanco: " + caminhoOrigem )
//                             + ( caminhoBackupLocal.isEmpty() ? "" : "\nBackup Local: " + caminhoBackupLocal )
//                             + ( caminhoBackupServidor.isEmpty() ? "" : "\nBackup Servidor: " + caminhoBackupServidor )
//                             , "Ok");
//    }
}

QString ConexaoBanco::SelecionarArquivo(QString tipoArquivo, QString diretorio)
{
    //Exemplo "Arquivo db (*.db);; Todos (*.*)"

    if( tipoArquivo.isEmpty() )
        tipoArquivo = "Todos (*.*)";

    if( diretorio.isEmpty() )
        diretorio = QCoreApplication::applicationDirPath();

    QString arquivo =
            QFileDialog::getSaveFileName(0, "Selecionar Arquivo", diretorio, tipoArquivo );

    QString retorno = arquivo;

    return retorno;
}

void ConexaoBanco::ImportaCSV()
{
    QString sArquivo;

    sArquivo = SelecionarArquivo( "Arquivo csv (*.csv)","" );

    QFile fArquivo( sArquivo );

    fArquivo.open( QIODevice::ReadOnly );

    QTextStream arquivoCSV( &fArquivo );
    QString sLinha, sSeparador(";");
    QStringList slDados;
    QSqlQuery sql( db );
    QString consulta;
    bool ok;
    QMap< QString, QString > query;
    QTime tHoraInicial, tHoraFinal;

    tHoraInicial = QTime::currentTime();

    //Como a primeira linha eh o cabe�alho, remove ela
    sLinha = arquivoCSV.readLine();

    while( !arquivoCSV.atEnd() )
    {
        sLinha = arquivoCSV.readLine();
        slDados = sLinha.split( sSeparador );

        //Menos o problema, observa�ao e resolu�ao

        query.insert( "atendimento_codigo", slDados.at( 0 ) );
        query.insert("atendimento_data", slDados.at( 1 ) );
        query.insert("atendimento_hora", slDados.at( 2 ) );
        query.insert("atendimento_tipoatendimento", slDados.at( 3 ) );
        query.insert("atendimento_status", slDados.at( 4 ) );
        query.insert("atendimento_funcionario", slDados.at( 5 ) );
        query.insert("atendimento_cliente", slDados.at( 6 ) );
        query.insert("atendimento_modulo", slDados.at( 7 ) );

        consulta = ConexaoBanco::MontaQueryInsert( "atendimento", query );
        ok = sql.exec( consulta );

        if( !ok )
        {
            Funcoes::ErroSQL( sql, "ConexaoBanco::ImportaCSV()");
            return ;
        }

        QStringList slTabelas;

        slTabelas.append("problema");
        slTabelas.append("observacao");
        slTabelas.append("resolucao");

        for( int i = 8; i < 11; ++i )
        {
            query.clear();
            int iPosicaoEnter = slDados.at( i ).indexOf( "\n" );
            QStringList slDataHoraUsuario = slDados.at( i ).left( iPosicaoEnter ).split( " " );

            query.insert( "atendimento_codigo", slDados.at( 0 ) );
            query.insert( slTabelas.at( 0 ) + "_data", slDataHoraUsuario.at( 1 ) );
            query.insert( slTabelas.at( 0 ) + "_hora", slDataHoraUsuario.at( 2 ) );
            query.insert( slTabelas.at( 0 ) + "_editado", slDataHoraUsuario.at( 3 ) );
            query.insert( "problema_problema", slDados.at( i ).mid( iPosicaoEnter + 1 ) );

            consulta = MontaQueryInsert( slTabelas.at( 0 ), query );
            ok = sql.exec( consulta );

            if( !ok )
            {
                Funcoes::ErroSQL( sql, "ConexaoBanco::ImportaCSV()" );
                return ;
            }
            slTabelas.removeAt( 0 );
        }//for( int i = 8; i < 11; ++i )
    }//while( !arquivoCSV.atEnd() )

    tHoraFinal = QTime::currentTime();

    QTime tDiferencaHoras = Funcoes::DiferencaEntreHoras( tHoraInicial, tHoraFinal );

    QMessageBox::information( 0, tituloMensagem, "Importa�ao concluida com sucesso."
                              "<br><b>Tempo gasto: " + tDiferencaHoras.toString("hh:mm:ss") + "<b><br>" );
}

//"\"Atendimento\"" + sSeparador +
//           + "\"Data\""  + sSeparador +
//           "\"Hora\""  + sSeparador +
//           "\"Tipo\""  + sSeparador +
//           "\"Status\""  + sSeparador +
//           "\"Funcionário\""  + sSeparador +
//           "\"Cliente\""  + sSeparador +
//           "\"Modulo/Tela\""  + sSeparador +
//           "\"Problema\""  + sSeparador +
//           "\"Observação\""  + sSeparador +
//           "\"Resolução\""  + sSeparador +

void ConexaoBanco::ExportaBancoParaCSV(QString dataInicial, QString dataFinal, QString sCondicao)
{
    //Exporta um arquivo CSV com o valor emtre "" separado por vírgula e sem espaço após a vírgula
    //Depois de cada registro tem um \n
    //Ex:
    /*"campo1","campo2","campo3"
      "valor1","valor do campo 2","valor do campo 3"
    */

    QString consulta, camposSelect;
    QSqlQuery sqlAtendimento, sqlOutrasTabelas;

    camposSelect = "atendimento.atendimento_codigo, atendimento_data, atendimento_hora, atendimento_status, "
                   "atendimento_funcionario, atendimento_cliente, atendimento_modulo, "
                   "problema.problema_numero, atendimento_tipoatendimento, usuario_nome ";

    consulta = "SELECT " + camposSelect + " FROM atendimento "
               "LEFT JOIN usuario on usuario.usuario_codigo = atendimento.usuario_codigo "
               "LEFT JOIN problema on atendimento.atendimento_codigo = problema.atendimento_codigo "
               "WHERE atendimento_data BETWEEN '" + dataInicial + "' AND '" + dataFinal + "' ";
    if( !sCondicao.isEmpty() )
    {
        consulta += " AND " + sCondicao;
    }
    consulta += " ORDER BY atendimento.atendimento_codigo";

//               "LEFT JOIN observacao on observacao.atendimento_codigo = problema.atendimento_codigo "
//               "AND observacao.problema_numero = problema.problema_numero "
//               "LEFT JOIN resolucao on problema.atendimento_codigo = resolucao.atendimento_codigo "
//               "AND resolucao.problema_numero = problema.problema_numero "
//               "GROUP BY atendimento.atendimento_codigo, problema.problema_numero

    bool ok = sqlAtendimento.exec( consulta );

    if( !ok )
    {
        QMessageBox::warning( 0, "Erro", "ExportaBancoParaCSV\nDescrição: " + sqlAtendimento.lastError().text() + "\n" + consulta, "&ok" );
        return ;
    }

    {//Cria o diretorio
        QDir diretorio;

        if( !diretorio.exists( QCoreApplication::applicationDirPath() + "/banco/csv" ) )
        {
            diretorio.mkdir( QCoreApplication::applicationDirPath() + "/banco/csv");
        }
    }

    QString caminhoArquivo( QCoreApplication::applicationDirPath() + "/banco/csv/banco_"
                            + QDateTime::currentDateTime().toString("dd_MM_yyyy_hh_mm_ss") + ".csv");

    QFile bancoCSV( caminhoArquivo );
    QString sSeparador(";");

    //Abre o arquivo para somente escrita
    bancoCSV.open( QIODevice::WriteOnly );

    //int qtdCampos = sqlAtendimento.record().count();

    QTextStream arquivoCSV(&bancoCSV);

    //Cabeçalho
    arquivoCSV <<  "\"Atendimento\"" + sSeparador +
            + "\"Data\""  + sSeparador +
            "\"Hora\""  + sSeparador +
            "\"Tipo\""  + sSeparador +
            "\"Status\""  + sSeparador +
            "\"Funcionário\""  + sSeparador +
            "\"Cliente\""  + sSeparador +
            "\"Modulo/Tela\""  + sSeparador +
            "\"Problema\""  + sSeparador +
            "\"Observação\""  + sSeparador +
            "\"Resolução\""  + sSeparador +
            "\n";

    QVector<QString > tabelas;

    tabelas.append("problema");
    tabelas.append("observacao");
    tabelas.append("resolucao");

    while( sqlAtendimento.next() )
    {
        QString valor;

        valor = "\"" + Funcoes::FormataCodigo( sqlAtendimento.record().value("atendimento_codigo").toString() )
                + ' ' + sqlAtendimento.record().value("usuario_nome").toString() + "\""  + sSeparador +
                "\"" + sqlAtendimento.record().value("atendimento_data").toDate().toString("dd/MM/yyyy") + "\""  + sSeparador +
                "\"" + sqlAtendimento.record().value("atendimento_hora").toTime().toString("hh:mm") + "\""  + sSeparador +
                "\"" + sqlAtendimento.record().value("atendimento_tipoatendimento").toString() + "\""  + sSeparador +
                "\"" + sqlAtendimento.record().value("atendimento_status").toString() + "\""  + sSeparador +
                "\"" + sqlAtendimento.record().value("atendimento_funcionario").toString() + "\""  + sSeparador +
                "\"" + sqlAtendimento.record().value("atendimento_cliente").toString() + "\""  + sSeparador +
                "\"" + sqlAtendimento.record().value("atendimento_modulo").toString() + "\""  + sSeparador
                ;

        arquivoCSV << valor;

        for(int j = 0; j < tabelas.size(); ++j)
        {
            QString tab = tabelas.at( j );

            consulta = "SELECT * FROM " + tab +
                       " WHERE atendimento_codigo = "
                       + sqlAtendimento.record().value("atendimento_codigo").toString()
                       + " AND problema_numero = "
                       + sqlAtendimento.record().value( "problema_numero" ).toString();
                       + " AND " + tab + "_" + tab + " <> '' "
                         "AND " + tab + "_data "
                         "BETWEEN '" + dataInicial + "' AND '" + dataFinal + "' "
                         "ORDER BY " + tab + "_numero DESC";

            ok = sqlOutrasTabelas.exec( consulta );

            valor.clear();

            while( sqlOutrasTabelas.next() )
            {
                valor+= Funcoes::FormataCodigo( sqlOutrasTabelas.record().value( tab + "_numero" ).toString() ) + " " ;
                valor+= sqlOutrasTabelas.record().value( tab + "_data" ).toDate().toString("dd/MM/yyyy") + " ";
                valor+= sqlOutrasTabelas.record().value( tab + "_hora" ).toTime().toString("hh:mm") + " ";
                valor+= sqlOutrasTabelas.record().value( tab + "_editado" ).toString();
                valor+= "\n";
                valor+= sqlOutrasTabelas.record().value( tab + "_" + tabelas.at( j )   ).toString();
                valor+= "\n";
            }

            arquivoCSV << "\"" + valor + "\"";

            if( j < 3)
                arquivoCSV << sSeparador;
        }
        arquivoCSV << "\n";
    }//while( sqlAtendimento.next() )

    QMessageBox::information(0, tituloMensagem, "Arquivo CSV exportado para: "
                             "<br><b>" + caminhoArquivo + "</b></br>","Ok");

}

QString ConexaoBanco::CamposTabela(QSqlDatabase dbBanco, QString tabela)
{
    QSqlQuery sql( dbBanco );
    QString consulta;
    QString camposTabelaOriginal;
    QStringList camposComTipo;
    QString camposSemTipo;
    QString criacao;
    QStringList campos;
    bool ok;

    //Pega estrutura da tabela
    consulta = "SELECT sql FROM sqlite_master WHERE type = 'table' AND name = '" + tabela + "'";
    ok = sql.exec( consulta );
    ok = sql.next();

    criacao = sql.record().value("sql").toString();

    if( criacao.isEmpty() )//Provavelmente o nome da tabela esta errado
    {
        return QString();
    }
    else
    {
        int parenteses1;
        //Acha o primeiro e segundo parenteses
        parenteses1 = criacao.indexOf( "(" );
        parenteses1++;

        QString criacaoTmp;

        criacaoTmp = criacao.mid( parenteses1, criacao.size() - parenteses1 );
        //tira o ultimo parenteses
        camposTabelaOriginal = criacaoTmp.left( criacaoTmp.size() -1 );
    }
    camposComTipo = camposTabelaOriginal.split( "," );

    int qtdCampos = camposComTipo.size();

    for( int i = 0; i < qtdCampos; ++i)
    {
        QString item = camposComTipo.value( i );
        QStringList itemTmp = item.split(" ");
        QString campoTmp;

        for( int j = 0; j < itemTmp.size(); ++j)
        {
            campoTmp = itemTmp.at( j );

            if( campoTmp != " " and !campoTmp.isEmpty() )
                break;
        }

        camposSemTipo.append( campoTmp + ", " );
    }
    //Tira a ultima virgula
    camposSemTipo = camposSemTipo.left( camposSemTipo.size() -2 );

    QString retorno = camposSemTipo;

    return retorno;
}

bool ConexaoBanco::ConectaBanco(const QString &sTipo)
{
    bool bConectou = false;

    if( sTipo == "QSQLITE" )
    {
        bConectou = BancoSqLite();
    }

    return bConectou;
}

bool ConexaoBanco::BancoMysql()
{
    QString caminho = QCoreApplication::applicationDirPath() + "/atendimento.ini";

    /*
      Chave BANCO:
      TIPOBANCO = tipo do banco de dados: ORACLE , MYSQL, sql SERVER, POSTGRESQL
      CAMINHOBANCO = IP ou nome da máquina em que esta o banco de dados
      DATABASENAME = nome do banco de dados
    */
    QSettings arquivoINI(caminho, QSettings::IniFormat, 0);

    QString tipoBanco, hostName, databaseName;

    tipoBanco = arquivoINI.value("BANCO/TIPOBANCO").toString();
    hostName = arquivoINI.value("BANCO/CAMINHOBANCO").toString();
    databaseName = arquivoINI.value("BANCO/DATABASENAME").toString();

    if(tipoBanco.isEmpty() || hostName.isEmpty())
    {
        QMessageBox::information(0, "Iniciando Sistema","Não foi possivel ler o arquivo de configuração");
        return false;
    }
    else
    {
        db = QSqlDatabase::addDatabase(tipoBanco);//Instancia a conexão com os parametros.

        db.setDatabaseName("aplicativo");//DSN
        db.setHostName("localhost");
        db.setUserName("root");
        db.setPassword("root");
        //Se conectar no banco, todas as consultas feitas pelas variaveis do tipo QSqlQuery, serão feitas nesse banco
        bool ok = db.open();
        if(!ok)
        {
            QMessageBox::critical(0, "Atendimento",QString::fromUtf8("Não foi possivel conectar-se ao banco de dados!\nHOSTNAME: ") + hostName +
                                  "\nDATABASENAME: " + databaseName + "\nTIPOBANCO: " + tipoBanco + QString::fromUtf8("\nDescrição do erro: ") + db.lastError().text());

            return false;

            int botao = QMessageBox::question(0, tituloMensagem, "Deseja informar nome de usuario e senha do banco de dados?", "&Sim", "&Não","", 1);

            if(botao == 0)
            {
                bool oK;
                QString usuario = QInputDialog::getText(0, tituloMensagem, "Usuário:", QLineEdit::Normal, "Digite aqui", &oK);

                QString senha = QInputDialog::getText(0, tituloMensagem, "Senha:", QLineEdit::Normal, "Digite aqui", &oK);

                db.setUserName(usuario);
                db.setPassword(senha);
                ok = db.open();
                if(!ok)
                {
                    QMessageBox::critical(0, "Iniciando o Sistema",QString::fromUtf8("Não foi possivel conectar-se ao banco de dados!\nHOSTNAME: ") + hostName +
                                          "\nDATABASENAME: " + databaseName + "\nTIPOBANCO: " + tipoBanco + "\nDescrição do erro: " + db.lastError().text());
                    return false;
                }
                else
                {
                    return true;
                }
            }
            else
            {
                return false;
            }
        }
        else
        {
            db.exec("create database if not exists aplicativo");
            db.exec("use aplicativo");

            if( db.tables().size() == 0 )
                CriarTabelas( db );

            AtualizaEstruturaDoBanco();
        }
    }
    return true;
}
