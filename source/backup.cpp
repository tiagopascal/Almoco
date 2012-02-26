#include "header/ConexaoBancoAtendimento.h"
#include "header/funcoes.h"
#include "header/backup.h"
#include "ui_backup.h"

#include <QDir>

Backup::Backup(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Backup), sFormatoDataHoraArquivo( "_dd_MM_yyyy_hh_mm_ss" )
{
    ui->setupUi(this);

    const QString pastaBackup( "backup" );
    sCaminhoBackup = ConexaoBanco::Banco().databaseName().replace( "almoco.db", "" ) + pastaBackup;
    sCaminhoBanco = ConexaoBanco::Banco().databaseName().replace( "almoco.db", "" );

    QDir diretorio( sCaminhoBanco );

    if( !diretorio.exists( pastaBackup ) )
        diretorio.mkdir( pastaBackup );

    CarregaBackup();
}

Backup::~Backup()
{
    delete ui;
}

void Backup::on_pushButton_voltar_clicked()
{
    this->close();
}

void Backup::on_pushButton_backup_clicked()
{
    QDateTime dataHora( QDateTime::currentDateTime() );
    QString sNomeBackup;
    QString sDataHora = dataHora.currentDateTime().toString( sFormatoDataHoraArquivo );

    //Coloca o nome do banco como exemplo: almoco_02_11_2010_18_07_10. Dia e Hora
    sNomeBackup = "almoco" + sDataHora + ".db";

    QFile arquivoBanco( sCaminhoBanco + "almoco.db" );

    if( !arquivoBanco.copy( sCaminhoBackup + QDir::separator() + sNomeBackup ) )
    {
        Funcoes::MensagemAndroid( "Backup", "Erro rotina on_pushButton_voltar_clicked\nDescrição: " + arquivoBanco.errorString(), "Ok", this );
    }
    else
    {
        sDataHora = dataHora.fromString( sDataHora, sFormatoDataHoraArquivo ).toString( "dd/MM/yyyy hh:mm:ss" );

        ui->listWidget_lista->addItem( sDataHora );
        ui->listWidget_lista->item( ui->listWidget_lista->count() -1 )->setSelected( true );
        ui->listWidget_lista->setFocus();
    }
}

void Backup::CarregaBackup()
{
    QDir diretorioBackup( sCaminhoBackup );

    QFileInfoList arquivosBackup = diretorioBackup.entryInfoList( QDir::Files );
    QDateTime dataHora( QDateTime::currentDateTime() );

    ui->listWidget_lista->clear();
    ui->listWidget_lista->setSortingEnabled( true );

    for( int i = 0; i < arquivosBackup.count(); ++i )
    {
        QString sDataHoraBackup = arquivosBackup.at( i ).fileName().replace( "almoco", "" );

        sDataHoraBackup = sDataHoraBackup.replace( ".db", "" );

        ui->listWidget_lista->addItem( dataHora.fromString( sDataHoraBackup, sFormatoDataHoraArquivo ).toString( "dd/MM/yyyy hh:mm:ss" ) );
        ui->listWidget_lista->item( i )->setWhatsThis( arquivosBackup.at( i ).filePath() );
    }
}

void Backup::on_pushButton_remover_clicked()
{
    if( ui->listWidget_lista->currentItem()  == 0 )
        return ;

    int iBotao = Funcoes::MensagemAndroid( "Backup", "Remover o backup selecionado?", "Sim", "Não", this );

    if( iBotao == 1 )
        return ;

    QFile arquivo( ui->listWidget_lista->currentItem()->whatsThis() );

    if( !arquivo.remove() )
    {
        Funcoes::MensagemAndroid( "Backup", "Erro ao remover o arquivo\nDescrição: " + arquivo.errorString(), "ok", this );
    }
    else
    {
        ui->listWidget_lista->takeItem( ui->listWidget_lista->currentRow() );
    }
}

void Backup::on_pushButton_restaurar_clicked()
{
    if( ui->listWidget_lista->currentItem() == 0 )
        return ;

    int iBotao = Funcoes::MensagemAndroid( "Backup", "Restaurar o backup selecionado?", "Sim", "Não", this );

    if( iBotao == 1 )
        return ;

    on_pushButton_backup_clicked();

    //QFile bancoAtual( ConexaoBanco::Banco().databaseName() );
    QFile bancoRestaurar( ui->listWidget_lista->currentItem()->whatsThis() );

    if( !bancoRestaurar.copy( sCaminhoBanco + QDir::separator() + "almocoRestaurar.db" ) )
    {
        Funcoes::MensagemAndroid( "Backup", "Erro ao restaurar backup.\nDescrição: " + bancoRestaurar.errorString(), "Ok", this );
    }
    else
    {
        iBotao = Funcoes::MensagemAndroid( "Backup", "Banco restaurado.\nSaia e entre no aplicativo novamente para fazer efeito.\nDeseja sair agora?", "Sim", "Não", this );

        if( iBotao == 0 )
            QApplication::closeAllWindows();
    }
}
