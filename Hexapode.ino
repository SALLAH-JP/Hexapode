#include <ax12.h>

// =====================================================================
//   HEXAPODE - FICHIER UNIQUE - DOSSIER NEUF, RIEN D'AUTRE DEDANS
//   Moniteur : 38400 baud
// =====================================================================

const int IR_PIN  = 2;
const int CENTRE  = 512;
const int SWING   = 80;
const int LIFT    = 80;
const int VITESSE = 150;
const int PAUSE   = 220;
const int H_DEBOUT   = 60;
const int H_ACCROUPI = 0;

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

int mode    = 0;   // 0=STOP 1=AVANCER 2=RECULER 3=GAUCHE 4=DROITE 5=ROND
int hauteur = H_DEBOUT;

// ---------- IR (NEC sans librairie) ----------
unsigned long mesurer(int niveau, unsigned long timeout_us) {
  unsigned long debut = micros();
  while (digitalRead(IR_PIN) == niveau) {
    if (micros() - debut > timeout_us) return 0;
  }
  return micros() - debut;
}

unsigned long lireIR() {
  if (digitalRead(IR_PIN) == HIGH) return 0;
  if (mesurer(LOW, 12000UL) < 8000) return 0;
  if (mesurer(HIGH, 6000UL) < 3000) return 0;
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

void traiterIR() {
  unsigned long code = lireIR();
  if (code == 0) return;
  Serial.print("IR 0x"); Serial.print(code, HEX);
  switch (code) {
    case 0xFF18E7: mode = 1; Serial.println(" AVANCER"); break;
    case 0xFF4AB5: mode = 2; Serial.println(" RECULER"); break;
    case 0xFF10EF: mode = 3; Serial.println(" GAUCHE");  break;
    case 0xFF5AA5: mode = 4; Serial.println(" DROITE");  break;
    case 0xFF9867: mode = 5; Serial.println(" ROND");    break;
    case 0xFFA25D: mode = 0; Serial.println(" STOP");    break;
    case 0xFFA857: hauteur = H_ACCROUPI; Serial.println(" ACCROUPI"); break;
    case 0xFF906F: hauteur = H_DEBOUT;   Serial.println(" DEBOUT");   break;
    default:       Serial.println(" inconnu");           break;
  }
}

void attente(int ms) {
  unsigned long fin = millis() + (unsigned long)ms;
  while (millis() < fin) traiterIR();
}

// ---------- Demarche ----------
void ecrirePatte(int p[6], int sG, int sD, int h, int coxaDir, bool lift) {
  int stride   = (p[5] == 0) ? sG : sD;
  int coxaMove = (long)p[3] * SWING * stride / 100;
  SetPosition(p[0], CENTRE + coxaDir * coxaMove);
  SetPosition(p[1], CENTRE + p[4] * h + (lift ? p[4] * LIFT : 0));
  SetPosition(p[2], CENTRE + p[4] * h);
}

void posePattes(int h) {
  for (int i = 0; i < 3; i++) ecrirePatte(A[i], 0, 0, h, 0, false);
  for (int i = 0; i < 3; i++) ecrirePatte(B[i], 0, 0, h, 0, false);
}

void pasGait(int sG, int sD, int h) {
  Serial.println("PAS");                      // <-- marqueur
  for (int i = 0; i < 3; i++) ecrirePatte(A[i], sG, sD, h, +1, true);
  for (int i = 0; i < 3; i++) ecrirePatte(B[i], sG, sD, h, -1, false);
  attente(PAUSE);
  for (int i = 0; i < 3; i++) ecrirePatte(A[i], sG, sD, h, +1, false);
  attente(PAUSE);
  for (int i = 0; i < 3; i++) ecrirePatte(B[i], sG, sD, h, +1, true);
  for (int i = 0; i < 3; i++) ecrirePatte(A[i], sG, sD, h, -1, false);
  attente(PAUSE);
  for (int i = 0; i < 3; i++) ecrirePatte(B[i], sG, sD, h, +1, false);
  attente(PAUSE);
}

void setup() {
  Serial.begin(38400);
  pinMode(IR_PIN, INPUT);
  ax12Init(1000000);
  delay(500);
  for (int id = 1; id <= 18; id++) ax12SetRegister2(id, AX_GOAL_SPEED_L, VITESSE);
  posePattes(H_DEBOUT);
  delay(1000);
  Serial.println("PRET");
}

void loop() {
  traiterIR();
  switch (mode) {
    case 1: pasGait(+100, +100, hauteur); break;
    case 2: pasGait(-100, -100, hauteur); break;
    case 3: pasGait(-100, +100, hauteur); break;
    case 4: pasGait(+100, -100, hauteur); break;
    case 5: pasGait(+100,  +40, hauteur); break;
    default: posePattes(hauteur); attente(50); break;
  }
}
