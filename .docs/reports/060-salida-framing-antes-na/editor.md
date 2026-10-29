# Salida de combat framing — operaciones del editor

Guía histórica: la configuración vigente se traslada a [061 — AN_EndRecovery](../061-fin-recovery-notify/editor.md). No aplicar la comprobación de anticipación temporal de esta guía al código nuevo.

Queda comprobar visualmente que la cámara ya está en plano general al comenzar la barra de NA y que no hace una salida intermedia al encadenar golpes. PIE quedó iniciado en `L_LocalMatch` con el parche aplicado. No hay assets nuevos, asignaciones ni cambios de notifies que realizar.

## 1. Preparación y valores

**Hecho — Codex por MCP.** Se descubrieron los schemas de Live Coding y control del editor; PIE estaba detenido. `EditorAppToolset.SearchCVars` confirmó `vamps.Camera.TransitionDuration=0.3`. `ObjectTools.list_properties/get_properties` leyó `NARecoveryDuration=0.5` y el montage `AM_Executioner_Combo` del DA.

No modificar el Data Asset para aplicar este cambio. La lectura posterior conserva ambos valores y la configuración de cámara sin diferencias.

## 2. Compilación

**Hecho — Codex por MCP.** `LiveCodingToolset.LiveCodingToolset.CompileLiveCoding` devolvió `Success`, con compilación y aplicación de un parche real. [Resultado](live-coding.json). No se usó una compilación sin cambios como evidencia.

No hace falta recrear `BP_VampsPlayerCameraManager`, personajes ni widgets. Este cambio se concentra en las clases nativas de cámara/personaje.

## 3. Arranque

**Hecho — Codex por MCP, verificación parcial.** `EditorAppToolset.StartPIE` inició `L_LocalMatch`. `SceneTools.find_actors` encontró el manager propio y `ObjectTools.get_properties` leyó `bCombatFraming=false` y `FramingSource=None` durante selección. La consulta de logs no mostró errores nuevos de cámara/configuración.

## 4. Observación de combate

**Pendiente — usuario:**

1. En `L_LocalMatch`, elegir EXECUTIONER → START. Hacer un solo ataque y quedarse quieto: después de cerrar la posibilidad de combo, la cámara debe regresar durante la recuperación y mostrar NA desde el inicio.
2. Encadenar dos y tres golpes, incluyendo una solicitud cerca del cierre de ventana. No debe aparecer una salida al plano general entre golpes que continúan válidamente. Repetir dejando vencer la ventana sin continuar y con un golpe que falla.
3. Moverse o solicitar dash durante Recovery. La liberación de la animación no debe adelantar ni alargar NA; la cámara debe seguir llegando al plano general a tiempo.
4. Probar bloqueo, interrupción y cambio de objetivo durante la salida. El framing debe corresponder a la cadena vigente y no quedar enganchado a un montage/generación anterior.
5. Verificar muerte/respawn y nueva ronda: no deben reutilizar una decisión de cierre de la cadena previa.

La duración de NA sigue siendo 0,5 s. Si el cierre del combo deja menos de 0,3 s, el regreso de cámara es más corto para respetar ese plazo. Mantener una ventana de continuación abierta tiene prioridad sobre iniciar anticipadamente la salida.

Las interacciones se dejan al usuario porque no hay una herramienta MCP directa para introducir esos inputs de juego. No se utiliza automatización de la interfaz.
