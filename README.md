# 🏴‍☠️ Pirate Editor

> ⚠️ **Projektet er stadig under udvikling!** Dette er et læringsprojekt, og nye funktioner og forbedringer tilføjes løbende.

En **2D spil-editor** skrevet i **C++** med SDL3 som efterfølger af *Pirate Platformer*.  
Projektet er et hobbyprojekt i spiludvikling, og giver mulighed for at bygge og redigere baner med grafiske assets fra PixelFrog's Treasure Hunters (https://pixelfrog-assets.itch.io/treasure-hunters).

## 🎮 Om projektet

I **Pirate Editor** kan du:
- Oprette og redigere 2D platformer-baner
- Placere pirater, skatte og andre objekter
- Gemme og indlæse baner til brug i spillet *Pirate Platformer*

Projektet er under udvikling og fokuserer på at lære C++-programmering, SDL3 og editor-design.

## 🧰 Teknologier

- **C++** – programmering og læring
- **SDL3** – cross-platform game framework
- **SDL3_image** – til billedindlæsning (PNG, JPG osv.)
- **Bash** – til build scripts

## 📦 Installation

### 1. Klon projektet
```code
git clone https://github.com/AndersBalleby/PirateEditor.git
cd PirateEditor
```

### 2. Byg med bash
./build.sh 

### 3. Kør editoren
./PirateEditor

## 📁 Projektstruktur
```text
PiratePlatformer/
├── src/          # Editor og spilrelateret C++ kode 
├── assets/       # Alle assets fra Treasure Hunters på itch.io
├── levels/       # Baner
├── include/      # SDL3 og SDL3_image (submoduler)
└── bash.sh       # Build script
```

## 🏗️ Inspiration

Projektet er inspireret af:
- *Pirate Platformer* – dets gameplay og asset-brug
- SDL3 dokumentation og eksempler: https://wiki.libsdl.org/
- ImGui for editor GUI (ikke implementeret endnu): https://github.com/ocornut/imgui

## ℹ️ Brug af AI
Alle former for kunstig intelligens er kun brugt som skriveassistent til denne README.  
Alt kode og design er lavet manuelt med mindre andet er angivet.

