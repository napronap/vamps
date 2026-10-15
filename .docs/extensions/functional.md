# Extensiones propias de Vamps

Estas funcionalidades se agregan después de reproducir y comprobar la [base FGOA adaptada](../game/functional.md), según el [roadmap](../roadmap.md). Se conservan como una capa de diseño propia, separada de la referencia. Implementación: [technical.md](technical.md).

## Identidades y skills

Se conservan los criterios de distancia, ritmo, movilidad, presión, defensa y aporte al equipo, los cinco perfiles y las dos skills por personaje en [identidad de personajes](characters/functional.md). El primer contraste compara duelista de presión, golpeador pesado y especialista en alcance. No condiciona la reproducción con un único melee.

Las skills refuerzan identidad sin eliminar todas las debilidades. No se adopta automáticamente el sistema de tres skills del original ni SUPPORT por incluir herramientas de equipo.

## Parry

Parry pertenece a una etapa posterior del combate propio. Su presencia entre las herramientas posibles de un perfil no agrega una ventana o resultado de parry a la guardia del primer alcance. Sus condiciones, entrada y consecuencias se definen antes de incorporarlo; los modelos de reportes antiguos no se restauran automáticamente.

## Ultimate

La ultimate tendrá comportamiento propio sin cinemática global ni interrupción del combate de los demás jugadores. La canalización, área y condición de disparo del NP original permanecen en la [referencia](../reports/016-analisis-fgo-arcade/analisis.md#10-identidad-recursos-y-partida); no pasan automáticamente a ser requisitos de Vamps.

## Preguntas para esta etapa

1. ¿Qué entrada, ventana, costo, resultado y riesgo tendrá parry respecto de la guardia común?
2. ¿Cómo se carga, activa, evita y resuelve la ultimate sin interrumpir el combate ajeno?

Las preguntas sobre personajes y sus dos skills están al final de su [documento funcional](characters/functional.md#preguntas). Estas decisiones se resuelven cuando se aborde esta capa, sin bloquear la base.
