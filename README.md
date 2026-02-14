# Pirate Level Editor

En **level editor** til mit [PiratePlatformer](https://github.com/AndersBalleby/PiratePlatformer) spil, bygget med **C++** og **SDL3**.
Editoren gør det nemt at skabe og redigere baner med et responsive interface der integrerer direkte i spillets engine.

---

## Features
- **Tile-baseret level editing** med live preview
- **Scene- og baggrundsstyring** (multi-layer support)
- **Audio-support** via SDL_mixer (musik & lydeffekter)
- **Farvetekst og UI-rendering** via et custom tekstsystem (`TextHandler`)
- **Resource management** til teksturer, lyd og assets
- **Indbygget FPS-counter og logging-system**

---

## Keybinds
| Handling | Tast |
|-----------|------|
| **Skift Edit Mode ON / OFF** | `Space` |
| **Bevæg kamera** | `A` / `D` eller `←` / `→` |
| **Skift til næste layer** | `↑` |
| **Skift til forrige layer** | `↓` |
| **Toggle tile palette** | `P` |
| **Toggle layer view** | `Tab` |
| **Vælg tiles (multi-select)** | `Ctrl + Venstreklik` |
| **Placér tile** | `Venstreklik` |
| **Slet tile** | `Højreklik` / `Delete` / `Backspace` |
| **Åbn “Load Scene” menu** | `Escape` |
| **Ny scene (fra Load Menu)** | `N` |
| **Bekræft (fx i dialoger)** | `Enter` |
| **Fortryd / luk dialog** | `Escape` |
| **Skift tile-index i paletten** | `Musehjul` |

---

## Teknologier & Dependencies
Projektet bruger **CMake** og **FetchContent** til at hente SDL-biblioteker automatisk — du behøver ikke installere noget manuelt.

### Projektet kræver følgende dependencies
- **C++20** compiler (f.eks. GCC 12+, Clang 15+ eller MSVC 2022)
- **CMake 3.20+**
- **Git**

### Følgende hentes automatisk ved build
- [SDL3](https://github.com/libsdl-org/SDL)
- [SDL3_image](https://github.com/libsdl-org/SDL_image)
- [SDL3_ttf](https://github.com/libsdl-org/SDL_ttf)
- [SDL3_mixer](https://github.com/libsdl-org/SDL_mixer) *(Fungerer kun på Windows og Linux - ingen lyd på macOS)*

---

## Cloning & Building

### Clone og build projektet
```bash
git clone https://github.com/AndersBalleby/PirateEditor.git
cd PirateEditor
mkdir build && cd build
cmake ..
make -j
./PirateEditor
```


### Projektstruktur
```bash
./src
├── core
│   ├── audio
│   │   ├── Audio.cpp
│   │   └── Audio.hpp
│   ├── math
│   │   └── vec.hpp
│   ├── resources
│   │   ├── ResourceManager.cpp
│   │   └── ResourceManager.hpp
│   ├── scene
│   │   ├── Background.cpp
│   │   ├── Scene.cpp
│   │   ├── Background.hpp
│   │   └── Scene.hpp
│   └── tiles
│       ├── TileManager.cpp
│       └── TileManager.hpp
│
├── editor
│   ├── Editor.cpp
│   ├── EditorUI.cpp
│   ├── FPS_Counter.cpp
│   ├── Editor.hpp
│   ├── EditorUI.hpp
│   └── FPS_Counter.hpp
│
├── logging
│   ├── Logger.cpp
│   └── Logger.hpp
│
├── sdl
│   ├── SDL_Handler.cpp
│   └── SDL_Handler.hpp
│
├── ui
│   ├── TextHandler.cpp
│   └── TextHandler.hpp
│
├── utils
│   ├── utils.cpp
│   └── utils.hpp
│
└── main.cpp
```

## Manglende Features
I fremtiden kunne følgende features blive implementeret:
- **Undo/Redo-system**
- **Spil spillet fra editor**
- **Grid snapping & tile collision tools**
- **GUI-forbedringer** (knapper, ikoner, drag-and-drop)
- **Script-baserede entities**
