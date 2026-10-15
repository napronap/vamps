# 011 — IA nueva y GameMode de prueba de combate

Continuación: [012 — AnimInstance y AnimBP Vamps](../012-animacion-vamps/reporte.md), migración de animación pendiente de guardado. La configuración de combate de esta iteración permanece vigente.

## Alcance y resultado

Se crean VampsAIController, VampsCombatTestGameMode y VampsCombatTestPlayerController. L_CharacterBasic ahora inicia el modo de prueba, que crea al rival, configura su IA y asigna el HUD de depuración. F6 rota Passive → Attack → Guard → Sidestep → Mixed → Passive; R reinicia vida, posiciones y acciones, conservando el modo seleccionado. F1 no forma parte del contexto nuevo.

La IA solo depende de VampsCharacter, GAS y las APIs nativas de foco/movimiento. Personajes y controller normal no reciben funciones ni referencias al GameMode de prueba. BP_VampsGameMode vuelve a HUD nativo y mantiene VampsPlayerController con sus cuatro entradas. En PIE se comprobaron por separado la escena de prueba y el modo normal.

La compilación por MCP aplicó un parche real. Assets y configuración quedaron guardados, con Passive restaurado y PIE detenido. La activación de ataque y guardia y el movimiento de IA se observaron; el daño/bloqueo y los botones F6/R siguen pendientes de comprobación manual según la [guía](editor.md).

## Código y responsabilidades

Rutas relativas a Source/vamps, con headers en Public e implementaciones en Private.

| Clase / archivos | Responsabilidad y conexiones |
|---|---|
| `AI/VampsAIController.h/.cpp` — nuevos | Hereda de AAIController y controla VampsCharacter. EVampsAIMode define los cinco modos. SetMode cancela acciones y reinicia las decisiones; el objetivo se asigna con SetFocus nativo. No consulta ningún GameMode, HUD, player controller ni clase del duelo anterior. |
| `GameModes/VampsCombatTestGameMode.h/.cpp` — nuevos | Hereda de GameModeBase. Configura clases del rival/controller, modo inicial y posiciones de ambos personajes. StartPlay crea y posee al rival; ResetCombat limpia habilidades/trazas, restaura vida y posición y entrega el jugador como foco de la IA. CycleAIMode pertenece exclusivamente a este GameMode. |
| `Player/VampsCombatTestPlayerController.h/.cpp` — nuevos | Hereda del controller propio VampsPlayerController. Añade solo las acciones CycleAIMode y ResetCombat y comunica esas solicitudes al GameMode de prueba. Reutiliza el registro/limpieza del Mapping Context y los cuatro bindings de juego de su padre. |
| `UI/DuelHUD.cpp` — modificado | Conserva los paneles de vida/estado de 010 y añade modo de IA, F6 y R. Consulta VampsCombatTestGameMode y VampsAIController; su uso queda restringido al GameMode de prueba. Se conserva el nombre reflejado DuelHUD, sin duplicar el HUD ni agregar un redirect. |

Las decisiones de IA usan un timer de 0,1 s, con una espera inicial de 0,75 s después de elegir modo o reiniciar. Passive no programa decisiones. Attack se acerca hasta AttackDistance y solicita ataques separados por AttackInterval. Guard mantiene la habilidad de guardia. Sidestep entrega movimiento tangencial al foco. Mixed alterna Attack/Guard/Sidestep cada MixedModeDuration segundos.

Tick entrega movimiento y permite que AAIController actualice ControlRotation hacia el foco. Al poseer, la IA habilita la rotación deseada por controller y deshabilita la orientación al movimiento; al dejar el pawn restaura ambos valores anteriores. No se añade Tick, objetivo ni lock-on al personaje. Pérdida de foco detiene acciones; pérdida de posesión y EndPlay limpian el timer. No se agrega navegación, Behavior Tree, State Tree ni reglas de muerte.

Para usar esta IA en otro GameMode: poseer un VampsCharacter, asignar el foco con SetFocus y elegir comportamiento con SetMode después de la posesión, o mediante sus defaults de Blueprint. No necesita el modo de prueba. Los personajes continúan exponiendo únicamente las acciones de combate existentes.

## Assets y configuración

Se agregan seis assets:

| Asset | Configuración |
|---|---|
| `/Game/Vamps/Blueprints/BP_VampsAIController` | Padre VampsAIController. AttackDistance = 100 cm, AttackInterval = 1 s, MixedModeDuration = 4 s. |
| `/Game/Vamps/Blueprints/BP_VampsCombatTestPlayerController` | Padre VampsCombatTestPlayerController. Usa IMC_VampsCombatTest, las cuatro acciones normales y las dos acciones de depuración. |
| `/Game/Vamps/Blueprints/BP_VampsCombatTestGameMode` | Padre VampsCombatTestGameMode. Jugador BP_VampsCharacter_Player, controller de prueba, rival BP_VampsCharacter, IA BP_VampsAIController, HUD DuelHUD e InitialAIMode = Passive. |
| `/Game/Vamps/Input/IA_CycleAIMode` | Boolean, sin triggers adicionales. |
| `/Game/Vamps/Input/IA_ResetCombat` | Boolean, sin triggers adicionales. |
| `/Game/Vamps/Input/IMC_VampsCombatTest` | Copia del contexto propio IMC_Vamps con modificadores instanciados propios; añade F6 y R. Conserva las referencias a las cuatro acciones de juego. |

Se modifican `/Game/Vamps/Maps/L_CharacterBasic` y `/Game/Vamps/Blueprints/BP_VampsGameMode`. El mapa usa BP_VampsCombatTestGameMode y se retira VampsTarget colocado: el rival lo crea el GameMode, evitando duplicarlo. PlayerSpawn = (-220, 0, 100), Yaw 0°; OpponentSpawn = (220, 0, 100), Yaw 180°. El modo normal conserva su personaje/controller y usa HUD nativo.

ConfigSettingsToolset actualiza Config/DefaultEngine.ini: GameDefaultMap y EditorStartupMap pasan a L_CharacterBasic; GlobalDefaultGameMode pasa a BP_VampsGameMode normal. El override del mapa activa la prueba de combate. No se agregan redirects. L_Duel y sus clases antiguas permanecen como referencia, sin participar en los defaults ni en el nuevo circuito. Mesh, montage, socket, atributos y habilidades no se modifican.

Se agregan este reporte y editor.md, con enlaces desde el índice y las iteraciones 009/010. Se conserva el trabajo previo de 010 que ya estaba sin commit al comenzar. No se modifican especificaciones funcionales/técnicas ni AGENTS.md.

## Verificación y límites

- clang-format aplicado a los siete archivos C++ afectados en esta etapa; revisión de responsabilidades, transiciones, timers, limpieza de acciones, posesión y reinicio. `git diff --check` sin errores de whitespace.
- BaseInput.ini de UE 5.8 confirma F1 → wireframe y no contiene un DebugExecBinding predeterminado para F6. El contexto normal no recibe F6 ni R.
- CompileLiveCoding devolvió Success y generó UnrealEditor-vamps.patch_3.lib; no fue NoChanges. Las tres clases nuevas se consultaron en el editor. La herramienta advirtió que los cambios de tipos pueden causar problemas de empaquetado; consolidar las nuevas clases con una compilación completa con el editor cerrado antes de reiniciar/empaquetar, según la guía.
- Los cuatro Blueprints afectados compilaron; padres, clases, acciones, mappings, parámetros y defaults se consultaron después de configurarlos. Guardado selectivo devolvió true e is_dirty = false en los ocho assets afectados.
- PIE con Passive creó exactamente jugador, rival, IA y controller de prueba, con el HUD habilitado. Ambos personajes comenzaron con 100/100 y sus dos habilidades inactivas.
- Guard activó GA_Guard y Duel.State.Guard en el rival. Attack acercó al rival desde X = 220 hasta aproximadamente -151, dentro de la distancia configurada, y activó GA_Attack/Duel.State.Attack. En esa inspección el jugador seguía con 100 de vida: no se confirmó contacto/daño.
- Sidestep cambió posición y orientación del rival sin activar ataque/guardia. Mixed se mantuvo seleccionado y produjo desplazamiento durante su alternancia; no se observó cada transición completa del ciclo.
- Una sesión con el GameMode normal encontró un único jugador, VampsPlayerController con IMC_Vamps y HUD nativo, sin VampsAIController, controller de prueba ni DuelHUD. Luego se restauró el override de prueba y Passive.
- LogPlayLevel no devolvió errores ni warnings en las sesiones inspeccionadas. Todas terminaron mediante StopPIE y se confirmó IsPIERunning = false. No se ejecutaron tests automatizados, se simularon inputs ni se agregó un arnés.

Pendiente: interacción con F6/R, ciclo completo de Mixed, daño/bloqueo y ajuste visual del notify Attack Trace existente. El acceso a ese notify y la entrega de inputs de juego no están cubiertos por el MCP disponible; los pasos precisos quedan en [editor.md](editor.md).
