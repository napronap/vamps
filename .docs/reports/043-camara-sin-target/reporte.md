# 043 — Cámara sin target y modo de observación

Configuración sustituida por [044 — Seguimiento libre suave y F7](../044-seguimiento-libre-f7/reporte.md): acompañamiento amortiguado de viaje y alternancia con F7. Los comandos siguientes son históricos.

## Alcance — 2026-10-20

Corrige el giro de cámara sin selección señalado por el usuario después de [042](../042-camara-combate/reporte.md). Añade un modo de desarrollo para deshabilitar el lock-on del jugador manteniendo enemigos/IA. El HUD y los assets permanecen sin cambios; no se modifican Data Assets ni estructuras de sus propiedades.

## Causa y solución

042 usaba el yaw del cuerpo cuando no había dirección hacia un enemigo. bOrientRotationToMovement gira el personaje hacia WASD, mientras WASD se transforma con el yaw de cámara publicado en ControlRotation. Ese seguimiento corporal cerraba una realimentación: ir de costado hacía girar también la vista y su referencia de movimiento. Reducir solo la ganancia habría conservado el acoplamiento.

El manager ahora conserva UnlockedHeadingYaw. Al perder selección captura el yaw de la vista anterior, separando el offset de composición de combate; si arranca sin target, toma el yaw inicial del controller. Mientras no haya selección conserva esa dirección y construye un sujeto virtual a FollowDistance por delante del jugador. El punto acompaña su traslación: no fija una órbita alrededor de una coordenada del mapa, no crea un actor ni representa un enemigo lógico.

La misma ruta calcula referencia angular, composición, Eye/Interest, filtro y transición con sujeto real o virtual. El giro corporal no actualiza la dirección virtual. Los encuadres de ataque y seguimiento conservan su offset propio sin sumarlo repetidamente al rumbo; readquisición vuelve al rival real con transición. La pérdida de un target destruido también se detecta cuando su weak pointer ya es inválido, antes de limpiar la identidad observada. Con target y posición horizontal coincidente se conserva la referencia orbital en lugar de usar el frente corporal.

La oclusión sigue protegiendo jugador y rival real; sin rival protege solo al jugador, no el punto virtual. Se conserva la publicación de yaw para Move/MobilityPressed, bOrientRotationToMovement, captura de Shift y root motion. Reset/cambio de pawn reinicializan la referencia con el flujo existente.

## Modo de desarrollo

`vamps.Debug.DisablePlayerLockOn 1` deshabilita la selección de personajes controlados por el jugador local. Valor inicial=false; `0` restaura la selección automática. Registro no Shipping, con ECVF_Cheat.

IsEligible rechaza candidatos del jugador durante el modo; CycleTarget y SelectTarget comparten esa comprobación, por lo que F y guardia no fuerzan una selección mientras esté activo. GetCurrentTarget también comprueba elegibilidad para que cámara y HUD vean ausencia inmediatamente. La lista de candidatos se conserva y el siguiente tick readquiere un rival al desactivar el modo. Los componentes de IA no se deshabilitan.

Sin selección real, approach/retirada dependientes del target conservan sus condiciones existentes; el punto virtual no los habilita. La evasión sin dirección sigue requiriendo target real. Para probar movilidad sin lock, usar Shift con WASD. El modo no evita daño ni altera energía o permisos de combate.

## Archivos y conexiones

- Source/vamps/Public/Player/VampsPlayerCameraManager.h: añade únicamente el rumbo virtual persistente, no editable y sin datos por personaje.
- Source/vamps/Private/Player/VampsPlayerCameraManager.cpp: reemplaza el seguimiento del cuerpo sin target, reutiliza la geometría de encuadre y detecta correctamente la pérdida de referencias destruidas.
- Source/vamps/Private/Characters/VampsLockOnComponent.cpp: CVar de desarrollo y elegibilidad compartida; conserva registro/lista/selección propios.
- No se modifican Blueprints, input mappings, montages, materiales, mapa, HUD, DA ni atributos. BP_VampsPlayerCameraManager y ambos controllers usan las mismas clases/referencias de 042.

## Referencia FGOA

Se releyeron las secciones de seguimiento sin dirección/foco de la [investigación](<D:/fgo arcade/fgoac/research/camera-20261019-1544/INFORME_CAMARA.md>) y la [adaptación](<D:/fgo arcade/fgoac/research/camera-20261019-1544/ESPECIFICACION_VAMPS.md>). El nativo conserva dirección/foco y aplica condiciones por acción, margen angular y una respuesta libre distinta; no demuestra seguir el yaw corporal completo en todo momento. C05/C06 tampoco midieron el target lógico runtime. El sujeto virtual trasladable es la adaptación solicitada para Vamps, no una afirmación de haber reconstruido exactamente ese caso nativo.

## Verificaciones y pendientes

- Revisión de pérdida/adquisición, offset de combate, target destruido/coincidente y reset. clang-format aplicado a los tres fuentes sin comentarios nuevos de namespace; git diff --check sin errores de espacios.
- LiveCodingToolset.CompileLiveCoding aplicó un parche [Success](live-coding.json), no NoChanges; [UBT Succeeded](build.log), sin errores ni warnings de compilación en la consulta. No se ejecutó build completo ni reinicio.
- EditorAppToolset.SearchCVars confirmó el modo registrado e inicialmente false. PIE arrancó con el manager propio, seguimiento normal y rival seleccionado; Eye=(-820,0,160,15), Interest=(-220,0,120,15), sin errores/warnings de LogVampsCombat en la consulta. [Captura de estado](pie-estado.json). Esto confirma arranque normal, no la activación del modo ni su respuesta a WASD.
- PIE detenido y confirmado IsPIERunning=false. No se editaron/guardaron assets ni se simularon inputs para suplir la comprobación manual.
- El modo se activa manualmente desde la consola del juego: no se descubrió una herramienta directa para escribir CVars. No se usa UI automatizada ni un atajo para activar esa operación. La [guía](editor.md) detalla comandos y observación.
- No se agregan ni ejecutan tests automatizados. No se atribuye comprobación visual hasta observarla.
