#include "esclavemodbustcp.h"

EsclaveModbusTCP::EsclaveModbusTCP(QString adresseIP, int numEsclave)
{
    _adresseIP     = adresseIP;
    _numEsclave	   = numEsclave;
    _transactionId = 1;
}

EsclaveModbusTCP::~EsclaveModbusTCP()
{
    deconnecter();
}

int EsclaveModbusTCP::connecter()
{
    if (_socket.state() != QTcpSocket::ConnectedState) {
        _socket.connectToHost(_adresseIP, PORT_MODBUS_TCP);
        _socket.waitForConnected(2000);
        if(_socket.state() != QAbstractSocket::ConnectedState) {
            qDebug()<<"Non connecté";
            _socket.close();
            return CONNECT_ERREUR;
        }
        else {
            qDebug()<<"Connection OK";
            return CONNECT_OK;
        }
    } else {
        qDebug()<<"Connection déjà effectuée";
        return CONNECT_OK;
    }
}

void EsclaveModbusTCP::deconnecter()
{
    if(_socket.state() != QTcpSocket::UnconnectedState) {
        _socket.disconnectFromHost();
        qDebug()<<"Déconnexion";
    }
}

int EsclaveModbusTCP::ecrire1Bit(unsigned short adresse, int valeur)
{
    _trameDemande = new unsigned char[LG_DEMANDE_ECRIRE1BIT]; // 12
    _trameReponse = new unsigned char[LG_REPONSE_ECRIRE1BIT]; // 12

    _fonctionModbus = FONCTION_ECRIRE1BIT; // 5
    remplirMBAP(LG_DEMANDE_ECRIRE1BIT); // 12

    _trameDemande[7] = _fonctionModbus; // 5
    _trameDemande[8] = adresse>>8 & 0x00FF;
    _trameDemande[9] = adresse    & 0x00FF;

    if(valeur>0)
        valeur = 0xFF;
    else
        valeur=0;
    _trameDemande[10] = valeur;
    _trameDemande[11] = 0;

    int statut;
    statut = envoyerTrameDemande(LG_DEMANDE_ECRIRE1BIT);
    if (statut == TRANSMISSION_ERREUR)
        return statut;

    statut = recevoirTrameReponse(LG_REPONSE_ECRIRE1BIT);

    delete [] _trameDemande;
    delete [] _trameReponse;

    return statut;
}

int EsclaveModbusTCP::ecrire1Mot(unsigned short adresse, unsigned short valeur)
{
    _trameDemande = new unsigned char[LG_DEMANDE_ECRIRE1MOT];
    _trameReponse = new unsigned char[LG_REPONSE_ECRIRE1MOT];

    _fonctionModbus = FONCTION_ECRIRE1MOT;
    remplirMBAP(LG_DEMANDE_ECRIRE1MOT);

    _trameDemande[7] = _fonctionModbus;
    _trameDemande[8] = adresse>>8 & 0x00FF;
    _trameDemande[9] = adresse    & 0x00FF;

    _trameDemande[10] = valeur>>8 & 0X00FF;
    _trameDemande[11] = valeur    & 0x00FF;

    int statut;
    statut = envoyerTrameDemande(LG_DEMANDE_ECRIRE1MOT);
    if (statut == TRANSMISSION_ERREUR)
        return statut;

    statut = recevoirTrameReponse(LG_REPONSE_ECRIRE1MOT);

    delete [] _trameDemande;
    delete [] _trameReponse;

    return statut;
}

int EsclaveModbusTCP::lireNMots(unsigned short adresse, unsigned int nbMots, unsigned short *tabMots)
{
    _trameDemande = new unsigned char[LG_DEMANDE_LECTURENMOTS];
    unsigned int nbOctetsReponse = LG_REPONSE_LECTURENMOTS+nbMots*2;
    _trameReponse = new unsigned char[nbOctetsReponse];

    _fonctionModbus = FONCTION_LECTURENMOTS;
    remplirMBAP(LG_DEMANDE_LECTURENMOTS);

    _trameDemande[7] = _fonctionModbus;
    _trameDemande[8] = adresse>>8 & 0x00FF;
    _trameDemande[9] = adresse    & 0x00FF;

    _trameDemande[10] = nbMots>>8 & 0x00FF;
    _trameDemande[11] = nbMots    & 0x00FF;

    int statut;
    statut = envoyerTrameDemande(LG_DEMANDE_LECTURENMOTS);
    if (statut == TRANSMISSION_ERREUR)
        return statut;

    statut = recevoirTrameReponse(nbOctetsReponse);

    if(statut == TRANSACTION_OK){
        for(unsigned int i=0; i<nbMots; i++){
            tabMots[i]=static_cast<quint16>(_trameReponse[2*i+9]<<8 | _trameReponse[2*i+10]);
        }
    }

    delete [] _trameDemande;
    delete [] _trameReponse;

    return statut;
}

int EsclaveModbusTCP::ecrireNMots(unsigned short adresse, unsigned int nbMots, unsigned short *tabMots)
{
    unsigned int nbOctetsDemande = LG_DEMANDE_ECRIRENMOTS+nbMots*2;
    _trameDemande = new unsigned char[nbOctetsDemande];
    _trameReponse = new unsigned char[LG_REPONSE_ECRIRENMOTS];

    _fonctionModbus = FONCTION_ECRIRENMOTS;
    remplirMBAP(nbOctetsDemande);

    _trameDemande[7] = _fonctionModbus;
    _trameDemande[8] = adresse>>8 & 0x00FF;
    _trameDemande[9] = adresse    & 0x00FF;

    _trameDemande[10] = nbMots>>8 & 0x00FF;
    _trameDemande[11] = nbMots    & 0x00FF;
    _trameDemande[12] = static_cast<quint8>(nbMots*2);

    for(unsigned int i=0; i<nbMots; i++){
        _trameDemande[i*2+13]=tabMots[i]>>8 & 0x00FF;
        _trameDemande[i*2+14]=tabMots[i]    & 0x00FF;
    }

    int statut;
    statut = envoyerTrameDemande(nbOctetsDemande);
    if (statut == TRANSMISSION_ERREUR)
        return statut;

    statut = recevoirTrameReponse(LG_REPONSE_ECRIRENMOTS);

    delete [] _trameDemande;
    delete [] _trameReponse;

    return statut;
}

void EsclaveModbusTCP::remplirMBAP(int lgTrame)
{
    // identificateur de transaction
    _trameDemande[0]=_transactionId>>8&0xFF;
    _trameDemande[1]=_transactionId&0xFF;
    _transactionId++;

    // Protocole Modbus
    _trameDemande[2]=0;
    _trameDemande[3]=0;

    // Lg trame
    _trameDemande[4]=(lgTrame-6)>>8 & 0xFF;
    _trameDemande[5]=(lgTrame-6) & 0xFF;

    // esclace
    _trameDemande[6]=_numEsclave;

}

int EsclaveModbusTCP::envoyerTrameDemande(int nbOctets)
{
    if(_socket.state() != QTcpSocket::ConnectedState)
        connecter();

    int nbOctetsEnvoyes = 0;
    nbOctetsEnvoyes = _socket.write((char*)_trameDemande, nbOctets);
    _socket.flush();

    if (nbOctetsEnvoyes == nbOctets) {
        qDebug()<<"Fonction : "<<_fonctionModbus;
        afficher(_trameDemande, nbOctets);
        return TRANSMISSION_OK;
    } else {
        qDebug()<<"Erreur transmission";
        return TRANSMISSION_ERREUR;
    }
}

int EsclaveModbusTCP::recevoirTrameReponse(int nbOctets)
{
   /*
   // Lecture sans traiter trame erreur Modbus
    while (_socket->bytesAvailable() < nbOctets) {
        if (!_socket->waitForReadyRead(TIMEOUT)) {
        qDebug()<<"ERREUR Timeout : waitForReadyRead";
        return ERREUR_TIMEOUT;
        }
    }
    _socket->read((char*)_trameReponse, nbOctets);

    afficher(_trameReponse, nbOctets);
    */

    // Lecture des 9 1er octets et vérification si trame d'erreur
    while (_socket.bytesAvailable() < LG_ERREUR) {
        if (!_socket.waitForReadyRead(TIMEOUT)) {
            qDebug()<<"ERREUR Timeout : waitForReadyRead";
            return TRANSACTION_TIMEOUT_ERREUR;
        }
    }
    _socket.read((char*)_trameReponse, LG_ERREUR);

    // Si trame d'erreur la traiter
    if(_trameReponse[7] == _fonctionModbus+128) {
        qDebug()<<"Réception d'une trame d'erreur";
        afficher(_trameReponse, LG_ERREUR);
        return _trameReponse[8]*-1;
    }

    // Lire suite de la trame de reponse
    while (_socket.bytesAvailable() < nbOctets-LG_ERREUR) {
        if (!_socket.waitForReadyRead(TIMEOUT)) {
            qDebug()<<"ERREUR Timeout : waitForReadyRead";
            return TRANSACTION_TIMEOUT_ERREUR;
        }
    }

    _socket.read((char*)_trameReponse+LG_ERREUR, nbOctets-LG_ERREUR);
    afficher(_trameReponse, nbOctets);

    return TRANSACTION_OK;
}

void EsclaveModbusTCP::afficher(unsigned char *trame, int nbOctets)
{
    QByteArray tab((const char*)trame, nbOctets);
    qDebug()<<nbOctets<<" octets : "<<QString(tab.toHex()).toLocal8Bit().constData();
}

TcpException::TcpException(int leCode, QString leMessage) : code(leCode), message(leMessage) {

}

int TcpException::getCode() {
    return code;
}

QString TcpException::getMessage() {
    return message;
}

QString TcpException::obtenirErreur(TcpException &e) {
    QString codeErreur = QString::number(e.getCode());
    QString erreur = "Numéro de l'erreur : " + codeErreur + " \nCause  : " + e.getMessage();
    return erreur;
}
