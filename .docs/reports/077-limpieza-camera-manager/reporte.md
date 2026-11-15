# 077 — Limpieza del CameraManager

`AVampsPlayerCameraManager` conserva los valores de seguimiento, encuadre y transición como constantes privadas. Se retiran las trece CVars `vamps.Camera.*` que se registraban fuera de Shipping para ajustar la cámara desde la consola. Los valores provenían del C++ y la búsqueda no encontró referencias a esas CVars en la configuración del proyecto ni en los paquetes guardados.

Se retiran las validaciones por frame de los valores fijos. El manager usa la selección de `UVampsLockOnComponent` y comprueba que el montage que originó el encuadre siga vigente. Conserva la última vista durante la muerte del jugador y restaura la oclusión al cambiar o resetear el pawn. Para los oclusores, la etiqueta `VampsCameraFade` identifica los componentes preparados con `Camera = Ignore` y el parámetro de material en el índice 0 de Custom Primitive Data.

El header explica cada método y cada grupo de variables, incluidos el seguimiento, el encuadre de combate, las transiciones y la oclusión. El cambio de código se limita al `.h` y al `.cpp` de `AVampsPlayerCameraManager`. Los controladores y `BP_VampsPlayerCameraManager` conservan sus referencias. Se revisaron los valores de cámara, los consumidores de lock-on y el cierre del encuadre. Pasaron `clang-format` y `git diff --check`. Quedan la compilación y la comprobación en PIE. [Guía del editor](editor.md).
