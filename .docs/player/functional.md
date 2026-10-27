# Controles, movilidad, target y cámara

Estas reglas pertenecen a la [base FGOA adaptada](../game/functional.md). La interpretación del botón compartido está en [combate](../combat/functional.md). Implementación: [technical.md](technical.md).

## Controles

| Entrada | Acción |
|---|---|
| WASD | Movimiento direccional |
| RMB, pulsación individual | Solicitud contextual de ataque/defensa; continuación dentro de ventana |
| Shift con dirección | Evasión breve; mantener para sprint |
| Shift sin dirección | Evasión alejándose del target |
| F | Siguiente enemigo según el orden vertical de la lista del HUD |

El lock-on permanece activo mientras exista un enemigo elegible. No requiere un control para activarlo/liberarlo. Los controles de depuración del [prototipo](../prototype/functional.md) no son controles de partida.

## Movimiento y evasión

WASD permite acercamiento, retirada, laterales y diagonales y conserva su referencia al yaw de cámara. Cuerpo y dirección de viaje pueden cambiar conservando el lock-on. Shift solicita evasión inmediatamente; su dirección se fija al pulsar. Sin dirección, se aleja del target seleccionado. Sin target o con posiciones horizontales coincidentes, ese retroceso se rechaza con diagnóstico; una dirección explícita permite evadir sin target.

Después del dash, mantener Shift con dirección permite sprint; soltarlo termina esa intención. Con dirección explícita, el personaje gira hacia ella al iniciar y usa el dash frontal; sin dirección, mira al target y usa la retirada. La evasión mantiene esa orientación durante su recorrido; el sprint conserva el giro gradual de locomoción. Cámara y selección continúan siguiendo al mismo rival. Ninguno concede invulnerabilidad.

NA permite locomoción, dash y sprint, conservando su plazo y prohibición de atacar/defender. Guard, Hit, Stun y muerte impiden dash; la reacción de guardia debe terminar antes de evadir. Fuera de esas restricciones, ataque/defensa se pueden iniciar durante movilidad con transiciones mezcladas; comenzar ataque sustituye el dash y recibir un contacto sustituye la movilidad por la reacción correspondiente.

Durante ataque se admite una solicitud de dash en su ventana de continuación o en Recovery. Se ejecuta después de cerrar contactos y ventana, conservando la recuperación del paso y la NA posterior. Una continuación ofensiva válida gana y descarta el dash solicitado. La solicitud pertenece al paso actual; interrupción, fin de acción, muerte o reset la eliminan. No se acumulan pulsaciones tempranas ni un dash tardío para otro paso.

## Energía de movilidad

La energía de dash/sprint es independiente de la carga de guardia. Un dash aceptado paga su coste una vez; sprint consume por segundo mientras existe intención direccional admitida. Sin energía suficiente se rechaza el dash. Al agotarse durante sprint, termina el sprint y sigue disponible la locomoción ordinaria; se necesita otra pulsación para solicitar nuevamente dash/sprint. El agotamiento no causa guard break ni stun.

La energía se recupera después del retraso desde el último gasto y fuera del dash/sprint; puede recuperarse durante reacciones. Un contacto interrumpe la intención sostenida, que requiere otra pulsación. Reiniciar restaura su capacidad. Los parámetros vigentes de Vamps están en [039](../reports/039-simplificacion-movilidad/reporte.md); no son mediciones de FGOA.

## Selección y seguimiento

F recorre el orden vertical de enemigos del HUD. Una guardia efectiva impone al agresor como target y la cámara acompaña el cambio; puede seleccionar fuera de ese recorrido. Un contacto ofensivo con otro enemigo permite continuar sin forzar selección. Daño abierto y advertencia de amenaza tampoco equivalen a guardia efectiva.

La selección, la amenaza entrante, la orientación corporal y la dirección de viaje son conceptos distintos.

## Cámara y oclusores

La cámara conserva tolerancia al pequeño reposicionamiento lateral, corrección gradual, cambios de distancia visual y planos de ataque/defensa. El movimiento de cámara al atacar forma parte del alcance. El target no se fija al píxel central.

El encuadre es la relación visual de jugador y target en pantalla, incluida distancia aparente y orientación de la vista. Los parámetros se ajustan observando el prototipo; no se exigen dos umbrales internos de FGO ni se presupone histéresis.

Los obstáculos visibles preparados para ello se desvanecen gradualmente cuando tapan la vista relevante y recuperan su presentación al dejar de taparla. Una pared visible de límite puede hacerlo sin perder colisión. Un límite invisible conserva su restricción y no tiene representación que desvanecer. Este tratamiento no permite atravesar físicamente cualquier sólido.

## HUD

El HUD muestra vida, carga de guardia y energía de movilidad por separado, NA, reacción, stun y señal de crítico conforme al resultado de combate. Incluye selección, lista de rivales, enemigos próximos y dirección de amenaza entrante. Un aviso de amenaza se distingue de confirmar contacto o bloqueo. Puntuación, kills/muertes/K/D, countdown de inicio/respawn, tiempo restante y resultado presentan el estado de [partida](../game/functional.md). La espera de respawn permite cámara libre separada del personaje eliminado. El panel final incluye el botón local «Otra partida».

La selección excluye aliados y eliminados y readmite enemigos cuando reaparecen. Cámara y HUD conservan el mismo target y presentan su ausencia cuando no hay enemigos elegibles. F9 alterna combate/debug en desarrollo sin cambiar el estado de partida; R/F6/F7 pertenecen al escenario de observación.
