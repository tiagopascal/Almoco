#ifndef ALMOCO_H
#define ALMOCO_H

#include <QDialog>
#include <QFile>

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
    void on_pushButton_adicinar_clicked();

    void on_pushButton_remover_clicked();

private:
    Ui::Almoco *ui;
    QString sCaminhoArquivoCSV;
    const size_t  iTamanhoCampo;

    QString FormataCodigo(const QString codigo,const int qtdZeros = 6);
    QString NumeroParaMoeda(double dValor);
    QString MoedaParaNumero(double dValor);
    void CriarBanco();
    void CarregaAlmoco();
    void LimparCampos();

};

#endif // ALMOCO_H
