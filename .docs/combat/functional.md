# Combate de la base FGOA adaptada

Este núcleo reproduce el intercambio de FGO Arcade con las [adaptaciones de alcance](../game/functional.md#alcance-de-la-base). Usa cadena y recuperación directamente, sin cartas. Implementación: [technical.md](technical.md). Controles, movilidad y cámara: [jugador](../player/functional.md). Parry y herramientas propias posteriores: [extensiones Vamps](../extensions/functional.md).

## Solicitud compartida y acción

Cada pulsación individual de RMB solicita ataque/defensa según la situación. Mantenerlo no repite solicitudes ni concede guardia permanente. Una solicitud ofensiva aceptada inicia la animación antes del contacto ofensivo. Cada acción tiene preparación, contactos/impactos, ventana de continuación y recuperación; comienzo ofensivo, duración y desplazamiento dependen de la acción del personaje.

Una pulsación defensiva válida puede iniciar una interacción de guardia. Después de bloquear, los siguientes impactos de la misma instancia ofensiva se defienden sin exigir una pulsación por impacto, mientras siga vigente esa interacción y el receptor pueda defender. Esto no equivale a bloquear automáticamente nuevas instancias ni a mantener guardia sosteniendo RMB.

La pulsación defensiva vence tras el tiempo configurado del personaje si no se usa. Cada paso de la cadena es una instancia nueva para defensa; los subimpactos y armas de ese paso comparten la identidad. La interacción recordada termina al vencer Guard, recibir daño abierto, entrar en stun o reiniciar el combate. Recovery ofensiva y NA conservan sus restricciones: un click en Recovery no rearma defensa.

La posibilidad defensiva se conserva durante toda la acción propia mientras el personaje puede defender, incluso después de comenzar sus contactos ofensivos. Requiere una solicitud defensiva válida y un contacto entrante. La pose o la reproducción de una animación no bastan para bloquear.

Un golpe entrante puede resolver defensa aunque haya intención ofensiva o acción propia en curso. No gana necesariamente quien pulsó antes: importan momento de contacto, distancia y acción. Los contactos simultáneos requieren un criterio explícito, independiente del orden accidental de procesamiento.

## Cadena y continuación

La cadena tiene hasta tres pasos. Una acción puede tener varios impactos sin contar como varios pasos.

Cada continuación exige una pulsación individual dentro de la ventana del paso y contacto habilitante. Un click anterior a la ventana se descarta para encadenar; no se guarda hasta que abra. Por ejemplo, en una acción ilustrativa de 30 frames cuya ventana abre en el 15, clicks en 0 y 5 solo inician el primer paso. Esos frames no fijan balance.

Alcanzar a cualquier enemigo habilita continuación, incluso si no es el seleccionado o bloquea el golpe. El contacto defendido cuenta para el atacante y no corta su combo. Acertar a otro rival no cambia automáticamente el target ofensivo. Un paso que no alcanza a nadie corta la cadena aunque sigan llegando pulsaciones.

La cadena termina tras el tercer paso, al fallar uno o voluntariamente al final de un paso por no continuar o usar evasión. Evasión permite cerrar en esa frontera; no habilita cancelación libre en cualquier punto de la animación. Una continuación ofensiva válida tiene prioridad sobre la evasión solicitada y la descarta. La recuperación depende de la acción y se conserva al salir por evasión.

## Recuperación y NA

NA es la recuperación que impide atacar y defender después del cierre ofensivo. Permite locomoción y dash/sprint sin acortar su plazo y es distinta de reacción al daño, stun y muerte. No hay draw real ni cartas ocultas que simulen esa indisponibilidad.

Bloquear cancela la ofensiva propia en curso, incluida su continuación pendiente, y pasa al defensor a la reacción de guardia. Puede verse un comienzo de ataque que termina sustituido por Guard. La interrupción no inicia NA ni conserva una continuación ofensiva para ejecutarla automáticamente después. Durante la reacción se siguen evaluando solicitudes defensivas, pero no se permite atacar.

Perder HP tampoco crea automáticamente NA. No se calculan excepciones a partir de cartas preparadas, consumidas o recolección.

## Guardia, daño y reacción

La guardia cubre 360°. Una guardia efectiva selecciona al agresor como nuevo target y la cámara sigue ese cambio. Recibir daño abierto no impone esa selección. El ataque defendido puede mantener presión: bloquear cancela la ofensiva del defensor, no la del agresor.

Solo se recuerda la instancia ofensiva de la interacción defensiva actual. Si A está siendo defendido y llega B, bloquear a B requiere una nueva pulsación válida; esa guardia cambia el lock-on y reemplaza la interacción recordada con A. Los ataques de A no desaparecen: si vuelve a conectar un impacto de su multihit, se necesita otra pulsación válida para bloquearlo. Seleccionar un rival con F no constituye por sí solo una guardia.

El bloqueo y el daño abierto deshabilitan temporalmente movimiento voluntario y ataque. Ambos usan la misma duración configurada en el atacante para el paso que conecta, sin calcularla a partir de la longitud de su sección o de un mínimo de animación. El desplazamiento impuesto por el contacto sigue siendo posible y respeta colisión. Las solicitudes defensivas se evalúan durante la reacción cuando el receptor puede defender; stun y NA impiden defensa.

Guard tiene una salida específica por dash al registrar el impacto final autorado del ataque enemigo, el de cierre con knockback. Los contactos anteriores no la habilitan. Se puede salir inmediatamente después de ese contacto, sin esperar el final de su empuje ni ReactionEndsAt. Salir termina la reacción y su memoria defensiva, pero conserva cualquier NA vigente. Un contacto nuevo reemplaza el permiso anterior; daño abierto, ruptura/stun, muerte o reset lo invalidan. Si el impacto final falla, no entrega permiso: la reacción previa termina por su plazo ordinario. El dash no agrega invulnerabilidad ni descarta contactos válidos ya capturados.

El impacto final se identifica explícitamente dentro del ataque. No equivale a cualquier contacto con empuje, al tercer paso de la cadena ni al final visual de una animación. En un lote simultáneo se mantienen las prioridades ruptura, daño abierto y bloqueo; solo un resultado final de Guard puede habilitar su salida.

Cada contacto aceptado instala de nuevo esa duración desde el contacto, reemplazando el plazo anterior. Los impactos de un mismo paso comparten la configuración. Las animaciones multihit se ajustan para que el intervalo entre sus contactos no supere esta duración; mientras conecten, mantienen la restricción y el último contacto fija su vencimiento final. Un impacto que falla no renueva el plazo ni el reposicionamiento garantiza los impactos siguientes.

El daño residual puede matar. La barra de guardia acumula carga al bloquear; al llenarse se produce guard break con stun y se impide seguir defendiendo. Reacción ordinaria y stun son estados distintos. El HUD distingue acumulación de guardia, bloqueo, reacción, stun y NA.

La carga de guardia es distinta de la [energía de movilidad](../player/functional.md#energía-de-movilidad). Gastar energía en dash/sprint no cambia GuardLoad ni causa ruptura al agotarse.

Cada contacto bloqueado agrega la carga configurada en el atacante, incluso los contactos independientes de dos armas. El contacto que llena la barra todavía aplica residual y provoca stun; los siguientes contactos se reciben abiertos. La capacidad, recuperación y duración de stun pertenecen al defensor. La barra disminuye a una tasa por segundo después del retraso desde el último bloqueo y solo fuera de reacción. Durante stun no se recupera; al terminar queda en cero. Recibir más golpes durante stun no renueva su duración, aunque aplica daño y desplazamiento.

El daño normal y el daño crítico son cantidades planas configurables, sin fórmula de escalado por atributos. La probabilidad de crítico es un porcentaje configurado en el Data Asset del atacante. Se realiza una tirada autoritativa por contacto de arma contra receptor, después de deduplicarlo; cada arma o subimpacto legítimo puede tener su propio resultado. Crítico tiene señal visible. Si se bloquea, se usa la misma reducción que para daño normal: daño recibido = máximo entre cero y daño del contacto menos coeficiente de bloqueo del defensor, configurado en su DA. El coeficiente es una reducción plana; un crítico solo cambia el daño de entrada, sin reglas especiales de ruptura ni reacción. Ni reacción ni stun se explican mediante cartas. Los contactos contra un enemigo que combate con otro aliado siguen las reglas normales; no hay sistema SUPPORT.

La interrupción de una ofensiva impide generar nuevos contactos, pero no descarta los contactos válidos ya capturados: estos terminan de resolverse una sola vez con el contexto original. No reabren ventanas, no reanudan la ofensiva interrumpida ni modifican una acción nueva como si fueran contactos de ella. Su tratamiento ante baja, destrucción o reinicio se distingue de la interrupción ordinaria.

Los contactos del frame se clasifican antes de aplicar sus consecuencias. Una pulsación admite una sola instancia entrante nueva; si hay memoria defensiva, se prioriza una instancia distinta. Entre candidatas se usa el target actual, después el orden de rivales y, para un empate restante, el nombre estable del actor. Los impactos de otras instancias necesitan otra pulsación. La carga proyectada determina qué contacto rompe guardia y cuáles quedan abiertos en ese lote.

Se aplica todo el daño válido del lote y se elige una sola reacción y desplazamiento por receptor: ruptura antes que daño abierto, y daño abierto antes que bloqueo; dentro de la misma prioridad se conserva el orden anterior. En ese lote simultáneo se usa el plazo del contacto que determina la reacción. Los contactos capturados por dos participantes vivos pueden intercambiarse aunque uno muera al procesar el lote. No se aplica daño adicional a un receptor ya eliminado. Destrucción o cambio de generación por reset invalida el contexto correspondiente.

## Acercamiento ofensivo

El ataque intenta cerrar distancia hasta el límite propio de la acción. Puede comenzar contra un target lejano, quedar corto y ejecutar igualmente el golpe. Alcance de desplazamiento y alcance de contacto son distintos. No se garantiza acierto ni se teletransporta sin límite al target. La geometría conserva sus restricciones.

Cada paso fija al inicio la dirección y el destino de avance hacia su target elegible, respetando el máximo y la distancia de detención configurados. El avance reemplaza la traslación de raíz ofensiva, que no se suma por separado. Cambiar F, perder el target o que este se aleje no recalcula una persecución durante ese paso. Sin target se ejecuta el ataque sin acercamiento; un paso posterior vuelve a consultar la selección.

Los parámetros y la implementación inicial del paso 3 se registran en [033](../reports/033-resultados-combate/reporte.md); la configuración vigente y el cierre por confirmación del usuario están en [035](../reports/035-revision-paso-3/reporte.md). La calibración se realiza sobre estas reglas, sin atribuir los valores iniciales a mediciones de FGOA.
