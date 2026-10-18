# 027 — Configurar el tiempo adicional por golpe

**Pendiente para el usuario:** ajustar, si lo desea, el adicional de cada golpe y observarlo durante el combate. La compilación completa, reapertura, preservación del DA y comprobación de arranque ya están hechas. El campo nuevo está guardado en 0 s en los tres golpes.

## 1. Preservar la configuración

**Hecho — Codex por MCP:** AssetTools.find_assets encontró /Game/Vamps/Data/DA_VampsCombatSettings. ObjectTools.list_properties/get_properties leyó sus propiedades completas y se guardó la copia durable [data-assets-antes.json](data-assets-antes.json). AssetTools.save_assets guardó únicamente ese DA antes de modificar código.

Valores a conservar: ComboSteps = 130/100, 130/100 y 150/250 cm; NARecoveryDuration = 0,5 s; AttackMontage = /Game/Vamps/Animations/Executioner/AM_Executioner_Combo; HitReactionMontage vacío; Weapons = una BP_VampsGreatSword en HandGrip_R con transform identidad.

## 2. Compilar y abrir

**Hecho — usuario:** ejecutó C:\dev\vamps\build.bat con Unreal cerrado y abrió con C:\dev\vamps\editor.bat, confirmando compilación correcta. El primer intento detectó un tooltip de UPROPERTY dividido en dos literales; se corrigió y el siguiente build terminó con Result: Succeeded. La compilación completa no tiene una herramienta MCP directa y no se automatizó por terminal ni interfaz.

El campo nuevo cambia el tamaño del struct contenido en el DA. Se usa arranque limpio para evitar las referencias transitorias y pérdidas de configuración que pueden producirse durante una recarga estructural en caliente.

## 3. Restaurar y verificar

**Hecho — Codex por MCP:** después de reabrir, ObjectTools.list_properties confirmó AdditionalOffensiveRestrictionDuration en ComboSteps y los tipos nativos. La comparación de todas las propiedades mostró los valores originales intactos y el campo nuevo en cero. ObjectTools.set_properties reaplicó las tres entradas con 0 s; AssetTools.save_assets guardó únicamente el DA afectado. La relectura coincidió exactamente con la copia previa más el campo nuevo, e is_dirty devolvió false. Resultado completo en [data-assets-despues.json](data-assets-despues.json).

**Hecho — Codex por MCP:** StartPIE/StopPIE confirmó los dos personajes en Idle con el DA preservado y un arma por personaje. LogVampsCombat quedó sin errores durante arranque/cierre y el DA continuó sin cambios pendientes.

No reasignar montages, armas ni sockets por suposición. Los valores anteriores están capturados y no deben sustituirse por los defaults de C++.

## 4. Ajustar el tiempo en el editor

**Pendiente — usuario:** abrir /Game/Vamps/Data/DA_VampsCombatSettings, expandir Combo Steps y ajustar Additional Offensive Restriction Duration en cada golpe. El valor está en segundos y no puede ser negativo. Por ejemplo, 0,2 agrega 0,2 s a la duración calculada de la sección, independientemente de su velocidad de reproducción.

La aplicación conserva el alcance actual: restricción ofensiva del receptor al bloquear. El campo no controla NA ni retrasa el comienzo del efecto. Para errores de configuración, filtrar LogVampsCombat en Output Log. Configurar secciones/notifies y observar la cadena siguen las guías [024](../024-cierre-ventana-combo/editor.md) y [026](../026-diagnosticos-paso-2/editor.md).
