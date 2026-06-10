#ifndef HEXAPODE_H
#define HEXAPODE_H

#include <Arduino.h>
#include <ax12.h>

const int IR_PIN = 2;     // sortie OUT du recepteur IR

// ====== Reglages (a ajuster apres le test en l'air) ======
const int CENTRE  = 512;
const int SWING   = 50;
const int LIFT    = 50;
const int VITESSE = 150;
const int PAUSE   = 220;   // ms entre les phases

// Hauteurs du corps
const int H_DEBOUT   = 60;
const int H_ACCROUPI = 0;

// ====== Modes ======
enum Mode { STOP, AVANCER, RECULER, GAUCHE, DROITE, ROND, LEVER };

// ====== Pattes ======
extern int A[3][6];
extern int B[3][6];

// ====== Fonctions demarche (fonctions.ino) ======
void hexaInit();
void posePattes(int hauteur);
void pasGait(int strideG, int strideD, int hauteur);
void leverUnePatte();
unsigned long lireIR();

// ====== Fonctions commande (hexapode.ino) ======
void traiterIR();        // lit l'IR et met a jour mode/hauteur
void attente(int ms);    // attente qui scrute l'IR en continu

#endif
