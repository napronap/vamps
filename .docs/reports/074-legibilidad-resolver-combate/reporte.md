# 074 — Legibilidad del resolver de combate

`UVampsCombatResolver` documenta cada condición del flujo en `Source/vamps/Private/Combat/VampsCombatResolver.cpp`. Los comentarios explican qué excluye cada guarda y cómo influyen la prioridad de ataque, el estado de los participantes y la selección de la reacción.

`Source/vamps/Public/Combat/VampsCombatResolver.h` describe el alcance de `DiscardPendingCombatFor()`. Durante el reset o `EndPlay` de `AVampsCharacter`, el resolver retira los contactos en cola en los que participa el pawn, sus reproducciones registradas y sus callbacks pendientes. El lote que ya se está resolviendo queda fuera de esa limpieza. Esta operación se introdujo en la [iteración 073](../073-cancelacion-ia-reset-combate/reporte.md).

Los comentarios de las tres colas que agregó el usuario se precisaron para describir el registro por orden de inicio del ataque, los contactos que esperan resolución y los callbacks que se ejecutan después de procesarlos. `FAttackPlayback` contiene el pawn y el ID de instancia del montage. La comprobación de vigencia se trasladó a `UVampsCombatResolver::IsAttackPlaybackActive()`, que recibe el registro por referencia constante y mantiene los criterios previos de validez, vida y coincidencia del montage.

El cambio se concentra en el código C++ del resolver. Los Blueprints y los assets de combate se conectan a través de `AVampsCharacter` y de las llamadas existentes al resolver. Se quitaron los puntos finales de los comentarios del `.cpp` y el `.h`.

## Verificación

`git diff --check` pasó. Live Coding devolvió `Success` y el log de Unreal confirmó la recarga con `Reload/Re-instancing Complete` antes del ajuste de puntuación. La [guía del editor](editor.md) registra la comprobación.
