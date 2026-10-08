# 007 — Revisión de scripts de build y editor

Diagnóstico posterior del entorno: [iteración 008 — Generación de proyectos Visual Studio](../008-generacion-visual-studio/reporte.md). No reemplaza las observaciones sobre estos scripts.

Revisión estática de `build.bat`, `editor.bat` y `vars.bat`, agregados y probados por el usuario. Codex no modifica los scripts, código C++ ni assets.

## Resultado

- `vars.bat` contiene una llamada a Build.bat además de las variables. Esto hace que `build.bat` solicite dos builds y que `editor.bat` compile antes de abrir Unreal. El segundo build puede resultar incremental sin trabajo, pero vuelve a invocar UBT.
- `editor.bat` no comprueba el resultado de esa compilación: intenta abrir el editor aunque falle.
- `call %~dp0\vars.bat` necesita comillas para funcionar cuando la ruta contiene espacios. `%~dp0` ya incluye la barra final.
- `PROJECT_DIR` apunta a una subcarpeta `vamps` que no representa la raíz actual del proyecto; no se utiliza en estos scripts.
- Las asignaciones deberían usar `set "NAME=value"` y los scripts de entrada `setlocal`, para evitar espacios finales accidentales y cambios persistentes en el entorno de la terminal.
- El target `vampsEditor`, plataforma `Win64`, configuración `Development` y ruta entre comillas del `.uproject` son coherentes con `Source/vampsEditor.Target.cs` y la asociación 5.8 del proyecto.
- `-WaitMutex` espera otra instancia de UBT; `-NoHotReload` deshabilita Hot Reload. Este último no activa Live Coding ni convierte un build externo en un parche en vivo.
- `editor.bat` usa correctamente el título vacío de `start` y reenvía `%*` al editor. `build.bat` no reenvía argumentos adicionales.

## Verificación

Se revisaron los scripts y el código instalado de UE 5.8: `Engine/Build/BatchFiles/Build.bat`, `GlobalOptions.cs` y `Configuration/Descriptors/TargetDescriptor.cs` de UnrealBuildTool. Los flags indicados existen en esa versión. No se ejecutaron builds ni se abrió el editor; el funcionamiento práctico fue comunicado por el usuario.

La revisión no cambia la configuración del combate ni reemplaza las instrucciones de la [iteración 006](../006-golpe-de-puno/editor.md). Recomendaciones de uso en la [guía](editor.md).
