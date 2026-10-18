# Evidencia de FGOA y del código Vamps

Soporte de [la guía principal](analisis.md). Los recortes se conservan dentro de esta iteración, con HUD y overlay de entradas completos. Las láminas facilitan localizar momentos; el MP4 conserva la secuencia a velocidad original.

## Videos reproducibles

Los tiempos de la tabla son del archivo original. Al abrir un recorte, su tiempo0 corresponde al inicio indicado. El manifiesto conserva frames de origen, FPS, hashes y el límite final exclusivo de cada rango.

| Escena / recorte | Original y tramo | Qué muestra | Qué no prueba |
|---|---|---|---|
| [V01 — Preparación y defensa](<evidencia/V01-newdemo2-00612-00690.mp4>) · [lámina](evidencia/V01-secuencia-1.jpg) | newdemo2,20,40–23,00 | `1st Attack` hacia20,70, barrera20,90 y defensas posteriores, sin draw intermedio. | Que la ventana ofensiva propia estuviera activa ni qué IDs se usaron. |
| [V02 — Guardias repetidas](<evidencia/V02-newdemo1-01569-01689.mp4>) · [lámina](evidencia/V02-secuencia-1.jpg) | newdemo1,52,30–56,30 | Barreras repetidas, pérdidas de HP y cartas conservadas durante presión. | Un click obligatorio por cada hit, la fórmula de freeze o la causa de cada cifra superpuesta. |
| [V03 — Guardia y retarget](<evidencia/V03-newdemo3-00405-00444.mp4>) · [lámina](evidencia/V03-secuencia-1.jpg) | newdemo3,13,50–14,80 | Master A seleccionado a la derecha; E entra por izquierda; barrera14,10 y LOCK en E14,30. | Un mapa por atacante o velocidad exacta del giro del cuerpo. |
| [V04 — Segundo retarget](<evidencia/V04-newdemo2-02016-02064.mp4>) · [lámina](evidencia/V04-secuencia-1.jpg) | newdemo2,67,20–68,80 | Barrera67,90–68,10 y cambio hacia E68,30. La F manual previa está en65,93. | Que el daño abierto previo cambiara por sí solo el objetivo. |
| [V05 — Avance y fallo](<evidencia/V05-newdemo3-00084-00156.mp4>) · [lámina](evidencia/V05-secuencia-1.jpg) | newdemo3,2,80–5,20 | Preparación/avance tras solicitud, golpe corto frente al rival lejano y draw sin segundo ataque. | Distancia de mundo, algoritmo de raid o root motion escalado. |
| [V06 — NA anterior al daño](<evidencia/V06-newdemo2-04146-04233.mp4>) · [lámina](evidencia/V06-secuencia-1.jpg) | newdemo2,138,20–141,10 | Draw/NA antes de los impactos; golpes sin barrera y guardia hacia140,80 al volver disponibilidad. | Que el daño cause siempre NA o que un ID por sí solo decida guardabilidad. |
| [V07 — Acciones y subimpactos](<evidencia/V07-full demo-01734-01854.mp4>) · [lámina](evidencia/V07-secuencia-1.jpg) | full demo,57,80–61,80 | Acciones1/2/3, contactos sucesivos y final de cadena. | IDs, cantidad exacta de ventanas activas o señal interna universal de último hit. |
| [V08 — Reacción y Stunned](<evidencia/V08-full demo-03120-03240.mp4>) · [lámina](evidencia/V08-secuencia-2.jpg) | full demo,104,00–108,00 | Daño/reacciones y `Stunned` explícito hacia107,40. | La causa interna del stun ni una barra de guardia agotada. |
| [V09 — Daño sin redraw](<evidencia/V09-newdemo2-01089-01221.mp4>) · [lámina](evidencia/V09-secuencia-1.jpg) | newdemo2,36,30–40,70 | Pérdida inicial de HP sin barrera clara, posterior guardia y salida de presión conservando mano. | Una regla universal de cancelar toda cadena al recibir daño. |
| [V10 — Movimiento y cámara](<evidencia/V10-newdemo1-00156-00252.mp4>) · [lámina](evidencia/V10-secuencia-1.jpg) | newdemo1,5,20–8,40 | Desplazamiento lateral/diagonal y cambios de encuadre manteniendo selección. | Umbrales exactos o histéresis de cámara. |
| [V11 — Evasión](<evidencia/V11-full demo-00510-00570.mp4>) · [lámina](evidencia/V11-secuencia-1.jpg) | full demo,17,00–19,00 | Movimiento evasivo con entrada direccional en el contexto grabado. | Frames de invulnerabilidad o salida Step durante Guard en un caso controlado. |
| [V12 — Oclusión](<evidencia/V12-newdemo2-03960-04020.mp4>) · [lámina](evidencia/V12-secuencia-1.jpg) | newdemo2,132,00–134,00 | Vegetación próxima se vuelve translúcida/tramada. | Que deje de colisionar o que todo obstáculo reciba ese tratamiento. |
| [V13 — Draw y movimiento](<evidencia/V13-full demo-01830-01878.mp4>) · [lámina](evidencia/V13-secuencia-1.jpg) | full demo,61,00–62,60 | Fin de cadena, reposición y locomoción. | Duración universal de NA para todos los repertorios. |

Se revisaron las secuencias relevantes mediante fotogramas y láminas; no se atribuye a esta iteración una nueva observación continua de los cuatro videos completos. La revisión016 ya conserva el muestreo integral y el análisis del overlay, incluidas las escenas V03/V04 sin nueva F. Los recortes permiten volver a inspeccionarlas sin depender de capturas aisladas.

La [lista de escenas016](../016-analisis-fgo-arcade/escenas.md) añade cadenas parciales, ataques ranged, aviso direccional, Extra, reservas, Grial y fin de partida. Es evidencia complementaria; sus hipótesis sobre el mecanismo interno deben contrastarse con las trazas siguientes.

## Trazas nativas

Direcciones de la tabla expresadas como RVA: base `0x140000000`. Los nombres de propósito son explicativos. Se adjuntan36 funciones con C provisional, instrucciones y metadatos, reutilizadas sin modificar del análisis guardado. [Manifiesto de trazas y datos](evidencia/trazas-nativas.json).

| Clave | Afirmación respaldada | Funciones / datos incluidos |
|---|---|---|
| N01 | Generación, modos que renuevan/conservan identidad y contexto FIFO. | [C36AE0 generador](evidencia/nativo/140c36ae0.c), [C385E0 entrada Attack](evidencia/nativo/140c385e0.c), [6B3060 contexto](evidencia/nativo/1406b3060.c). |
| N02 | Admisión de guardia y memoria de últimos GuardID/HitID; registro de resultados. | [C521D0](evidencia/nativo/140c521d0.c), [C525A0 input/estado](evidencia/nativo/140c525a0.c), [C57EC0 guardia](evidencia/nativo/140c57ec0.c), [C588F0 golpe](evidencia/nativo/140c588f0.c). |
| N03 | count del JSON→runtime→descriptor→reacción y asignación del calendario. | [30D7D0](evidencia/nativo/14030d7d0.c), [30BA30](evidencia/nativo/14030ba30.c), [C56000](evidencia/nativo/140c56000.c), [C573E0](evidencia/nativo/140c573e0.c), [D10380](evidencia/nativo/140d10380.c). |
| N04 | Umbral de movimiento, calendario y compensación de la demora de reacción. | [D10460 is_freeze](evidencia/nativo/140d10460.c), [C38F90 ASM](evidencia/nativo/140c38f90.asm.txt), en particular C3912C..C3914D; [marcas de movimientos](evidencia/timing-guard-motions.json). El C automático omitía la compensación. |
| N05 | Reacción posterior y permisos distintos para ataque, locomoción y Step. | [C26230](evidencia/nativo/140c26230.c), [C27430](evidencia/nativo/140c27430.c), [C388F0](evidencia/nativo/140c388f0.c), [C7B9E0](evidencia/nativo/140c7b9e0.c), [C7BE80](evidencia/nativo/140c7be80.c), [C6AFE0](evidencia/nativo/140c6afe0.c), [C7C050](evidencia/nativo/140c7c050.c), [C3BE90](evidencia/nativo/140c3be90.c). |
| N06 | Memoria por ventana, índice temporal de subgolpe y empuje seleccionado por resultado. | [C5C870](evidencia/nativo/140c5c870.c), [C585A0](evidencia/nativo/140c585a0.c), [CCDA70](evidencia/nativo/140ccda70.c), [ejemplos extraídos](evidencia/orientation-examples.json). |
| N07 | Fuente del contacto, giro defensivo y seguimiento ofensivo independientes. | [C56770 snapshot](evidencia/nativo/140c56770.c), [CD6690](evidencia/nativo/140cd6690.c), [CD6B10](evidencia/nativo/140cd6b10.c), [CD44A0](evidencia/nativo/140cd44a0.c), [CD6590](evidencia/nativo/140cd6590.c), [datos Q01 y variantes](evidencia/orientation-examples.json). |
| N08 | Coste de guardia distinto de stun y contexto de ruptura. | [C534E0 coste](evidencia/nativo/140c534e0.c), [CD1320 descuento](evidencia/nativo/140cd1320.c), [D0FC30](evidencia/nativo/140d0fc30.c), [D10910](evidencia/nativo/140d10910.c), [A15AC0](evidencia/nativo/140a15ac0.c), [A1B140](evidencia/nativo/140a1b140.c), [campos de ataque inventariados](evidencia/attack-data-fields.json). |
| N09 | Orden del mundo, dos direcciones y límites de simultaneidad. | C5C870 ASM, llamadas C5D6DF/C5D6FE; recorrido CE1C80/CE25C0 y notas detalladas en el [informe fase2](<D:/fgo arcade/fgoac/research/combat-phase2-20261017-1405/INFORME_INSTANCIAS_Y_SIMULTANEIDAD.md>). No es un resultado dinámico de empate. |

Los informes originales y sus notas contienen la cadena completa y más funciones que el subconjunto adjunto:

- [Fase1: combate y estructura general](<D:/fgo arcade/fgoac/research/combat-20261017/INFORME_COMBATE.md>).
- [Fase2: ciclo de vida, simultaneidad y limpiezas](<D:/fgo arcade/fgoac/research/combat-phase2-20261017-1405/INFORME_INSTANCIAS_Y_SIMULTANEIDAD.md>).
- [Fase3: tiempos, permisos, orientación y guard break](<D:/fgo arcade/fgoac/research/combat-phase3-20261018-0022/INFORME_GUARDIA_TIEMPOS_Y_REACCIONES.md>).

## Código Vamps revisado

Se capturaron42 archivos propios de `Source/vamps/Public` y `Private`; se siguieron especialmente personajes, resolver, armas, movimiento, habilidad, notifies, entrada, cámara, lock-on, IA y atributos. [Hashes y rutas de cada fuente](evidencia/fuentes-vamps.json). Los fuentes copiados completos permanecen en [la carpeta de investigación](<D:/fgo arcade/fgoac/research/vamps-contrast-20261018-0838/evidence/code>), sin duplicarlos dentro del proyecto Vamps.

| Clave | Punto revisado | Archivo y entrada |
|---|---|---|
| C01 | Estado, identidad de reproducción y serial local. | [VampsCombatState.h](C:/dev/vamps/Source/vamps/Public/Combat/VampsCombatState.h:7), [BeginCombatStep](C:/dev/vamps/Source/vamps/Private/Characters/VampsCharacter.cpp:276). |
| C02 | Botón compartido, consumo y restricciones de fase. | [RequestCombatAction](C:/dev/vamps/Source/vamps/Private/Characters/VampsCharacter.cpp:194), [ConsumeDefenseRequest](C:/dev/vamps/Source/vamps/Private/Characters/VampsCharacter.cpp:614), [SetCombatPhase](C:/dev/vamps/Source/vamps/Private/Characters/VampsCharacter.cpp:663). |
| C03 | Duración calculada y renovación `max`. | [QueueCombatContact](C:/dev/vamps/Source/vamps/Private/Combat/VampsCombatResolver.cpp:29), [TryCalculateCombatSectionDuration](C:/dev/vamps/Source/vamps/Private/Characters/VampsCharacter.cpp:512), [ApplyOffensiveRestriction](C:/dev/vamps/Source/vamps/Private/Characters/VampsCharacter.cpp:570). |
| C04 | Lote, clasificación previa, retarget, aplicación y elección de movimiento. | [ResolveCombatContacts](C:/dev/vamps/Source/vamps/Private/Combat/VampsCombatResolver.cpp:142). La igualdad de ActionNumber aparece antes de RegisterCombatContact; ReceiveCombatHit queda fuera de esa condición. |
| C05 | Restricción de un ANS y memoria por arma. | [Validación de GA_Attack](C:/dev/vamps/Source/vamps/Private/Combat/Abilities/GA_Attack.cpp:95), [SetCombatContacts](C:/dev/vamps/Source/vamps/Private/Characters/VampsCharacter.cpp:391), [ResetSectionHits](C:/dev/vamps/Source/vamps/Private/Combat/VampsWeapon.cpp:84), [SweepWeaponSegment](C:/dev/vamps/Source/vamps/Private/Combat/VampsWeapon.cpp:200). |
| C06 | Daño, falta de Guard reaction y orientación/push. | [ReceiveCombatHit](C:/dev/vamps/Source/vamps/Private/Characters/VampsCharacter.cpp:735), [ReceiveCombatDisplacement](C:/dev/vamps/Source/vamps/Private/Combat/VampsCombatMovementComponent.cpp:10), [AddMovementInput](C:/dev/vamps/Source/vamps/Private/Characters/VampsCharacter.cpp:184). |
| C07 | Input por Started, ausencia de mantener/soltar y evasión en la ruta propia. | [VampsPlayerController.cpp](C:/dev/vamps/Source/vamps/Private/Player/VampsPlayerController.cpp:105), [IA](C:/dev/vamps/Source/vamps/Private/AI/VampsAIController.cpp:1). |
| C08 | Datos de paso y estado visual de defensa. | [VampsCombatSettings.h](C:/dev/vamps/Source/vamps/Public/Combat/Data/VampsCombatSettings.h:24), [VampsAnimInstance.cpp](C:/dev/vamps/Source/vamps/Private/Animation/VampsAnimInstance.cpp:36). |
| C09 | Selección y cámara separadas. | [LockOn](C:/dev/vamps/Source/vamps/Private/Characters/VampsLockOnComponent.cpp:69), [CameraManager](C:/dev/vamps/Source/vamps/Private/Player/VampsPlayerCameraManager.cpp:7). |
| C10 | Salud, autoridad y escena local. | [AttributeSet](C:/dev/vamps/Source/vamps/Private/Combat/Attributes/VampsAttributeSet.cpp:6), [GameMode](C:/dev/vamps/Source/vamps/Private/GameModes/VampsCombatTestGameMode.cpp:86). |

## Procedencia y límites

El hash SHA256 de `App/ago.exe`, vuelto a leer en esta revisión, es `B45623B75A96466C80CFC81116DCC353B27658840E296BAB948CD8FE80CFFC46`. Coincide con fase3. No se reabrió Ghidra ni se ejecutó el juego: se reutilizaron exportaciones y se contrastaron con los informes que conservan su auditoría de ASM.

| Grabación original | Duración nominal | Frames | SHA256 |
|---|---:|---:|---|
| [full demo](<D:/OBS recordings/full demo.mp4>) |183,03s|5491|`141D2CF54E031CAD3EB9AAAB076211D82B19DF3BB30441C73EAC12784A7B85F5`|
| [newdemo1](<D:/OBS recordings/newdemo1.mp4>) |160,77s|4823|`068B7A59DC1B74233C2303F65F7B9B55C0D788CA44B42ECB26D5BA6871C080B2`|
| [newdemo2](<D:/OBS recordings/newdemo2.mp4>) |156,17s|4685|`C67F8CBCA00754C8E7A7770D095A646BEADB4CBC7DF2C07EC326C5A9722C8F9F`|
| [newdemo3](<D:/OBS recordings/newdemo3.mp4>) |155,20s|4656|`4C673785C6A0EBF146E1A846A41246C88A774563710F8AFEB7811AD4FAC8216A`|

[Manifiesto de video](evidencia/manifest.json): originales1280×720/30fps y recortes H264 a idéntico FPS, sin interpolación ni audio; conservan el frame completo y se recodificaron, no son copias byte a byte del flujo comprimido original. Las láminas muestrean cada0,2s y recortan el área del juego para facilitar lectura. No se generaron imágenes sintéticas. El script de extracción queda en [research/scripts](<D:/fgo arcade/fgoac/research/vamps-contrast-20261018-0838/scripts/extract_video_evidence.py>).

No están calibradas la sincronía overlay/render/HUD ni la relación de30fps grabados con la simulación. Una barrera+GUARD y el contexto permiten reconocer defensa; un destello azul o un prompt aislado no bastan. Un `1st Attack` no identifica una fase de colisión. El cambio de cámara no mide yaw corporal. Un texto Stunned no identifica el productor de stun.

Los valores del DA citados desde027 son una captura histórica, no una lectura actual de Unreal. No hubo herramientas MCP directas del editor disponibles ni sesiones conectadas. Tampoco se ejecutaron compilación, PIE, tests o escritura en assets. La guía registra las comprobaciones manuales útiles sin declararlas hechas.
