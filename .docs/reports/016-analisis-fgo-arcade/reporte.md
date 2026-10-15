# 016 — Análisis de cuatro videos de FGO Arcade

Continuación vigente: [017 — Base FGOA y extensiones Vamps](../017-base-fgoa-y-extensiones/reporte.md). Integra el alcance de esta investigación en las especificaciones principales y reemplaza la copia `alcance/` por documentos de juego, combate y jugador. Este reporte conserva los resultados de 016; su comparación documental es histórica.

## Alcance y resultado

Se analizaron `full demo.mp4`, `newdemo1.mp4`, `newdemo2.mp4` y `newdemo3.mp4`, aportados por el usuario en `D:\OBS recordings`, junto con controles y aclaraciones. Se contrastó la referencia con documentos/fuentes activas y se incorporaron R01–R07, comprobando las escenas indicadas. Revisión consolidada el 15 de octubre de 2026.

**La revisión de videos queda cerrada.** Se integraron respuestas y un par funcional/técnico separado con el alcance elegido. La continuación tras golpe bloqueado queda resuelta para atacante y defensor. P02–P04 concentran fórmulas, partida/portador y decisiones técnicas. No se piden más grabaciones ni operaciones Unreal. Ranged y SUPPORT quedan fuera; crítico queda incluido.

No hubo cambios de código, assets ni configuración del juego. Las especificaciones funcionales/técnicas y el roadmap conservan su contenido. El setup Unreal sigue en [015](../015-retirada-duel/reporte.md) y sus dependencias.

## Entregables

| Archivo | Resultado |
|---|---|
| [analisis.md](analisis.md) | Reglas consolidadas de las cuatro sesiones, evidencia y límites; reemplaza interpretaciones iniciales sobre guardia y NA |
| [escenas.md](escenas.md) | Nuevo índice de secuencias, ampliaciones y tiempos para localizar cada conclusión |
| [comparacion.md](comparacion.md) | Contraste actualizado con documentación/código; conserva los desajustes de nombres y capacidades identificados |
| [preguntas.md](preguntas.md) | Respuestas R01–R07/P01 integradas y tres grupos restantes P02–P04 |
| Alcance funcional, integrado por 017 en [juego](../../game/functional.md), [combate](../../combat/functional.md) y [jugador](../../player/functional.md) | Comportamientos y límites elegidos para Vamps, separados de la referencia y del prototipo actual |
| Base técnica, integrada por 017 en [juego](../../game/technical.md), [combate](../../combat/technical.md) y [jugador](../../player/technical.md) | Responsabilidades y flujo futuro sobre la base verificada, sin fingir implementación |
| [editor.md](editor.md) | Aportes del usuario, cierre de revisión y continuación documental; sin setup Unreal nuevo |
| `evidencia/E01…E14.jpg` | Catorce capturas conservadas del primer video |
| `evidencia/N01…N18.jpg` | Dieciocho láminas nuevas; N07 renombrada para corregir la atribución anterior de amenaza no seleccionada |
| [Índice general](../../README.md) | Enlaces a la investigación vigente y distinción respecto al setup de 015 |

Los MP4 permanecen en su ubicación original. Scripts de decodificación, detector de overlay y muestras de trabajo quedan fuera del repositorio, en el directorio de visualizaciones de esta conversación. No se instalaron dependencias ni se copiaron videos al proyecto.

## Hallazgos principales

- La defensa no se limita a preparación: Holmes defiende después de iniciar gesto ofensivo en newdemo3 00:11,70. No se convierte una animación en bloqueo automático.
- NA corresponde al draw, separado de reacción y Stunned. Defender conserva cartas y no dispara draw; el consumo ofensivo sigue la progresión de la cadena.
- Un primer ataque que queda corto se ejecuta, consume su carta y corta la continuación pese a nuevas pulsaciones. Las cadenas pueden terminar con uno o dos pasos; el usuario extiende la regla de fallo a cualquier paso.
- Se corrobora retarget por guardia efectiva en newdemo3 00:14,30 y newdemo2 01:08,30, sin nuevo F. Acertar a otro enemigo habilita combo sin ese cambio forzado.
- Se confirma continuidad del atacante cuyo golpe se bloquea y del defensor que retoma ofensiva después de bloquear; ambos resultados se distinguen en la resolución. Se incorpora movimiento de cámara al atacar.
- Se distingue recuperación ofensiva aparente tras defensa de disponibilidad del botón: sigue admitiendo solicitudes de ataque/defensa, sin NA por bloquear. La cadena visual puede alternar inicio ofensivo y resolución defensiva por timings; no se fija un cooldown adicional universal.
- Recibir daño no consume cartas/inicia NA universalmente: Da Vinci pierde HP antes de defender, conserva cinco cartas y sale del intercambio sin redraw visible.
- Acercamiento limitado por acción, distinción entre alcance de avance y contacto, y contraste de ataques estáticos ranged. Root motion escalado es una hipótesis técnica.
- Pulsaciones de continuación dentro de ventana, rechazo del click temprano y cierre voluntario al final del paso por no continuar/evasión.
- Lock-on continuo, F en orden HUD, cámara de referencia y fade de oclusores visibles, separado de colisión/boundaries.
- Vamps adopta 360°, residual letal, guard break/stun, Shift sin dirección hacia atrás y cadena/NA sin cartas ni Extra; un personaje con respawn, equipos/puntos y portador de recompensa. Ultimate posterior sin cinemática global. No hay previsión de compatibilidad con cartas.

## Relaciones de código y assets

Se leyeron `VampsCharacter`, `VampsCharacter_Player`, controllers normal/de prueba, `GA_Attack`, `GA_Guard`, `FistHitboxComponent`, `ANS_AttackTrace`, `VampsAnimInstance`, AttributeSet, HUD y GameMode de prueba. Se volvió a contrastar recepción/exclusión de habilidades, input, cámara, trazas, atributos y escena de observación para el par de alcance. [comparacion.md](comparacion.md#relaciones-actuales-que-afectan-el-rediseño) explica diferencias; [base técnica vigente](../../game/technical.md) conecta las decisiones sin agregar clases futuras innecesarias.

Los Blueprints y recursos `/Game/Vamps` se referencian según 013–015; no se abrieron, compilaron ni verificaron sus defaults. No se modificaron conexiones entre clases, Blueprints o animación. Observar FGO no valida la ejecución de Vamps.

## Verificaciones y límites

| Archivo | Bytes | Frames | Duración nominal |
|---|---:|---:|---:|
| full demo.mp4 | 270.081.224 | 5491 | 183,03 s |
| newdemo1.mp4 | 210.750.185 | 4823 | 160,77 s |
| newdemo2.mp4 | 234.711.109 | 4685 | 156,17 s |
| newdemo3.mp4 | 222.784.388 | 4656 | 155,20 s |

Se confirmó acceso y decodificación a 30 fps/1280 × 720. Se recorrieron las sesiones por muestreo y ampliaciones cada 0,1–0,25 s. El overlay se examinó en todos los fotogramas y se contrastó especialmente ausencia/presencia de F en retarget. Se revisaron secuencias y 32 imágenes conservadas, incluyendo defensa dentro de ofensiva, gesto propio, cambio de selección, cartas listas/usadas, fade, amenaza y Extra con B/A/B.

Se revisaron enlaces locales, referencias internas y whitespace. Las fuentes y especificaciones originales conservan su contenido. No se ejecutaron tests automatizados, compilación C++ ni operaciones Unreal; no hubo automatización del editor.

Los fotogramas no revelan fórmulas, fase exacta de tracing, mecanismos de movimiento/cámara ni resolución de red. Los efectos simultáneos limitan la atribución de algunos contactos. Se distingue evidencia visual de explicación aportada e hipótesis, y no se supone equivalencia de la build grabada con todas las versiones del arcade.

## Continuación

La integración documental se realizó en [017](../017-base-fgoa-y-extensiones/reporte.md). [P02–P04](preguntas.md#reglas-que-requieren-respuesta) enlazan las preguntas vigentes de cada funcionalidad. El desempate se elige al diseñar la resolución; valores de balance/cámara se calibran con prototipo. [editor.md](editor.md) identifica los aportes y la continuación. No se implementó código ni se prepararon assets.
