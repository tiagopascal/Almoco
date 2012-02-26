#include "header/almoco.h"
#include "ui_almoco.h"

#include <QMessageBox>

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

    HabilitaBotoes( true );

    operacao = NENHUMA;
    CarregaAlmoco();    
}

Almoco::~Almoco()
{
    delete ui;
}

//void Almoco::keyPressEvent(QKeyEvent *event)
//{
//    if( event->key() == Qt::Key_MediaPrevious )
//    {
//        this->close();
//    }
//}

void Almoco::HabilitaBotoes(bool bHabilitar)
{
    ui->pushButton_remover->setVisible( bHabilitar );
    ui->pushButton_editar->setVisible( bHabilitar );
    ui->listWidget_lista->setEnabled( bHabilitar );
    ui->pushButton_cancelar->setVisible( !bHabilitar );
}

void Almoco::CarregaAlmoco( bool bCarregarUltimoAlmoco )
{
    if( operacao == CARREGANDO )
        return ;

    QSqlQuery sql( db );
    QString consulta;
    QString sWhere;
    double dValorAlmoco = 0;
    double dValorVoce = 0;
    double dValorEmpresa = 0;
    bool bTodos = false;
    QString sCodigoEmpresa = ui->comboBox_restaurantesLancamento->itemData( ui->comboBox_restaurantesLancamento->currentIndex() ).toString() ;
    this->setEnabled( false );

    if( ui->comboBox_restaurantesLancamento->currentText() == "Todos" )
        bTodos = true;

    //consulta = "select almoco.*, restaurante.nome, restaurante.valor as valorRestaurante from almoco left join restaurante on almoco.restaurante = restaurante.codigo ";
    consulta = "select almoco.*, ifnull( restaurante.nome, 'Não informado') as nomerestaurante, "
            "case when restaurante.valor is null then almoco.valor else almoco.valor - restaurante.valor end as valorvoce,"
            "ifnull( restaurante.valor, 0 ) as valorempresa from almoco "
            "left join restaurante on almoco.restaurante = restaurante.codigo ";

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

        this->setEnabled( true );

        return ;
    }

    while( sql.next() )
    {
        QListWidgetItem *novoItem = new QListWidgetItem( );

        novoItem->setText( "Data:  " + sql.record().value("data").toDate().toString("dd-MM-yyyy")
                           + "\nValor: " + NumeroParaMoeda( sql.record().value("valor").toString() )
                           + "\nRestaurante: " + sql.record().value("nomerestaurante").toString() );
        novoItem->setWhatsThis( sql.record().value("codigo").toString() );
        ui->listWidget_lista->addItem( novoItem );        

        dValorAlmoco += sql.record().value("valor").toDouble();
        dValorEmpresa += sql.record().value("valorempresa").toDouble();
        dValorVoce += sql.record().value("valorvoce").toDouble();
    }

//    consulta = "select sum(valor) as valor from almoco " + sWhere;

//    if( !sql.exec( consulta ) )
//    {
//        Funcoes::ErroSQL( sql, "Almoco::CarregaAlmoco", this );

//        this->setEnabled( true );

//        return ;
//    }

//    sql.next();

//    dValorAlmoco = sql.record().value("valor").toDouble();
//    ui->label_almoco->setText( Funcoes::NumeroParaMoeda( sql.record().value("valor").toString() ) );
    ui->label_almoco->setText( Funcoes::NumeroParaMoeda( QString::number( dValorAlmoco ) ) );

//    double dValorEmpresa = 0;

//    if( bTodos )
//    {
//        dValorEmpresa = ConexaoBanco::ValorCampo( db, "restaurante",  " 1 = 1 " , "sum(valor) as valor", "valor" ).toDouble() * ui->listWidget_lista->count();
//    }
//    else
//    {
//        dValorEmpresa = ConexaoBanco::ValorCampo( db, "restaurante",  "codigo = " + sCodigoEmpresa , "valor" ).toDouble() * ui->listWidget_lista->count();
//    }

//    consulta = "select sum(valor - " + sValorEmpresa + ") as valor from almoco " + sWhere;

//    if( !sql.exec( consulta ) )
//    {
//        Funcoes::ErroSQL( sql, "Almoco::CarregaAlmoco", this );

//        this->setEnabled( true );

//        return ;
//    }

//    sql.next();

    //ui->label_voce->setText( Funcoes::NumeroParaMoeda( sql.record().value("valor").toString() ) );

    ui->label_empresa->setText( Funcoes::NumeroParaMoeda( QString::number( dValorEmpresa ) ) );

    ui->label_voce->setText( Funcoes::NumeroParaMoeda( QString::number( dValorVoce ) ) );

    //double dValorVoce = ui->label_almoco->text().toDouble() - ui->label_empresa->text().toDouble() ;

    //ui->label_voce->setText( Funcoes::NumeroParaMoeda( QString::number( dValorVoce ) ) );
    //ui->label_empresa->setText( QString::number( sValorEmpresa.toDouble() * ui->listWidget_lista->count() ) );

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

    if( operacao == EDITAR )
    {
        consulta = "update almoco set valor = " + sValor + ", data = '"
                + ui->dateEdit_data->date().toString("yyyy-MM-dd") + "', "
                + "restaurante = " +
                ui->comboBox_restaurantesLancamento->itemData(
                ui->comboBox_restaurantesLancamento->currentIndex() ).toString()
                + " where codigo = " + ui->listWidget_lista->currentItem()->whatsThis();
           HabilitaBotoes( true );
    }
    else
    {
        consulta = "insert into almoco( valor, data, restaurante) values(" + sValor + ", '"
                + ui->dateEdit_data->date().toString("yyyy-MM-dd") + "', " +
                ui->comboBox_restaurantesLancamento->itemData(
                ui->comboBox_restaurantesLancamento->currentIndex() ).toString() + ")";
    }

    if( !sql.exec( consulta ) )
    {
        Funcoes::ErroSQL( sql, "on_pushButton_adicinar_clicked", this);

        this->setEnabled( true );

        return ;
    }

    operacao = NENHUMA;

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

    int iBotao = Funcoes::MensagemAndroid( "Almoço", "Remover almoço selecionado?", "Sim", "Não", this );//QMessageBox::question( this, "", "Remover almoço selecionado?","Sim", "Não" );

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
    if( operacao == EDITAR )
        return ;

    CarregaAlmoco();
}

void Almoco::on_pushButton_editar_clicked()
{
    if( ui->listWidget_lista->currentItem()  == 0 )
        return ;

    operacao = EDITAR;

    QString sTexto = ui->listWidget_lista->currentItem()->text();
    int iDoisPontos = sTexto.indexOf( ":" );
    int iEnter = sTexto.indexOf( "\n" );

    ui->dateEdit_data->setDate( QDate::fromString( sTexto.left( iEnter -1 ) ) );

    iDoisPontos = sTexto.indexOf( ":", iEnter );
    iEnter = sTexto.indexOf( "\n", iDoisPontos );

    ui->lineEdit_valor->setText( sTexto.mid( iDoisPontos + 1, iEnter - iDoisPontos -1 ) );

    HabilitaBotoes( false );
}

void Almoco::on_pushButton_cancelar_clicked()
{
    operacao = NENHUMA;

    ui->lineEdit_valor->setText( "" );
    ui->dateEdit_data->setDate( QDate::currentDate() );

    HabilitaBotoes( true );
}

void Almoco::on_pushButton_voltar_clicked()
{
    this->close();
}
