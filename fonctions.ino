#include "header.h"

// ---- Mapping standard PhantomX ----
// { coxaID, femurID, tibiaID, sCoxa, sFemur, cote(0=Gauche,1=Droite) }
int A[3][6] = {
  {  2,  4,  6,  1,  1, 1 },   // Avant-Droit   (RF)
  { 13, 15, 17, -1, -1, 0 },   // Milieu-Gauche (LM)
  {  8, 10, 12,  1,  1, 1 },   // Arriere-Droit (RR)
};
int B[3][6] = {
  {  1,  3,  5, -1, -1, 0 },   // Avant-Gauche   (LF)
  { 14, 16, 18,  1,  1, 1 },   // Milieu-Droit   (RM)
  {  7,  9, 11, -1, -1, 0 },   // Arriere-Gauche (LR)
};

// Ecrit une patte.
//   coxaDir : +1 = pied en avant, -1 = pied en arriere, 0 = centre
//   lift    : true = patte levee (en l'air)
static void ecrirePatte(int p[6], int strideG, int strideD, int hauteur,
                        int coxaDir, bool lift) {
  int stride   = (p[5] == 0) ? strideG : strideD;       // cote 0 = gauche
  int coxaMove = (long)p[3] * SWING * stride / 100;     // sCoxa * SWING * stride%
  int coxa     = CENTRE + coxaDir * coxaMove;
  int femur    = CENTRE + p[4] * hauteur + (lift ? p[4] * LIFT : 0);
  int tibia    = CENTRE + p[4] * hauteur;
  SetPosition(p[0], coxa);
  SetPosition(p[1], femur);
  SetPosition(p[2], tibia);
}

void posePattes(int hauteur) {
  for (int i = 0; i < 3; i++) ecrirePatte(A[i], 0, 0, hauteur, 0, false);
  for (int i = 0; i < 3; i++) ecrirePatte(B[i], 0, 0, hauteur, 0, false);
}

// strideG / strideD : -100..+100  (avance des cotes gauche / droit)
//   avancer : +100/+100   reculer : -100/-100
//   gauche  : -100/+100   droite  : +100/-100   rond : +100/+40
void pasGait(int strideG, int strideD, int hauteur) {
  // Phase 1 : A leve + avance,  B au sol pousse
  for (int i = 0; i < 3; i++) ecrirePatte(A[i], strideG, strideD, hauteur, +1, true);
  for (int i = 0; i < 3; i++) ecrirePatte(B[i], strideG, strideD, hauteur, -1, false);
  delay(PAUSE);
  // Phase 2 : A repose (pied en avant)
  for (int i = 0; i < 3; i++) ecrirePatte(A[i], strideG, strideD, hauteur, +1, false);
  delay(PAUSE);
  // Phase 3 : B leve + avance,  A au sol pousse
  for (int i = 0; i < 3; i++) ecrirePatte(B[i], strideG, strideD, hauteur, +1, true);
  for (int i = 0; i < 3; i++) ecrirePatte(A[i], strideG, strideD, hauteur, -1, false);
  delay(PAUSE);
  // Phase 4 : B repose
  for (int i = 0; i < 3; i++) ecrirePatte(B[i], strideG, strideD, hauteur, +1, false);
  delay(PAUSE);
}

void leverUnePatte() {
  posePattes(H_DEBOUT);                 // toutes debout
  // leve la patte Avant-Droite (A[0])
  SetPosition(A[0][1], CENTRE + A[0][4] * (H_DEBOUT + 2 * LIFT));
  delay(60);
}

void hexaInit() {
  ax12Init(1000000);
  delay(1000);
  for (int id = 1; id <= 18; id++) ax12SetRegister2(id, AX_GOAL_SPEED_L, VITESSE);
  posePattes(H_DEBOUT);
  delay(1500);
}
