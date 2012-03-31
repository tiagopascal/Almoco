#ifndef ALMOCO_H
#define ALMOCO_H

#include "header/ConexaoBancoAtendimento.h"
#include <header/funcoes.h>
#include <QDialog>
//Banco
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlDriver>
#include <QtSql/QSqlError>
#include <QtSql/QSqlRecord>
//#include <QCloseEvent>
//#include <QKeyEvent>

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

    void on_comboBox_restaurantesLancamento_currentIndexChanged(int index);

    void on_pushButton_editar_clicked();

    void on_pushButton_cancelar_clicked();

    void on_pushButton_voltar_clicked();

    void on_dateEdit_filtro_editingFinished();

    void on_dateEdit_filtroAte_editingFinished();

    void on_pushButton_salvar_clicked();

protected:
//    void keyPressEvent(QKeyEvent *event);


private:
    Ui::Almoco *ui;    
    QSqlDatabase db;

    enum Operacao{ NENHUMA, CARREGANDO, EDITAR };

    Operacao operacao;
    QString FormataCodigo(const QString codigo,const int qtdZeros = 6);
    QString NumeroParaMoeda(QString sValor);
    QString MoedaParaNumero(QString Valor);
    void LimparCampos();

    void CarregaAlmoco( bool bCarregarUltimoAlmoco = false );
    void HabilitaBotoes( bool bHabilitar );
    //void Espera( bool bMostrar = false );
};

#endif // ALMOCO_H
