#ifndef IHM_H
#define IHM_H

#include <QMainWindow>
#include "barriere.h"

QT_BEGIN_NAMESPACE
namespace Ui { class IHM; }
QT_END_NAMESPACE

class IHM : public QMainWindow
{
    Q_OBJECT

public:
    IHM(QWidget *parent = nullptr);
    ~IHM();


private:
    Ui::IHM *ui;
    Barriere *laBarriere = nullptr;

private slots:
    void connecterWago();
    void deconnecterWago();
    void allumerBalise();
    void eteindreBalise();
    void descendreBarriere();
    void monterBarriere();
    void lireCapteurs();

};
#endif // IHM_H
