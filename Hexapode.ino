#include <ax12.h>

// =====================================================================
//   HEXAPODE - 1 appui = N_PAS pas, puis repos. (Fiable : 1 trame/appui)
//   STOP ou autre direction capte entre les pas. Pas de repetition requise.
//   Regle N_PAS pour la distance par appui. Dossier neuf, ce fichier seul.
//   Moniteur 38400.
// =====================================================================

const int IR_PIN  = 2;
const int CENTRE  = 512;
const int SWING   = 100;    // course du pied (avant/arriere)
const int LIFT    = 200;    // hauteur de lever d'une patte (plus haut = moins de glisse)
const int VITESSE = 200;    // vitesse servos (plus petit = plus vif et ferme)
const int PAUSE   = 150;    // ms entre phases
const int H_DEBOUT   = 80; // corps plus haut -> les pieds appuient mieux
const int H_ACCROUPI = 0;
const int NB_PAS  = 3;     // <-- nombre de pas par appui (monte/descend a ton gout)

int hauteur = H_DEBOUT;

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

void initServos() {
  for (int id = 1; id <= 18; id++) {
    ax12SetRegister(id, AX_TORQUE_ENABLE, 0);
    delay(2);
    ax12SetRegister(id, AX_TORQUE_ENABLE, 1);
    ax12SetRegister2(id, AX_GOAL_SPEED_L, VITESSE);
  }
}

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

bool estMarche(unsigned long code) {
  return code == 0xFF18E7 || code == 0xFF4AB5 || code == 0xFF10EF ||
         code == 0xFF5AA5 || code == 0xFF9867;
}

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
  delay(PAUSE);
}

void unPas(int sG, int sD, int h) {
  for (int i = 0; i < 3; i++) ecrirePatte(A[i], sG, sD, h, +1, true);
  for (int i = 0; i < 3; i++) ecrirePatte(B[i], sG, sD, h, -1, false);
  delay(PAUSE);
  for (int i = 0; i < 3; i++) ecrirePatte(A[i], sG, sD, h, +1, false);
  delay(PAUSE);
  for (int i = 0; i < 3; i++) ecrirePatte(B[i], sG, sD, h, +1, true);
  for (int i = 0; i < 3; i++) ecrirePatte(A[i], sG, sD, h, -1, false);
  delay(PAUSE);
  for (int i = 0; i < 3; i++) ecrirePatte(B[i], sG, sD, h, +1, false);
  delay(PAUSE);
}

// Salut : se cale sur 5 pattes, leve la patte avant-droite et l'agite
void salut() {
  Serial.println("SALUT");
  delay(300);

  int coxa  = A[2][0];               // coxa avant-droit
  int femur = A[2][1];               // femur avant-droit
  int sF    = A[0][4];               // signe du femur

  // lever la patte avant-droite haut en l'air
  // (signe inverse : sur ce robot la patte monte dans l'autre sens)
  SetPosition(femur, 0);
  delay(1500);

  // agiter : balancer la coxa d'avant en arriere quelques fois
  for (int k = 0; k < 3; k++) {
    SetPosition(coxa, CENTRE + 120);
    delay(250);
    SetPosition(coxa, CENTRE - 120);
    delay(250);
  }
  SetPosition(coxa, CENTRE);
  delay(200);

  // reposer la patte
  initServos();
  posePattes(hauteur);
}

void faireUnPas(unsigned long code, int h) {
  switch (code) {
    case 0xFF18E7: unPas(+100, +100, h); break; // avancer
    case 0xFF4AB5: unPas(-100, -100, h); break; // reculer
    case 0xFF5AA5: unPas(-100, +100, h); break; // gauche
    case 0xFF10EF: unPas(+100, -100, h); break; // droite
    case 0xFF9867: unPas(+100,  +40, h); break; // rond
  }
}

// Execute une rafale de NB_PAS pas, interruptible par STOP / autre direction
void marcher(unsigned long code) {
  for (int k = 0; k < NB_PAS; k++) {
    faireUnPas(code, hauteur);
    // entre chaque pas : nouvel appui ?
    unsigned long c = lireIR();
    if (c == 0xFFA25D) { Serial.println("STOP"); break; }       // arret immediat
    if (estMarche(c))  { code = c; Serial.print("-> 0x"); Serial.println(c, HEX); }
    else if (c == 0xFFA857) hauteur = H_ACCROUPI;
    else if (c == 0xFF906F) hauteur = H_DEBOUT;
  }
  initServos();
  posePattes(hauteur);
}

void setup() {
  Serial.begin(38400);
  pinMode(IR_PIN, INPUT);
  ax12Init(1000000);
  delay(500);
  initServos();
  posePattes(H_DEBOUT);
  delay(1000);
  Serial.println("PRET");
}

void loop() {
  unsigned long code = lireIR();
  if (code == 0) return;

  Serial.print("IR 0x"); Serial.println(code, HEX);

  if (estMarche(code))          marcher(code);
  else if (code == 0xFFB04F)    salut();
  else if (code == 0xFFA25D)  { initServos(); posePattes(hauteur); }
  else if (code == 0xFFA857)  { hauteur = H_ACCROUPI; posePattes(hauteur); }
  else if (code == 0xFF906F)  { hauteur = H_DEBOUT;   posePattes(hauteur); }
}
