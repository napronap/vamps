# 026 — Diagnósticos y cancelación controlada del paso 2

La configuración vigente de tiempo adicional por golpe y preservación del DA está en [027 — Delay por golpe](../027-delay-por-golpe/reporte.md), con su [guía](../027-delay-por-golpe/editor.md). Esta iteración conserva la revisión general de diagnósticos.

## Resultado — 2026-10-17

Se revisó la cadena del paso 2 y sus conexiones de inicio, input, IA, animación, movimiento, armas, HUD y reinicio. Los asserts y casteos fatales del código propio se reemplazaron por validaciones explícitas con diagnóstico y cancelación de la operación afectada. No se agregaron assets, valores sustitutos, montages alternativos ni rutas de recuperación no solicitadas.

Los mensajes de código se agrupan en LogVampsCombat e identifican función, objeto o asset y causa. Los problemas de datos del HUD se muestran en rojo en pantalla. Las condiciones normales, como NA, muerte, bloqueo, ausencia de target, cliente sin autoridad, notifies viejos o limpieza durante teardown, conservan sus salidas normales.

La revisión, la propagación del error de habilitación de trazas y el ajuste final del rechazo de NA inválida compilaron por Live Coding con Success y parches reales. El arranque y la limpieza en PIE quedaron verificados sin errores en LogVampsCombat. Esta iteración continúa [025 — Validación del montage](../025-validacion-montage/reporte.md); la configuración jugable de la etapa 2 sigue abierta.

## Archivos y relaciones

Rutas relativas a Source/vamps/.

| Archivos | Cambio |
|---|---|
| Public/Combat/VampsCombatLog.h y Private/Combat/VampsCombatLog.cpp | Categoría compartida LogVampsCombat para filtrar los diagnósticos de las rutas revisadas |
| Public/Characters/VampsCharacter.h y Private/Characters/VampsCharacter.cpp | Validan equipamiento, sockets, instancia, fase, duración, NA, restricciones y daño. Una inicialización fallida destruye las armas parciales y no otorga la habilidad. BeginCombatAction y BeginCombatStep devuelven éxito; TryCalculateCombatSectionDuration solo entrega una duración válida. Una duración NA inválida cierra contactos y deja el combate indisponible hasta resetear, sin inventar un tiempo. Se elimina el clamp silencioso de daño negativo: se informa y rechaza |
| Private/Combat/Abilities/GA_Attack.cpp | Conserva la validación de secciones/notifies; agrega función y contexto al mensaje, valida las armas vivas, informa fallos del task y cancela si el personaje no puede comenzar la acción. Migra LogVampsAttack a la categoría compartida |
| Public/Combat/VampsWeapon.h y Private/Combat/VampsWeapon.cpp | Validan owner, mesh, marcadores, radio, escala y muestreo. Un arma inválida al crearse informa el problema y se destruye; una traza inválida se detiene. SetTracingEnabled devuelve éxito para que el personaje cancele el ataque si un arma rechaza la ventana |
| Private/Combat/VampsCombatMovementComponent.cpp | Valida receptor, autoridad, dirección, distancias y reacción antes del desplazamiento. Informa reproducción fallida sin sustituir el montage ni continuar con datos inválidos. Conserva la reacción opcional cuando no está asignada, según el alcance anterior |
| Private/Combat/VampsCombatResolver.cpp | Rechaza contactos/peticiones inválidos antes de encolarlos y obtiene la duración mediante la API de éxito/error. Un bloqueo sin atacante registrado se informa y sus contactos se descartan sin consumir defensa. Se mantiene el orden contactos → continuaciones → locomoción |
| Private/AI/VampsAIController.cpp | Valida pawn, modo, tiempos y montage. Detiene las decisiones inválidas con el motivo; conserva la lógica de solicitud y continuación |
| Private/Animation/ANS_AttackTrace.cpp y ANS_ComboWindow.cpp | Informan un payload sin mesh/owner válido; no dereferencian un mesh nulo ni ocultan un owner incorrecto |
| Private/Animation/VampsAnimInstance.cpp | Informa un pawn incompatible y cancela esa actualización; conserva el retorno normal de la preview sin pawn de juego |
| Private/UI/VampsCombatTestHUD.cpp | Reemplaza los casteos/asserts por validación de datos y mensajes visibles; evita acceder a un LockOn ausente después de rechazar el panel |
| Private/GameModes/VampsCombatTestGameMode.cpp | Valida clases, spawn y participantes antes de iniciar o reiniciar. Cancela la creación parcial del oponente si falla su controlador |
| Private/Player/VampsPlayerController.cpp y VampsCombatTestPlayerController.cpp | Validan mapping, subsistema, componentes, assets/tipos de input, pawn y GameMode; una operación inválida informa función y causa antes de regresar |
| Private/Player/VampsPlayerCameraManager.cpp y Private/Characters/VampsLockOnComponent.cpp | Sustituyen los asserts de las conexiones de cámara/selección usadas por el combate; las reglas de seguimiento y elección no cambian |

No se modificaron propiedades reflejadas, layouts de datos, Blueprints, montages, notifies ni otros .uasset. Las firmas nativas de éxito/error tienen actualizados sus llamadores. La revisión cubre los árboles propios Public y Private; no modifica código de variantes de ejemplo ni del motor.

## Verificación y pendientes

- Se revisaron los retornos y sus llamadores para evitar continuar con una duración inventada, una instancia inválida o un equipamiento parcialmente creado.
- Se aplicó clang-format a los archivos C++ modificados y se comprobó whitespace con git diff --check.
- Live Coding por MCP: revisión, propagación del rechazo de trazas y ajuste final del rechazo de NA inválida compilados con Success y parches reales. UHT no escribió archivos generados; no hubo cambios de propiedades reflejadas. No se necesitó compilación completa ni reinicio para aplicar esta revisión en el editor abierto.
- Búsqueda en Source/vamps/Public y Private: no quedan invocaciones a check/checkf, variantes, asserts ensure, CastChecked ni otras funciones Checked. El único texto coincidente de la búsqueda amplia es la palabra inglesa «check» en un mensaje de log. Tampoco hay logs Fatal.
- ObjectTools confirmó DA_VampsCombatSettings → AM_Executioner_Combo y un BP_VampsGreatSword en HandGrip_R, con transform identidad. No se cambiaron esas referencias ni assets.
- StartPIE/StopPIE en /Game/Vamps/Maps/L_CharacterBasic: dos personajes en Idle, ComboStep=0, CanStart=true y CanDealDamage=false; cada uno con una instancia diferente de BP_VampsGreatSword. get_class confirmó VampsCombatMovementComponent en ambos CharMoveComp. Después de detener PIE no quedaron actores VampsWeapon.
- LogVampsCombat está registrada y no tuvo entradas de error durante arranque, sesión y cierre observados. La consulta general solo devolvió el error de sesión MCP y los rechazos de montage anteriores a esta revisión; no errores nuevos del arranque comprobado.
- No se solicitaron ataques por MCP: no hay herramienta directa para ese input. La cadena con animaciones definitivas y las ramas de configuración inválida del resto de componentes no se marcaron como observadas. Pasos en la [guía](editor.md). No se agregaron ni ejecutaron tests automatizados.
