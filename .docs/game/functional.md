# Juego y partida de la base FGOA adaptada

La primera entrega usa Executioner para todos los participantes y reproduce el intercambio, movilidad, lock-on y presentación de FGO Arcade dentro de una partida por equipos. La [referencia analizada](../reports/016-analisis-fgo-arcade/analisis.md) conserva los comportamientos originales; esta especificación aplica las decisiones elegidas para Vamps. [Combate](../combat/functional.md) y [jugador](../player/functional.md) detallan sus reglas. Implementación: [technical.md](technical.md).

## Alcance de la base

| Área | Regla para Vamps |
|---|---|
| Intercambio | Botón ataque/defensa compartido, hasta tres pasos, guardia 360°, residual letal, crítico, guard break/stun y recuperación NA |
| Cartas | Sin mano, selección Q/A/B, bonificaciones, Extra Attack ni disponibilidad causada por estados de cartas; cadena/NA directas |
| Personaje | Executioner para todos en la primera parte de 6; selección de personaje en la segunda parte; respawn del mismo personaje, sin reservas ni sustitución al morir |
| Movilidad y presentación | Evasión/sprint, lock-on continuo, selección manual/por guardia, cámara de acciones, HUD de amenaza y desvanecimiento de oclusores |
| Shift sin dirección | Evasión alejándose del target como adaptación explícita |
| Recursos | Carga de guardia y energía de movilidad separadas; agotamiento de movilidad sin guard break |
| Partida | A contra B, puntos por bajas con recompensa creciente por racha, respawn y resultado con estadísticas |
| Fuera del primer alcance | Ranged y SUPPORT; atacar al mismo enemigo que otro aliado sigue permitido |
| Después de la base | [Parry, skills, identidades propias y ultimate de Vamps](../extensions/functional.md) |

Estas simplificaciones y adaptaciones son parte del primer alcance; reproducir la base no exige implementar antes los sistemas originales excluidos ni prepararlos para después. Los parámetros se calibran para Vamps; no se copian valores del HUD de FGO como balance obligatorio.

## Entregas del paso 6

La primera parte entrega el ciclo local 2v2: el jugador y un bot en A, dos bots en B, todos con Executioner. La segunda parte agrega selección de personaje y su flujo de preparación; sus opciones se especifican al abordarla. No se construye esa selección como requisito del primer ciclo.

## Equipos y bajas

Los aliados no se seleccionan como enemigos ni reciben contactos ofensivos. Dos aliados pueden golpear al mismo enemigo; no se agrega SUPPORT. La derrota individual se distingue del resultado de equipo.

Cada baja suma una kill al autor del contacto letal y una muerte a la víctima. Las estadísticas acumuladas sobreviven al respawn. La racha cuenta las kills obtenidas desde la última muerte y vuelve a cero al morir. Los contactos capturados válidamente pueden causar bajas recíprocas; un participante que muere termina ese lote con racha cero, aunque reciba una kill por el intercambio.

La baja reproduce una animación de muerte propia del personaje: variante defensiva si el contacto letal fue bloqueado, incluida una ruptura letal, y variante normal si fue abierto. La selección usa el resultado del contacto letal, antes de limpiar reacción/guardia. El cuerpo permanece visible durante la espera y deja de bloquear movimiento; no vuelve a locomoción al terminar la animación. Durante el countdown de respawn, el jugador controla una cámara libre, separada del personaje eliminado.

## Puntuación

El objetivo inicial es **1000 puntos**. Derrotar a un enemigo otorga al equipo del atacante **100 + 25 × racha de la víctima**. Una víctima sin kills en su racha concede 100; con cuatro concede 200. Objetivo, recompensa base e incremento son parámetros ajustables.

El valor de las víctimas se toma antes de aplicar las bajas del mismo lote de contactos. Después se registran kills, muertes y puntos, se reinician las rachas de los eliminados y se evalúa el resultado. El orden de procesamiento no cambia la recompensa de un intercambio simultáneo. La recompensa creciente reemplaza al portador; no existe un portador especial.

## Respawn y protección

Mientras la partida continúa, cada eliminado reaparece tras **5 segundos** en el spawn point de su equipo, con el mismo personaje. No consume reservas ni cambia su repertorio. La vida vuelve al máximo configurado en los stats del personaje, la carga de guardia a cero y la energía de movilidad a su máximo vigente; el modo de partida no impone una vida global.

La reaparición concede hasta **2 segundos de protección** frente a contactos ofensivos enemigos: no aplican daño, carga de guardia, reacción ni desplazamiento y no habilitan continuación por contacto. Ese plazo se mide desde la aparición efectiva y termina antes si el personaje inicia un ataque aceptado. Una solicitud rechazada no la termina. El HUD muestra un mensaje de espera con la cuenta regresiva hasta el respawn.

## Duración y cierre

Cada partida comienza con una cuenta regresiva visible de **5 segundos**, presentada como la espera de respawn. Durante esa preparación los participantes no pueden combatir ni moverse y la IA no toma decisiones de combate. Al terminar comienza el juego y una cuenta de tiempo restante de **5 minutos** en el HUD. Ambos valores son parámetros iniciales ajustables.

Al terminar un lote, si un único equipo alcanza o supera el objetivo, gana ese equipo. Si ambos lo alcanzan en el mismo lote, se prioriza el mejor **K/D de equipo**, calculado con kills y muertes acumuladas de sus integrantes; si el K/D también coincide, el resultado es empate. Un equipo con kills y cero muertes tiene ratio superior a cualquier ratio finito; cero kills y cero muertes representa ratio cero.

Si se agota el tiempo antes del cierre por objetivo, se consolidan los contactos válidos capturados hasta ese instante y gana el equipo con más puntos. A igual puntuación decide el K/D de equipo; si también coincide, hay empate.

El resultado detiene combate, movimiento de participantes, decisiones de IA, acumulación de puntos y respawns. Aparece un panel pequeño con «Equipo A ganador», «Equipo B ganador» o «Empate», puntuación y kills/muertes/K/D por participante y equipo.

El botón «Otra partida» inicia un ciclo nuevo en la entrega local: basta la aceptación del jugador local y se reinician puntuación, estadísticas y rachas. La espera a que todos acepten pertenece a una ampliación posterior; no se implementa ahora.

Se busca una partida más larga mediante la relación entre recompensa y objetivo. Bajar ambos proporcionalmente conserva esa relación; el balance inicial puede ajustarse manteniendo estas reglas.

## Preguntas

1. ¿La cámara libre durante la espera vuela por el escenario con WASD y mouse o gira alrededor del cuerpo con el mouse?
