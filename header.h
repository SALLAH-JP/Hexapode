#ifndef HEXAPODE_H
#define HEXAPODE_H

#include <Arduino.h>
#include <ax12.h>
#include <IRremote.h>

int RECV_PIN = 2;

IRrecv irrecv(RECV_PIN);

decode_results results;

// ====== Reglages (a ajuster apres le test en l'air) ======
const int CENTRE  = 512;   // position neutre des servos
const int SWING   = 50;    // amplitude coxa (avance / recule)
const int LIFT    = 50;    // hauteur de lever d'une patte (femur)
const int VITESSE = 150;   // vitesse des servos (0 = max, ~150 = doux)
const int PAUSE   = 220;   // ms entre les phases (plus grand = plus lent)

// Hauteurs du corps (offset applique au femur / tibia)
const int H_DEBOUT   = 60;  // debout haut  (regler ~40-90 pour le sol)
const int H_ACCROUPI = 0;   // accroupie

// ====== Modes de deplacement ======
enum Mode { STOP, AVANCER, RECULER, GAUCHE, DROITE, ROND, LEVER };

// ====== Pattes ======
// chaque ligne : { coxaID, femurID, tibiaID, sCoxa, sFemur, cote }
//   sCoxa / sFemur : signes (+1 / -1) -> a ajuster au test en l'air
//   cote           : 0 = gauche, 1 = droite
extern int A[3][6];   // tripode A
extern int B[3][6];   // tripode B

// ====== Fonctions ======
void hexaInit();                                       // init bus + vitesse + pose debout
void posePattes(int hauteur);                          // toutes les pattes au repos
void pasGait(int strideG, int strideD, int hauteur);   // un pas tripode complet
void leverUnePatte();                                  // leve une patte, les autres au repos

#endif
