# 072 — Revisión de código

## Controlador de IA

- `AVampsAIController::OnPossess` usa el `VampsCharacter` válido que le entregan los GameModes y orienta al bot según el foco del controller. Se retira el respaldo de los flags de rotación porque el respawn mantiene la posesión y el cierre de ronda destruye el pawn al desposeerlo. `SetMode` arma el timer de decisión solo con un `AVampsCharacter` poseído. `OnUnPossess` cancela el timer y consume el input pendiente. El reset de combate queda en `AVampsCharacter::UnPossessed`.
- En partida, el controlador toma el objetivo elegible de `UVampsLockOnComponent`. El componente selecciona otro objetivo en su tick cuando corresponde. En el mapa de prueba, el controlador usa el foco que le asigna `AVampsCombatTestGameMode`. Si falta objetivo o el bot está muerto, el controlador espera. La comprobación de permisos de movimiento y acción queda en el personaje.
- `AVampsCharacter::CanRequestCombatContinuation` centraliza el permiso, la ventana y la existencia de un paso siguiente. La IA consulta esa función y conserva sus decisiones propias de esperar un contacto, evitar solicitudes duplicadas, acercarse y espaciar ataques. `UGA_Attack` valida la configuración del montage. Se retiró el `default` del `switch` y el log de configuración de `SetMode`. Se conserva la guarda de tiempos y el reset al cambiar de comportamiento en `Mixed`.

`/Game/Vamps/Blueprints/BP_VampsAIController` conserva su padre nativo y sus defaults. `BP_VampsCombatTestGameMode` selecciona el controller del rival en `L_CharacterBasic`. `AVampsMatchGameMode` instancia el controller nativo para los bots de `L_LocalMatch`. El cambio se limita a C++.

## Verificación

Se revisaron las rutas de posesión, destrucción, foco, lock-on y solicitud de ataque. Se aplicó `clang-format` y pasó `git diff --check`. Quedan la compilación y la comprobación en Unreal, descritas en la [guía del editor](editor.md).
