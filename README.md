# 🕷️ Hexapode Commandable — PhantomX / ArbotiX

Robot hexapode (6 pattes, 18 servos AX-12A) piloté au clavier : déplacement, rotation, marche en cercle, changement de posture et lever de patte. Basé sur un châssis **Trossen PhantomX** avec contrôleur **ArbotiX-M**.

---

## 🎥 Démonstration vidéo

<!-- Remplace VIDEO_ID par l'identifiant de ta vidéo YouTube -->
[![Démonstration de l'hexapode](https://img.youtube.com/vi/VIDEO_ID/0.jpg)](https://www.youtube.com/watch?v=VIDEO_ID)

> *🎬 Vidéo à venir — lien à insérer ici.*

---

## 📋 Description

L'hexapode se déplace grâce à une **démarche tripode** (deux groupes de 3 pattes en alternance). Le pilotage se fait par **commandes série** (Serial Monitor à 38400 bauds), ce qui permet ensuite un pilotage **sans fil** en branchant simplement un module Bluetooth ou XBee sur le même port série, sans changer le code.

Fonctions disponibles :
- Avancer / reculer
- Tourner à gauche / à droite (différentiel)
- Marcher en faisant un cercle
- Lever une patte
- Changer de posture : **accroupie** ou **debout haut** (indépendant du déplacement)

---

## 🦿 Matériel

| Élément | Détail |
|---|---|
| Châssis | Hexapode PhantomX (Trossen Robotics) |
| Contrôleur | ArbotiX-M Robocontroller (ATmega644p) |
| Servomoteurs | 18 × Dynamixel **AX-12A** (IDs 1 à 18) |
| Alimentation | Batterie **LiPo 3S 11,1 V** |
| Programmation | Adaptateur **UartSBee** (FTDI-USB, switch sur **3V3**) |

---

## 📁 Structure du projet

```
hexapode_commande/
├── hexapode_commande.ino   # Programme principal (commandes + machine à états)
├── hexapode.h              # Déclarations : constantes, modes, prototypes
└── hexapode.cpp            # Fonctions : démarche, postures, init
```

Sketches utilitaires (diagnostic) :
- `scan_servos_ax12.ino` — vérifie que les 18 servos répondent et lit leurs positions
- `test_mouvement_ax12.ino` — test de commande (petites oscillations)

---

## ⚙️ Installation

1. **Arduino IDE 1.0.6** (⚠️ *pas* la version 2.x : le core ArbotiX est incompatible avec l'IDE récent).
2. Installer le **core ArbotiX** (dépôt `vanadiumlabs/arbotix`, branche `master`, ancien format) : copier les dossiers `hardware` et `libraries` dans le dossier *sketchbook*.
3. Brancher le **UartSBee** (switch sur **3V3**) : USB côté PC, câble FTDI côté ArbotiX (fil **vert → GRN**, fil **noir → BLK**).
4. Dans l'IDE : **Tools → Board → ArbotiX**, puis sélectionner le bon port COM.
5. Ouvrir `hexapode_commande.ino` (les 3 fichiers doivent être dans le même dossier), puis **Upload**.

---

## 🎮 Commandes

Ouvrir le **Serial Monitor** à **38400 bauds**, taper la touche puis Entrée :

| Touche | Action |
|:---:|---|
| `z` | Avancer |
| `s` | Reculer |
| `q` | Tourner à gauche |
| `d` | Tourner à droite |
| `o` | Marcher en cercle |
| `l` | Lever une patte |
| `c` | Posture accroupie |
| `h` | Posture debout haut |
| `x` | Stop |

> `c` et `h` changent uniquement la **hauteur du corps** : on peut continuer à se déplacer accroupi ou debout.

---

## 🔧 Calibration (à faire une fois)

⚠️ **Premier essai toujours robot EN L'AIR** (corps surélevé, pattes dans le vide).

1. Taper `z` (avancer) et observer : les 2 trépieds doivent **alterner**, les pieds levés monter **vers le haut** et balancer **vers l'avant**.
2. Corriger les signes dans `hexapode.cpp` (colonnes `sCoxa` / `sFemur`) :
   - Patte qui se lève vers le bas → inverser son `sFemur`
   - Patte qui balance vers l'arrière → inverser son `sCoxa`
3. Quand le motif est correct en l'air → régler `H_DEBOUT` (≈ 40–90) pour la posture au sol.
4. Ajuster `SWING`, `LIFT`, `PAUSE`, `VITESSE` au besoin (amplitude / vitesse).

---

## 🔋 Batterie & charge

- Batterie : **LiPo 3S (11,1 V)**, à charger avec un **chargeur à équilibrage 3S**.
- Brancher le **connecteur d'équilibrage** (blanc, 4 fils) sur le port **3S** du chargeur, fil **noir côté −**.
- LED chargeur : **rouge = en charge**, **verte = chargée**, **rouge clignotante = polarité inversée / court-circuit → débrancher**.

### ⚠️ Sécurité
- Ne **jamais** charger une LiPo sans surveillance ; surface non inflammable.
- Arrêter si la batterie **gonfle, chauffe ou sent bizarre**.
- Garder un **doigt près de l'alimentation** lors des premiers essais de marche.
- Tester en l'air avant de poser le robot au sol.

---

## 👤 Auteur

**SALLAH Jean-Paul** — [@SALLAH-JP](https://github.com/SALLAH-JP)

---

*Projet réalisé sur un hexapode PhantomX (Trossen Robotics) — Université des Mascareignes.*
