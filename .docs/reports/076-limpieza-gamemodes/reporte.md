# 076 — Limpieza de GameModes

`AVampsMatchGameMode` usa `AvailableCharacters` directamente. La configuración del GameMode aporta cuatro o más perfiles distintos, clases de luchador válidas, el controlador local y un punto de aparición para cada equipo. Se retiran las validaciones y logs del GameMode de partida. `SelectCharacter` asigna el perfil y avisa al HUD.

`StartLocalRound` elige los perfiles, reemplaza el plantel, asigna los controladores y empieza la preparación. `SpawnParticipants` asigna equipo y perfil antes de `FinishSpawning` y crea a cada luchador en su posición de formación. `BeginPlay` del personaje inicializa el combate. El GameMode reinicializa la animación con el perfil asignado. `RespawnParticipant` reutiliza la misma posición, limpia `RespawnAt` antes de restaurar vida y vuelve a habilitar colisión y movimiento.

El cierre de ronda calcula el ganador antes de publicar `Finished`. Cada baja suma una kill a un equipo y una muerte al contrario, así que en esta partida 2v2 comparar kills conserva el desempate por K/D. Las bajas del frame se procesan primero para sumar puntos y rachas y luego para limpiar las rachas de los caídos. Volver al selector reutiliza el estado que dejó el cierre de ronda.

`AVampsCombatTestGameMode` usa los componentes nativos en el reset. El ciclo de IA vuelve de `Mixed` a `Passive` de forma explícita. Las acciones de prueba salen cuando el rival o su controlador ya no están disponibles.

Los Blueprints `BP_VampsMatchGameMode` y `BP_VampsCombatTestGameMode` conservan sus propiedades y su vínculo con `L_LocalMatch` y `L_CharacterBasic`, respectivamente. Los cambios de C++ se limitan a los GameModes.

## Verificación

`VampsMatchGameMode.cpp` quedó en 298 líneas. Se revisaron la inicialización de los participantes, las bajas simultáneas, el respawn y los criterios de victoria. Se aplicó `clang-format` y pasó `git diff --check`. Quedan la compilación y la comprobación en PIE. [Guía del editor](editor.md).
