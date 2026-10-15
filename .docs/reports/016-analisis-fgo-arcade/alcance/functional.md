# Alcance funcional de Vamps basado en FGO Arcade

El primer alcance emula el intercambio ataque/guardia, la movilidad, el lock-on y la presentación de FGO Arcade con un único personaje melee jugable. Conserva cadena y recuperación sin cartas. La [referencia original](../analisis.md) permanece separada; este par consolida las decisiones del diálogo y no modifica todavía los documentos del prototipo instalado. Implementación: [technical.md](technical.md).

## Alcance y controles

| Entrada | Acción |
|---|---|
| WASD | Movimiento direccional |
| RMB, pulsación individual | Solicitud compartida de ataque/defensa y continuación dentro de ventana |
| Shift con dirección | Evasión breve; mantener para sprint |
| Shift sin dirección | Evasión hacia atrás |
| F | Siguiente enemigo según el orden vertical de la lista del HUD |

Se incluyen combo de hasta tres pasos, guardia de 360°, daño residual, crítico, recurso de guardia/guard break, reacción al daño, evasión/sprint, cámara de referencia, HUD de target/amenaza y partida por equipos con puntuación y respawn. SUPPORT queda fuera: atacar al mismo enemigo que otro aliado sigue siendo un contacto normal del combate de equipo.

No hay cartas, mano, selección Q/A/B, bonificaciones de cartas, Extra Attack ni reservas de personajes. No se deja lógica, datos ni interfaces preparados para incorporar cartas. Ranged queda fuera de esta etapa. Parry y otras mecánicas originales se abordan después de emular la base. La ultimate tiene una etapa posterior propia.

## Solicitud compartida y fases de ataque

Una solicitud aceptada inicia la acción del paso actual; la animación comienza antes de su contacto ofensivo. Cada acción tiene preparación, contactos/impactos, ventana de continuación y recuperación. Su comienzo ofensivo, duración y desplazamiento pertenecen al repertorio del personaje.

La posibilidad de resolver defensa se conserva durante toda la acción de ataque mientras el personaje puede defender. No se cierra automáticamente al activar el primer tramo de contacto propio. Hace falta una solicitud defensiva válida y contacto entrante: reproducir una animación no concede bloqueo permanente.

Un golpe entrante puede resolver la interacción como defensa aunque haya una intención ofensiva o una acción propia en curso. El resultado depende de la situación y del momento de contacto, no solo de cuál jugador pulsó antes. No se decide simultaneidad mediante un orden accidental de procesamiento; el criterio de empate se elegirá al diseñar la resolución.

## Continuación y cierre de cadena

Cada continuación requiere una pulsación individual dentro de la ventana del paso. Mantener RMB no repite solicitudes. Un click demasiado temprano se descarta para encadenar; no se guarda hasta que abra la ventana.

Ejemplo ilustrativo: para un ataque de 30 frames con ventana desde frame 15, pulsar en 0 y 5 solo inicia el primer paso. Una pulsación válida en la ventana puede habilitar el siguiente, si el contacto del paso permite continuar. Esos frames no son valores de balance de Vamps.

Alcanzar a cualquier enemigo habilita la continuación mediante una pulsación válida, aunque no sea el seleccionado o el enemigo bloquee el golpe. El bloqueo cuenta como contacto ofensivo y no corta el combo del atacante. Ese contacto no cambia automáticamente el target del atacante. Un paso que no alcanza a nadie corta la cadena; insistir con RMB no la prolonga. Una acción puede tener varios impactos, sin confundir esos impactos con pasos adicionales.

La cadena termina al completar el tercer paso, al fallar un paso o voluntariamente al final de un paso por no continuar o por usar evasión. La interrupción por evasión se admite en esa frontera; no se presupone cancelación libre a mitad de cualquier animación. La recuperación depende de la acción.

## NA sin cartas y defensa

NA representa la recuperación que impide atacar y defender después del cierre ofensivo. Conserva locomoción; no equivale a hitstun, stun ni muerte. No hay un redraw real ni una simulación de cartas ocultas.

Un bloqueo no inicia NA por sí mismo ni deshabilita el botón compartido. El defensor conserva el contexto de cadena y puede seguir solicitando ataque o defensa; cada pulsación se evalúa según el momento de la interacción. La salida ofensiva puede demorarse por recuperación y timings de la acción, aunque la entrada siga habilitada. Bloquear no obliga a cerrar su cadena. Si ya se ejecutó una acción ofensiva, su progresión y cierre siguen sus reglas normales; no se calculan excepciones a partir de mano preparada, cartas consumidas o recolección. Perder HP tampoco crea automáticamente una fase de cartas/NA.

Las cadenas de bloqueos describen el resultado visible de esas interacciones. Puede mostrarse un inicio de ataque que termine resuelto como defensa al llegar otro golpe. La presentación no crea por sí misma una restricción de input ni garantiza salida ofensiva inmediata. La continuación de combo conserva su ventana y sigue descartando clicks demasiado tempranos.

La guardia cubre 360°. Al resolver una guardia efectiva se selecciona al agresor como nuevo target y la cámara sigue el cambio; la cadena puede continuar contra él. El daño abierto no se equipara a esa guardia efectiva. El ataque defendido puede mantener presión: no se cancela incondicionalmente toda la ofensiva del agresor.

El daño residual depende de stats y puede matar. El recurso de guardia se agota bajo defensa; al agotarse, se impide seguir defendiendo y se produce guard break con stun. El HUD diferencia recurso, bloqueo, reacción, stun y NA. La fórmula, recuperación del recurso y duración del break se definen antes de implementar esa parte.

El daño abierto puede producir reacción y desplazamiento; esas consecuencias no se explican mediante cartas. Crítico pertenece a la resolución del ataque y tiene señal visible. Se definirán su probabilidad, bonificación y relación con daño defendido.

## Acercamiento y movilidad

El ataque intenta cerrar distancia hasta su límite propio. Puede empezar contra un target lejano, quedar corto y ejecutar igualmente su golpe. Alcance del desplazamiento y alcance de contacto se configuran por separado. No se garantiza acertar ni se teletransporta sin límite al target.

WASD permite acercamiento, retirada, laterales y diagonales. Cuerpo y dirección de viaje pueden cambiar manteniendo lock-on. La evasión con dirección respeta la dirección solicitada; sin dirección realiza el movimiento hacia atrás. La carrera sostenida conserva seguimiento de target. Velocidades, distancias, invulnerabilidad, costo y umbral tap/hold se ajustan con el prototipo.

## Target, cámara y obstáculos

El lock-on permanece activo mientras exista un enemigo elegible; F recorre la lista vertical del HUD. Una guardia efectiva contra otro enemigo puede imponer selección fuera de ese recorrido. Un contacto ofensivo no seleccionado permite continuar sin imponer el mismo cambio.

La cámara conserva el comportamiento de referencia: tolerancia al pequeño reposicionamiento lateral, corrección gradual, cambios de distancia visual y planos de ataque/defensa. El movimiento de cámara asociado a atacar forma parte explícita del alcance. El target no se fija al píxel central. Encuadre significa la relación visual de jugador y target dentro de la pantalla, incluida distancia aparente y orientación de la vista. Sus parámetros se ajustarán observando el prototipo, sin afirmar dos umbrales internos de FGO.

Los obstáculos visibles preparados para ello se desvanecen gradualmente cuando tapan la vista relevante y recuperan su presentación al dejar de taparla. Una pared visible de boundary puede recibir ese tratamiento sin perder colisión. Un límite invisible conserva su restricción y no tiene representación que desvanecer. La cámara mantiene target sin convertir el desvanecimiento en permiso para atravesar físicamente cualquier sólido.

El HUD muestra enemigos próximos y dirección de amenaza entrante, además de selección y lista de rivales. El aviso de amenaza se distingue de confirmar un contacto o un bloqueo.

## Partida, respawn y portador de recompensa

La partida enfrenta equipos. Cada jugador usa el mismo personaje al reaparecer; no cambia de personaje ni agota reservas. Morir concede puntos al equipo adversario según la regla de baja y provoca respawn del mismo personaje mientras la partida continúa.

Se incluye un portador visible de recompensa de puntuación sin temática Grial. Matar al portador vale más que una baja normal. La regla de aparición/asignación, transferencia y fin de partida se define expresamente; no se convierte la hipótesis sobre el kill leader original en una regla ya comprobada.

Se busca aumentar la duración mediante la relación entre valor por baja y objetivo de victoria. Bajar puntos por baja manteniendo el objetivo exige más bajas; reducir ambos proporcionalmente no cambia esa relación. Valores, tiempo y tamaño de equipo se ajustarán después de fijar las condiciones de partida.

## Ultimate posterior

Se incorporará una ultimate con comportamiento propio en una etapa posterior, sin cinemática global ni interrupción del combate de los demás jugadores. La canalización/área y condición de disparo descritas para el NP original permanecen en la referencia; no se convierten automáticamente en requisitos de esta ultimate.

## Precisiones para cerrar el documento final

1. Definir vigencia/renovación de una solicitud defensiva, consumo/regeneración de guardia y consecuencias temporales de reacción/guard break. Los valores de balance se calibrarán después.
2. Fijar condiciones de victoria, equipos, asignación/transferencia del portador y efectos adicionales; elegir el nuevo target al perder/morir el actual.
3. Definir la regla de crítico, incluida su interacción con guardia. SUPPORT permanece excluido.
4. Elegir el criterio de empate al diseñar la resolución de contactos. El mecanismo interno de FGO no es un requisito de implementación.

No se requieren nuevas grabaciones para resolver estas decisiones. [preguntas.md](../preguntas.md) concentra el diálogo restante.
