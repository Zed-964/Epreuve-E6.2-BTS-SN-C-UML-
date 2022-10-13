#ifndef ESCLAVEMODBUSTCP_H
#define ESCLAVEMODBUSTCP_H

#include <QTcpSocket>
#include <QString>

// LG pour LONGUEUR_TRAME
const unsigned int PORT_MODBUS_TCP = 502;
const unsigned int LG_MBAP = 7;

const unsigned int FONCTION_ECRIRE1BIT = 5;
const unsigned int LG_DEMANDE_ECRIRE1BIT = LG_MBAP+5;
const unsigned int LG_REPONSE_ECRIRE1BIT = LG_MBAP+5;

const unsigned int FONCTION_ECRIRE1MOT = 6;
const unsigned int LG_DEMANDE_ECRIRE1MOT = LG_MBAP+5;
const unsigned int LG_REPONSE_ECRIRE1MOT = LG_MBAP+5;

const unsigned char FONCTION_LECTURENMOTS = 3;
const unsigned int LG_DEMANDE_LECTURENMOTS = LG_MBAP+5;
const unsigned int LG_REPONSE_LECTURENMOTS = LG_MBAP+2;

const unsigned char FONCTION_ECRIRENMOTS = 16;
const unsigned int LG_DEMANDE_ECRIRENMOTS = LG_MBAP+6;
const unsigned int LG_REPONSE_ECRIRENMOTS = LG_MBAP+5;

const unsigned int LG_ERREUR = LG_MBAP+2;
const unsigned int TIMEOUT = 2000;

const int CONNECT_OK = 0;
const int CONNECT_ERREUR = -300;
const int TRANSMISSION_OK = 0;
const int TRANSMISSION_ERREUR = -301;
const int TRANSACTION_OK = 0;
const int TRANSACTION_TIMEOUT_ERREUR = -302;

class EsclaveModbusTCP {
private:
    QString         _adresseIP;
    int             _numEsclave;
    QTcpSocket      _socket;
    unsigned short  _transactionId;

    unsigned char*  _trameDemande;
    unsigned char*  _trameReponse;
    unsigned char	_fonctionModbus;

    void    remplirMBAP         (int lgTrame);
    int     envoyerTrameDemande (int nbOctets);
    int     recevoirTrameReponse(int nbOctets);
    void    afficher            (unsigned char* trame, int nbOctets);

public:
    EsclaveModbusTCP    (QString adresseIP, int numEsclave=0xFF);
    ~EsclaveModbusTCP   ();
    int     connecter   ();
    void    deconnecter ();
    int     ecrire1Bit  (unsigned short adresse, int valeur);
    int     ecrire1Mot  (unsigned short adresse, unsigned short valeur);
    int     lireNMots   (unsigned short adresse, unsigned int nbMots, unsigned short* tabMots);
    int     ecrireNMots (unsigned short adresse, unsigned int nbMots, unsigned short* tabMots);
};

class TcpException {
private:
    int code;
    QString message;

public:
    TcpException(int code, QString message);
    QString obtenirErreur(TcpException &e);
    int getCode();
    QString getMessage();
};

#endif // ESCLAVEMODBUSTCP_H
