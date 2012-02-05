#include "almoco.h"
#include "ui_almoco.h"
#include <QDir>
#include <QTextStream>
#include <QMessageBox>

Almoco::Almoco(QWidget *parent) :
    QDialog(parent), ui(new Ui::Almoco), iTamanhoCampo( 10 )
{
    ui->setupUi(this);


    ui->dateEdit_data->setDate( QDate::currentDate() );

    ui->lineEdit_valor->setFocus();
    ui->lineEdit_valor->setText( "" );
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
//    const QString sCabecalho ( "Código" + sSeparador + "Valor" + sSeparador + "Data\n" );

    if( arquivoCSV.exists() )
    {
        if( QMessageBox::question( this, "Banco", "Criar novo banco?", "Sim", "Não", "", 1 ) == 0 )
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

        almoco.setFieldWidth( iTamanhoCampo );
        almoco.setFieldAlignment( QTextStream::AlignLeft );
        almoco << "Código" + sSeparador;
        almoco.setFieldAlignment( QTextStream::AlignRight );
        almoco << "Valor" + sSeparador;
        almoco.setFieldAlignment( QTextStream::AlignRight );
        almoco << "Data\n";
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
    double dTotal = 0.00;

    arquivoCSV.open( QFile::ReadOnly );

    QTextStream almoco( &arquivoCSV );

    ui->listWidget_lista->clear();

    while( !almoco.atEnd() )
    {
        sLinha = almoco.readLine();
        sDados = sLinha.split( ";" );

        if( sDados.count() == 3 )
        {
            if( sDados.at( 0 ).left( 1 ) != "x" )
            {
                dTotal += sDados.at( 1 ).toDouble();

                ui->listWidget_lista->addItem( sDados.at( 0 ) + " " + sDados.at( 1 ) + " " + sDados.at( 2 )  );
            }
        }
    }

    QString sTotal;

    sTotal = NumeroParaMoeda( dTotal );

    ui->label_total->setText( sTotal );
}

void Almoco::on_pushButton_adicinar_clicked()
{
    QString sValor = ui->lineEdit_valor->text();

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

    sValor = NumeroParaMoeda( sValor.toDouble() );

    QString sCodigo;
    const QString sSeparador(";");

    sCodigo = QString::number( ui->listWidget_lista->count() );

    sCodigo = FormataCodigo( sCodigo );

    QFile arquivoCSV( sCaminhoArquivoCSV );

    arquivoCSV.open( QFile::WriteOnly | QFile::Append );

    QTextStream almoco( &arquivoCSV );

    almoco.setFieldWidth( iTamanhoCampo );
    almoco.setFieldAlignment( QTextStream::AlignLeft );
    almoco << "" + sCodigo + "" + sSeparador ;
    almoco.setFieldAlignment( QTextStream::AlignRight );
    almoco << "" + sValor + "" + sSeparador;
    almoco.setFieldAlignment( QTextStream::AlignRight );
    almoco << ui->dateEdit_data->date().toString("dd-MM-yyyy") + "\n";
    arquivoCSV.close();

    LimparCampos();
    CarregaAlmoco();
}

void Almoco::LimparCampos()
{
    ui->lineEdit_valor->selectAll();
    ui->lineEdit_valor->setFocus();
}

QString Almoco::MoedaParaNumero(double dValor)
{
    QString numero( QString::number( dValor, 'g', 8 ) );
    //Ex: 3.000,33 vai ficar 3000.33
    numero = numero.replace(".", "").replace(",", ".");

    return QString( numero );
}

QString Almoco::NumeroParaMoeda(double dValor)
{
    QString sValor( QString::number( dValor, 'g', 8 ) );

    if( sValor.contains( "." ) && !sValor.contains( "," ) )
    {
        sValor = sValor.replace( ".", "," );
    }

    QString moeda;
    //Formata o numero com duas casas apos a virgula
    moeda = QString("%L1").arg( sValor.replace(".", "").replace(",", ".").toDouble(), 0, 'f', 2);

    moeda = FormataCodigo( moeda, 5 );

    return QString( moeda );
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

void Almoco::on_pushButton_remover_clicked()
{
    if( ui->listWidget_lista->currentItem() == 0 )
        return ;

    QString sCodigoDeletar = ui->listWidget_lista->currentItem()->text().mid( 0, 6);
    QString sCodigoProcurar;

    QFile arquivoCSV( sCaminhoArquivoCSV );

    arquivoCSV.open( QFile::ReadWrite );

    QTextStream almoco( &arquivoCSV );

    //Como a primeira linha é o cabeçalho, pula ela
    sCodigoProcurar = almoco.readLine();

    while( !almoco.atEnd() )
    {
        int iPosicao;

        iPosicao = almoco.pos();

        sCodigoProcurar =  almoco.read( 6 );//Como tem que tirar os

        if( sCodigoProcurar == sCodigoDeletar )
        {

            QString sLetra;

            sLetra = "x";

            almoco.seek( iPosicao );
            almoco << sLetra;
            break;
        }

        sCodigoProcurar = almoco.readLine();
    }
    arquivoCSV.close();
    CarregaAlmoco();
}
