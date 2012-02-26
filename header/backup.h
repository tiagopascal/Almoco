#ifndef BACKUP_H
#define BACKUP_H

#include <QDialog>

namespace Ui {
    class Backup;
}

class Backup : public QDialog
{
    Q_OBJECT

public:
    explicit Backup(QWidget *parent = 0);
    ~Backup();

private slots:
    void on_pushButton_voltar_clicked();

    void on_pushButton_backup_clicked();

    void on_pushButton_remover_clicked();

    void on_pushButton_restaurar_clicked();

private:
    Ui::Backup *ui;
    QString sCaminhoBackup;
    QString sCaminhoBanco;
    const QString sFormatoDataHoraArquivo;

    void CarregaBackup();
};

#endif // BACKUP_H
