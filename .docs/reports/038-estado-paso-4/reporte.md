# 038 — Estado del paso 4

Implementación posterior: [039 — Simplificación de movilidad](../039-simplificacion-movilidad/reporte.md), con [guía vigente](../039-simplificacion-movilidad/editor.md). Esta revisión conserva el estado anterior y sus propuestas.

## Alcance y resultado — 2026-10-19

Revisión del trabajo recibido en el commit `0d5ef81` (`dash and sprint`), contrastando roadmap, fuentes, [implementación 037](../037-movilidad-combate/reporte.md) y lecturas actuales del editor mediante MCP directo. El árbol de trabajo estaba limpio al iniciar.

El paso 4 está implementado y configurado, con cierre pendiente de revisión del impacto final y observación jugable. Las etapas 0–3 están cerradas según el roadmap. La frase general «Próxima etapa: 4» conserva ese estado, aunque la implementación ya existe en 037.

No hubo cambios de código, configuración ni assets. Se agregan este reporte, su [guía](editor.md), la [captura MCP](estado-mcp.json) y sus enlaces desde el índice. No se compila ni se inicia PIE en esta revisión.

## Implementación recibida

- `UVampsCombatMovementComponent`: avance limitado por paso, dash con colisión y ocho montages direccionales; sprint modifica velocidad. Suprime traslación raíz ofensiva para no sumar avance.
- `AVampsCharacter` y `FVampsCombatState`: permisos, intención de movilidad, salida de Guard tras impacto final, conservación de NA y limpieza por contacto/reset/muerte. `UVampsAttributeSet` conserva energía separada de GuardLoad.
- `UVampsCombatResolver`, `AVampsWeapon` y `UGA_Attack`: identifican impacto final por arma/subimpacto; contactos se resuelven antes de continuación y dash. Una continuación válida prevalece sobre dash.
- `AVampsPlayerController`: entrada Shift y dirección actual. `UVampsAnimInstance` y `AVampsCombatTestHUD`: estados visuales y diagnóstico de movilidad.
- Según 037, `IA_Mobility` está asignada a LeftShift en ambos contextos y a los dos Blueprints de controller; `BS_Executioner` incorpora la muestra de sprint. Esas conexiones no se volvieron a consultar en esta revisión.

Las responsabilidades, archivos modificados y conexiones originales se detallan en 037. Se revisó su presencia en los fuentes actuales, sin atribuir esos cambios a esta iteración.

## Verificación actual por MCP

`ObjectTools.list_properties/get_properties` confirmó:

- `/Game/Vamps/Data/DA_VampsCombatSettings`: tres pasos con avance 250 cm / 0,25 s y detención 130 cm; arma/impacto final 0/0 en los tres.
- Energía 100, dash 25, sprint 20/s, recuperación 25/s tras 1 s; dash 350 cm / 0,3 s y sprint 700 cm/s.
- Ocho referencias a `/Game/Vamps/Animations/Executioner/AM_Executioner_Dash_<dirección>`, en orden Fw/FR/R/BR/Bw/BL/L/FL.
- Los ocho montages tienen duración 1 s, un segmento direccional correspondiente en DefaultSlot, rango 0–1 s, velocidad 1, una repetición, RateScale=1 y Auto Blend Out activo.

La lectura confirma configuración en el editor conectado. No acredita observación de animación, guardado actual, build local ni gameplay. El build/arranque limpio del usuario, preservación del DA, guardado y compilaciones previas están acreditados en 037; su última captura Live Coding registra Success. No se ejecutaron tests automatizados ni se automatizó UI.

## Próximos pasos

1. Revisar el impacto final de cada sección de `AM_Executioner_Combo`, conforme al [paso 4 de 037](../037-movilidad-combate/editor.md#4-verificar-la-autoría-del-impacto-final). El valor 0 solo representa el primer subimpacto; si el arma tiene reinicios, hay que elegir su índice final correcto.
2. Completar las observaciones del [paso 5 de 037](../037-movilidad-combate/editor.md#5-comprobar-el-comportamiento-integrado): avance/fallo/colisión, direcciones y blending, energía/sprint, Guard multihit, NA/Hit/Stun, prioridad de continuación y limpieza.
3. Registrar resultados y confirmar cierre de 4; después actualizar el estado del roadmap cuando se solicite ese cierre.
4. Continuar con [etapa 5](../../roadmap.md#5-presentación-de-combate-y-cámara-de-referencia): planos/distancia/FOV de acción, oclusores con fade/restauración y HUD con amenaza previa al contacto, manteniendo coherencia de target.
5. Después siguen equipos/respawn/puntuación/portador (6), integración de red (7), calibración (8) y extensiones Vamps (9).

## Aclaraciones sobre movimiento, dirección y GAS

TryStartCombatDash define desplazamiento de cápsula y ApplyRootMotionToVelocity calcula su velocidad; TickCharacterPose descarta el root motion de dash/ataque. Desactivar Enable Root Motion no explica por sí solo una cápsula inmóvil durante un dash aceptado. Un mesh que sale de la cápsula puede indicar traslación raíz sin extraer/bloquear, pero no se observó ni diagnosticó ese caso en PIE. Activar extracción no reemplaza la investigación de un desplazamiento de cápsula ausente.

Para la locomoción planteada por el usuario, bastan como propuesta un dash frontal con giro hacia la dirección actual y otro de retirada sin input. Hay que orientar antes de fijar el dash: la locomoción actual gira gradualmente a 500 grados/s, y usar solo el forward del frame anterior puede dar otra dirección al pulsar movimiento y Shift simultáneamente. No se necesita historial de teclas; conservar una dirección de solicitud sigue siendo pertinente mientras el dash pueda esperar al cierre de una ventana de combo. El código actual almacena dirección actual y pendiente, no un historial last pushed. La cámara no acepta giro manual, pero sí cambia yaw mediante seguimiento automático.

Una GA_Dash no es requisito del motor. Como recomendación para este proyecto con GAS, puede concentrar ejecución, coste, montage y cancelación, manteniendo desplazamiento/colisión en MovementComponent y arbitraje de contactos/continuación en el resolver. No se implementó una GA nueva ni se cambió la regla vigente de ocho direcciones.

Fuentes oficiales consultadas: [Root Motion](https://dev.epicgames.com/documentation/en-us/unreal-engine/root-motion-in-unreal-engine) y [Gameplay Abilities](https://dev.epicgames.com/documentation/en-us/unreal-engine/using-gameplay-abilities-in-unreal-engine).
