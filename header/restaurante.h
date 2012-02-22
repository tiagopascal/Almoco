#ifndef RESTAURANTE_H
#define RESTAURANTE_H

#include <QDialog>
#include <QKeyEvent>

namespace Ui {
    class Restaurante;
}

class Restaurante : public QDialog
{
    Q_OBJECT

public:
    explicit Restaurante(QWidget *parent = 0);
    ~Restaurante();

private slots:
    void on_lineEdit_valor_editingFinished();

    void on_pushButton_adicionar_clicked();

    void on_pushButton_remover_clicked();

    void on_pushButton_editar_clicked();

    void on_pushButton_cancelar_clicked();

private:
    void keyPressEvent(QKeyEvent *event);

private:
    Ui::Restaurante *ui;

    enum Operacao { NENHUMA, EDITAR };

    Operacao operacao;

    void CarregaRestaurante(bool bCarregarUltimoRestaurante );
    void HabilitarCampos( bool bHabilitar );
};

#endif // RESTAURANTE_H
