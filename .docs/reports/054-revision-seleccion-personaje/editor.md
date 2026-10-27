# Editor — Preparación del paso 6.2

Guía vigente de implementación: [055 — Selector, build y observación](../055-selector-executioner/editor.md). Los pasos siguientes conservan la preparación del diseño.

El diseño del selector está acordado y falta implementarlo. No hay configuración del editor que el usuario deba completar ahora ni es necesario preparar el segundo personaje. Reglas en [partida](../../game/functional.md#selección-de-personaje-e-inicio) y análisis técnico en el [reporte](reporte.md).

## 1. Definir la entrega

**Hecho — usuario.** Executioner es la única entrada; selección propia con mouse y START, sin confirmación adicional; bots asignados desde el catálogo con repetidos permitidos; PLAY AGAIN vuelve al selector conservando la preselección. El usuario preparará después el segundo personaje y su DA, fuera de esta entrega. El ciclo 6.1 sigue cerrado.

## 2. Inventariar referencias al implementar

**Pendiente — Codex por MCP.** Con el catálogo acordado y MCP directo disponible, descubrir herramientas y parámetros y leer los Blueprints/DA/AnimBP/armas/montages que integrarán cada entrada. Partir de `/Game/Vamps/Blueprints/BP_VampsCharacter_Player`, `BP_VampsCharacter` y `/Game/Vamps/Data/DA_VampsCombatSettings` como configuración existente, sin asumir que sus valores actuales son defaults.

Si se cambia una clase de DA o un struct de sus propiedades, buscar primero todos sus assets y derivados, capturar valores completos de forma durable y guardarlos por MCP antes de editar/compilar. Restaurar/comparar tras carga estable y guardar solo los afectados. Esta revisión solo actualiza documentación; no modifica esos tipos ni assets.

## 3. Preparar el contenido elegido

**Pendiente — Codex por MCP, durante la implementación.** Verificar las referencias existentes de Executioner y configurar únicamente los assets del selector que se necesiten. No crear otro DA de combate, variantes ni un segundo personaje. Las rutas de assets propios del selector se determinarán con su implementación.

Las operaciones sin una herramienta MCP directa quedarán como pasos exactos para el usuario cuando se conozcan el asset y cambio requerido. No se automatizará la interfaz ni se usará un atajo para evitar ese paso.

## 4. Compilación y comprobación de la futura implementación

**Pendiente — Codex por MCP.** Compilar con LiveCodingToolset.LiveCodingToolset.CompileLiveCoding cuando corresponda y verificar el resultado, distinguiendo NoChanges de un parche real. Si se necesita un build o reinicio sin soporte MCP, detallar el paso al usuario.

**Pendiente — usuario, después de implementar.** Comprobar que el mapa abre en selección sin combate y muestra Executioner; START es la única acción para iniciar la preparación de 5 s con los cuatro participantes; recursos/armas/animaciones se conservan y respawn mantiene personaje. PLAY AGAIN vuelve al selector con Executioner preseleccionado y permite iniciar otra ronda limpia. El selector debe informar una configuración inválida antes de empezar, sin sustituirla por otra opción.

No se ejecutaron operaciones del editor ni tests automatizados en esta revisión. No hay configuración pendiente que deba aplicarse ahora para seguir usando L_LocalMatch de 6.1.
