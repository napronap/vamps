# 005 — IA básica y controles de prueba

**Continuación:** [006](../006-golpe-de-puno/reporte.md) reemplaza el arma por la hitbox del puño y ajusta la distancia de ataque de IA. Este reporte conserva los cambios de 005.

**Fecha:** 2026-10-05. Código y documentación; sin cambios en assets ni configuración del proyecto.

Se agrega un rival controlable con F1 para observar el [combate mínimo de 004](../004-hit-y-guard/reporte.md). Se mantienen un solo ataque, guardia sostenida, trazas por sockets, HUD y lock-on. No se agregan combos, parry, fases, resultados ni buffer.

## Cambios y relaciones

Las rutas de código son relativas a `Source/vamps/`, con headers en Public e implementaciones en Private.

| Archivo / clase | Responsabilidad y cambio |
|---|---|
| `AI/DuelAIController.h/.cpp` | Reintroduce un controlador pequeño. Decide por timer y solicita acciones a su personaje; no calcula contactos, daño ni bloqueos. Expone EDuelAIMode para elegir la situación de prueba |
| `GameModes/DuelGameMode.cpp` | Asigna ese controlador al rival al crearlo. R reinicia su comportamiento después de restaurar vida, posiciones y objetivos, conservando el modo |
| `Player/DuelPlayerController.h/.cpp` | Crea la Input Action de F1 en memoria y llama CycleAI. Conserva ataque/guardia separados y los demás controles |
| `UI/DuelHUD.cpp` | Agrega el modo de IA y la ayuda de F1. Mantiene vida y estado IDLE/ATTACK/GUARD |

El comportamiento y los controles actuales están en [prototipo funcional](../../prototype/functional.md); el timer, movimiento y responsabilidades en [prototipo técnico](../../prototype/technical.md). Este reporte conserva los nombres/teclas históricos de 005. El enum solo selecciona modos de prueba, sin duplicar el estado GAS.

Los modos anteriores Combo y Reactive se reemplazan por Attack y Guard: golpe único repetido y defensa mantenida. Mixed alterna comportamientos de forma fija, sin anticipar inputs del jugador. Los timers regulan decisiones de la IA; los contactos siguen dependiendo del arma y del notify de la animación.

No se modifican las habilidades, el personaje, las trazas ni el Gameplay Effect en esta iteración. La IA utiliza RequestAttack/RequestGuard/RequestMove y el mismo lock-on que ya tiene el rival. AIModule ya estaba en las dependencias; no se agregan módulos.

## Blueprints y assets

- `/Game/Duel/Blueprints/BP_DuelGameMode` conserva sus referencias al personaje, PlayerController y HUD. Su padre nativo crea el controlador del rival; no necesita un Blueprint nuevo de IA.
- `/Game/Duel/Blueprints/BP_DuelCharacter` conserva Auto Possess AI = Disabled. El GameMode asigna la clase nativa solo al rival. Arma, sockets y Attack Montage siguen la base 004.
- `/Game/Duel/Blueprints/BP_DuelPlayerController` conserva las referencias Move, Look, Lock y Reset y `/Game/Duel/Input/IMC_Duel`. F1 se mapea por código, sin necesitar IA_DuelAIMode.
- `/Game/Duel/Maps/L_Duel` no cambia. Si el usuario agregó G/H al Level Blueprint siguiendo 004, debe desconectarlos para evitar que interfieran con los modos.
- `/Game/Duel/Animations/ABP_Duel` no requiere lógica de IA. La pose opcional de guardia sigue bIsGuarding.

No se realizaron operaciones de editor ni se confirmó configuración manual del usuario. La [guía](editor.md) contiene los pasos pendientes y reemplaza las pruebas manuales del rival estático de 004.

## Documentación y verificación

Se actualizan los pares funcional/técnico de juego y combate, roadmap e índice. La carpeta 004 indica la continuación en 005 y conserva su reporte histórico.

Revisión estática del flujo F1 → controller → personaje, cancelación al cambiar de modo, limpieza del timer, reinicio conservando modo y lectura del HUD. Las firmas de posesión se contrastaron con Pawn.h de UE 5.8. Se aplicó clang-format a los seis archivos C++ afectados. `git diff --check` no encontró errores de whitespace; los 100 enlaces locales de archivos resuelven y no hay reportes sueltos. Content y Config no tienen cambios.

No se ejecutan tests automatizados. No se compiló ni se ejecutó PIE: no hay herramientas MCP directas de Unreal disponibles en esta sesión. La revisión de código no confirma el funcionamiento jugable; queda pendiente la compilación completa y las comprobaciones de la guía.

La IA es local, sin navegación alrededor de obstáculos ni lógica de muerte. AttackDistance = 150 cm solo decide cuándo solicitar un ataque: una mesh corta puede no alcanzar desde esa distancia. Ajustar ese valor del controlador nativo al arma elegida si hace falta; no reemplazar las trazas por daño basado en distancia. El prototipo no incorpora input por red ni predicción.
