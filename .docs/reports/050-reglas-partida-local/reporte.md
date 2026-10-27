# 050 — Reglas de la partida local del paso 6

Implementación posterior: [051 — Partida local 2v2](../051-partida-local/reporte.md), con [guía del editor](../051-partida-local/editor.md). Registra acceso MCP, código, assets y verificaciones de 6.1.

## Resultado — 2026-10-27

Integra las anotaciones sobre [049](../049-revision-paso-6/reporte.md) y sus precisiones posteriores: primera entrega local 2v2, todos Executioner; recompensa por racha; eliminación del portador; respawn por equipo; contadores, muerte, cámara libre y resultado con estadísticas/revancha. El estado vigente queda en [partida funcional](../../game/functional.md) y su [técnico](../../game/technical.md), con el [roadmap](../../roadmap.md) dividido en 6.1 y 6.2.

No hubo cambios de código, configuración ni assets. Se actualizaron especificaciones y planificación, sin implementar ni marcar cerrado el paso 6. Se preservan los cambios previos de 049 que ya estaban en el árbol de trabajo.

## Decisiones incorporadas

| Anotación | Incorporación |
|---|---|
| 1 — Personajes | Executioner para todos; la selección de personaje pasa a la segunda parte del paso 6. La respuesta adicional fija jugador y bot en A contra dos bots en B. No se incorpora ahora un selector ni lógica de skills/perfiles. |
| 2 — Puntos y victoria | El usuario delega valores iniciales, pide mejor ratio para el cierre simultáneo y recompensa creciente por kills. Se eligen objetivo 1000, base 100 e incremento 25 por kill de la racha de la víctima. La respuesta adicional elige racha desde la última muerte y empate si también coincide K/D. |
| 3 — Portador | Retirado del alcance de partida y de la integración de red del roadmap. Los reportes anteriores conservan su contenido histórico con enlace a esta revisión. |
| 4 — Respawn | Spawn point por equipo, espera inicial de 5 s y protección de 2 s. Vida desde el máximo del personaje, carga de guardia cero y energía máxima; sin imponer una vida global de partida. |
| 5 — Espera | Mensaje con countdown de respawn. La precisión posterior pide muerte normal/defensiva y cámara libre durante la espera. |
| 6 — Resultado/revancha | Ganador A/B o empate, detención del ciclo y panel pequeño con puntos y kills/muertes/K/D por participante/equipo. Botón local para otra partida; aceptación de todos reservada a una ampliación posterior. |

Los parámetros de puntuación elegidos por Codex son balance inicial autorizado, no valores extraídos del original. La respuesta posterior del usuario fija que atacar termina la protección restante; una solicitud rechazada no la consume.

## Contadores, muerte y protección

El usuario pide contadores similares al de respawn para inicio y duración. Se eligen como valores iniciales ajustables 5 s de preparación y 5 min de partida. Al vencer el tiempo decide puntuación y luego K/D; la igualdad completa produce empate. El contador depende de la autoridad y no del tiempo acumulado al dibujar el HUD.

La muerte usará dos referencias por personaje en `UVampsCombatSettings`, planificadas como `DeathMontage` y `GuardDeathMontage`. La variante defensiva depende del contacto letal bloqueado, incluida ruptura letal; la normal, del contacto abierto. Se captura antes de que el callback de Health borre la reacción. La presentación se ejecutará después de consolidar el lote, sin sustituir una referencia faltante por otra animación. Se mantiene el cuerpo visible sin bloqueo físico durante la espera; la cámara se separa del eliminado y admite control libre.

Inventario de archivos: existen `/Game/GreatSwordAnimV1/Animation/RM/A_GreatSwordCombatAnimationV1_Death_RM`, `A_GreatSwordCombatAnimationV1_Defence_Death_RM` y `A_GreatSwordCombatAnimationV1_Defence_Idle_Hit_Death_RM`. No se encontraron montages de muerte propios bajo `/Game/Vamps`. Esta lectura de nombres en disco no certifica tipo, skeleton, duración ni cuál variante defensiva corresponde mejor; falta inspección MCP y observación del recurso.

**Ampliación del DA pendiente:** no hay MCP directo de Unreal disponible en esta sesión. AGENTS.md exige buscar los assets de la clase y sus derivadas, capturar todos sus valores y guardarlos antes de editar la clase. Por eso no se agregaron aún los dos campos al header ni se modificó/compiló el DA. La [guía](editor.md#2-preservar-el-da-antes-de-agregar-campos) detalla el requisito previo; no se usa un snapshot histórico ni defaults para reemplazarlo.

## Archivos y relaciones

- `.docs/game/functional.md`: reemplaza preguntas respondidas por reglas vigentes, elimina portador y agrega entregas, puntuación por racha, respawn/protección, contadores, muerte/cámara y resultado. El tipo de control libre queda como pregunta concreta.
- `.docs/game/technical.md`: diseña autoridad de partida, identidad persistente, contexto de baja, consolidación de lote, montages de muerte y presentación/revancha; distingue diseño futuro de código existente.
- `.docs/player/functional.md` y `technical.md`: sustituyen presentación de portador por estadísticas/countdown/resultado y conectan el HUD existente de 048 con los datos futuros de partida.
- `.docs/roadmap.md`: registra 6 en definición, divide ciclo Executioner y selección de personaje y retira portador de 6/7/exclusiones adaptadas. Actualiza resumen y descripciones de movilidad/presentación con referencias 039–048, sin nuevas aprobaciones de cierre.
- `.docs/README.md`, enlace desde 049 y este par de reportes: navegación hacia las reglas vigentes. Los reportes históricos no se reescriben como si ya hubieran tenido estas decisiones.

El ciclo deberá conectar `AVampsCharacter`, `UVampsCombatResolver`, `UVampsLockOnComponent`, IA, cámara y `AVampsCombatTestHUD` con una autoridad propia. No existe todavía esa implementación. El GameMode de observación y R conservan su función previa.

## Verificación de recursos y simultaneidad

Se revisaron `UVampsAttributeSet`, `AVampsCharacter::BeginPlay/ResetCombatState` y la declaración de `UVampsCombatSettings`: MaxHealth ya existe como atributo y la inicialización nativa parte de 100; guardia y energía ya tienen sus capacidades/restauración. No se presenta una configuración actual de vida por personaje como comprobada: permitir configurarla es parte del próximo cambio. No se modificó ninguna clase/estructura de DA ni se inspeccionaron sus valores actuales por MCP.

El cálculo de recompensa necesita rachas anteriores al lote, crédito único de contacto letal y racha final cero para cualquier eliminado, incluso si obtiene una kill simultánea. El resultado usa totales de equipo, no promedio de ratios individuales ni números redondeados del HUD. La retirada del pawn debe esperar al cierre de contactos válidos, conforme a la revisión 049.

## Pendientes y límites

Queda elegir el tipo de control de la cámara libre en las [preguntas funcionales](../../game/functional.md#preguntas). Los contadores, las variantes de muerte y el fin de protección al atacar ya se incorporaron. La ampliación del DA necesita primero la captura actual por MCP; no se presenta como implementada.

Se revisaron referencias locales y coherencia de los documentos afectados; `git diff --check` sin errores de espacios. No corresponde compilar una iteración documental. No se realizaron operaciones de Unreal, guardado de assets, automatización de UI ni creación/ejecución de tests. La [guía](editor.md) conserva lo necesario para continuar.
