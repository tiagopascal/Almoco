#include "header/menu.h"
#include "ui_menu.h"
#include "header/almoco.h"
#include "header/restaurante.h"
#include "header/ConexaoBancoAtendimento.h"

Menu::Menu(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Menu)
{
    ui->setupUi(this);
    setWindowTitle( "Principal" );

    bFechar = false;

    ConexaoBanco::ConectaBanco( "QSQLITE" );
}

Menu::~Menu()
{
    delete ui;
}

void Menu::keyPressEvent(QKeyEvent *event)
{
    if( event->key()  == Qt::Key_MediaPrevious )
    {
        this->close();
    }
}

void Menu::on_pushButton_cadRestaurante_clicked()
{
    Restaurante restaurante( this );

    //this->setEnabled( false );

    restaurante.exec();

    //this->setEnabled( true );
}

void Menu::on_pushButton_cadAlmoco_clicked()
{
    Almoco almoco( this );

    //setVisible( false );

    //almoco.setWindowModality( Qt::ApplicationModal );
    almoco.exec();

    //setVisible( true );
}

void Menu::on_pushButton_Sair_clicked()
{
    bFechar = true;

    this->close();
}
