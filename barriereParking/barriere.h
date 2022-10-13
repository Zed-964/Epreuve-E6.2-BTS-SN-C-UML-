#ifndef BARRIERE_H
#define BARRIERE_H

#include <QString>
#include "esclavemodbustcp.h"

class Barriere
{
public:


    Barriere();
    ~Barriere();
    void connexionWago(QString adresseIP);
    void deconnectionWago();
    void allumer();
    void eteindre();
    void monter();
    void descendre();
    QString lectureCapteurs();
    int getStatutBarriere();

private:

    EsclaveModbusTCP* connexionTCP = nullptr;
    int tabCapteurs[8] = {0,0,0,0,0,0,0,0};

    const QString labelEntrees[8] = {"FDCHaut", "FDCBas", "Boucle 1", "Boucle 2", "Ticket 4", "POM", "Clavier", "KA1"};
    const unsigned short ADRESSE_CAPTEURS = 0x0000;
    const unsigned short ADRESSE_MONTER = 0x0200;
    const unsigned short ADRESSE_DESCENDRE = 0x0201;
    const unsigned short ADRESSE_BALISE = 0x0202;

};
#endif //BARRIERE_H
