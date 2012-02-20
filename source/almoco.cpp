#include "header/almoco.h"
#include "ui_almoco.h"
#include "header/ConexaoBancoAtendimento.h"

#include <QDir>
#include <QTextStream>
#include <QMessageBox>
#include <QDesktopWidget>

Almoco::Almoco(QWidget *parent) :
    QDialog(parent), ui(new Ui::Almoco)
{
    ui->setupUi(this);

    ConexaoBanco::ConectaBanco( "QSQLITE" );

    db = ConexaoBanco::Banco();

    ui->tabWidget->setCurrentIndex( 0 );

    ui->dateEdit_data->setDate( QDate::currentDate() );

    ui->lineEdit_valor->setFocus();
    ui->lineEdit_valor->setText( "" );
    ui->dateEdit_data->setFocus();
    ui->lineEdit_valor->setFocus();

    QDate data = QDate::currentDate();

    ui->dateEdit_de->setDate( QDate( data.year(),  data.month(), 1 ) );
    ui->dateEdit_ate->setDate( QDate( data.year(), data.month(), data.daysInMonth() ) );

    ui->checkBox_pordata->setChecked( false );
    on_checkBox_pordata_clicked();
    labelEspera = 0;

    CarregaAlmoco();
}

Almoco::~Almoco()
{
    delete ui;
}

void Almoco::CarregaAlmoco( bool bCarregarUltimoAlmoco )
{
    QSqlQuery sql( db );
    QString consulta;

    consulta = "select * from almoco";

    if( bCarregarUltimoAlmoco )
    {
        consulta.append( "where codigo > "
                         + ui->listWidget_lista->item( ui->listWidget_lista->count() - 1 )->text().left( 6 ) );
    }
    else
    {
        ui->listWidget_lista->clear();
        ui->listWidget_lista->addItem( "Código  Data             Valor" );
    }

    if( ui->checkBox_pordata->isChecked() )
    {
        QString sCondicao;

        if( bCarregarUltimoAlmoco )
        {
            sCondicao = " and ";
        }
        else
        {
            sCondicao = " where ";
        }

        consulta.append( sCondicao +  " data between '" + ui->dateEdit_de->date().toString( "dd-MM-yyyy" )
                         + "' and '" + ui->dateEdit_ate->date().toString( "dd-MM-yyyy" ) + "' ");
    }

    if( !sql.exec( consulta ) )
    {
        Funcoes::ErroSQL( sql, "CarregaAlmoco", this);

        return ;
    }

    while( sql.next() )
    {
        ui->listWidget_lista->addItem( FormataCodigo( sql.record().value("codigo").toString() )
                                       + "  " + sql.record().value("data").toString()
                                       + "  " + NumeroParaMoeda( sql.record().value("valor").toString() ) );
    }

    if( !sql.exec( "select sum(valor) as valor from almoco" ) )
    {
        Funcoes::ErroSQL( sql, "CarregaAlmoco", this);

        return ;
    }

    sql.next();

    ui->label_total->setText( NumeroParaMoeda( sql.record().value("valor").toString() ) );
}

void Almoco::CarregaRestaurante(bool bCarregarUltimoRestaurante)
{
    if( bCarregarUltimoRestaurante )
    {
        ui->listWidget_restaurantesCadastrados->addItem(
                    ui->lineEdit_restaurante->text() );

        return ;
    }

    QSqlQuery sql( db );
    QString consulta;

    consulta = "select nome, valor from restaurante ";

    if( !sql.exec( consulta ) )
    {
        Funcoes::ErroSQL( sql, "CarregaRestaurante", this );

        return ;
    }

    int iPosicao = 0;

    while( sql.next() )
    {
        ui->listWidget_restaurantesCadastrados->addItem( sql.record().value("nome").toString() );
        ui->listWidget_restaurantesCadastrados->item( iPosicao )->setData(
                    0, NumeroParaMoeda( sql.record().value("codigo").toString() ) );
        iPosicao++;
    }
}

void Almoco::LimparCampos()
{
    ui->lineEdit_valor->selectAll();
    ui->lineEdit_valor->setFocus();
}

QString Almoco::MoedaParaNumero(QString sValor)
{
    QString numero( sValor );
    //Ex: 3.000,33 vai ficar 3000.33
    numero = numero.replace(".", "").replace(",", ".");

    return QString( numero );
}

QString Almoco::NumeroParaMoeda(QString sValor)
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

void Almoco::Espera(bool bMostrar)
{
    if( !labelEspera == 0 )
    {
        delete labelEspera;
        labelEspera = 0;
    }

    if( bMostrar )
    {
        this->setEnabled( false );

        labelEspera = new QLabel( "Processando...", 0 );

        //Centraliza o label
        QRect rect = QApplication::desktop()->availableGeometry( labelEspera );

        labelEspera->move( rect.center() - labelEspera->rect().center()  );
    }
    else
    {
        this->setEnabled( true );
    }
}

void Almoco::on_pushButton_adicionar_clicked()
{
    QSqlQuery sql( db );
    QString consulta;
    QString sValor;

    sValor = ui->lineEdit_valor->text();

    if( sValor.isEmpty() )
    {
        ui->lineEdit_valor->setFocus();
        return ;
    }

    if( sValor.toDouble() == 0 )
    {
        sValor = MoedaParaNumero( sValor );
    }

    Espera( true );

    consulta = "insert into almoco( valor, data) values(" + sValor + ", '"
            + ui->dateEdit_data->date().toString("dd-MM-yyyy") + "')";

    if( !sql.exec( consulta ) )
    {
        Funcoes::ErroSQL( sql, "on_pushButton_adicinar_clicked", this);

        Espera( false );

        return ;
    }    

    QMessageBox::information( this, "Almoco", "Alcoço adicionado com sucesso!", "Ok");

    ui->lineEdit_valor->selectAll();
    ui->lineEdit_valor->setFocus();

    Espera( false );
}

void Almoco::on_lineEdit_valor_editingFinished()
{
    QString sValor = ui->lineEdit_valor->text();

    if( sValor.contains( "." ) && !sValor.contains( "," ) )
    {
        sValor = sValor.replace( ".", "," );
    }

    sValor = NumeroParaMoeda( sValor );

    ui->lineEdit_valor->setText( sValor );
}

void Almoco::on_pushButton_remover_clicked()
{
    if( ui->listWidget_lista->currentItem() == 0 )
        return ;
    else if( ui->listWidget_lista->currentRow() == 0 )
        return ;

    Espera( true );

    QString sCodigo = ui->listWidget_lista->currentItem()->text().left( 6 );

    QSqlQuery sql( db );
    QString consulta;

    consulta = "delete from almoco where codigo = " + sCodigo;

    if( !sql.exec( consulta ) )
    {
        Funcoes::ErroSQL( sql, "on_pushButton_remover_clicked", this);

        return ;
    }

    //ui->listWidget_lista->removeItemWidget( ui->listWidget_lista->currentItem() );
    ui->listWidget_lista->takeItem( ui->listWidget_lista->currentRow() );

    consulta = "select sum(valor) as valor from almoco";

    if( ui->checkBox_pordata->isChecked() )
    {
            consulta.append( " where data between '" + ui->dateEdit_de->date().toString( "dd-MM-yyyy" )
                             + "' and '" + ui->dateEdit_ate->date().toString( "dd-MM-yyyy" ) + "' " );
    }

    if( !sql.exec( consulta ) )
    {
        Funcoes::ErroSQL( sql, "CarregaAlmoco", this);

        Espera( false );

        return ;
    }

    sql.next();

    ui->label_total->setText( NumeroParaMoeda( sql.record().value("valor").toString() ) );

    if( ui->listWidget_lista->count() > 0 )
        ui->listWidget_lista->setCurrentRow( ui->listWidget_lista->count() -1 );

    Espera( true );

    //CarregaAlmoco();
}

void Almoco::on_checkBox_pordata_clicked()
{
    bool bHabilitar;

    if( ui->checkBox_pordata->isChecked() )
    {
        bHabilitar = true;
    }
    else
    {
        bHabilitar = false;
    }

    ui->dateEdit_de->setEnabled( bHabilitar );
    ui->dateEdit_ate->setEnabled( bHabilitar );    
}

void Almoco::on_pushButton_mostrar_clicked()
{
    Espera( true );

    CarregaAlmoco();

    Espera( false );
}

void Almoco::on_tabWidget_currentChanged(int index)
{
    if( index == 1 )
    {
        CarregaAlmoco();
    }
    else if( index == 2 )
    {
        CarregaRestaurante();
    }
}

void Almoco::on_lineEdit_valorPagoEmpresa_editingFinished()
{
    QString sValor = ui->lineEdit_valorPagoEmpresa->text();

    if( sValor.contains( "." ) && !sValor.contains( "," ) )
    {
        sValor = sValor.replace( ".", "," );
    }

    sValor = NumeroParaMoeda( sValor );

    ui->lineEdit_valorPagoEmpresa->setText( sValor );
}

void Almoco::on_pushButton_adicionarRestaurante_clicked()
{
    QString sRestaurante;

    sRestaurante = ui->lineEdit_restaurante->text();

    if( sRestaurante.isEmpty() )
    {
        ui->lineEdit_restaurante->setFocus();
        return ;
    }

    QString sValor = ui->lineEdit_valorPagoEmpresa->text();

    if( sValor.isEmpty() )
    {
        ui->lineEdit_restaurante->setFocus();
        return ;
    }

    if( sValor.toDouble() == 0 )
    {
        sValor = MoedaParaNumero( sValor );
    }

    QSqlQuery sql( db );
    QString consulta;

    Espera( true );

    consulta = "insert into restaurante( nome, valor ) values('" + sRestaurante + "', " + sValor + ")";

    if( !sql.exec( consulta ) )
    {
        Funcoes::ErroSQL( sql, "on_pushButton_adicionarRestaurante_clicked", this );

        Espera( false );

        return ;
    }

    CarregaRestaurante( true );

    ui->lineEdit_restaurante->selectAll();
    ui->lineEdit_restaurante->setFocus();

    Espera( false );
}

void Almoco::on_pushButton_removerRestaurante_clicked()
{
    if( ui->listWidget_restaurantesCadastrados->currentItem() == 0)
        return ;

    QString sRestaurante = ui->listWidget_restaurantesCadastrados->currentItem()->text();

    int iBotao = QMessageBox::question( this, "Almoco", "Tem certeza que quer remover o restaurante <br><b>"
                                        + sRestaurante + "</b></br>", "Sim", "Não");

    if( iBotao == 1 )
        return ;

    QSqlQuery sql( db );

    if( !sql.exec( "delete from restaurante where nome = '" + sRestaurante + "'"  ) )
    {
        Funcoes::ErroSQL( sql, "on_pushButton_removerRestaurante_clicked", this );

        return ;
    }

    ui->listWidget_restaurantesCadastrados->takeItem(
                ui->listWidget_restaurantesCadastrados->currentRow() );
}

void Almoco::on_listWidget_restaurantesCadastrados_clicked(const QModelIndex &index)
{
    ui->label_ValorrestauranteSelecionado->setText(  index.data( 0 ).toString()  );
}
