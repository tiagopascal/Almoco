#ifndef FUNCOES_H
#define FUNCOES_H

#include <QApplication>
#include <QString>
#include <QDesktopWidget>//Para a função "centralizarWidget"
#include <QDate>
#include <QMessageBox>
#include <QSettings>
#include <QFile>
#include <QVector>
#include <QComboBox>
#include <QMainWindow>
#include <QTime>
#include <QSqlQuery>

namespace Funcoes
{
    QString SistemaOperacional();
    void centralizarWidget(QWidget *widget, bool deixarRedimensionar = false);
    void centralizarWidget(QWidget &widget, bool deixarRedimensionar = false);
    QString FormataCodigo(QString codigo, int qtdZeros = 6);
    QDate FormataData(QString data, QString tipoBanco = "QSQLITE");
    //QString FormataDataTexto(QDate data, char separador = '/');
    QString IP_LocalHost();

    QString LeLog(QString caminhoArquivo, QString sessao, QString chave = "" , QString valorDefault = "");
    bool GravaLog(QString caminhoArquivo, QString sessao, QString chave = "" , QString valor = "");

    bool CriarArquivo(QString caminhoArquivo);
    QString MontaQueryInsert(QString nomeTabela, const QVector<QVector<QString> > query);
    QString MontaQueryUpdate(QString nomeTabela, QVector<QVector<QString> > query, QString condicao);
    void TemNovaVersao();
    QString NovaVersao();
    bool AtualizaSistema();
    void RenomearArquivoAtualizacao();
    QString EscolheVersaoMaisNova(QString caminho);
    bool AtualizaDlls(QString caminho);
    QString NumeroVersaoAtualExecutavel();
    void CarregaCombo(QComboBox &combo, QString tabela, QString campo, QString valorPrimeiroIndice = "", QString campoItemData = "", bool limparComboAntes = false, QString condicao = "");

    int PedeMinuto(int hora);
    int PedeSegundo(int minuto);
    QString SubstituirBarraInvertida(const QByteArray &string);

    QString CriptografarSenha(const QString senha);

    QTime DiferencaEntreHoras(QTime hora1, QTime hora2 = QTime::currentTime() );

    void ErroSQL( QSqlQuery sql, QString rotina, QWidget *tela = 0 );

    void IconeAplicativo();
    void copyFolder(QString sourceFolder, QString destFolder);
    void LimparPonteiros();
    QString MoedaParaNumero(QString sValor);
    QString NumeroParaMoeda(QString sValor);
}
#endif // FUNCOES_H
