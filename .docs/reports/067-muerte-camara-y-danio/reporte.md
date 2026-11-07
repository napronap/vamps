# 067 — Muerte, cámara y daño por perfil

## Cambios de código

- `AVampsCharacter` presenta la muerte al aplicar un contacto letal y elige `Death Montage` o `Guard Death Montage` del Data Asset según el resultado de ese contacto. `AVampsMatchGameMode` conserva el lote de bajas, la puntuación y el respawn. Al cerrar una ronda se congela a los personajes vivos y los eliminados pueden completar su animación.
- `AVampsCombatTestGameMode` restaura la colisión de la cápsula y el modo de movimiento al reiniciar un personaje muerto. Su reset de combate ya detiene los montages. Esto cubre el reinicio con R en `L_CharacterBasic`.
- `AVampsPlayerCameraManager` mantiene la última vista del jugador durante la muerte y vuelve a actualizarla al restaurar el personaje. El barrido `ECC_Camera` ignora a los demás `AVampsCharacter` registrados por Lock On y conserva la colisión con geometría.
- `AVampsMatchPlayerController` conserva su clase para `BP_VampsMatchPlayerController`. Se retiran el actor de cámara libre, su propiedad transitoria, su archivo `.cpp` y sus llamadas desde el GameMode. `UVampsHUDWidget` conserva la cuenta regresiva de respawn.

## Daño y preservación de perfiles

- El usuario abrió Unreal y dejó PIE detenido. Se identificaron por MCP cuatro DAs de `UVampsCombatSettings` y ninguna clase derivada. Antes de editar se capturaron las 30 propiedades de cada perfil, incluidos arrays, transforms y referencias de armas, animaciones y locomoción. Se guardaron los cuatro paquetes por MCP y se confirmó `dirty=false`. [Respaldo completo](danio-antes.json).
- `AttackDamage` se retira de `AVampsCharacter` y pasa a **Damage → Attack Damage** de `UVampsCombatSettings`. Se inicializa en 12 en los cuatro perfiles, el mismo valor leído en los defaults del jugador y del rival. `UVampsCombatResolver::QueueCombatContact` valida y lee el daño base del DA del atacante. Los críticos y la reducción por bloqueo conservan sus campos y su cálculo.
- Los perfiles son `DA_ExecutionerCombatSettings`, `DA_DualSwordsCombatSettings`, `DA_SwordAndShieldCombatSettings` y `DA_LancerCombatSettings`, todos en `/Game/Vamps/Data/`. La partida asigna al personaje el DA de su perfil antes de terminar el spawn. En `L_CharacterBasic` se usa `Combat Settings` del Blueprint, con Dual Swords en `BP_VampsCharacter_Player` y Executioner en `BP_VampsCharacter` al capturar.

## Verificación

- Se aplicó clang-format a los archivos C++ modificados y pasó `git diff --check`. La búsqueda de llamadas confirmó que la presentación de muerte se dispara desde el personaje y que se retiraron las referencias nativas a la cámara libre.
- La [guía](editor.md) registra la compilación y las comprobaciones en Unreal que quedan pendientes. Los cambios de propiedades reflejadas requieren compilar con el editor cerrado y verificar los cuatro DAs al reabrir.
