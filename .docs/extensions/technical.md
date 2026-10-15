# Implementación de extensiones Vamps

Comportamiento: [functional.md](functional.md). Dependencias: [combate](../combat/technical.md), [jugador](../player/technical.md) y [partida](../game/technical.md) de la base FGOA adaptada.

## Integración sobre la base

Las extensiones reutilizan solicitudes, acciones, resolución de contactos y autoridad de la base. Se extrae lógica compartida cuando varias herramientas la necesitan; no se duplican estados de disponibilidad, daño o selección en cada personaje.

La configuración y habilidades de los perfiles se desarrollan en [identidad técnica](characters/technical.md). Diferencias numéricas no justifican por sí solas una clase o componente por personaje.

Parry se integra como regla explícita de resolución cuando se definan sus condiciones funcionales; no se reserva hoy un resultado, tag o binding sin uso. Su efecto sobre cadena, guardia, NA y reacción se determina al diseñarlo.

La ultimate requiere un flujo que mantenga activos a los demás participantes. No justifica agregar anticipadamente atributos de carga, habilidades vacías, inputs, assets o infraestructura de cinemática. Área, carga y efectos se implementan según el diseño propio que se elija.

## Preguntas

1. Con las reglas propias elegidas, ¿qué partes reutilizan una acción parametrizada y cuáles requieren una habilidad específica?
