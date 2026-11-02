# 062 — Configuración por personaje

**Queda para el usuario:** crear/asignar los montages y armas de los tres DAs nuevos y ajustar sus parámetros. El renombrado de Executioner, los DAs y la comprobación de GameModes ya están hechos y guardados. Los nuevos perfiles todavía no se usan en los GameModes ni en el selector. Los marcadores `End Recovery` de Executioner siguen pendientes según [061](../061-fin-recovery-notify/editor.md).

## 1. Preservar la configuración

**Hecho — Codex por MCP.** Con PIE detenido se buscaron `VampsCombatSettings` y sus derivadas mediante `ObjectTools.search_subclasses` y `AssetTools.find_assets`. Solo existía `/Game/Vamps/Data/DA_VampsCombatSettings`. Se leyeron todas sus propiedades con `ObjectTools.list_properties/get_properties`, incluidos arma, socket, transform, montages, arrays y escalares; se guardó [antes.json](antes.json) y el asset con `AssetTools.save_assets`.

Para reproducir la revisión, abrir el DA desde Content Browser y comprobar sus grupos Combat, Reaction, Death, Damage, Guard, Mobility y Approach. La captura contiene los valores anteriores al renombrado.

## 2. Renombrar Executioner y conservar los modos actuales

**Hecho — Codex por MCP.** `AssetTools.move` renombró `/Game/Vamps/Data/DA_VampsCombatSettings` a `/Game/Vamps/Data/DA_ExecutionerCombatSettings`. En Class Defaults de `BP_VampsCharacter` y `BP_VampsCharacter_Player`, `Combat Settings` apunta ahora al nuevo asset. Se guardaron el DA y ambos Blueprints; sus dependencias guardadas confirman la nueva ruta y la lectura completa del DA coincide con la captura previa.

Reproducción manual: renombrar el DA en Content Browser, revisar `Combat Settings` en ambos Blueprints y guardar los assets afectados. Conservar los montages, `BP_VampsGreatSword`, `HandGrip_R`, el transform y los valores originales.

## 3. Crear los perfiles nuevos

**Hecho — Codex por MCP.** `DataAssetTools.create` creó en `/Game/Vamps/Data/` tres assets de clase `VampsCombatSettings`:

- `DA_DualSwordsCombatSettings`.
- `DA_SwordAndShieldCombatSettings`.
- `DA_LancerCombatSettings`.

Reproducción manual: Content Browser → `/Game/Vamps/Data` → Miscellaneous → Data Asset → `VampsCombatSettings`. Crear los tres nombres, configurar tres entradas en `Combo Steps` y los valores de [valores-nuevos.json](valores-nuevos.json); el [reporte](reporte.md#parámetros-iniciales) resume sus diferencias. `ObjectTools.set_properties` aplicó los datos y `AssetTools.save_assets` confirmó su guardado. `Weapons` está vacío y Attack, Hit Reaction, Guard Reaction, Death, Guard Death, Forward Dash y Backward Dash Montage están sin asignar.

## 4. Comprobar GameModes y mapas

**Hecho — Codex por MCP.** Se consultaron los Class Defaults y los World Settings de ambos mapas; se comprobó el arranque mediante `EditorAppToolset.StartPIE`, se inspeccionaron los actores con `SceneTools.find_actors` y `ObjectTools.get_properties`, y se detuvo cada sesión con `StopPIE`.

1. `/Game/Vamps/Maps/L_CharacterBasic`: World Settings → GameMode Override = `BP_VampsCombatTestGameMode`. Default Pawn = `BP_VampsCharacter_Player`, Opponent = `BP_VampsCharacter`, Opponent Controller = `BP_VampsAIController`, modo inicial `Passive`. PIE creó dos personajes equipados con el DA de Executioner.
2. `/Game/Vamps/Maps/L_LocalMatch`: override `BP_VampsMatchGameMode`. Default Pawn = `BP_VampsCharacter_Player`, Bot Character = `BP_VampsCharacter`. PIE abrió selección con cuatro participantes y dos equipos, todos con el DA de Executioner.
3. En Class Defaults de `BP_VampsGameMode`, se conservaron el pawn jugador, `BP_VampsPlayerController` y HUD nativo. Su comprobación fue de configuración.
4. Project Settings → Maps & Modes conserva `L_LocalMatch` como Game Default Map/Editor Startup Map y `BP_VampsMatchGameMode` como Default GameMode. No se modificaron los mapas ni `DefaultEngine.ini`.

Ambas sesiones quedaron detenidas y se volvió a `L_LocalMatch`. No se evaluó el ataque ni se activaron los perfiles nuevos. No se necesitó recompilar C++ ni reiniciar Unreal.

## 5. Completar los perfiles cuando estén listos sus recursos

**Pendiente — usuario.** En cada DA nuevo:

1. Asignar su `Attack Montage` con tres secciones ordenadas; configurar `Attack Trace`, `Combo Window` y `End Recovery` según el contrato de [061](../061-fin-recovery-notify/editor.md#3-colocar-los-marcadores). Ajustar `Combo Steps` y NA a las animaciones.
2. Asignar montages propios de Hit/Guard, Death/Guard Death y dash delantero/trasero. Death y Guard Death deben ser distintos, con Auto Blend Out desactivado; el resto conserva las validaciones existentes del juego.
3. Configurar `Weapons` con clases concretas derivadas de `VampsWeapon`, sockets y transforms del personaje: las dos espadas, espada/escudo y lanza según las trazas elegidas. Las clases de esas armas no se crearon en esta entrega.
4. Guardar cada DA. Asignarlo al Blueprint del personaje correspondiente cuando se prepare ese personaje; mantener los Blueprints de Executioner usados por los GameModes actuales con `DA_ExecutionerCombatSettings`.

Estos pasos completan los recursos que el usuario decidió preparar después. No requieren cambiar los GameModes para conservar los modos actuales de Executioner.
