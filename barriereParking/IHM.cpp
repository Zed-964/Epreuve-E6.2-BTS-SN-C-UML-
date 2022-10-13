#include "IHM.h"
#include "ui_IHM.h"
#include "esclavemodbustcp.h"

IHM::IHM(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::IHM)
{
    ui->setupUi(this);

    connect(ui->pushButton_connecter, &QPushButton::clicked, this, &IHM::connecterWago);
    connect(ui->pushButton_deconnecter, &QPushButton::clicked, this, &IHM::deconnecterWago);
    connect(ui->pushButton_quitter, &QPushButton::clicked, this, &IHM::close);
    connect(ui->pushButton_effacerLog, &QPushButton::clicked, ui->textEdit_log, &QTextEdit::clear);
    connect(ui->pushButton_allumerBalise, &QPushButton::clicked, this, &IHM::allumerBalise);
    connect(ui->pushButton_eteindreBalise, &QPushButton::clicked, this, &IHM::eteindreBalise);
    connect(ui->pushButton_descendreBarriere, &QPushButton::clicked, this, &IHM::descendreBarriere);
    connect(ui->pushButton_monterBarriere, &QPushButton::clicked, this, &IHM::monterBarriere);
    connect(ui->pushButton_lireCapteurs, &QPushButton::clicked, this, &IHM::lireCapteurs);

    this->setWindowTitle("Gestion d'une barrière de parking");
    ui->label_image->setPixmap(QPixmap("../barriereParking/barriere.jpeg"));
    ui->pushButton_deconnecter->setEnabled(false);
    ui->pushButton_eteindreBalise->setEnabled(false);
    ui->pushButton_allumerBalise->setEnabled(false);
    ui->pushButton_monterBarriere->setEnabled(false);
    ui->pushButton_descendreBarriere->setEnabled(false);
}

IHM::~IHM()
{
    if (laBarriere != nullptr) {
        delete laBarriere;
        laBarriere = nullptr;
    }
    delete ui;
}

void IHM::connecterWago()
{
    ui->textEdit_log->append("Demande de connection au Wago");

    QString adresseIP = ui->lineEdit_adresseIPWago->text();
    laBarriere = new Barriere();
    try {
        laBarriere->connexionWago(adresseIP);

        ui->pushButton_connecter->setEnabled(false);

        ui->textEdit_log->append("Connection au Wago établie");

        ui->pushButton_deconnecter->setEnabled(true);
        ui->pushButton_allumerBalise->setEnabled(true);

        laBarriere->lectureCapteurs();

        if (laBarriere->getStatutBarriere() == 1)
            ui->pushButton_descendreBarriere->setEnabled(true);
        else
            ui->pushButton_monterBarriere->setEnabled(true);
    } catch (TcpException &e) {
        ui->textEdit_log->append("Echec de la connection au Wago");
        ui->textEdit_log->append(e.obtenirErreur(e));
    }
    try {
    ui->textEdit_log->append(laBarriere->lectureCapteurs());
    } catch (TcpException &e) {
        ui->textEdit_log->append("Echec lors de la lecture des capteurs");
        ui->textEdit_log->append(e.obtenirErreur(e));
    }
}

void IHM::deconnecterWago()
{
    ui->textEdit_log->append("Deconnection du Wago");

    laBarriere->deconnectionWago();

    delete laBarriere;
    laBarriere = nullptr;

    ui->pushButton_connecter->setEnabled(true);
    ui->pushButton_deconnecter->setEnabled(false);
}

void IHM::allumerBalise()
{
    try {
        ui->textEdit_log->append("Allumer balise");
        laBarriere->allumer();
        ui->pushButton_allumerBalise->setEnabled(false);
        ui->pushButton_eteindreBalise->setEnabled(true);
    } catch (TcpException &e) {
        ui->textEdit_log->append("Echec allumer balise");
        ui->textEdit_log->append(e.obtenirErreur(e));
    }
}

void IHM::eteindreBalise()
{
    try {
        ui->textEdit_log->append("Eteindre balise");
        laBarriere->eteindre();
        ui->pushButton_eteindreBalise->setEnabled(false);
        ui->pushButton_allumerBalise->setEnabled(true);
    } catch (TcpException &e) {
        ui->textEdit_log->append("Echec Eteindre balise");
        ui->textEdit_log->append(e.obtenirErreur(e));
    }
}

void IHM::descendreBarriere()
{
    try {
        ui->textEdit_log->append("Baisser barrière");
        laBarriere->descendre();
        ui->pushButton_monterBarriere->setEnabled(true);
        ui->pushButton_descendreBarriere->setEnabled(false);
    } catch (TcpException &e) {
        ui->textEdit_log->append("Echec baisser barrière");
        ui->textEdit_log->append(e.obtenirErreur(e));
    }
}

void IHM::monterBarriere()
{
    try {
        ui->textEdit_log->append("Monter barrière");
        laBarriere->monter();
        ui->pushButton_descendreBarriere->setEnabled(true);
        ui->pushButton_monterBarriere->setEnabled(false);
    } catch (TcpException &e) {
        ui->textEdit_log->append("Echec monter barrière");
        ui->textEdit_log->append(e.obtenirErreur(e));
    }
}

void IHM::lireCapteurs() {
    try {
    ui->textEdit_log->append("Lecture capteurs : ");
    ui->textEdit_log->append(laBarriere->lectureCapteurs());
    } catch (TcpException &e) {
        ui->textEdit_log->append("Echec lecture capteurs");
        ui->textEdit_log->append(e.obtenirErreur(e));
    }
}













