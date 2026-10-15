# Roadmap: referencia primero, extensiones después

La prioridad es reproducir la [base FGOA adaptada](game/functional.md) con un único melee jugable. Las exclusiones y adaptaciones elegidas forman parte de ese primer alcance: no se implementan cartas para luego retirarlas. Las [extensiones Vamps](extensions/functional.md) se agregan después de comprobar la base.

## 1. Punto de partida y decisiones del núcleo

Leer el [prototipo existente](prototype/functional.md) y su [flujo GAS](prototype/technical.md#cómo-leer-el-código). Las conexiones de Unreal se conservan en [013–015](reports/015-retirada-duel/editor.md); el contacto del puño/notify continúa pendiente de confirmación manual. Este montaje permite observar el cambio de reglas, pero su cancelación de ataque por guardia no es el resultado objetivo de FGOA.

Resolver [vigencia defensiva y arbitraje](combat/functional.md#preguntas) al diseñar las fases/contactos. No se reabre la revisión de videos ni se recuperan buffer, prioridades por ID u otros modelos retirados.

## 2. Intercambio y cadena de referencia

Implementar el [combate compartido](combat/technical.md) con fases, defensa válida durante la acción y hasta tres pasos. Comprobar manualmente:

- Pulsación temprana descartada y continuación válida dentro de ventana.
- Contacto abierto, defendido o contra otro enemigo que habilita continuidad, sin retarget ofensivo obligatorio.
- Fallo que corta cadena y cierre al final de paso por no continuar/evasión.
- Defensa que conserva solicitudes y contexto de cadena, sin NA por bloquear ni cancelación incondicional del agresor.

## 3. Recuperación, daño y recursos

Completar NA, reacción, residual letal, guardia/guard break y crítico tras resolver sus [reglas](combat/functional.md#preguntas). Comprobar la diferencia entre entrada disponible, ejecución ofensiva demorada y presentación; locomoción durante NA; y stun distinto de recuperación ofensiva. Calibrar valores con observación del prototipo.

## 4. Movilidad, selección y presentación

Incorporar avance limitado por acción, evasión/sprint, [selección, cámara y HUD](player/technical.md). Comprobar un ataque que queda corto, guardia que cambia al agresor, F según lista, desplazamiento lateral con seguimiento, cámara al atacar y oclusores que se desvanecen sin eliminar colisión. Definir selección inicial/pérdida de target antes de implementar esos casos.

## 5. Partida por equipos

Resolver [formato, puntuación, portador y respawn](game/functional.md#preguntas). Integrar muerte, respawn del mismo personaje, puntos, portador y cierre de partida, con autoridad/entrega de solicitudes para varios participantes. Comprobar la coherencia entre estado de combate, selección y presentación; replicar atributos por sí solo no completa esta etapa.

## 6. Capa propia de Vamps

Sobre la base reproducida, desarrollar [perfiles y dos skills por personaje](extensions/characters/functional.md), parry y ultimate propia sin cinemática global. El orden interno se elige al abordar esta capa. Estas funcionalidades no son requisitos previos de la reproducción ni justifican hoy bindings, clases o atributos anticipados.

En cada implementación se revisa código, se compila por MCP directo cuando corresponda y se comprueba manualmente en el editor. No se crean ni ejecutan tests automatizados. Las operaciones sin MCP directo se documentan para el usuario, sin automatizar la UI.
