#include "header/almoco.h"
#include "ui_almoco.h"

#include <QDir>
#include <QTextStream>
#include <QMessageBox>
#include <QDesktopWidget>

Almoco::Almoco(QWidget *parent) :
    QDialog(parent), ui(new Ui::Almoco)
{
    ui->setupUi(this);

    operacao = CARREGANDO;

    db = ConexaoBanco::Banco();

    QSqlQuery sql( db );

    if( !sql.exec( "select nome, codigo from restaurante " ) )
    {
        Funcoes::ErroSQL( sql, "Almoco::Almoco" );

        return ;
    }

    ui->comboBox_restaurantesLancamento->clear();    

    int iPosicao = 0;

    while( sql.next() )
    {
        ui->comboBox_restaurantesLancamento->addItem( sql.record().value("nome").toString() );
        ui->comboBox_restaurantesLancamento->setItemData( iPosicao, sql.record().value("codigo") );
        iPosicao++;
    }

    ui->comboBox_restaurantesLancamento->addItem( "Todos" );
    ui->comboBox_restaurantesLancamento->setItemData( iPosicao, QVariant( 0 ) );

    QDate dataAtual ( QDate::currentDate() );

    int iMes;

    if( dataAtual.month() == 1 )
        iMes = 12;
    else
        iMes = dataAtual.month() - 1;

    ui->dateEdit_filtro->setDate( QDate( dataAtual.year(), iMes, 20 ) );
    ui->dateEdit_data->setDate( dataAtual );
    ui->dateEdit_filtroAte->setDate( dataAtual );

    operacao = NENHUMA;
    CarregaAlmoco();
}

Almoco::~Almoco()
{
    delete ui;
}

void Almoco::keyPressEvent(QKeyEvent *event)
{        
//    if( event->key() == Qt::Key_MediaPrevious )
//    {
//        this->close();
//    }
}

void Almoco::CarregaAlmoco( bool bCarregarUltimoAlmoco )
{
    if( operacao == CARREGANDO )
        return ;

    QSqlQuery sql( db );
    QString consulta;
    QString sWhere;
    bool bTodos = false;
    QString sCodigoEmpresa = ui->comboBox_restaurantesLancamento->itemData( ui->comboBox_restaurantesLancamento->currentIndex() ).toString() ;
    this->setEnabled( false );

    if( ui->comboBox_restaurantesLancamento->currentText() == "Todos" )
        bTodos = true;

    consulta = "select almoco.*, restaurante.nome, restaurante.valor as valorRestaurante from almoco left join restaurante on almoco.restaurante = restaurante.codigo ";

    if( bTodos )
    {
        sWhere = "";
        //sWhere = " where almoco.data between '" + ui->dateEdit_filtro->date().toString("dd-MM-yyyy") + "' and '" + ui->dateEdit_filtroAte->date().toString("dd-MM-yyyy") + "' " ;
    }
    else
    {
        if( sCodigoEmpresa.isEmpty() )
            sCodigoEmpresa = "0";

        sWhere = " where almoco.restaurante = " + sCodigoEmpresa ;//+ " and almoco.data between '" + ui->dateEdit_filtro->date().toString("dd-MM-yyyy") + "' and '" + ui->dateEdit_filtroAte->date().toString("dd-MM-yyyy") + "' ";
    }

    consulta += sWhere ;

    if( bCarregarUltimoAlmoco and !bTodos )
    {
        int iQtd = ui->listWidget_lista->count();

        if( iQtd > 0 )
        {
            consulta.append( " and almoco.codigo > " + ui->listWidget_lista->item( iQtd - 1 )->whatsThis() );
        }
    }
    else
    {
        ui->listWidget_lista->clear();
    }

    if( !sql.exec( consulta ) )
    {
        Funcoes::ErroSQL( sql, "CarregaAlmoco", this);

        QMessageBox::information( this, "", sql.lastError().text(), "ok");

        this->setEnabled( true );

        return ;
    }

    while( sql.next() )
    {
        QListWidgetItem *novoItem = new QListWidgetItem( );

        novoItem->setText( "Data:  " + sql.record().value("data").toString()
                           + "\nValor: " + NumeroParaMoeda( sql.record().value("valor").toString() )
                           + "\nRestaurante: " + sql.record().value("nome").toString() );
        novoItem->setWhatsThis( sql.record().value("codigo").toString() );
        ui->listWidget_lista->addItem( novoItem );
    }

    consulta = "select sum(valor) as valor from almoco " + sWhere;

    if( !sql.exec( consulta ) )
    {
        Funcoes::ErroSQL( sql, "Almoco::CarregaAlmoco", this );

        this->setEnabled( true );

        return ;
    }

    sql.next();

    ui->label_almoco->setText( Funcoes::NumeroParaMoeda( sql.record().value("valor").toString() ) );

    QString sValorEmpresa;

    if( bTodos )
    {
        sValorEmpresa = ConexaoBanco::ValorCampo( db, "restaurante",  " 1 = 1 " , "sum(valor) as valor", "valor" );
    }
    else
    {
        sValorEmpresa = ConexaoBanco::ValorCampo( db, "restaurante",  "codigo = " + sCodigoEmpresa , "valor" );
    }

    consulta = "select sum(valor - " + sValorEmpresa + ") as valor from almoco " + sWhere;

    if( !sql.exec( consulta ) )
    {
        Funcoes::ErroSQL( sql, "Almoco::CarregaAlmoco", this );

        this->setEnabled( true );

        return ;
    }

    sql.next();

    ui->label_voce->setText( Funcoes::NumeroParaMoeda( sql.record().value("valor").toString() ) );

    //double dValorVoce = ui->label_almoco->text().toDouble() - ui->label_empresa->text().toDouble() ;

    //ui->label_voce->setText( Funcoes::NumeroParaMoeda( QString::number( dValorVoce ) ) );
    ui->label_empresa->setText( QString::number( sValorEmpresa.toDouble() * ui->listWidget_lista->count() ) );

    this->setEnabled( true );
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

    this->setEnabled( false  );

    consulta = "insert into almoco( valor, data, restaurante) values(" + sValor + ", '"
            + ui->dateEdit_data->date().toString("dd-MM-yyyy") + "', " +
            ui->comboBox_restaurantesLancamento->itemData(
            ui->comboBox_restaurantesLancamento->currentIndex() ).toString() + ")";

    if( !sql.exec( consulta ) )
    {
        Funcoes::ErroSQL( sql, "on_pushButton_adicinar_clicked", this);

        this->setEnabled( true );

        return ;
    }

    CarregaAlmoco();

    this->setEnabled( true );
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

    int iBotao = QMessageBox::question( this, "", "Remover almoço selecionado?","Sim", "Não" );

    if( iBotao == 1 )
        return ;

    this->setEnabled( false  );

    QString sCodigo = ui->listWidget_lista->currentItem()->whatsThis();

    QSqlQuery sql( db );
    QString consulta;

    consulta = "delete from almoco where codigo = " + sCodigo;

    if( !sql.exec( consulta ) )
    {
        Funcoes::ErroSQL( sql, "on_pushButton_remover_clicked", this);

        this->setEnabled( true );

        return ;
    }

    //ui->listWidget_lista->removeItemWidget( ui->listWidget_lista->currentItem() );
    ui->listWidget_lista->takeItem( ui->listWidget_lista->currentRow() );

    if( ui->listWidget_lista->count() > 0 )
        ui->listWidget_lista->setCurrentRow( ui->listWidget_lista->count() -1 );

    CarregaAlmoco();

    this->setEnabled( true  );
}


void Almoco::on_comboBox_restaurantesLancamento_currentIndexChanged(int index)
{
    CarregaAlmoco();
}
