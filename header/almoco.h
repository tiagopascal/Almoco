#ifndef ALMOCO_H
#define ALMOCO_H

#include <QDialog>
//Banco
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlDriver>
#include <QtSql/QSqlError>
#include <QtSql/QSqlRecord>
#include <QLabel>

#include <header/funcoes.h>

namespace Ui {
    class Almoco;
}

class Almoco : public QDialog
{
    Q_OBJECT

public:
    explicit Almoco(QWidget *parent = 0);
    ~Almoco();

private slots:


    void on_pushButton_adicionar_clicked();

    void on_lineEdit_valor_editingFinished();

    void on_pushButton_remover_clicked();

    void on_checkBox_pordata_clicked();

    void on_pushButton_mostrar_clicked();

    void on_tabWidget_currentChanged(int index);

    void on_lineEdit_valorPagoEmpresa_editingFinished();

    void on_pushButton_adicionarRestaurante_clicked();

    void on_pushButton_removerRestaurante_clicked();

    void on_listWidget_restaurantesCadastrados_clicked(const QModelIndex &index);

private:
    Ui::Almoco *ui;
    QLabel *labelEspera;
    QSqlDatabase db;

    QString FormataCodigo(const QString codigo,const int qtdZeros = 6);
    QString NumeroParaMoeda(QString sValor);
    QString MoedaParaNumero(QString Valor);

    void CarregaAlmoco( bool bCarregarUltimoAlmoco = false );
    void CarregaRestaurante( bool bCarregarUltimoRestaurante = false );
    void LimparCampos();    
    void Espera( bool bMostrar = false );
};

#endif // ALMOCO_H
