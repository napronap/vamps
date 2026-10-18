# 025 — Validación del montage sin cerrar el editor

La revisión completa vigente del paso 2 está en [026 — Diagnósticos y cancelación controlada](../026-diagnosticos-paso-2/reporte.md), con su [guía](../026-diagnosticos-paso-2/editor.md). Amplía esta corrección a las rutas propias y sustituye LogVampsAttack por LogVampsCombat.

## Resultado — 2026-10-17

El usuario encontró un cierre al intentar atacar después de preparar su montage. El log confirmó el checkf de UGA_Attack que exige un ANS_AttackTrace dentro de cada sección, con preparación y recuperación a sus lados. Ese mensaje agrupaba ausencia de notify y errores de tiempos, sin identificar la sección.

Se reemplazaron las validaciones fatales de configuración de esta habilidad por rechazo explícito del ataque y errores en LogVampsAttack. La configuración inválida no se omite ni se sustituye por otra. El usuario confirmó compilación completa y reapertura; MCP volvió a responder y la categoría nueva está registrada en el editor. Tras los intentos manuales, el log identificó la sección Default, de 0 a 1,933333 s, sin ANS_AttackTrace en el montage. Los ataques fueron rechazados y el editor siguió respondiendo por MCP.

Continúa [024 — Cierre de Combo Window](../024-cierre-ventana-combo/reporte.md). Se mantienen un montage, avance por orden y decisión al cierre de la ventana. La etapa 2 sigue abierta. No se modificaron assets, headers, tipos reflejados, especificaciones ni roadmap.

## Código y relaciones

- AGENTS.md: se reemplazó la regla ambigua sobre ifs y fallbacks por una sección explícita. Permite validaciones con if, prohíbe fallbacks no solicitados y asserts fatales, y exige diagnóstico con función y motivo antes de cancelar la operación. Distingue errores de las condiciones normales del juego. Esta aclaración posterior modifica instrucciones; no cambia código ni assets.
- Source/vamps/Private/Combat/Abilities/GA_Attack.cpp: ValidateAttackConfiguration conserva las condiciones del DA, armas equipadas, secciones, velocidad, blend-out, distancias y notifies. Devuelve el primer problema y ActivateAbility cancela antes de CommitAbility o de comenzar la reproducción. Los errores de notifies identifican montage, sección y tiempos; se distinguen ausencia y duplicación. Una reproducción que no obtiene su instancia también informa un error y cancela.
- Source/vamps/Private/Characters/VampsCharacter.cpp: RequestCombatAction solo registra ActionStarted si la activación dejó una instancia de montage válida. Un rechazo inmediato conserva LastRequest = Rejected; no inicia sección ni NA.
- Los asserts de contratos internos del personaje y el CastChecked del avatar no forman parte de esta sustitución. El alcance son los datos editables usados para iniciar esta habilidad.

## Assets y verificación

- El usuario compiló con build.bat y reabrió con editor.bat. La DLL resultante tiene fecha posterior a los archivos C++ modificados. No se ejecutó una compilación completa por terminal ni se atribuyó este resultado a Live Coding.
- ObjectTools confirmó que /Game/Vamps/Data/DA_VampsCombatSettings referencia /Game/Vamps/Animations/Executioner/AM_Executioner_Combo. NARecoveryDuration permanece en 0,5 s. La creación/configuración del montage corresponde al usuario; Codex no lo modificó ni guardó.
- LogsToolset confirmó que LogVampsAttack está registrada tras reabrir. Inicialmente no había entradas de esa categoría ni una sesión PIE activa.
- Tras los intentos manuales del usuario, LogsToolset.GetLogEntries registró rechazos de BP_VampsCharacter_Player_C_0 por ausencia de ANS_AttackTrace en Default [0,000000, 1,933333]. El editor continuó disponible y, al consultar, PIE ya no estaba activo. Se verificó el rechazo sin cierre; aún no el ataque válido ni las secciones posteriores, porque la validación devuelve el primer problema.
- La introspección MCP de AnimMontage no expone Notifies ni CompositeSections. No se usó automatización de interfaz ni un atajo para leer o modificar esas partes. La activación manual del ataque y la lectura posterior del diagnóstico quedan en la [guía](editor.md).
- Se revisaron las rutas de rechazo y limpieza, se aplicó clang-format a ambos archivos y git diff --check no informó errores de whitespace. No se crearon ni ejecutaron tests automatizados.

## Pendiente

Agregar ANS_AttackTrace en la pista Notifies del montage dentro de Default, respetando preparación y recuperación, y volver a intentar el ataque. Default es un nombre válido: renombrarlo no resuelve la ausencia del notify. Corregir los demás problemas que pueda señalar la siguiente activación y observar la cadena con las animaciones definitivas. El rechazo sin cierre está verificado; todavía no se certifica el ataque válido ni la cadena de este montage.
