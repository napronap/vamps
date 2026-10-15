# 019 — Cámara automática sobre lock-on

Plan de continuación: [020 — Roadmap de emulación FGOA](../020-roadmap-emulacion-fgoa/reporte.md). Esa revisión documental conserva los resultados y pendientes de implementación de esta iteración.

## Alcance y resultado

Se implementa el seguimiento automático elegido para la cámara local. `AVampsPlayerCameraManager` consulta el objetivo de `UVampsLockOnComponent`, corrige gradualmente el yaw con tolerancia horizontal y mantiene una inclinación fija. Sin objetivo vuelve detrás del personaje. Cámara y HUD consumen la misma selección; el manager no mantiene otra lista ni elige rivales.

La clase nativa compiló y aplicó cambios mediante Live Coding (`Success`, no `NoChanges`). Los assets están configurados y guardados por MCP; siete Blueprints compilaron con warnings como errores. En PIE se verificaron seguimiento lateral, ausencia de objetivos y readquisición mediante propiedades y transformaciones. La compilación completa y comprobación visual de [editor.md](editor.md) se cerraron después por confirmación del usuario de la etapa 0 el 15 de octubre de 2026; [registro en 020](../020-roadmap-emulacion-fgoa/reporte.md#cierre-de-la-etapa-0).

Esta iteración continúa [018](../018-lockon-component/reporte.md) y reemplaza su configuración de cámara libre/Look. Es una primera implementación del seguimiento: los cambios de distancia, planos de ataque/defensa y desvanecimiento de oclusores de la especificación FGOA quedan fuera de esta entrega.

## Código y responsabilidades

Rutas relativas a `Source/vamps/`, con headers en Public e implementaciones en Private.

| Archivos/clase | Cambio y relación |
|---|---|
| `Player/VampsPlayerCameraManager.h/.cpp`, nuevos | Deriva directamente de `APlayerCameraManager`. Sobrescribe su método virtual `ProcessViewRotation` con la firma nativa; consulta el pawn local y su `LockOn` antes de que el controller aplique `ControlRotation` |
| `Player/VampsPlayerController.h/.cpp` | Constructor asigna `AVampsPlayerCameraManager` a la propiedad heredada `PlayerCameraManagerClass`. Retira propiedad, binding y método de Look. Move sigue relativo al yaw de ControlRotation, ahora dirigido por la cámara |
| `Characters/VampsCharacter_Player.cpp` | Eleva el pivote del SpringArm mediante `TargetOffset = (0, 0, 60)`. Conserva brazo de 400 cm, colisión nativa y cámara adjunta |
| `UI/VampsCombatTestHUD.cpp` | Cambia la ayuda de `Mouse look` a `Auto camera`. La selección y el marcado del HUD siguen perteneciendo a 018 |

El flujo es `LockOn → CameraManager.ProcessViewRotation → ControlRotation → CameraBoom → FollowCamera`. El pawn permanece como ViewTarget del motor; no se cambia al rival ni se orienta el cuerpo para forzarlo a mirar al target. El manager opera sobre jugadores locales con `AVampsCharacter`; para otras vistas conserva el procesamiento de la clase base.

La dirección horizontal va del jugador al objetivo. Dentro de ±6° no corrige yaw; fuera, interpola hacia el borde de esa tolerancia. Usa el recorrido angular corto y el factor exponencial `1 - exp(-TrackingSpeed * DeltaTime)`. Con posiciones horizontales coincidentes conserva el yaw. Sin target utiliza el frente del personaje, sin tolerancia. Descarta el delta de mirada manual y conserva límites/modificadores del procesamiento nativo. No se agregan métodos que oculten otros de Unreal; los identificadores nuevos se revisaron contra las clases base.

| Parámetro en BP_VampsPlayerCameraManager | Valor inicial |
|---|---|
| Follow Pitch | −15° |
| Tracking Speed | 6 s⁻¹ |
| Horizontal Tolerance | 6° |

Son valores iniciales ajustables, no mediciones de FGOA. La tolerancia se aplica al rumbo jugador/objetivo, no a coordenadas de pantalla. El pitch no sigue la altura del rival. La corrección de rotación se realiza en el manager; no se duplica con Camera Rotation Lag en el SpringArm.

## Assets y conexiones

| Ruta bajo `/Game/Vamps` | Estado y configuración |
|---|---|
| `Blueprints/BP_VampsPlayerCameraManager`, nuevo | Padre nativo `/Script/vamps.VampsPlayerCameraManager`, con los tres valores anteriores |
| `Blueprints/BP_VampsPlayerController` | Player Camera Manager Class = `BP_VampsPlayerCameraManager`; conserva `IMC_Vamps` e `IA_CycleTarget` |
| `Blueprints/BP_VampsCombatTestPlayerController` | Misma asignación explícita; conserva `IMC_VampsCombatTest`, Cycle Target, F6 y R. No hereda defaults del Blueprint normal |
| `Input/IMC_Vamps` | Retira únicamente el mapping de `IA_Look`; quedan WASD, LMB, RMB y F, siete mappings |
| `Input/IMC_VampsCombatTest` | Retira únicamente el mapping de `IA_Look`; quedan los anteriores más F6 y R, nueve mappings |

`IA_Look` permanece como asset sin uso en estos contextos; no se eliminó. Los mappings restantes conservaron sus datos y modificadores. `BP_VampsCharacter_Player` recibe el offset del constructor nativo; el CDO confirmó brazo 400, offset Z 60, Use Pawn Control Rotation y Do Collision Test activos, Camera Lag y Camera Rotation Lag desactivados. No se agregó lógica Blueprint ni se reutilizó lógica Third Person.

También se compilaron y guardaron `BP_VampsCharacter`, `BP_VampsCharacter_Player`, `BP_VampsGameMode` y `BP_VampsCombatTestGameMode`, sin cambios adicionales de comportamiento ni diferencias binarias en esos cuatro assets. Los dos controllers y los dos contextos sí tienen cambios de asset.

## Verificaciones y límites

- Se aplicó clang-format a los seis archivos C++ de esta iteración. `git diff --check` no detectó errores de whitespace. No se agregaron ni ejecutaron tests automatizados.
- `CompileLiveCoding` devolvió `Success` y enlazó/aplicó cambios. Unreal advirtió que los cambios de tipos podían causar fallas de empaquetado; la compilación completa con el editor cerrado quedó posteriormente cerrada por confirmación del usuario de la etapa 0. No se ocultó el aviso ni se modificó el motor.
- Los siete Blueprints indicados compilaron con `warnings_as_errors = true`, sin errores ni warnings. Los nueve assets quedaron guardados y `is_dirty = false`; se releyeron padre, parámetros y referencias de ambos controllers.
- En `L_CharacterBasic`, PIE creó `BP_VampsPlayerCameraManager_C_0` y el controller lo referenció. Había un rival colocado en el mapa además del creado por el GameMode; Codex no creó ese actor.
- Con jugador en `(-220, 0)` y target en `(200, -280)`, el yaw de cámara se estabilizó en −27,69°, frente a un rumbo de −33,69°. Al mover temporalmente ese mismo target a `(200, 280)`, se observó un yaw intermedio de +20,20° y luego +27,69°. La selección conservó el mismo actor.
- Con Health de ambos rivales a cero, `currentTarget` pasó a `None`, el yaw volvió aproximadamente a 0° como el del jugador y el pitch permaneció en −15°. Al restaurar Health del rival desplazado volvió la selección y el yaw +27,69°.
- Se restauraron vida y posición temporales y se detuvo PIE. La posición del actor del mapa fuente se confirmó en `(200, -280, 88)`, igual a la observada al iniciar. El mapa terminó con `is_dirty = true` y no se guardó por Codex. La revisión posterior del mapa forma parte del cierre de 0 confirmado por el usuario; no se consultó de nuevo su estado de guardado.
- LogPlayLevel no devolvió errores/warnings en la consulta. La verificación fue por referencias y transformaciones, no una aprobación visual del encuadre, colisión o movimiento con teclado.

La [guía](editor.md) conserva los pasos de compilación, revisión del mapa y observación de F, WASD y seguimiento, cerrados por el usuario junto con contacto/daño. El cambio de target con la defensa rediseñada se comprueba en la etapa 1; los planos de combate se implementan y verifican en 5.
