# 014 — Copias nativas con nomenclatura Vamps

Continuación: [015 — Retirada de Duel](../015-retirada-duel/reporte.md). El usuario confirmó compilación completa y PIE de esta migración y autorizó retirar los originales. La limpieza física residual se documenta allí.

## Resultado

Se crean clases y archivos nuevos para las piezas de Duel que todavía usaba el setup Vamps. Se actualizan sus consumidores y el GameMode de prueba. No se renombran clases, no se agregan redirects y no se borran las versiones anteriores. No se consultan ni modifican assets `/Game/Duel`.

## Código

Rutas relativas a Source/vamps, con headers Public e implementaciones Private:

| Archivos nuevos | Responsabilidad |
|---|---|
| UI/VampsCombatTestHUD.h/.cpp | AVampsCombatTestHUD hereda directamente de AHUD. Copia el HUD propio de vida/estado y modo de IA. Solo consulta VampsCharacter, VampsCombatTestGameMode y VampsAIController; no hereda de DuelHUD. |
| Combat/Effects/GE_VampsDamage.h/.cpp | UGE_VampsDamage hereda directamente de UGameplayEffect. Efecto instantáneo sobre Health, aditivo y SetByCaller con Vamps.Data.Damage. No hereda de GE_DuelDamage. |
| Combat/VampsCombatTags.h/.cpp | Namespace VampsTags y tres tags nativos nuevos: Vamps.State.Attack, Vamps.State.Guard, Vamps.Data.Damage. Las definiciones Duel anteriores permanecen separadas, sin alias ni redirects. |

Consumidores modificados:

- Characters/VampsCharacter.cpp consulta los tags nuevos y crea el efecto GE_VampsDamage al recibir un golpe.
- Combat/Abilities/GA_Attack.cpp y GA_Guard.cpp usan VampsTags para estado y exclusión mutua.
- GameModes/VampsCombatTestGameMode.cpp asigna VampsCombatTestHUD como clase nativa de HUD.

Los archivos DuelHUD, GE_DuelDamage y CombatTags originales quedan para el grupo histórico anterior. El código activo nuevo no los incluye ni usa su namespace. Se aplicó clang-format a los diez archivos propios agregados o modificados; git diff --check pasó.

## Assets

BP_VampsCombatTestGameMode ahora referencia `/Script/vamps.VampsCombatTestHUD`; el resto de sus defaults se conserva, con Passive inicial. BP_VampsGameMode normal conserva HUD nativo de Unreal.

Al consultar los recursos de animación, AS_DuelAttack1/2/3 ya habían sido sustituidos por el usuario por `/Game/Vamps/Animations/AS_BasicAttack1/2/3`. Se conservan esos nombres; Codex no creó otra copia ni renombró esos recursos. AM_BasicAttack ya apuntaba a AS_BasicAttack1 y se verificó su segmento 0–1 s, Rate/Loop Count = 1 y DefaultSlot. Las asignaciones de mesh, AnimBP y montage de los personajes permanecen como en 013.

No hubo cambios de configuración ni nuevos redirectors generados por Codex. Esto no elimina redirectors o recursos históricos preexistentes.

## Verificaciones y límites

- LiveCodingToolset.CompileLiveCoding devolvió Success, generó patch_0 e incorporó dos clases nuevas. No fue NoChanges.
- Los tres tags Vamps quedaron registrados. El CDO del efecto nuevo confirmó Instant, Health y SetByCaller Vamps.Data.Damage.
- PIE Guard: VampsCombatTestHUD creado, rival con GA_Guard activa y tag Vamps.State.Guard.
- PIE Attack: rival con GA_Attack activa y tag Vamps.State.Attack. No se observó pérdida de vida; el contacto/notify del montage sigue sin confirmar.
- AnimBP y siete Blueprints de Vamps compilados con warnings_as_errors = true sin errores ni warnings de compilación.
- Dependencias de assets Vamps sin referencias a `/Game/Duel` ni AS_Duel. Todos guardados; Passive restaurado y PIE detenido. LogPlayLevel sin errores/warnings en esas sesiones.
- No se ejecutaron tests automatizados ni inputs por UI.

**Pendiente — usuario:** consolidar las clases reflejadas nuevas con una compilación completa con Unreal cerrado antes de la próxima reapertura o empaquetado. El parche actual está aplicado, pero Live Coding informó el riesgo de referencias a tipos nuevos. No hay herramienta MCP directa para compilar con el editor cerrado. Pasos en la [guía](editor.md).

Esta iteración sustituye los nombres activos DuelHUD, GE_DuelDamage y DuelTags identificados en [013](../013-recuperacion-blueprints/reporte.md). La eliminación de clases y assets históricos no forma parte de este pedido.
