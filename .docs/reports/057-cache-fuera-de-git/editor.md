# Operaciones — Caché fuera de Git

Solo queda revisar y realizar el commit. Esta iteración no requiere operaciones en el editor Unreal.

## 1. Ignorar y retirar del seguimiento

**Hecho — Codex, mediante archivos y terminal.** Agregar `/.cache/` al `.gitignore` raíz y ejecutar, desde la raíz del repositorio:

```powershell
git rm -r --cached -- .cache
```

Se retiraron 163 archivos del índice. Se verificó la presencia local de esos 163 archivos, el total de 227 archivos locales antes y después, y la aplicación de la regla con `git check-ignore -v`.

## 2. Revisar el commit

**Pendiente — usuario.** Revisar las 163 bajas de `.cache/` junto con la regla de `.gitignore` y realizar el commit cuando corresponda. Las bajas son esperadas: dejan de versionar la caché, sin borrarla localmente. No hace falta regenerarla.
