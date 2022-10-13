#include "barriere.h"

Barriere::Barriere() {
}

Barriere::~Barriere() {
    if(connexionTCP != nullptr) {
        connexionTCP->deconnecter();
        delete connexionTCP;
        connexionTCP = nullptr;
    }
}

void Barriere::connexionWago(QString adresseIP)
{
    connexionTCP = new EsclaveModbusTCP(adresseIP);
        if (connexionTCP->connecter() != 0 ) {
            throw TcpException(CONNECT_ERREUR, "Erreur lors de la connection au Wago");
        }
}

void Barriere::deconnectionWago() {
    connexionTCP->deconnecter();
}

void Barriere::allumer() {
    int resultat;
    QString message;
    if ((resultat = connexionTCP->ecrire1Bit(ADRESSE_BALISE, 1)) == (TRANSMISSION_ERREUR | TRANSACTION_TIMEOUT_ERREUR)) {
        if (resultat == TRANSMISSION_ERREUR) {
            message = "Erreur lors de la transmission";
        }
        else {
            message = "Erreur transaction timeout";
        }
        throw TcpException(resultat, message);
    }
}

void Barriere::eteindre() {
    int resultat;
    QString message;
    if ((resultat = connexionTCP->ecrire1Bit(ADRESSE_BALISE, 0)) == (TRANSMISSION_ERREUR | TRANSACTION_TIMEOUT_ERREUR)) {
        if (resultat == TRANSMISSION_ERREUR) {
            message = "Erreur lors de la transmission";
        }
        else {
            message = "Erreur transaction timeout";
        }
        throw TcpException(resultat, message);
    }
}

void Barriere::monter() {
    int resultat;
    QString message;
    this->lectureCapteurs();
    this->allumer();
    if (tabCapteurs[1] == 1 & tabCapteurs[7] == 1) {

        if ((resultat = connexionTCP->ecrire1Bit(ADRESSE_DESCENDRE, 0) | connexionTCP->ecrire1Bit(ADRESSE_MONTER, 1)) == (TRANSMISSION_ERREUR | TRANSACTION_TIMEOUT_ERREUR)) {
            if (resultat == TRANSMISSION_ERREUR) {
                message = "Erreur lors de la transmission";
            }
            else {
                message = "Erreur transaction timeout";
            }
            throw TcpException(resultat, message);
        }
        this->lectureCapteurs();
        while (tabCapteurs[0] == 0) {
            this->lectureCapteurs();
        this->eteindre();
        }

    }
}
void Barriere::descendre() {
    int resultat;
    QString message;
    this->lectureCapteurs();
    allumer();
    if (tabCapteurs[0] == 1 & tabCapteurs[7] == 1) {
        if ((resultat = connexionTCP->ecrire1Bit(ADRESSE_MONTER, 0) | connexionTCP->ecrire1Bit(ADRESSE_DESCENDRE, 1)) == (TRANSMISSION_ERREUR | TRANSACTION_TIMEOUT_ERREUR)) {

            if (resultat == TRANSMISSION_ERREUR) {
                message = "Erreur lors de la transmission";
            }
            else {
                message = "Erreur transaction timeout";
            }
            throw TcpException(resultat, message);
        }
        this->lectureCapteurs();
        while (tabCapteurs[1] == 0) {
            this->lectureCapteurs();
        this->eteindre();
        }

    }
}

QString Barriere::lectureCapteurs() {
    unsigned short capteurs;
    int resultat;
    QString message;
    QString lesCapteurs;
     resultat = connexionTCP->lireNMots(ADRESSE_CAPTEURS, 1, &capteurs);
    if (resultat == (TRANSMISSION_ERREUR | TRANSACTION_TIMEOUT_ERREUR)) {
        if (resultat == TRANSMISSION_ERREUR) {
            message = "Erreur lors de la transmission";
        }
        else {
            message = "Erreur transaction timeout";
        }
        throw TcpException(resultat, message);
    } else {
        for (int i = 0; i < 8; i++) {
            if ((capteurs & (1<<i)) != 0) {
                lesCapteurs = lesCapteurs + labelEntrees[i] + " - ";
                tabCapteurs[i] = 1;
            }
        }
    }
    return lesCapteurs;
}

int Barriere::getStatutBarriere() {
    if (tabCapteurs[0] == 1)
        return 1;
    else
        return 0;
}
