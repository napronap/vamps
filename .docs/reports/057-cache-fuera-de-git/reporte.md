# 057 — Caché local fuera del seguimiento de Git

## Resultado — 2026-10-27

Se agregó `/.cache/` al `.gitignore` raíz y se ejecutó `git rm -r --cached -- .cache`. Los 163 archivos previamente versionados quedaron como bajas preparadas para el próximo commit. La carpeta conserva sus 227 archivos locales de índices clangd.

## Archivos y motivo

- `.gitignore`: ignora la caché raíz para evitar incorporar sus archivos regenerados.
- Índice de Git: retira únicamente `.cache/` del seguimiento; la operación usa `--cached` para conservar las copias locales.
- `.docs/README.md`: enlaza este reporte y su [guía](editor.md).
- Esta carpeta: registra el mantenimiento y las comprobaciones.

No hubo cambios de código, clases, Blueprints ni otros assets.

## Verificación

Git ya no sigue archivos bajo `.cache/`. Se verificó que los 163 archivos antes versionados siguen presentes en disco y que la carpeta conserva el total de 227 archivos observado antes de la operación. `git check-ignore -v` confirma que la regla raíz ignora los índices locales.

El commit queda a cargo del usuario. Las bajas preparadas son eliminaciones del repositorio, no del disco. Las operaciones y revisión final están en la [guía](editor.md).
