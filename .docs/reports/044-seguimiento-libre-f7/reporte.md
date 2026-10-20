# 044 — Seguimiento libre suave y depuración con F7

Estado posterior comprobado: [045 — Estado de cámara](../045-estado-camara/reporte.md). El editor actual carga la DLL principal, arranca PIE y conserva registros de alternancia F7; la observación visual integrada sigue pendiente. El fallo de recarga descrito abajo es histórico.

## Alcance

Reemplaza el rumbo completamente fijo sin target de [043](../043-camara-sin-target/reporte.md) por acompañamiento amortiguado del desplazamiento. Sustituye el comando de consola del lock-on de prueba por F7. HUD diferido; no se modifican assets ni Data Assets.

## Cámara

AVampsPlayerCameraManager conserva la referencia independiente del cuerpo y añade al punto virtual una anticipación horizontal de 0,5 segundos de velocidad real. Su longitud máxima es la mitad de FollowDistance: con 600 cm, hasta 300 cm. Esto limita la desviación angular a 30° respecto al rumbo de referencia, también al retroceder. No acumula correcciones sobre el yaw publicado cada frame, por lo que mantener un lateral no produce una órbita continua por la realimentación entre cámara y WASD.

La respuesta angular sin target es exponencial, de 1,5 por segundo, frente a los 6 del seguimiento con target. Conserva el filtro espacial, las transiciones, los encuadres y el tratamiento de oclusión. Al detenerse desaparece la anticipación y la vista recupera suavemente su referencia. El cuerpo sigue usando bOrientRotationToMovement; su yaw no se consulta para orientar la cámara. Son valores globales internos, sin parámetros por personaje.

## F7

AVampsCombatTestPlayerController enlaza F7 mediante EnhancedInputComponent.BindDebugKey, fuera de Shipping, y llama a UVampsLockOnComponent.ToggleDebugLockOn sobre el pawn local. El componente conserva una bandera de instancia, inicialmente desactivada, y actualiza la selección inmediatamente al alternar. Durante el modo sin lock-on, la elegibilidad compartida impide que F o una guardia efectiva seleccionen un rival. Al restaurarlo se vuelve a buscar un candidato elegible. La IA conserva su selección propia. El binding requiere F7 sin modificadores y no se ejecuta con la partida pausada.

Se elimina el registro de vamps.Debug.DisablePlayerLockOn. La bandera dura mientras exista el componente: detener Play y comenzar otra sesión restaura el lock-on normal. R conserva la bandera porque reinicia el combate sobre los personajes existentes. El cambio de estado se informa en LogVampsCombat.

## Archivos y conexiones

- Source/vamps/Private/Player/VampsPlayerCameraManager.cpp: anticipación de viaje y respuesta angular libre.
- Source/vamps/Public/Characters/VampsLockOnComponent.h y Private/Characters/VampsLockOnComponent.cpp: alternancia de desarrollo por instancia y retiro de CVar.
- Source/vamps/Public/Player/VampsCombatTestPlayerController.h y Private/Player/VampsCombatTestPlayerController.cpp: binding F7 y entrega al componente del pawn.
- BP_VampsCombatTestPlayerController hereda el binding de su clase C++; BP_VampsPlayerCameraManager recibe el cambio de su padre. BP_VampsCharacter_Player conserva su componente LockOn. No hay nuevas Input Actions, asignaciones de Blueprint ni cambios de mapas/materiales.
- Índice de documentación y enlaces históricos de 043 actualizados. Las especificaciones funcionales/técnicas permanecen sin editar según las reglas de esta tarea.

## Verificación

Revisión de código y clang-format aplicados; git diff --check sin errores de espacios. PIE detenido por MCP antes de compilar. No se agregan ni ejecutan tests automatizados.

La primera compilación detectó que EnhancedInputComponent elimina BindKey convencional; se corrigió usando su API BindDebugKey con la firma de UE 5.8. La compilación posterior terminó UBT Succeeded en 30,78 s y generó/enlazó patch_3. Evidencias: [UBT del intento definitivo](build.log), [diagnóstico inicial](build-intento-inicial.log) y [respuestas MCP](live-coding.json).

**El parche no quedó confirmado como aplicado.** Durante la recarga, el editor registró Property size mismatch para VampsPlayerCameraManager (11496 frente a 11504 bytes), reinstanció VampsLockOnComponent y terminó con EXCEPTION_ACCESS_VIOLATION en el destructor del manager de patch_2. El proceso salió y MCP perdió el transporte. [Log del cierre](live-coding-crash.log). El rumbo privado añadido en 043 cambió el tamaño del manager; la recarga acumulada necesita un arranque limpio. El log no permite atribuir todo el fallo a una única causa.

No se guardaron assets durante esa recarga ni se ejecutó una reapertura por shell. La compilación completa y el arranque limpio quedan para el usuario porque las herramientas MCP disponibles no los cubren y las reglas del repositorio prohíben sustituirlos por automatización de interfaz u otro atajo. Arranque en PIE, estado runtime y F7 sin verificar hasta completar la [guía](editor.md).

La sensación de movimiento y la pulsación física de F7 requieren la comprobación de la [guía](editor.md). El algoritmo limita el acompañamiento a 30°: no gira automáticamente todo el rumbo hacia la dirección de locomoción. No se presenta este ajuste como una reconstrucción exacta de FGOA.
