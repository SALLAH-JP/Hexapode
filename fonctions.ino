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

static void ecrirePatte(int p[6], int strideG, int strideD, int hauteur,
                        int coxaDir, bool lift) {
  int stride   = (p[5] == 0) ? strideG : strideD;
  int coxaMove = (long)p[3] * SWING * stride / 100;
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

// Toutes les pauses passent par attente() -> l'IR reste a l'ecoute
void pasGait(int strideG, int strideD, int hauteur) {
  for (int i = 0; i < 3; i++) ecrirePatte(A[i], strideG, strideD, hauteur, +1, true);
  for (int i = 0; i < 3; i++) ecrirePatte(B[i], strideG, strideD, hauteur, -1, false);
  attente(PAUSE);
  for (int i = 0; i < 3; i++) ecrirePatte(A[i], strideG, strideD, hauteur, +1, false);
  attente(PAUSE);
  for (int i = 0; i < 3; i++) ecrirePatte(B[i], strideG, strideD, hauteur, +1, true);
  for (int i = 0; i < 3; i++) ecrirePatte(A[i], strideG, strideD, hauteur, -1, false);
  attente(PAUSE);
  for (int i = 0; i < 3; i++) ecrirePatte(B[i], strideG, strideD, hauteur, +1, false);
  attente(PAUSE);
}

void leverUnePatte() {
  posePattes(H_DEBOUT);
  SetPosition(A[0][1], CENTRE + A[0][4] * (H_DEBOUT + 2 * LIFT));
  attente(60);
}

void hexaInit() {
  ax12Init(1000000);
  delay(1000);
  for (int id = 1; id <= 18; id++) ax12SetRegister2(id, AX_GOAL_SPEED_L, VITESSE);
  posePattes(H_DEBOUT);
  delay(1500);
}

// ====== Decodeur NEC sans librairie ======
static unsigned long mesurer(int niveau, unsigned long timeout_us) {
  unsigned long debut = micros();
  while (digitalRead(IR_PIN) == niveau) {
    if (micros() - debut > timeout_us) return 0;
  }
  return micros() - debut;
}

unsigned long lireIR() {
  if (digitalRead(IR_PIN) == HIGH) return 0;

  if (mesurer(LOW, 12000UL) < 8000) return 0;     // mark de tete ~9 ms
  if (mesurer(HIGH, 6000UL) < 3000) return 0;     // space de tete ~4,5 ms

  unsigned long code = 0;
  for (int i = 0; i < 32; i++) {
    if (mesurer(LOW, 2000UL) == 0) return 0;
    unsigned long espace = mesurer(HIGH, 3000UL);
    if (espace == 0) return 0;
    code <<= 1;
    if (espace > 1000) code |= 1;
  }
  return code;
}
