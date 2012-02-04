#include "almoco.h"
#include "ui_almoco.h"
#include <QDir>
#include <QTextStream>
#include <QMessageBox>

Almoco::Almoco(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Almoco)
{
    ui->setupUi(this);


    ui->dateEdit_data->setDate( QDate::currentDate() );

    ui->lineEdit_valor->setFocus();
    ui->lineEdit_valor->setText( "0.00" );
    ui->dateEdit_data->setFocus();
    ui->lineEdit_valor->setFocus();

    CriarBanco();
    CarregaAlmoco();
}

Almoco::~Almoco()
{
    delete ui;
}

void Almoco::CriarBanco()
{
    QFile arquivoCSV;
    {
        const QString sBarra = QDir::separator();
        QString sCaminhoCsv;
        QDir diretorio;

        //Quando testado no computador esse caminho não existe
        if( diretorio.exists( sBarra + "mnt" + sBarra + "sdcard" ) )
        {
            sCaminhoCsv = sBarra + "mnt" + sBarra + "sdcard";
        }
        else
        {
           sCaminhoCsv = sBarra + "sdcard" + sBarra;
        }

        sCaminhoArquivoCSV = sCaminhoCsv;

        diretorio.setCurrent( sCaminhoCsv );
        arquivoCSV.setFileName( "almoco.csv" );

        sCaminhoArquivoCSV.append( sBarra + "almoco.csv" );
    }
    const QString sSeparador(";");
    const QString sCabecalho ( "\"Código\"" + sSeparador + "\"Valor\"" + sSeparador + "\"Data\"\n" );

    if( arquivoCSV.exists() )
    {
        QTextStream almoco( &arquivoCSV );
        QString sLinha = almoco.readLine();

        if( sLinha != sCabecalho )
        {
            if( !arquivoCSV.remove() )
                QMessageBox::warning( this, "Erro", "Erro ao remover csv:\n"
                                      + arquivoCSV.errorString(), "ok");
        }
        else
        {
            return ;
        }
    }

    if ( arquivoCSV.open( QFile::WriteOnly | QFile::Append ) )
    {
        QTextStream almoco( &arquivoCSV );
        almoco.setFieldWidth( 10 );
        //almoco.setFieldAlignment( QTextStream::AlignLeft );

        almoco << "\"Código\"" + sSeparador << "\"Valor\"" + sSeparador << "\"Data\"\n";
    }
    else
    {
        QMessageBox::warning( this, "Erro", "Erro ao criar banco:\n" + arquivoCSV.errorString(), "ok");
    }
}

void Almoco::CarregaAlmoco()
{
    QString sLinha;
    QStringList sDados;
    QFile arquivoCSV( sCaminhoArquivoCSV );

    arquivoCSV.open( QFile::ReadOnly );

    QTextStream almoco( &arquivoCSV );

    ui->listWidget_lista->clear();

    while( !almoco.atEnd() )
    {
        sLinha = almoco.readLine();
        sDados = sLinha.replace("\"", "").split( ";" );

        ui->listWidget_lista->addItem( sDados.at( 0 ) + " " + sDados.at( 1 ) + " " + sDados.at( 2 )  );
    }
}

void Almoco::on_pushButton_clicked()
{
    QString sValor = ui->lineEdit_valor->text();

    sValor = ui->lineEdit_valor->text();

    if( sValor.isNull() )
    {
        ui->lineEdit_valor->selectAll();
        ui->lineEdit_valor->setFocus();

        return ;
    }
    else if( sValor.isEmpty() )
    {
        ui->lineEdit_valor->selectAll();
        ui->lineEdit_valor->clear();
        ui->lineEdit_valor->setFocus();

        return ;
    }
    QString sCodigo;
    const QString sSeparador(";");

    sCodigo = QString::number( ui->listWidget_lista->count() );

    sCodigo = FormataCodigo( sCodigo );

    QFile arquivoCSV( sCaminhoArquivoCSV );

   bool ok = arquivoCSV.open( QFile::WriteOnly | QFile::Append );

    QTextStream almoco( &arquivoCSV );


    almoco.setFieldWidth( 10 );
    //almoco.setFieldAlignment( QTextStream::AlignLeft );
    almoco << "\"" + sCodigo + "\"" + sSeparador << "\"" + sValor + "\""
              + sSeparador << ui->dateEdit_data->date().toString("dd-MM-yyyy") + "\"\n";
    arquivoCSV.close();

    LimparCampos();
    CarregaAlmoco();
}

void Almoco::LimparCampos()
{
    ui->lineEdit_valor->selectAll();
    ui->lineEdit_valor->setFocus();
}

QString Almoco::FormataCodigo(QString codigo, int qtdZeros)
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
