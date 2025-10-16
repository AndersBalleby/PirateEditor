# PirateEditor
En Level Editor i C++ og SDL3 til at oprette eller ændre i levels til mit spil [PiratePlatformer](https://github.com/AndersBalleby/PiratePlatformer), som er lavet i C.

Note: PirateEditor fungerer kun på Linux på nuværende tidspunkt, men kan også bygges på Windows. Det er endnu heller ikke kompatibelt med macOS med audio fra SDL_Mixer.

---

## 🚀 Kom i gang
### Krav
- C++ compiler (f.eks. g++, clang eller MSVC)
- [CMake](https://cmake.org/) >= 3.20
- Git

### Byg og kør
```bash
# Hent projektet
git clone https://github.com/AndersBalleby/PirateEditor.git
cd PirateEditor

# Konfigurer og byg
cmake -S . -B build
cmake --build build

# Kør programmet
./build/PirateEditor     # Linux/macOS
build\PirateEditor   # Windows
