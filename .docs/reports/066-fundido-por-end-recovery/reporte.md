# 066 — Fundido de ataque por End Recovery

Cuando termina una cadena, `End Recovery` inicia NA y cierra la habilidad para que Unreal aplique el `Blend Out` del montage. Si el jugador encadena el siguiente golpe, el resolver procesa la continuación antes del cierre. El montage de ataque requiere `Enable Auto Blend Out` desactivado.

## Código y conexiones

- `UGA_Attack` valida todo el montage antes de reproducirlo. Cada sección exige un `Attack Trace` y un `End Recovery`. `Combo Window` es obligatoria antes de la última sección y opcional en la última. Se conservan las comprobaciones de duplicados, orden y `Reset Weapon Hits`. `End Recovery` debe caer estrictamente después de los contactos y del cierre de la ventana, si existe, y antes del fin de la sección.
- `AVampsCharacter` detecta en el tick autoritativo una instancia de ataque que desapareció o quedó activa sin reproducirse. Consulta `UVampsCombatResolver` para distinguir la pausa de un `End Recovery` en cola para ese paso. Si falta el cierre, cancela la habilidad y la limpieza existente informa el fallo y devuelve el personaje a `Idle`. El fin de la sección por sí solo no inicia NA.
- `UAN_EndRecovery` sigue enviando el cierre al resolver. `ResolveCombatRecoveryEnd` inicia NA y cancela la habilidad de ataque. La tarea GAS aplica el fundido del montage al detenerlo.

Los cuatro perfiles usan montages referenciados desde `UVampsCombatSettings::AttackMontage`. La [guía del editor](editor.md) lista sus rutas y la configuración. `AM_BasicAttack` carece de referentes en el registro de assets. Los Blueprints siguen tomando el montage del Data Asset del personaje. La iteración cambia código nativo y la casilla de los montages de ataque.

## Verificación

- Se aplicó clang-format a los cuatro archivos C++ modificados. `git diff --check` pasó.
- `LiveCodingToolset.LiveCodingToolset.CompileLiveCoding` devolvió `Success` y enlazó `UnrealEditor-vamps.patch_0`. Se aplicó un parche real.
- `ObjectTools.set_properties` cambió `bEnableAutoBlendOut` a `false` en los cuatro montages. `AssetTools.save_assets` confirmó el guardado y la relectura dejó los cuatro sin cambios pendientes. `Blend Out` siguió en 0,25 s con curva Hermite Cubic. `Blend Out Trigger Time` y `Rate Scale` conservaron sus valores. Los montages de Executioner e Impaler ya tenían cambios locales al inicio de la iteración.
- Con los tiempos del log de Lancer, `1,173398 s` queda entre el cierre de `0,856087 s` y el fin de sección de `1,333333 s`. La regla nueva admite ese instante.
- MCP no expone las secciones y notifies para inspección directa. Se confirmó que PIE está detenido. La [guía](editor.md) deja la comprobación en juego del ataque aislado y encadenado al usuario.
