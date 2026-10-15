# 014 — HUD, efecto y tags Vamps

El usuario ya confirmó la consolidación y PIE de esta migración. Para la eliminación posterior de los originales, seguir [015 — Finalizar la retirada de Duel](../015-retirada-duel/editor.md).

**Queda para el usuario:** consolidar las dos clases nativas nuevas con Unreal cerrado antes de volver a abrir o empaquetar. En la sesión actual, el parche está aplicado, los assets están guardados, Passive está restaurado y PIE está detenido. La comprobación visual, inputs y daño por contacto siguen siendo manuales.

[Reporte](reporte.md) · [Configuración de personajes y animación](../013-recuperacion-blueprints/editor.md).

## 1. Incorporar código nuevo

**Hecho — Codex por MCP.** LiveCodingToolset.LiveCodingToolset.CompileLiveCoding devolvió Success y aplicó patch_0: VampsCombatTestHUD, GE_VampsDamage y VampsCombatTags están disponibles en la sesión. No es una compilación NoChanges.

**Pendiente — usuario.** Con PIE detenido, cerrar Unreal. Compilar `vampsEditor / Development Editor / Win64` para `C:\dev\vamps\vamps.uproject` desde Rider/Visual Studio y reabrir después del éxito. AGENTS.md exige dejar al usuario compilaciones/reinicios sin MCP directo; Live Coding no sustituye esta consolidación de tipos nuevos. No eliminar las clases anteriores en este paso.

## 2. Asignar HUD

**Hecho — Codex por MCP.** ObjectTools.set_properties/get_properties, BlueprintTools.compile_blueprint y AssetTools.save_assets/is_dirty.

Abrir `/Game/Vamps/Blueprints/BP_VampsCombatTestGameMode` → Class Defaults → HUD Class = VampsCombatTestHUD. Initial AI Mode = Passive. Compilar y guardar. El GameMode normal BP_VampsGameMode mantiene HUD nativo; no necesita el HUD de combate.

No crear Class Redirects ni reparentar el HUD anterior: la nueva clase deriva de AHUD.

## 3. Tags y efecto

**Hecho — Codex por MCP y edición de código.** GameplayTagsToolset.ListTags confirmó Vamps.State.Attack, Vamps.State.Guard y Vamps.Data.Damage registrados desde código nativo. No agregar duplicados en un INI ni usar RenameTag.

GE_VampsDamage es un efecto nativo instantáneo que modifica Health mediante SetByCaller Vamps.Data.Damage. VampsCharacter crea el spec directamente; no hace falta crear un Blueprint del efecto ni asignarlo al personaje. GA_Attack y GA_Guard usan los tags nuevos desde sus constructores.

## 4. Animaciones y demás assets

**Hecho — usuario.** Las animaciones ya están en `/Game/Vamps/Animations` con nombres AS_BasicAttack1/2/3.

**Hecho — Codex por MCP.** Verificar AM_BasicAttack → DefaultSlot → AS_BasicAttack1, segmento de 0 a 1 s y Rate/Loop Count = 1. No se creó AS_VampsAttack1 porque el recurso ya tenía nomenclatura nueva. ABP_VampsCharacter, BS_VampsCharacter y las asignaciones de los personajes conservan la configuración de 013.

Compilados y guardados los siete Blueprints de Vamps y el AnimBP. Ninguna dependencia de assets Vamps consultada apunta a `/Game/Duel` o AS_Duel. No se realizó limpieza global de redirectors ni se abrieron assets históricos.

## 5. Verificación

**Hecho — Codex por MCP.** EditorAppToolset inició PIE con Guard y Attack iniciales temporales. SceneTools confirmó el HUD nuevo; AbilitySystemInspectorToolset confirmó Vamps.State.Guard y Vamps.State.Attack junto con las habilidades activas. ObjectTools verificó el efecto y LogsToolset no encontró errores/warnings de LogPlayLevel. Se detuvo PIE y se restauró/guardó Passive.

**Pendiente — usuario.** Después de consolidar y reabrir: Play en L_CharacterBasic, observar los paneles del HUD y comprobar LMB/RMB, F6 y R. Si los ataques no quitan vida, revisar Attack Trace únicamente en AM_BasicAttack, como indica la guía de 013. El MCP no permite revisar/editar esos notifies. No se da por comprobado daño o bloqueo por contacto.
