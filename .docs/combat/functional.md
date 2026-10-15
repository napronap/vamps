# Combate de la base FGOA adaptada

Este núcleo reproduce el intercambio de FGO Arcade con las [adaptaciones de alcance](../game/functional.md#alcance-de-la-base). Usa cadena y recuperación directamente, sin cartas. Implementación: [technical.md](technical.md). Controles, movilidad y cámara: [jugador](../player/functional.md). Parry y herramientas propias posteriores: [extensiones Vamps](../extensions/functional.md).

## Solicitud compartida y acción

Cada pulsación individual de RMB solicita ataque/defensa según la situación. Mantenerlo no repite solicitudes ni concede guardia permanente. Una solicitud ofensiva aceptada inicia la animación antes del contacto ofensivo. Cada acción tiene preparación, contactos/impactos, ventana de continuación y recuperación; comienzo ofensivo, duración y desplazamiento dependen de la acción del personaje.

La posibilidad defensiva se conserva durante toda la acción propia mientras el personaje puede defender, incluso después de comenzar sus contactos ofensivos. Requiere una solicitud defensiva válida y un contacto entrante. La pose o la reproducción de una animación no bastan para bloquear.

Un golpe entrante puede resolver defensa aunque haya intención ofensiva o acción propia en curso. No gana necesariamente quien pulsó antes: importan momento de contacto, distancia y acción. Los contactos simultáneos requieren un criterio explícito, independiente del orden accidental de procesamiento.

## Cadena y continuación

La cadena tiene hasta tres pasos. Una acción puede tener varios impactos sin contar como varios pasos.

Cada continuación exige una pulsación individual dentro de la ventana del paso y contacto habilitante. Un click anterior a la ventana se descarta para encadenar; no se guarda hasta que abra. Por ejemplo, en una acción ilustrativa de 30 frames cuya ventana abre en el 15, clicks en 0 y 5 solo inician el primer paso. Esos frames no fijan balance.

Alcanzar a cualquier enemigo habilita continuación, incluso si no es el seleccionado o bloquea el golpe. El contacto defendido cuenta para el atacante y no corta su combo. Acertar a otro rival no cambia automáticamente el target ofensivo. Un paso que no alcanza a nadie corta la cadena aunque sigan llegando pulsaciones.

La cadena termina tras el tercer paso, al fallar uno o voluntariamente al final de un paso por no continuar o usar evasión. Evasión permite cerrar en esa frontera; no habilita cancelación libre en cualquier punto de la animación. La recuperación depende de la acción.

## Recuperación y NA

NA es la recuperación que impide atacar y defender después del cierre ofensivo. Permite locomoción y es distinta de reacción al daño, stun y muerte. No hay draw real ni cartas ocultas que simulen esa indisponibilidad.

Bloquear no inicia NA, no deshabilita el botón compartido ni obliga a cerrar la cadena del defensor. Este conserva su contexto y puede seguir solicitando ataque o defensa. La salida ofensiva puede demorarse por recuperación y timings aunque la entrada esté habilitada. Si ya ejecutó una acción ofensiva, su progresión y cierre conservan sus reglas; el bloqueo no borra ese compromiso.

Puede mostrarse un inicio de ataque que termine resuelto como defensa al llegar otro golpe. Esa presentación no garantiza salida ofensiva inmediata ni introduce una restricción de input. La continuación conserva su ventana y el rechazo de clicks tempranos. No se agrega un cooldown universal por bloqueo.

Perder HP tampoco crea automáticamente NA. No se calculan excepciones a partir de cartas preparadas, consumidas o recolección.

## Guardia, daño y reacción

La guardia cubre 360°. Una guardia efectiva selecciona al agresor como nuevo target y la cámara sigue ese cambio; el defensor puede continuar su cadena contra él. Recibir daño abierto no impone esa selección. El ataque defendido puede mantener presión: no se cancela incondicionalmente toda la ofensiva del agresor.

El daño residual depende de stats y puede matar. Defender consume un recurso de guardia; al agotarse se produce guard break con stun y se impide seguir defendiendo. El HUD distingue recurso, bloqueo, reacción, stun y NA.

El daño abierto puede producir reacción y desplazamiento. Crítico pertenece a la resolución del ataque y tiene señal visible. Ni reacción ni stun se explican mediante cartas. Los contactos contra un enemigo que combate con otro aliado siguen las reglas normales; no hay sistema SUPPORT.

## Acercamiento ofensivo

El ataque intenta cerrar distancia hasta el límite propio de la acción. Puede comenzar contra un target lejano, quedar corto y ejecutar igualmente el golpe. Alcance de desplazamiento y alcance de contacto son distintos. No se garantiza acierto ni se teletransporta sin límite al target. La geometría conserva sus restricciones.

## Preguntas

1. ¿Cuánto dura una solicitud defensiva válida, cómo se renueva y cuándo se consume?
2. ¿Cómo se consume y recupera guardia, y qué restricciones temporales producen reacción y guard break? ¿Qué protecciones o posibilidades de defensa existen entre impactos?
3. ¿Cómo se calcula el daño residual y cuál es la regla de crítico, incluida su interacción con guardia? Las cantidades y tiempos se calibrarán después de elegir las reglas.
4. Al diseñar la resolución, ¿qué criterio resuelve contactos simultáneos? El mecanismo interno de FGO no es un requisito.
