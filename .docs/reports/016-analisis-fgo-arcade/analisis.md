# Análisis consolidado de FGO Arcade

Revisión del 15 de octubre de 2026, integrada con las respuestas R01–R07 y comprobación de las escenas señaladas. La referencia queda separada de la especificación vigente de [juego](../../game/functional.md), [combate](../../combat/functional.md) y [jugador](../../player/functional.md), integrada por [017](../017-base-fgoa-y-extensiones/reporte.md). [preguntas.md](preguntas.md) enlaza las precisiones de cada área; no se piden nuevas grabaciones.

| Fuente | Duración nominal | Fotogramas | Aporte principal |
|---|---:|---:|---|
| [full demo.mp4](<D:/OBS recordings/full demo.mp4>) | 03:03,03 | 5491 | Intercambio cercano, cadenas completas, Extra Attack, reacción, reservas y Grial |
| [newdemo1.mp4](<D:/OBS recordings/newdemo1.mp4>) | 02:40,77 | 4823 | Locomoción, cámara, cambio de objetivo y defensa prolongada sin redraw |
| [newdemo2.mp4](<D:/OBS recordings/newdemo2.mp4>) | 02:36,17 | 4685 | Preparación que termina en guardia, presión de varios enemigos y daño sin draw inmediato |
| [newdemo3.mp4](<D:/OBS recordings/newdemo3.mp4>) | 02:35,20 | 4656 | Acercamiento limitado, ataques fallidos, cadenas parciales y defensa contra otro target |

Todos son 1280 × 720 a 30 fps. Los tiempos corresponden al archivo, no al reloj de la partida. La [lista de escenas](escenas.md) localiza la evidencia; la [comparación con Vamps](comparacion.md) registra correcciones frente a los documentos y la base actuales. Esta investigación sigue separada de las especificaciones. **Los personajes ranged quedan fuera del alcance inicial**, según la indicación del usuario; sus escenas ayudan a entender reglas compartidas y diferencias entre repertorios.

## Cómo se interpreta la evidencia

Se recorrieron los videos completos por muestreo, el tramo jugable del primero cada 0,5 s y los nuevos cada 1 s. Se ampliaron escenas cada 0,1–0,25 s y se examinaron todos los fotogramas del overlay para localizar entradas. Se conservaron catorce capturas originales y dieciocho láminas nuevas. Las ampliaciones del diálogo comprueban defensa durante ofensiva, cambio forzado de target, aviso direccional y desvanecimiento.

- **Observado:** animación, posición, entrada iluminada o HUD visibles.
- **Explicación aportada:** regla descrita por el usuario a partir de jugar; se integra cuando la evidencia es compatible. Sus expresiones tentativas conservan ese carácter.
- **Inferencia:** explicación posible de lo visible, sin afirmar el mecanismo interno.

Cada fotograma equivale aproximadamente a 33 ms; la sincronización entre overlay, render y HUD no está calibrada. No se deducen ventanas exactas, distancias de mundo, fórmulas ni reglas de red. No se analizó audio. La build se identifica como `FGOA KH VER. BY Cloud23333`; las conclusiones corresponden a las sesiones aportadas.

Para reconocer defensa se contrastan barrera, pose, barra GUARD, contacto entrante y HP. Un destello azul o `Guard with rapid taps!` aislados no prueban bloqueo. El giro visual de una carta tampoco prueba consumo: se comprueban casillas vacías persistentes y la secuencia posterior.

## Controles

| Entrada | Función | Resultado de la revisión |
|---|---|---|
| RMB | Botón compartido | Solicitudes ofensivas y defensivas; predominan pulsaciones cortas repetidas |
| Shift | Esquiva; mantener para sprint | Movimientos evasivos breves y carreras sostenidas con WASD |
| F | Cambiar target | Cambia marcador y encuadre; no activa/libera lock-on |
| WASD | Locomoción | Acercamiento, retirada, laterales y diagonales durante disponibilidad y draw |
| LMB | Interacción con pantalla/cartas | También se usa en `newdemo1`, 00:15–00:17; no es un botón ofensivo independiente |
| 1–3 | Skills | No se detectó su activación manual en las cuatro grabaciones |

El lock-on permanece activo. Cuerpo, dirección de viaje, target y amenaza entrante son variables distintas. No se ejercitó una prueba aislada de mantener RMB para resolver su semántica de guardia, continuación o NP.

## 1. Botón compartido: preparación, contacto y guardia

La solicitud ofensiva inicia una preparación/animación; **iniciar la animación no significa que el ataque ya pueda contactar**. El comienzo ofensivo depende del personaje y de la acción. Una solicitud puede terminar en guardia por presión entrante. La defensa también puede resolverse durante una animación que ya empezó su gesto ofensivo: no queda limitada al intervalo anterior al primer contacto propio.

El ejemplo de A activo en frame 15 y B en frame 10, solicitados a la vez, describe esa regla temporal. Los 30/15/10 frames son ilustrativos, no mediciones. B tampoco gana solo por comenzar antes: el contacto debe llegar efectivamente a A antes de que salga su ofensiva; distancia, avance y repertorio siguen importando.

En `newdemo2`, 00:20,50–00:20,70, Da Vinci organiza tres cartas y aparece `1st Attack`; a 00:20,90 aparece barrera ante el rival antes de observar la salida ofensiva propia. La presión continúa con defensa y sin redraw. [N03](evidencia/N03-preparacion-guardia.jpg). `newdemo1`, 00:52–00:56, muestra preparación, defensa repetida y salida ofensiva posterior.

En `newdemo3`, 00:10,70–00:11,90, se ve primera defensa, nuevo `1st Attack` hacia el target lejano, gesto ofensivo en 00:11,50 y barrera ante otro contacto en 00:11,70–00:11,90. [N13](evidencia/N13-defensa-tras-gesto-ofensivo.jpg). Corrobora defensa dentro de una acción larga con pulsaciones todavía llegando. No demuestra defensa automática por reproducir una animación ni mide ventanas internas. El contacto entrante es ranged; la disponibilidad defensiva durante toda la acción es la regla explicada por el usuario y adoptada para Vamps.

**La defensa no se habilita solamente después de perder HP ni se cierra necesariamente al comenzar el gesto ofensivo propio.** `Guard with rapid taps!` es una instrucción bajo presión; no identifica el detector. RMB pulsado tampoco equivale a guardia permanente.

El comportamiento se explica mediante solicitud compartida, fase propia, contacto y resolución ofensiva/defensiva. No se demuestra si FGO usa trazas, hitboxes, reloj, notifies o una combinación. Fase temporal y comprobación espacial pueden coexistir. El empate se decidirá al diseñar la resolución; las pulsaciones de continuación se definen mediante ventana, sin conservar una solicitud demasiado temprana.

## 2. Cartas, cadenas y fallo de ataque

Se distinguen **pulsación**, **acción/carta**, **impacto** y **cadena**. Hay cinco cartas disponibles, tres posiciones ordenadas y, cuando corresponde, Extra Attack. Una acción numerada puede producir varios impactos. `full demo` muestra la cadena completa de d'Eon en 00:57–01:01 y una cuarta acción de Dantès en 02:22–02:24. [Segundo ataque](evidencia/E03-cadena-005890.jpg), [Extra Attack](evidencia/E11-extra-022290.jpg).

En `newdemo2`, 02:25,60, aparece `Extra Attack Gained` con Buster/Arts/Buster ([N18](evidencia/N18-extra-bab.jpg)); no se fija Q/A/B en ese orden como condición exclusiva del Extra original. Vamps lo excluye independientemente de esa condición.

El consumo corresponde a las acciones ofensivas realizadas hasta donde progresa la cadena, según la explicación aportada y las casillas vacías. Seleccionar tres cartas no implica ejecutar ni consumir las tres. Al cerrar la secuencia se realiza un nuevo draw incluso si quedaron acciones sin ejecutar.

**Un ataque que no alcanza a nadie corta la continuación y lleva al draw tras consumir la acción realizada.** La regla aportada se aplica a cualquiera de los tres pasos. La evidencia más limpia aísla el primero: `newdemo3`, 00:03–00:05, muestra a Holmes iniciar contra un rival lejano, avanzar sin WASD, golpear sin alcanzarlo y gastar la primera carta. Siguen pulsaciones RMB pero no aparece `2nd Attack`; a 00:04,70 aparece draw con NA. [N08](evidencia/N08-fallo-primer-ataque.jpg).

En `newdemo3`, 00:06–00:10, se llega a `2nd Attack`, quedan dos casillas vacías y termina en draw con la tercera sin ejecutar. [N09](evidencia/N09-cadena-parcial.jpg). Hay guardia e intervenciones próximas: **confirma la cadena parcial, pero no aísla con seguridad un fallo causal del segundo golpe**. La extensión de la regla a todos los pasos proviene de la aclaración del usuario.

Master A sigue seleccionado mientras Holmes alcanza a otro enemigo cercano y continúa al segundo paso en `newdemo3`, 00:06,30–00:07,30. R03 confirma ese contacto: **alcanzar a otro enemigo habilita continuación sin forzar cambio de target ofensivo**. El caso corresponde a la referencia aproximada del usuario a 00:05. El diálogo posterior confirma además que **un golpe bloqueado habilita continuación para el atacante**: el contacto defendido no corta por sí mismo su combo y permite las cadenas de presión contra guardia. El defensor también puede retomar su propia ofensiva después de bloquear.

La continuación requiere **pulsaciones individuales dentro de la ventana del paso**; una pulsación fuera se descarta para encadenar. Un ataque ilustrativo de 30 frames con ventana desde frame 15 y clicks en 0 y 5 inicia solo el primer paso. No hay buffer que conserve el click temprano hasta abrir la ventana. La cadena puede cerrarse al terminar un paso por falta de continuación o esquiva. Recuperación y disponibilidad dependen de la acción, sin un intervalo universal.

## 3. NA es draw; guardia no lo dispara

**Cannot Attack or Guard corresponde a la indisponibilidad durante el draw de cartas.** No es un nombre genérico para hitstun, stun o toda reacción al daño. Verlo bajo golpes puede significar que esos golpes llegaron mientras ya se reponían las cartas.

Al concluir una ofensiva completa o parcial se ven cartas usadas, `Command Card: Draw`, restricción de ataque/guardia y luego una mano nueva. Se conserva locomoción: `full demo`, 01:01–01:02, muestra S/D/A y desplazamiento durante draw. [E06](evidencia/E06-redraw-010150.jpg). La duración visible de un caso no fija un cooldown universal ni un tiempo igual a toda la animación.

**Defender no consume cartas ni genera draw/NA por sí mismo.** En `newdemo1`, 00:52,50–00:56,00, hay barreras y daño residual con cartas seleccionadas, sin draw entre bloqueos; después CHANCE y salida ofensiva. [N04](evidencia/N04-guardia-cartas.jpg). La aclaración distingue cartas preparadas/listas de utilizadas. Una ofensiva ya lanzada puede comprometer consumo aunque después se defienda; el draw posterior no se adjudica automáticamente al bloqueo.

El usuario describe una recuperación ofensiva aparentemente mayor después de defender. Se conserva como interpretación temporal, sin asignar un cooldown fijo adicional: puede depender de recuperación, presión entrante y momentos de resolución. **El botón compartido permanece habilitado para ataque y defensa.** Una cadena visible de bloqueos no implica NA ni que las solicitudes ofensivas estén deshabilitadas; puede verse preparación de ataque que termine siendo defensa por la llegada del siguiente golpe.

En `newdemo2`, 02:18,40–02:20,80, Gilgamesh entra en draw antes de los golpes y pulsa RMB sin barrera durante ellos; al terminar draw y restaurarse las cartas aparece defensa. [N10](evidencia/N10-draw-impide-guardia.jpg). No hace falta otro NA por daño para explicar el episodio. No se midió si los golpes modifican la duración del draw.

## 4. Cartas originales: listas, utilizadas y daño

La hipótesis «recibir daño consume cartas y por eso entra en NA» **no se confirma como regla universal**. No se aplica al daño residual de guardia, que pierde HP sin draw. Hay además un contraejemplo con golpes previos a la defensa en `newdemo2`:

| Tiempo | HP de Da Vinci | Cartas / situación |
|---|---:|---|
| 00:36,30 | 13.544 | Cinco cartas; locomoción previa al golpe |
| 00:37,10 | 12.108 | Impactos y `Guard with rapid taps!`; cinco cartas, sin draw |
| 00:38,10 | 10.292 | Más pérdida de HP; misma mano, sin NA |
| 00:38,90 | 9.789 | Ahora se reconoce barrera defensiva |
| 00:40,10 | 9.789 | Vuelve a moverse; cinco cartas y sin redraw visible |

[N05](evidencia/N05-danio-sin-draw.jpg). El overlay no registra RMB desde el comienzo de esos golpes hasta aproximadamente 00:38,03; la pérdida inicial tampoco se explica como una sucesión completa de bloqueos solicitados. Hay reacción y luego posibilidad de defender conservando la mano.

La escena indicada aproximadamente en 00:51 se localiza en `newdemo2`, 00:54,33–00:56,50: desde neutral se organiza ofensiva, la presión termina en defensa y la secuencia queda lista sin casillas usadas persistentes ni draw. Siguen contactos defendidos con cartas en la mano ([N06](evidencia/N06-presion-lateral.jpg)). Preparar/seleccionar no equivale a consumir definitivamente.

La explicación contempla casos finos originales: una acción ya lanzada puede consumir aunque termine resuelta como defensa, y cartas listas que finalmente se consumen pueden exigir nuevo draw. No se fija una regla universal de daño que cause esa transición. **Esto permanece solo en la referencia: Vamps no implementa cartas, selección, bonificaciones ni excepciones de disponibilidad causadas por estados de mano.** Cadena y NA tendrán reglas directas, sin preparar compatibilidad con cartas.

## 5. Defensa, daño residual y otros targets

Defender puede reducir daño sin eliminarlo ni cancelar toda la presión del agresor. En `full demo`, 01:05,50–01:06,90, barrera/GUARD coinciden con pérdidas de 27, 143 y 106 HP entre capturas. [E04](evidencia/E04-guardia-010550.jpg), [E05](evidencia/E05-guardia-010590.jpg). `newdemo1`, 00:53–00:56, reitera defensa con HP que llega a 77; no demuestra muerte causada exclusivamente por chip damage.

La defensa **responde a enemigos diferentes del seleccionado y una guardia efectiva puede cambiar el lock hacia el agresor**. En `newdemo3`, 00:13,70–00:14,50, Master A está a la derecha; Master E entra por la izquierda, aparece barrera y luego LOCK sobre E sin F. Después se ataca al nuevo objetivo y continúa la cadena. [N14](evidencia/N14-guardia-cambia-target.jpg).

En `newdemo2` hay F manual alrededor de 01:05,93. Da Vinci recibe daño abierto de E conservando otro target en 01:06,90–01:07,70; después aparece barrera y hacia 01:08,30 el lock pasa a E sin nuevo F. [N15](evidencia/N15-retarget-davinci.jpg). Se distingue daño de guardia efectiva y cambio manual de forzado.

N07 se conserva como [guardia en la secuencia](evidencia/N07-guardia-en-secuencia.jpg): no aísla el emisor del contacto. Antes se había usado como prueba más fuerte de ataque lateral; N14/N15 aportan evidencia más precisa de agresor distinto y retarget.

Se buscó expresamente el caso de `newdemo2`: en 00:54–00:56,50 Da Vinci conserva Master E seleccionado mientras Master C cruza por el lateral, ataca y coincide con contactos defendidos. [N06](evidencia/N06-presion-lateral.jpg). **Es compatible con el episodio recordado, pero la presión de Master E y otras acciones se superponen: no se asegura qué contacto convirtió la preparación ni que llegó antes de la primera activación ofensiva propia.** 00:20,70–00:20,90 sí muestra preparación seguida de guardia, contra el seleccionado. Gilgamesh en 02:19 ya estaba en draw y no sirve como conversión.

El círculo de proximidad muestra posiciones relativas y una flecha roja añade aviso de amenaza en la dirección de un enemigo. En `newdemo2`, 00:46,80–00:47,40, aparece aviso y luego golpe abierto con el seleccionado lejos. [N17](evidencia/N17-aviso-direccional.jpg). El usuario lo relaciona con un ataque lanzado que se aproxima; no se recupera el cálculo predictivo. La flecha no prueba bloqueo ni cobertura trasera.

La cobertura lateral no certifica 360° ni defensa trasera como hecho medido del original. **Vamps elige guardia de 360°, daño residual según stats que puede matar y recurso de guardia con agotamiento/guard break y stun.** La fórmula de la barra original no se recupera y no se copia de otro juego. Parry y otras mecánicas originales quedan para después de emular la base.

## 6. Reacción al daño y stun

Un golpe puede producir retroceso, levantamiento, caída y pérdida de control locomotor. Eso se distingue de bajar HP y reponer cartas. En `full demo`, 01:44–01:46, d'Eon recibe impactos y desplazamientos durante indisponibilidad; hacia 01:47 aparece `Stunned`. [Reacción](evidencia/E07-reaccion-014450.jpg), [stun](evidencia/E08-stunned-014750.jpg). El texto también aparece sobre un enemigo después de otra cadena.

Da Vinci en `newdemo2`, 00:37–00:39, muestra daño → reacción → defensa: el daño no impide necesariamente defender durante todo el intercambio. No se identifican ventanas exactas entre golpes ni un timer renovado por cada contacto. Vamps incorpora reacción y stun por guard break; sus tiempos/protecciones se calibrarán, sin asociarlos a consumo de cartas.

## 7. Acercamiento y rango por acción

Los ataques melee incorporan movimiento ofensivo propio y limitado: **intentan cerrar distancia hasta lo permitido por la acción; no garantizan llegar ni acertar**. Holmes, `newdemo3` 00:03,30, avanza tras soltar W, queda corto, golpea igualmente y entra en draw después de fallar. [N08](evidencia/N08-fallo-primer-ataque.jpg). Alcance de acercamiento, alcance de contacto y distancia desde la que se admite la solicitud son conceptos distintos.

La explicación aportada establece perfiles por personaje y ataque Q/A/B, con acercamiento adaptable y máximo propio. Holmes, d'Eon y Dantès son compatibles. **Root motion escalado o motion warping son mecanismos posibles**, no hechos recuperados de imágenes. No se midieron mínimos, separación final, factor de escala ni seguimiento continuo dentro de la acción.

Larva/Tiamat, segundo personaje de `newdemo3`, dispara desde su posición sin avance melee durante 01:25–01:27. [N11](evidencia/N11-ataque-estatico.jpg). No todo ataque debe acercar por compartir RMB. El repertorio mixto de Gilgamesh se toma como explicación del usuario; no se catalogan todos sus Q/A/B desde las superposiciones. Tiamat se localiza en `newdemo3`.

El rango máximo de proyectiles descrito no se cuantifica ni se separa siempre de evasión/intercepción. No exige proyectiles ahora. Para melee, el resultado útil es un perfil de comienzo ofensivo, desplazamiento permitido, contacto y recuperación por acción, con valores propios para Vamps.

## 8. Movimiento, esquiva y sprint

WASD permite acercarse, retroceder, laterales y diagonales. El cuerpo puede orientarse hacia el desplazamiento conservando target: lock-on fijo no exige la misma pose mirando al rival. `newdemo1`, 00:05–00:14 y 01:57–02:10, contiene carreras largas y cambios de dirección. [N01](evidencia/N01-camara-recorrido.jpg), [N02](evidencia/N02-camara-encuadre.jpg).

Shift breve + dirección produce movimientos evasivos; mantenerlo con dirección produce sprint, conforme al binding aportado. El primer video muestra evasión con estela/cúpula blanca en 00:17–00:19. [E02](evidencia/E02-esquiva-001750.jpg). Conservar HP ante un ataque próximo no mide invulnerabilidad: también puede fallar por posición.

**Shift sin dirección no muestra una esquiva inmóvil inequívoca**, compatible con la aclaración del usuario. Los intervalos sin WASD son breves y varios ocurren durante draw o daño (`newdemo1`, 01:23–01:27); no aíslan neutral disponible. Se retira la suposición de que spot dodge garantiza una acción estacionaria. La adaptación de Vamps queda definida a continuación.

Los laterales son compatibles con una referencia basada en el encuadre; no distinguen una base estrictamente relativa al yaw de cámara de otra derivada del target cuando coinciden. Costo, invulnerabilidad y umbral tap/hold se calibrarán. **Vamps elige Shift sin dirección como evasión hacia atrás**: adaptación explícita frente a la peculiaridad del original.

## 9. Cámara y lock-on continuo

F cambia selección y encuadre. `full demo`, 00:29,20–00:29,40, cambia Master C por Master E. [E01](evidencia/E01-objetivo-002940.jpg). La aclaración establece recorrido por el orden vertical de la lista de rivales del HUD, además de cambios forzados por defensa efectiva. Se distingue cambio manual, cambio forzado y seguimiento del mismo objetivo.

Encuadre significa la composición de la vista: posiciones visibles de jugador/target, distancia aparente, corrección de orientación y acercamientos durante acciones. No es otra entrada ni otro modo de lock-on.

**El encuadre tolera desplazamiento lateral; el target no queda clavado al centro exacto.** En `newdemo1`, 00:05,20–00:08,40, se cambia de lateral/diagonal sin F; el personaje recorre el encuadre y cambia la referencia del fondo. En 01:57–02:01 cambia la dirección de viaje y el objetivo recorre una zona horizontal amplia mientras la cámara acompaña. [N01](evidencia/N01-camara-recorrido.jpg), [N02](evidencia/N02-camara-encuadre.jpg). Este segundo tramo incluye cambio de selección y muerte del target: no mide un umbral con objetivo fijo.

La hipótesis de tolerancia pequeña y corrección al salir de ella es compatible. **No se demuestran dos umbrales separados ni histéresis:** seguimiento suavizado, encuadre de ambos personajes o una combinación también pueden producirlo. La regla útil es tolerar pequeño reposicionamiento y corregir gradualmente al alejarse de la relación deseada; algoritmo y números se elegirán después.

Hay acercamientos visuales de cámara durante acciones y planos especiales de entrada; no se separa movimiento de cámara de FOV. Vamps conservará corrección, zoom y presentación de ataque/defensa de la referencia, ajustando parámetros propios.

`newdemo2`, 02:12–02:14, muestra vegetación próxima que pierde opacidad mediante una trama visible y deja ver personaje/combate. [N16](evidencia/N16-desvanecimiento.jpg). Corrige la lectura anterior limitada a vegetación opaca; no demuestra que todo obstáculo use el mismo tratamiento.

`newdemo1`, alrededor de 00:46, alcanza un límite sin pared visible. Desvanecer presentación y quitar colisión son operaciones distintas: una barrera invisible no tiene píxeles que desvanecer; una pared visible de límite puede ocultarse para la vista conservando la restricción de movimiento. El lock-on no obliga a eliminar límites físicos ni permitir a la cámara atravesar cualquier sólido.

## 10. Identidad, recursos y partida

Hay equipos con combatientes activos y reserva individual. `full demo` pasa por Musashi → d'Eon → Dantès al caer, cambiando modelo, HP, cartas y repertorio. [Entrada de d'Eon](evidencia/E09-personaje-005450.jpg), [Dantès](evidencia/E10-personaje-020250.jpg). No es balance controlado ni prueba los arquetipos propuestos para Vamps.

Se observan HP, NP, crítico, bonificaciones Q/A/B, apoyos y tres bindings de skills. Crítico designa `CRITICAL` y probabilidades de ataque; apoyo designa `SUPPORT`, distinto de golpear al mismo enemigo que un compañero. **Vamps incluye crítico y excluye SUPPORT** después de aclarar esa diferencia. Esto no excluye contactos contra enemigos que ya combaten con otro aliado.

NP aumenta durante ofensiva/presión sin aislar contribuciones. Hay `NP Ready` con 100% e instrucción de mantener botón; no se aisló una ejecución propia completa. [E13](evidencia/E13-np-025200.jpg). El usuario explica el original adicional: canaliza con área marcada, dispara al terminar solo si hay alguien dentro, reproduce cinemática visible para todos y aplica daño/buff según la acción. No se adjudica a una escena grabada. La ultimate futura de Vamps tendrá tratamiento propio sin cortar combate ni cinemática global; no se fijan carga, área o efecto.

Hay reloj inicial de 240, contador con denominador 7, `Grail Manifests`, portador y `Defeat Gauge Gain Up`. Adquirir el Grial tras derrotar al portador coincide con restauración de HP: Dantès 6.633 → 12.071 en `full demo`, 02:42,70–02:42,90 ([E12](evidencia/E12-grial-024290.jpg)); Tiamat 8.305 → 13.434 en `newdemo3`, 02:06–02:07 ([N12](evidencia/N12-grial-cura.jpg)). La repetición refuerza la asociación, sin fijar todas las reglas de posesión/puntuación.

Las partidas muestran `BATTLE FINISH` y WIN/LOSE sin consumir necesariamente todo el reloj. [E14](evidencia/E14-fin-025500.jpg). Morir individualmente no termina la partida; un cierre tras una muerte tampoco demuestra derrota por agotar reservas. Vamps conserva equipos/puntos por bajas, un personaje jugable con respawn del mismo y un portador de recompensa sin temática Grial. Matar al portador vale más que una baja normal. La asignación al kill leader del equipo ganador y el momento de aparición son una hipótesis del original: se definirán para Vamps junto con transferencia y efectos adicionales.

## Cierre de la investigación

El núcleo queda explicado mediante solicitud compartida, fases propias, defensa durante la acción, continuación por pulsación válida/contacto incluso con otro rival, cierre voluntario al final del paso, draw/NA y acercamiento limitado. Guardia efectiva puede cambiar target; un contacto ofensivo no seleccionado no obliga a hacerlo. Se suma tolerancia de encuadre y desvanecimiento de obstáculos visibles.

Las particularidades de mano lista/utilizada permanecen solo como funcionamiento original. Vamps representa cadena y recuperación directamente, sin cartas ni preparación para agregarlas. Las decisiones adoptadas se integran en la [base FGOA adaptada](../../game/functional.md), con [extensiones propias](../../extensions/functional.md) posteriores. [preguntas.md](preguntas.md) indica dónde resolver las precisiones restantes.
