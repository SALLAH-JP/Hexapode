# 🤖 Hexapode PhantomX — commande par télécommande IR
 
[![Arduino](https://img.shields.io/badge/Arduino-IDE%201.0.6-00979D?logo=arduino&logoColor=white)](https://www.arduino.cc/)
[![ArbotiX-M](https://img.shields.io/badge/ArbotiX--M-ATmega644p-FF6900)](https://www.trossenrobotics.com/)
[![Dynamixel](https://img.shields.io/badge/Dynamixel-AX--12A-512BD4)](https://www.robotis.us/)
[![Langage](https://img.shields.io/badge/Langage-C%2FC%2B%2B-00599C?logo=cplusplus&logoColor=white)](https://isocpp.org/)
[![Protocole IR](https://img.shields.io/badge/IR-NEC-yellow.svg)](#)
[![YouTube](https://img.shields.io/badge/YouTube-Démo-FF0000?logo=youtube&logoColor=white)](https://youtu.be/_myjrokKvec)
 
Robot hexapode (6 pattes, 18 servos Dynamixel AX-12A) piloté par une
télécommande infrarouge, sur carte **ArbotiX-M** (ATmega644p).
 
## 🎥 Démonstration vidéo
 
[![Démonstration de l'hexapode télécommandé](https://img.youtube.com/vi/_myjrokKvec/maxresdefault.jpg)](https://youtu.be/_myjrokKvec)
 
▶️ **Voir la vidéo : https://youtu.be/_myjrokKvec**
 
## 🔧 Matériel
 
- **Carte** : ArbotiX-M Robocontroller (ATmega644p, 16 MHz)
- **Servos** : 18 × Dynamixel AX-12A, IDs 1 à 18, bus TTL 1 Mbps
- **Récepteur IR** : module type VS1838B / TSOP, sortie sur la **broche 2** (signal),
  alimenté en 5 V et GND par le connecteur 3 broches de l'ArbotiX
  (Signal–Voltage–Ground, le signal étant la broche la plus proche du processeur)
- **Alimentation** : batterie LiPo 3S (11,1 V) sur le jack d'alimentation
- **Programmation** : adaptateur FTDI (UartSBee en 3V3), Arduino IDE 1.0.6 + core ArbotiX
## 💻 Installation logicielle
 
1. Arduino IDE **1.0.6** (les versions récentes ne compilent pas le core ArbotiX).
2. Copier le core ArbotiX (branche `master`) dans
   `Documents/Arduino/hardware/arbotix/` et la bibliothèque dans
   `Documents/Arduino/libraries/`.
3. Carte : **ArbotiX** (pas « ArbotiX w/ RX Shield »).
4. Bon port COM, switch du FTDI sur **3V3**.
5. Moniteur série à **38400 bauds**.
## 📁 Organisation du code
 
Le projet tient dans **un seul fichier** `Hexapode.ino` placé dans un dossier
du même nom (`Hexapode`). Ne rien mettre d'autre dans le dossier : l'IDE
compile tous les fichiers présents, et d'anciens fichiers fausseraient le résultat.
 
## 🎮 Commandes (codes NEC)
 
| Touche   | Code IR     | Action                                  |
|----------|-------------|-----------------------------------------|
| Avancer  | `0xFF18E7`  | rafale de `NB_PAS` pas en avant         |
| Reculer  | `0xFF4AB5`  | rafale en arrière                       |
| Gauche   | `0xFF10EF`  | rafale en tournant à gauche             |
| Droite   | `0xFF5AA5`  | rafale en tournant à droite             |
| Rond     | `0xFF9867`  | avance en tournant (cercle)             |
| Salut    | `0xFFB04F`  | lève une patte avant et fait un coucou  |
| Stop     | `0xFFA25D`  | arrêt, retour au repos                  |
| Accroupi | `0xFFA857`  | baisse le corps                         |
| Debout   | `0xFF906F`  | relève le corps                         |
 
Un appui = `NB_PAS` pas, puis retour au repos. STOP et les changements de
direction sont pris en compte entre deux pas. Tapoter enchaîne les rafales.
 
## ⚙️ Réglages (en haut du fichier)
 
| Paramètre   | Rôle                                         | Valeur actuelle |
|-------------|----------------------------------------------|-----------------|
| `NB_PAS`    | nombre de pas par appui                      | 6               |
| `SWING`     | course du pied (avant/arrière)               | 90              |
| `LIFT`      | hauteur de lever d'une patte                 | 150             |
| `H_DEBOUT`  | hauteur du corps debout                      | 100             |
| `VITESSE`   | vitesse des servos (petit = plus vif)        | 120             |
| `PAUSE`     | durée entre les phases d'un pas (ms)         | 200             |
 
### Régler la marche
 
Si le robot **glisse au lieu d'avancer** : augmenter `LIFT` (la patte doit
décoller franchement du sol) et `H_DEBOUT` (les pieds appuient mieux).
Si un pas est trop court : augmenter `SWING`.
Si une direction est inversée, c'est un signe `sCoxa`/`sFemur` à changer dans
les tableaux `A` et `B` (colonnes 4 et 5).
 
## 🔬 Détails techniques
 
- **Décodeur IR sans bibliothèque** : `lireIR()` décode les trames NEC par
  mesure de durées (sondage). Fiable pour les commandes ; les répétitions ne
  sont pas exploitées (la télécommande utilisée n'en émet pas de façon stable),
  d'où le choix « un appui = plusieurs pas ».
- **Initialisation des servos** : `initServos()` réactive le couple et vide le
  registre d'erreur des 18 servos (utile après une surcharge en marche).
- **Démarche tripode** : deux groupes de 3 pattes (`A` et `B`) alternent
  appui au sol et retour en l'air.
## 🔋 Sécurité batterie
 
Charger les LiPo sur chargeur équilibreur (iMAX B6), 1C max, sous surveillance,
sur surface ininflammable. Arrêter si la batterie chauffe ou gonfle.
 
## 👤 Auteur
 
**SALLAH Jean-Paul** — [github.com/SALLAH-JP](https://github.com/SALLAH-JP)
