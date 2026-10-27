# Editor — Reglas de la partida local

**Para el usuario:** las reglas están en [partida](../../game/functional.md). Falta precisar el tipo de control de cámara libre. Para agregar los montages al DA se necesita restablecer MCP directo y capturar su configuración antes de editar la clase; ese paso no se realizó. La selección de personaje se aborda en 6.2.

## 1. Completar las precisiones de comportamiento

**Hecho — usuario.** La cámara libre con vuelo WASD/mouse está implementada en 051 y su funcionamiento quedó confirmado en [053](../053-cierre-local-y-documentacion/editor.md). La regla se integra en [partida](../../game/functional.md#equipos-y-bajas); no queda una elección pendiente de cámara para 6.1.

## 2. Preservar el DA antes de agregar campos

**Pendiente — Codex por MCP.** Restablecido el acceso directo al editor, buscar `/Script/vamps.VampsCombatSettings` y sus derivadas en todas las raíces de contenido. Capturar los valores completos de todos los assets encontrados, no solo `/Game/Vamps/Data/DA_VampsCombatSettings`: escalares, montages, armas, arrays de pasos y transforms anidados. Guardar copia durable dentro de esta iteración y guardar los assets afectados por MCP antes de editar el header.

La regla de [AGENTS.md](../../../AGENTS.md) exige «Antes de modificar la clase de un Data Asset [...] buscar por MCP los assets existentes» y «Leer y guardar una copia durable de sus valores actuales antes de editar o compilar». Esta captura no está disponible ahora, así que los campos `DeathMontage`/`GuardDeathMontage` siguen pendientes; no se solicita compilar ni guardar una clase ya modificada.

## 3. Montages y asignación posterior

**Pendiente — Codex por MCP, después de la captura.** Inspeccionar tipo, skeleton y duración de los recursos `A_GreatSwordCombatAnimationV1_Death_RM`, `A_GreatSwordCombatAnimationV1_Defence_Death_RM` y `A_GreatSwordCombatAnimationV1_Defence_Idle_Hit_Death_RM` en `/Game/GreatSwordAnimV1/Animation/RM`. Preparar montages propios para la variante normal y la defensiva elegida bajo `/Game/Vamps/Animations/Executioner`, con slot compatible con ABP_Executioner y pose final de muerte conservada hasta respawn. Los assets propios nuevos todavía no existen y no se modificará el pack fuente.

Agregar las dos referencias requeridas al DA solo después de preservar valores. Tras la compilación/arranque limpio que corresponda, releer, restaurar y comparar toda la configuración anterior, asignar los dos montages, verificar referencias sin transitorios y guardar solo los assets afectados. Si una operación no tiene herramienta directa, documentar el paso manual concreto antes de continuar, sin automatizar UI.

## 4. Implementación y preparación del escenario

**Pendiente — Codex por MCP, cuando exista implementación y herramientas disponibles.** Preparar las referencias y spawn points de A/B para el ciclo local. Actualmente solo existe la escena de observación `/Game/Vamps/Maps/L_CharacterBasic`; esta revisión no crea un mapa, GameMode ni assets de partida. Los nombres y transforms nuevos se concretarán en la implementación, sin asumir posiciones del duelo como spawns finales de 2v2.

La vida se configurará desde stats por personaje. La preservación de datos para ampliar el DA está en el paso 2 y precede a cualquier edición de sus tipos.

## 5. Comprobaciones posteriores

**Pendiente — usuario, después de implementar y compilar el ciclo.** Comprobar asignación A/B y exclusión de aliados; contador inicial de 5 s y reloj de 5 min; baja única con recompensa por racha; muerte normal/defensiva; cámara de espera y countdown; reset de racha/recursos; protección de hasta 2 s que termina al atacar; cierre por 1000 puntos o tiempo y desempate por K/D; detención de acciones/respawns y «Otra partida» con estadísticas limpias. Incluir una baja recíproca: su recompensa no debe depender del orden del lote.

Los pasos exactos de editor, valores asignados y resultados se registrarán en la guía de implementación. No hay cambios de assets que guardar ni operaciones de compilación realizadas por Codex en esta revisión.
