# Controles, movilidad, target y cámara

Estas reglas pertenecen a la [base FGOA adaptada](../game/functional.md). La interpretación del botón compartido está en [combate](../combat/functional.md). Implementación: [technical.md](technical.md).

## Controles

| Entrada | Acción |
|---|---|
| WASD | Movimiento direccional |
| RMB, pulsación individual | Solicitud contextual de ataque/defensa; continuación dentro de ventana |
| Shift con dirección | Evasión breve; mantener para sprint |
| Shift sin dirección | Evasión hacia atrás |
| F | Siguiente enemigo según el orden vertical de la lista del HUD |

El lock-on permanece activo mientras exista un enemigo elegible. No requiere un control para activarlo/liberarlo. Los controles de depuración del [prototipo](../prototype/functional.md) no son controles de partida.

## Movimiento y evasión

WASD permite acercamiento, retirada, laterales y diagonales. Cuerpo y dirección de viaje pueden cambiar conservando el lock-on. La evasión con dirección respeta la dirección solicitada; sin dirección va hacia atrás. El sprint sostenido conserva seguimiento del target.

La locomoción permanece disponible durante NA; reacción y stun tienen sus propias restricciones. El cierre de una cadena por evasión respeta la frontera definida en [combate](../combat/functional.md#cadena-y-continuación). Velocidades, distancias, invulnerabilidad, costo y umbral entre pulsación breve y sostenida se calibran con el prototipo.

## Selección y seguimiento

F recorre el orden vertical de enemigos del HUD. Una guardia efectiva impone al agresor como target y la cámara acompaña el cambio; puede seleccionar fuera de ese recorrido. Un contacto ofensivo con otro enemigo permite continuar sin forzar selección. Daño abierto y advertencia de amenaza tampoco equivalen a guardia efectiva.

La selección, la amenaza entrante, la orientación corporal y la dirección de viaje son conceptos distintos.

## Cámara y oclusores

La cámara conserva tolerancia al pequeño reposicionamiento lateral, corrección gradual, cambios de distancia visual y planos de ataque/defensa. El movimiento de cámara al atacar forma parte del alcance. El target no se fija al píxel central.

El encuadre es la relación visual de jugador y target en pantalla, incluida distancia aparente y orientación de la vista. Los parámetros se ajustan observando el prototipo; no se exigen dos umbrales internos de FGO ni se presupone histéresis.

Los obstáculos visibles preparados para ello se desvanecen gradualmente cuando tapan la vista relevante y recuperan su presentación al dejar de taparla. Una pared visible de límite puede hacerlo sin perder colisión. Un límite invisible conserva su restricción y no tiene representación que desvanecer. Este tratamiento no permite atravesar físicamente cualquier sólido.

## HUD

El HUD muestra vida, recurso de guardia, NA, reacción, stun y señal de crítico conforme al resultado de combate. Incluye selección, lista de rivales, enemigos próximos y dirección de amenaza entrante. Un aviso de amenaza se distingue de confirmar contacto o bloqueo. Puntuación y portador presentan el estado de [partida](../game/functional.md).

## Preguntas

1. ¿Cómo se selecciona el target inicial y el siguiente al morir o perder elegibilidad el actual? ¿Qué muestra y hace la cámara cuando no quedan enemigos elegibles?
2. ¿Qué restricciones y consumo tiene la evasión y cómo se relacionan con sprint? Los valores y el umbral entre ambos se calibrarán después.
