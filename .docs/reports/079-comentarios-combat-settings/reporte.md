# 079 — Comentarios de CombatSettings

Se documentaron las 37 propiedades de `UVampsCombatSettings`, `FVampsWeaponSettings` y `FVampsCombatStepSettings` en `VampsCombatSettings.h`. El personaje usa el Data Asset para equipar armas y elegir animaciones. Cada sección del montage de ataque toma del mismo índice en `ComboSteps` sus parámetros de alineación, empuje y duración de reacción. Los comentarios indican qué valores corresponden al atacante o al defensor y cuándo se aplican los ajustes de guardia y movilidad.

Se verificó que las declaraciones, los valores iniciales y los atributos de reflexión quedaron intactos. Pasaron `clang-format` y `git diff --check`. [Guía del editor](editor.md).
