#include "header.h"

int A[3][6] = {
  {  2,  4,  6,  1,  1, 1 },
  { 13, 15, 17, -1, -1, 0 },
  {  8, 10, 12,  1,  1, 1 },
};
int B[3][6] = {
  {  1,  3,  5, -1, -1, 0 },
  { 14, 16, 18,  1,  1, 1 },
  {  7,  9, 11, -1, -1, 0 },
};

static void ecrirePatte(int p[6], int strideG, int strideD, int hauteur,
                        int coxaDir, bool lift) {
  int stride   = (p[5] == 0) ? strideG : strideD;
  int coxaMove = (long)p[3] * SWING * stride / 100;
  SetPosition(p[0], CENTRE + coxaDir * coxaMove);
  SetPosition(p[1], CENTRE + p[4] * hauteur + (lift ? p[4] * LIFT : 0));
  SetPosition(p[2], CENTRE + p[4] * hauteur);
}

void posePattes(int hauteur) {
  for (int i = 0; i < 3; i++) ecrirePatte(A[i], 0, 0, hauteur, 0, false);
  for (int i = 0; i < 3; i++) ecrirePatte(B[i], 0, 0, hauteur, 0, false);
}

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
  delay(500);
  // Vider les erreurs sur tous les servos et reactiver le couple
  for (int id = 1; id <= 18; id++) {
    ax12SetRegister(id, AX_TORQUE_ENABLE, 0);
    delay(2);
    ax12SetRegister(id, AX_TORQUE_ENABLE, 1);
    ax12SetRegister2(id, AX_GOAL_SPEED_L, VITESSE);
  }
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
