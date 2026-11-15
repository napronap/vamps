# 080 — Retiro de RelativeTransform de las armas

Se retiró `FVampsWeaponSettings::RelativeTransform` y su aplicación en `AVampsCharacter::InitializeCombatWeapons`. El personaje ahora adjunta el arma al socket con `SnapToTargetIncludingScale`, que deja su transform relativo en identidad y conserva el montaje de las armas configuradas. Cada entrada de `Weapons` sigue identificando la clase y el socket del arma.

MCP encontró cuatro perfiles nativos de `UVampsCombatSettings` y ninguna clase derivada. Se capturaron las 31 propiedades de cada perfil y se verificaron después del guardado. Las seis entradas de `Weapons` tenían transform identidad. Esto incluye la lanza de Impaler, que en una captura histórica tenía escala `0.25`. [Captura previa completa](data-assets-antes.json).

Se aplicó `clang-format` a las líneas modificadas, pasó `git diff --check` y la búsqueda en C++ confirmó que el campo y su uso se retiraron. Después de la compilación y el arranque limpio, MCP confirmó que el struct ya no expone `RelativeTransform`. Las 31 propiedades de cada perfil coincidieron con la captura previa al excluir ese campo de las seis entradas de armas. Se verificaron las referencias, el orden y la cantidad de armas y pasos de combo. Los cuatro perfiles estaban guardados y ninguno requirió restauración. [Relectura final](data-assets-despues.json) · [Guía del editor](editor.md).
