#include "header/restaurante.h"
#include "header/funcoes.h"
#include "header/ConexaoBancoAtendimento.h"
#include "ui_restaurante.h"

//Banco
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlDriver>
#include <QtSql/QSqlError>
#include <QtSql/QSqlRecord>
#include <QVector>

Restaurante::Restaurante(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Restaurante)
{
    ui->setupUi(this);

    HabilitarCampos( true );

    CarregaRestaurante( false );

    operacao = NENHUMA;
}

Restaurante::~Restaurante()
{
    delete ui;
}

void Restaurante::keyPressEvent(QKeyEvent *event)
{
    if( event->key() == Qt::Key_MediaPrevious )
    {
        this->close();
    }
}

void Restaurante::CarregaRestaurante(bool bCarregarUltimoRestaurante)
{
    QString sRestaurante = ui->lineEdit_restaurante->text();
    QString consulta;

    consulta = "select codigo, nome, valor from restaurante ";

    if( bCarregarUltimoRestaurante )
    {
        int iQtd = ui->listWidget_lista->count();

        if( iQtd > 0 )
        {
            consulta.append( " where codigo > " + ui->listWidget_lista->item( iQtd - 1 )->whatsThis() );
        }
    }
    else
    {
        ui->listWidget_lista->clear();
    }

    this->setEnabled( false );

    QSqlQuery sql( ConexaoBanco::Banco() );

    if( !sql.exec( consulta ) )
    {
        Funcoes::ErroSQL( sql, "CarregaRestaurante", this );

        this->setEnabled( true );

        return ;
    }

    int iPosicao = 0;

    while( sql.next() )
    {
        QListWidgetItem *novoItem = new QListWidgetItem( );

        sRestaurante = sql.record().value("nome").toString();

        novoItem->setText( sRestaurante + "\nValor: " + Funcoes::NumeroParaMoeda( sql.record().value("valor").toString() )  );
        novoItem->setWhatsThis( sql.record().value("codigo").toString() );

        ui->listWidget_lista->addItem( novoItem );

        iPosicao++;
    }

    this->setEnabled( true );
}

void Restaurante::on_lineEdit_valor_editingFinished()
{
    QString sValor = ui->lineEdit_valor->text();

    if( sValor.contains( "." ) && !sValor.contains( "," ) )
    {
        sValor = sValor.replace( ".", "," );
    }

    sValor = Funcoes::NumeroParaMoeda( sValor );

    ui->lineEdit_valor->setText( sValor );
}

void Restaurante::on_pushButton_adicionar_clicked()
{
    QString sRestaurante;

    sRestaurante = ui->lineEdit_restaurante->text();

    if( sRestaurante.isEmpty() )
    {
        ui->lineEdit_restaurante->setFocus();

        return ;
    }

    QString sValor = ui->lineEdit_valor->text();

    if( sValor.isEmpty() )
    {
        ui->lineEdit_restaurante->setFocus();

        return ;
    }

    if( sValor.toDouble() == 0 )
    {
        sValor = Funcoes::MoedaParaNumero( sValor );
    }

    QSqlQuery sql( ConexaoBanco::Banco() );
    QString consulta;

    this->setEnabled( false  );

    if( operacao == EDITAR )
        consulta = "update restaurante set nome = '" + sRestaurante + "', valor = " +  sValor + " where codigo =  " + ui->listWidget_lista->currentItem()->whatsThis();
    else
        consulta = "insert into restaurante( nome, valor ) values('" + sRestaurante + "', " + sValor + ")";

    if( !sql.exec( consulta ) )
    {
        Funcoes::ErroSQL( sql, "Restaurante::on_pushButton_adicionar_clicked()", this );

        this->setEnabled( true );

        return ;
    }

    CarregaRestaurante( false );
    operacao = NENHUMA;
    HabilitarCampos( true );
    this->setEnabled( true );
}

void Restaurante::on_pushButton_remover_clicked()
{
    if( ui->listWidget_lista->currentItem() == 0)
        return ;

    int iBotao = QMessageBox::question( this, "Almoco", "Deseja remover o restuarante selecionado?", "Sim", "Não");

    if( iBotao == 1 )
        return ;

    QString sCodigo = ui->listWidget_lista->currentItem()->whatsThis();

    QSqlQuery sql( ConexaoBanco::Banco() );

    if( !sql.exec( "delete from restaurante where codigo = " + sCodigo ) )
    {
        Funcoes::ErroSQL( sql, "on_pushButton_remover_clicked", this );

        return ;
    }

    ui->listWidget_lista->takeItem(
                ui->listWidget_lista->currentRow() );
}

void Restaurante::on_pushButton_editar_clicked()
{
    if( ui->listWidget_lista->currentItem() == 0 )
        return ;

    operacao = EDITAR;

    QString sTexto = ui->listWidget_lista->currentItem()->text();
    int iDoisPontos = sTexto.indexOf( ":" );
    int iEnter = sTexto.indexOf( "\n" );

    ui->lineEdit_restaurante->setText( sTexto.left( iEnter -1 ) );

    ui->lineEdit_valor->setText( sTexto.mid( iDoisPontos + 1 ) );

    HabilitarCampos( false );
}

void Restaurante::HabilitarCampos(bool bHabilitar)
{
    ui->pushButton_remover->setVisible( bHabilitar );
    ui->pushButton_editar->setVisible( bHabilitar );
    ui->pushButton_cancelar->setVisible( !bHabilitar );
    ui->listWidget_lista->setEnabled( bHabilitar );
}

void Restaurante::on_pushButton_cancelar_clicked()
{
    ui->lineEdit_restaurante->setText( "" );
    ui->lineEdit_valor->setText( "" );

    HabilitarCampos( true );
    operacao = NENHUMA;
}
