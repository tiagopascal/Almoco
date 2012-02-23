#ifndef MENU_H
#define MENU_H

#include <QDialog>
#include <QCloseEvent>

namespace Ui {
    class Menu;
}

class Menu : public QDialog
{
    Q_OBJECT

public:
    explicit Menu(QWidget *parent = 0);
    ~Menu();

private slots:

    void on_pushButton_cadRestaurante_clicked();

    void on_pushButton_cadAlmoco_clicked();

    void on_pushButton_Sair_clicked();

protected:
    void keyPressEvent(QKeyEvent *event);

private:
    Ui::Menu *ui;
    bool bFechar;
};

#endif // MENU_H
