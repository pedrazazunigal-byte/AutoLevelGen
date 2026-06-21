# 🎮 Auto Level Gen — Mod para Geometry Dash (Geode)

Genera niveles aleatorios automáticamente con un solo botón dentro del juego.

---

## ✨ Características

- Botón **"Auto Nivel"** en el menú de creación
- Generación procedural con semilla configurable
- 4 niveles de dificultad: Fácil, Normal, Difícil, Extremo
- 7 modos de juego: Cubo, Nave, Bola, UFO, Ola, Robot, Araña
- Longitud de nivel ajustable (5–100 secciones)
- El nivel se abre directo en el editor para que lo edites

---

## 📱 Instalar desde Termux (sin PC)

### Paso 1 — Instalar git en Termux
```bash
pkg install git
```

### Paso 2 — Clonar este repositorio
```bash
git clone https://github.com/TUUSUARIO/AutoLevelGen.git
cd AutoLevelGen
```

### Paso 3 — Subir a GitHub
```bash
git init
git add .
git commit -m "primer commit"
git remote add origin https://github.com/TUUSUARIO/AutoLevelGen.git
git push -u origin main
```

### Paso 4 — Esperar la compilación
1. Abre **github.com** en tu navegador
2. Ve a tu repositorio → pestaña **Actions**
3. Espera ~5 minutos a que aparezca ✅ verde

### Paso 5 — Descargar el .geode
1. Haz clic en el workflow completado
2. Baja hasta **Artifacts**
3. Descarga `AutoLevelGen-XXXXX.geode`
4. Cópialo a la carpeta de mods de Geode en tu dispositivo

---

## ⚙️ Ajustes del mod

| Ajuste | Descripción | Valor por defecto |
|--------|-------------|-------------------|
| Semilla | 0 = aleatoria cada vez | 0 |
| Longitud | Secciones del nivel | 20 |
| Dificultad | Fácil / Normal / Difícil / Extremo | Normal |
| Modo de juego | Cubo / Nave / Bola / UFO... | Cubo |

---

## 🔄 Para actualizar el mod

Edita el código desde Termux y vuelve a hacer push:
```bash
git add .
git commit -m "mejoras"
git push
```
GitHub compilará automáticamente la nueva versión.

---

## 📁 Estructura del proyecto

```
AutoLevelGen/
├── src/
│   ├── main.cpp           # Hook en el juego, botón UI
│   ├── LevelGenerator.hpp # Definiciones del generador
│   └── LevelGenerator.cpp # Lógica procedural
├── .github/
│   └── workflows/
│       └── build.yml      # Compilación automática
├── CMakeLists.txt         # Configuración de compilación
└── mod.json               # Metadata del mod
```
