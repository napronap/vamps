# 017 — Base FGOA y extensiones Vamps

Continuación vigente: [018 — Componente de lock-on y HUD](../018-lockon-component/reporte.md), con implementación nativa y configuración/compilación del editor pendientes. Las especificaciones reorganizadas en esta iteración conservan su contenido.

## Alcance y resultado

Se revisaron los últimos documentos de [016](../016-analisis-fgo-arcade/reporte.md): análisis, comparación, respuestas y alcance funcional/técnico. Sus decisiones se integran en las especificaciones principales. El orden de trabajo prioriza reproducir la base FGOA adaptada y agregar después las herramientas propias de Vamps.

Se mantienen dentro del primer alcance las adaptaciones ya elegidas: cadena/NA sin cartas, un melee con respawn del mismo personaje, evasión hacia atrás sin dirección y portador sin temática Grial. Se conservan las exclusiones de cartas/QAB, Extra Attack, reservas, SUPPORT y ranged inicial. Parry, perfiles, dos skills por personaje y ultimate propia quedan en documentos de extensiones posteriores.

No hubo cambios de código, assets ni configuración. No se implementaron los comportamientos documentados ni se modificaron sus conexiones en Unreal.

## Archivos y organización

| Archivos | Resultado y motivo |
|---|---|
| `.docs/game/functional.md` y `technical.md` | Alcance, adaptaciones y partida por equipos, muerte/respawn, portador, autoridad y dependencias |
| `.docs/combat/functional.md` y `technical.md` | Solicitud compartida, fases, continuación por ventana/contacto, defensa durante acción, NA, residual, reacción, guard break y crítico |
| `.docs/player/functional.md` y `technical.md`, nuevos | Controles, movilidad, lock-on, retarget, encuadre/cámara por acción, oclusión y HUD |
| `.docs/prototype/functional.md` y `technical.md`, nuevos | Estado real de las fuentes y conexiones conocidas, separado del objetivo; corrige nombres Duel, F1/Tab y seguimiento inexistente |
| `.docs/extensions/functional.md` y `technical.md`, nuevos | Capa posterior de parry, herramientas propias y ultimate sin interrupción/cinemática global |
| `.docs/characters/functional.md` y `technical.md` → `.docs/extensions/characters/` | Conserva criterios, cinco perfiles, dos skills, ejemplos y preguntas; actualiza dependencia a la base de referencia |
| `.docs/reports/016-analisis-fgo-arcade/alcance/functional.md` y `technical.md`, retirados | Contenido distribuido en las especificaciones principales; elimina la copia paralela del alcance |
| Análisis, comparación, preguntas, reporte y guía de 016 | Referencias actualizadas; comparación identificada como histórica y preguntas enlazadas a su funcionalidad |
| Guías de 003, 004, 005 y 006 y reportes de 004/005 | Enlaces al flujo/modos del prototipo, conservando el historial de cada iteración |
| Reporte de 015 | Enlace a la iteración documental vigente, sin resolver sus pendientes |
| `.docs/roadmap.md` y `.docs/README.md` | Prioridades de reproducción, extensiones posteriores e índice de todos los pares y entregables |
| Este reporte y [editor.md](editor.md) | Alcance, comprobaciones, atribución y continuidad sin operaciones Unreal nuevas |

Las preguntas que el análisis no resuelve permanecen al final de la funcionalidad correspondiente. No se inventan vigencia defensiva, fórmula de crítico/guardia, condiciones de partida, selección al perder target ni desempate de contactos. Se agregan las preguntas necesarias sobre respawn, selección inicial y restricciones de evasión, sin convertirlas en reglas elegidas.

## Relaciones de clases y assets

Se revisaron las fuentes propias para distinguir lo existente de lo que exige la referencia. `AVampsPlayerController` enlaza Attack/Guard separados; `AVampsCharacter` conserva GAS y `ReceiveHit`; `UGA_Attack`/`UGA_Guard` se excluyen mutuamente. `UFistHitboxComponent`/`UANS_AttackTrace` detectan el golpe; `UVampsAttributeSet`/`UGE_VampsDamage` mantienen vida y daño. El núcleo objetivo debe sustituir la cancelación incondicional del agresor por resolución de contacto y continuidad para ambos participantes.

`AVampsCharacter_Player` solo añade cámara libre. `AVampsAIController` solicita acciones y el GameMode/controller/HUD de prueba permiten observarlas; no implementan target, muerte o partida por equipos. `UVampsAnimInstance` conserva presentación, sin decidir disponibilidad. No se añadieron ni modificaron clases.

Las conexiones de `/Game/Vamps/Blueprints`, `ABP_VampsCharacter`, `BS_VampsCharacter`, `AM_BasicAttack` y `L_CharacterBasic` se conservan según 013–015 y se consolidan en [prototipo técnico](../../prototype/technical.md#blueprints-y-animación). No se consultaron nuevamente sus defaults ni se abrieron/compilaron assets. La revisión de fuentes no certifica el contacto del montage ni el estado de la sesión Unreal.

## Verificaciones y límites

- Revisión documental contra las decisiones de 016 y lectura de fuentes de controller, personaje, habilidades, contacto, atributos, animación, IA, HUD y GameMode.
- Conservación de perfiles/skills originales en su nueva ubicación; separación entre evidencia FGOA, especificación objetivo y prototipo existente.
- Seis pares funcional/técnico completos; 421 enlaces internos y anclas revisados en 54 Markdown, sin destinos ausentes. Las cuatro referencias externas a videos se conservaron sin volver a abrir los archivos.
- Sin referencias enlazadas a las rutas retiradas; revisión de whitespace de todos los Markdown y `git diff --check` sin incidencias.
- Sin compilación C++, operaciones Unreal ni tests automatizados: el alcance es documental.

Los valores de movimiento/cámara y balance requieren calibración posterior. La [guía](editor.md) distingue esta edición de los pendientes de setup anteriores. La investigación de videos sigue cerrada; no se requieren nuevas grabaciones.
