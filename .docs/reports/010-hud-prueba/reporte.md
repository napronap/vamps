# 010 — HUD de prueba para los personajes nuevos

Configuración vigente: [011 — IA y GameMode de prueba](../011-ia-combate-prueba/reporte.md). El HUD pasa exclusivamente a BP_VampsCombatTestGameMode y añade modo de IA y F6/R; BP_VampsGameMode normal vuelve a HUD nativo.

## Alcance y resultado

Se adapta el HUD propio del duelo a VampsCharacter y se asigna al GameMode de L_CharacterBasic. Muestra vida, barra de salud y estado IDLE/ATTACK/GUARD del jugador a la izquierda y de los demás personajes de la escena a la derecha, identificados por su nombre de actor. Conserva la ayuda de WASD, mouse, LMB y RMB.

Live Coding compiló y aplicó un parche real. En PIE se confirmó que DuelHUD se crea para BP_VampsPlayerController y tiene bShowHUD habilitado. La comprobación visual y los cambios de estado durante interacción quedan pendientes según la [guía](editor.md): la herramienta de captura devolvió `Failed to capture any editor windows.`

## Archivos, clases y assets

| Archivo / asset | Cambio y motivo |
|---|---|
| `Source/vamps/Public/UI/DuelHUD.h` | DrawFighter recibe VampsCharacter y una posición Y para distribuir los paneles de depuración. Se conserva el nombre reflejado DuelHUD y la herencia directa de AHUD. |
| `Source/vamps/Private/UI/DuelHUD.cpp` | Lee Health/MaxHealth del AttributeSet existente y los estados desde los tags consultados por IsAttacking/IsGuarding. Descubre los demás VampsCharacter mediante TActorIterator, porque la escena nueva no tiene un GameMode de duelo ni una referencia Opponent. Retira dependencias de DuelGameMode, DuelAIController y VampsDuelCharacter, el modo IA y los controles anteriores. |
| `/Game/Vamps/Blueprints/BP_VampsGameMode` | HUD Class cambia de HUD nativo a `/Script/vamps.DuelHUD`. Compilado y guardado por MCP. Default Pawn Class y Player Controller Class siguen apuntando a los Blueprints propios de la iteración 009. |

El HUD es de depuración: consulta la escena al dibujar, sin cachear actores ni agregar bindings de atributos, setters o getters al personaje. Usa los datos actuales de GAS; no conserva un estado de combate ni una copia de la vida. VampsAttributeSet ya garantiza MaxHealth mayor o igual a 1. No se agrega un widget UMG, una clase nueva ni compatibilidad con el personaje anterior.

No se modifican personajes, habilidades, inputs, montage, socket, mapa ni Config. La configuración del HUD nativo documentada en 009 queda reemplazada por esta iteración; los pendientes del notify y las comprobaciones del combate siguen vigentes. Se agregan el reporte y la guía, con enlaces desde el índice y la iteración anterior.

## Verificación

- clang-format aplicado a los dos archivos C++ modificados y `git diff --check` sin errores de whitespace.
- `LiveCodingToolset.LiveCodingToolset.CompileLiveCoding` devolvió `Result: Success`, generó `UnrealEditor-vamps.patch_2.lib` y finalizó con `Live coding succeeded`. Fue un parche con cambios, no NoChanges.
- ObjectTools confirmó HUD Class = DuelHUD y preservación del personaje/controller en BP_VampsGameMode. BlueprintTools.compile_blueprint finalizó correctamente; AssetTools.save_assets devolvió true e is_dirty devolvió false.
- EditorAppToolset.StartPIE inició L_CharacterBasic. SceneTools encontró DuelHUD_0; ObjectTools confirmó PlayerOwner = BP_VampsPlayerController_C_0 y bShowHUD = true. GAS informó Health/MaxHealth = 100/100 y sin tags activos en jugador y receptor, correspondientes al estado inicial IDLE.
- LogsToolset no devolvió errores ni warnings de LogPlayLevel. StopPIE terminó la sesión e IsPIERunning confirmó false. El editor quedó en L_CharacterBasic, con PIE detenido.
- CaptureEditorImage falló; no se verificó visualmente el render ni se entrega una captura. No se simularon inputs ni se ejecutaron tests automatizados.

Pendiente: observar los paneles y los cambios de vida/estado mediante interacción manual. No se verificaron daño ni bloqueo en esta iteración.
