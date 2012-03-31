#ifndef CONEXAOBANCOATENDIMENTO_H
#define CONEXAOBANCOATENDIMENTO_H

#include <QString>
#include <QDate>
#include <QSqlDatabase>
#include <QMap>

namespace ConexaoBanco
{
    void CriaUsuarioPadrao();
    bool BancoSqLite();
//    bool BancoMysql();
    bool ConectaBanco( const QString &sTipo = QString( "QSQLITE" ) );
//    void ExportaBancoParaCSV(QString dataInicial, QString dataFinal, QString sCondicao);
//    void ImportaCSV();
    QString MontaQueryInsert(const QString &nomeTabela, const QMap<QString, QString> &query);
    QString MontaQueryUpdate(const QString &nomeTabela, const QMap<QString, QString> &query, QString condicao);
    QString SelecionarArquivo(QString tipoArquivo, QString diretorio);
//    void BackUpBancoSQLite(bool bMensagem = true);
    //bool ConfiguraIniBackupSQLite(bool backupLocal = true, bool backupServidor = true);
    QString CriaPastaBanco();

    bool VerificaSeArquivoExiste(QString caminhoArquivo);
    bool VerificaSeCaminhoExiste(QString caminho);
//    QString CamposTabela(QSqlDatabase dbBanco, QString tabela);
//    bool AdicionarCampoTabela(QString tabela, QStringList novosCamposEtipo, char separador = ',', bool transacao = false);
//    bool RemoverCampoTabela(QString tabela, QString campoRemover, char separador = ',', bool transacao = false);
    bool VerificaSeCampoExiste(QString tabela, QString campoVerificar, bool transacao = false);
//    void RemoverTabelasTemp(QString tabela = "", int qtd = 10, bool transacao = false);
//    bool AlterarTipoCampoTabela(QString tabela, QString campoNovoTipo, bool transacao = false);
    bool CriarTabelas(QSqlDatabase dbBanco, bool transacao = false);
    void FecharBanco();
    QString CaminhoBanco();
    bool AtualizaEstruturaDoBanco();
    bool AtualizaTabelaVersao(QString versao, QDate data);

    QSqlDatabase Banco();

    QString ValorCampo( QSqlDatabase dbBanco , QString tabela, QString condicao, QString campoRetorno, QString alias = "", bool bLimit1 = true );
}
#endif // CONEXAOBANCOATENDIMENTO_H
