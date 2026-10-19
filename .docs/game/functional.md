# Juego y partida de la base FGOA adaptada

La primera prioridad es reproducir el intercambio, movilidad, lock-on y presentación de FGO Arcade con un único personaje melee jugable. La [referencia analizada](../reports/016-analisis-fgo-arcade/analisis.md) conserva los comportamientos originales; esta especificación aplica las decisiones elegidas para Vamps. [Combate](../combat/functional.md) y [jugador](../player/functional.md) detallan sus reglas. Implementación: [technical.md](technical.md).

## Alcance de la base

| Área | Regla para Vamps |
|---|---|
| Intercambio | Botón ataque/defensa compartido, hasta tres pasos, guardia 360°, residual letal, crítico, guard break/stun y recuperación NA |
| Cartas | Sin mano, selección Q/A/B, bonificaciones, Extra Attack ni disponibilidad causada por estados de cartas; cadena/NA directas |
| Personaje | Un melee jugable; reaparece el mismo personaje, sin reservas ni sustitución al morir |
| Movilidad y presentación | Evasión/sprint, lock-on continuo, selección manual/por guardia, cámara de acciones, HUD de amenaza y desvanecimiento de oclusores |
| Shift sin dirección | Evasión alejándose del target como adaptación explícita |
| Recursos | Carga de guardia y energía de movilidad separadas; agotamiento de movilidad sin guard break |
| Partida | Equipos, puntos por bajas y portador de recompensa sin temática Grial |
| Fuera del primer alcance | Ranged y SUPPORT; atacar al mismo enemigo que otro aliado sigue permitido |
| Después de la base | [Parry, skills, identidades propias y ultimate de Vamps](../extensions/functional.md) |

Estas simplificaciones y adaptaciones son parte del primer alcance; reproducir la base no exige implementar antes los sistemas originales excluidos ni prepararlos para después. Los parámetros se calibran para Vamps; no se copian valores del HUD de FGO como balance obligatorio.

## Equipos, muerte y respawn

La partida enfrenta equipos. Morir concede puntos al equipo adversario según la regla de baja y provoca respawn del mismo personaje mientras la partida continúa. No agota reservas ni cambia el repertorio por sustitución de personaje. La derrota individual se distingue del resultado de equipo.

## Portador de recompensa

Un portador visible concede más puntos al morir que una baja normal. No usa temática Grial. Asignación, aparición y transferencia requieren reglas expresas; la hipótesis de kill leader del equipo ganador no se convierte en requisito.

La curación al adquirir el Grial pertenece a la evidencia del original; no establece curación ni otro bonus para este portador.

## Duración y cierre

Se busca una partida más larga mediante la relación entre valor de baja y objetivo de victoria. Bajar puntos por baja conservando el objetivo exige más bajas; reducir ambos proporcionalmente conserva esa relación. Los valores y tiempos se ajustan después de fijar las condiciones de partida.

## Preguntas

1. ¿Qué tamaño tienen los equipos y cuáles son las condiciones de victoria, límite de tiempo y empate?
2. ¿Cuándo aparece el portador, cómo se asigna y cómo se transfiere tras morir? ¿Tiene efectos adicionales a la recompensa mayor por baja?
3. ¿Cuánto tarda el respawn y dónde reaparece cada jugador?
