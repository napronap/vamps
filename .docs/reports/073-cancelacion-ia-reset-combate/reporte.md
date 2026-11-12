# 073 — Cancelación de acciones de IA y reset de combate

## Cambio de conducta

`AVampsAIController` llama a `AVampsCharacter::CancelCombatAction()` al cambiar de modo o al pasar a otra conducta en `Mixed`. La llamada corta el ataque y desarma la solicitud de defensa. Conserva la reacción activa, incluido stun o guardia, la carga de guardia, la energía de movilidad y el plazo de `NARecovery` que ya estuviera en curso. Los impactos capturados siguen en el resolver. Esto reemplaza el reset de IA descrito en la [iteración 072](../072-revision-codigo/reporte.md).

## Reset y resolución de combate

`AVampsCharacter` retira `CombatGeneration`. `ResetCombatState()` y `EndPlay()` llaman a `UVampsCombatResolver::DiscardPendingCombatFor()` para retirar del resolver los contactos entrantes y salientes, los registros de ataque y los callbacks pendientes de ese pawn. En las rutas nativas los resets ocurren fuera del lote local de contactos. El resolver conserva el orden de ataque, las comprobaciones de validez y las del paso para cerrar Recovery o continuar un combo. Un atacante que muere durante el lote todavía puede aplicar un contacto ya aceptado.

`AVampsPlayerCameraManager` se suscribe a `OnCombatReset` del pawn observado para limpiar el encuadre y restaurar las oclusiones al resetearlo o al salir de juego. La cámara sigue el montage que originó el encuadre hasta que deja de ser vigente.

La [iteración 074](../074-legibilidad-resolver-combate/reporte.md) amplía los comentarios del resolver y documenta el alcance de la limpieza.

## Archivos y assets

- `AI/VampsAIController.h` y `.cpp` cambian el manejo de la transición entre conductas.
- `Characters/VampsCharacter.h` y `.cpp` separan la cancelación de acción y el reset completo.
- `Combat/VampsCombatResolver.h` y `.cpp` retiran la generación de los contactos y limpian las colas por pawn.
- `Player/VampsPlayerCameraManager.h` y `.cpp` enlazan el evento nativo de reset y retiran los contadores de cámara.

`/Game/Vamps/Blueprints/BP_VampsAIController` hereda el cambio de `AVampsAIController` en el modo de entrenamiento. En `/Game/Vamps/Maps/L_LocalMatch`, el GameMode usa el controlador nativo. La cámara y las habilidades conservan sus asignaciones. El cambio se limita a C++ y los entregables de esta iteración.

## Verificación

La revisión de código confirmó las rutas de cancelación, reset y suscripción de cámara. La búsqueda en `Source/vamps` quedó sin referencias a `CombatGeneration` ni a sus copias. `git diff --check` pasó. Las herramientas MCP de Unreal no estaban disponibles en la sesión. Quedan la compilación y las comprobaciones en PIE de la [guía del editor](editor.md).
