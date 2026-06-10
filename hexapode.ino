#include <ax12.h>
#include "header.h"

// =====================================================================
//   HEXAPODE COMMANDABLE PAR TELECOMMANDE IR (decodeur NEC sans lib)
//   L'IR est scrute PENDANT les pauses de la demarche via attente(),
//   donc les appuis sont captes meme quand le robot marche.
// =====================================================================

Mode mode    = STOP;
int  hauteur = H_DEBOUT;

// Lit l'IR et met a jour mode / hauteur
void traiterIR() {
  unsigned long code = lireIR();
  if (code == 0) return;

  Serial.print("IR: 0x");
  Serial.println(code, HEX);

  switch (code) {
    case 0xFF18E7: mode = AVANCER; break;
    case 0xFF4AB5: mode = RECULER; break;
    case 0xFF10EF: mode = GAUCHE;  break;
    case 0xFF5AA5: mode = DROITE;  break;
    case 0xFF9867: mode = ROND;    break;
    case 0xFFB04F: mode = LEVER;   break;
    case 0xFFA25D: mode = STOP;    break;
    case 0xFFA857: hauteur = H_ACCROUPI; break;
    case 0xFF906F: hauteur = H_DEBOUT;   break;
  }
}

// Attente qui scrute l'IR en continu (remplace delay)
void attente(int ms) {
  unsigned long fin = millis() + (unsigned long)ms;
  while (millis() < fin) {
    traiterIR();
  }
}

void setup() {
  Serial.begin(38400);
  pinMode(IR_PIN, INPUT);
  hexaInit();
  Serial.println("Hexapode IR pret.");
}

void loop() {
  traiterIR();

  switch (mode) {
    case AVANCER: pasGait(+100, +100, hauteur); break;
    case RECULER: pasGait(-100, -100, hauteur); break;
    case GAUCHE:  pasGait(-100, +100, hauteur); break;
    case DROITE:  pasGait(+100, -100, hauteur); break;
    case ROND:    pasGait(+100,  +40, hauteur); break;
    case LEVER:   leverUnePatte();              break;
    case STOP:    posePattes(hauteur); attente(50); break;
  }
}
