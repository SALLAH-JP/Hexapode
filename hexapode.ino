#include <ax12.h>
#include <IRremote.h>
#include "header.h"

// =====================================================================
//   HEXAPODE COMMANDABLE  (PhantomX / ArbotiX / 18x AX-12A)
//   Pilotage au clavier via le Serial Monitor (38400 baud).
//
//   *** TESTER D'ABORD ROBOT EN L'AIR et ajuster les signes ***
//   dans hexapode.cpp (colonnes sCoxa / sFemur).
// =====================================================================

Mode mode    = STOP;        // mode de deplacement courant
int  hauteur = H_DEBOUT;    // etat de hauteur (independant du deplacement)

void aide() {
  Serial.println("=== Hexapode - commandes ===");
  Serial.println("z = avancer     s = reculer");
  Serial.println("q = gauche      d = droite");
  Serial.println("o = rond        l = lever une patte");
  Serial.println("c = accroupie   h = debout haut");
  Serial.println("x = stop");
}

void setup() {
  Serial.begin(9600);
  irrecv.enableIRIn();
  hexaInit();
}

void loop() {
  // --- lecture des commandes ---
  if (irrecv.decode(&results)) {
    Serial.println(results.value, HEX);
    switch (results.value) {
      case 0xFF18E7 : mode = AVANCER; break;
      case 0xFF4AB5 : mode = RECULER; break;
      case 0xFF10EF : mode = GAUCHE;  break;
      case 0xFF5AA5 : mode = DROITE;  break;
      case 0xFF9867 : mode = ROND;    break;
      case 0xFFB04F : mode = LEVER;   break;
      case 0xFFA25D : mode = STOP;    break;
      // accroupie / debout : changent juste l'ETAT, le deplacement continue
      case 0xFFA857 : hauteur = H_ACCROUPI; break;
      case 0xFF906F : hauteur = H_DEBOUT;   break;
    }
    
    irrecv.resume();
  }

  // --- execution du mode courant ---
  switch (mode) {
    case AVANCER: pasGait(+100, +100, hauteur); break;
    case RECULER: pasGait(-100, -100, hauteur); break;
    case GAUCHE:  pasGait(-100, +100, hauteur); break;   // gauche recule, droite avance
    case DROITE:  pasGait(+100, -100, hauteur); break;
    case ROND:    pasGait(+100,  +40, hauteur); break;   // avance en tournant
    case LEVER:   leverUnePatte();              break;
    case STOP:    posePattes(hauteur); delay(50); break;
  }
}
