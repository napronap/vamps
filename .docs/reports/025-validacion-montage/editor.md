# 025 — Diagnosticar y corregir el montage

Aplicar ahora [026 — Diagnósticos del paso 2](../026-diagnosticos-paso-2/editor.md): la categoría vigente es LogVampsCombat y la revisión incluye armas, movimiento y las conexiones del combate. Esta guía conserva la evidencia del primer error de montage.

**Pendiente para el usuario:** agregar ANS_AttackTrace en la sección Default [0,000000, 1,933333] del montage y volver a intentar el ataque. El diagnóstico ya se obtuvo y el rechazo no cerró el editor. La compilación completa y reapertura están hechas. El montage asignado es /Game/Vamps/Animations/Executioner/AM_Executioner_Combo.

Esta guía continúa la [configuración de secciones y ventanas de 024](../024-cierre-ventana-combo/editor.md). No requiere cambios en Manny, AnimBP, Blueprints de armas ni referencias del DA.

La aclaración posterior de AGENTS.md sobre validaciones, errores y fallbacks solo cambia instrucciones del repositorio. No requiere compilación, reinicio ni operaciones adicionales en el editor; los pendientes de esta guía corresponden al montage.

## 1. Compilar y reabrir

**Hecho — usuario:** ejecutó C:\dev\vamps\build.bat con Unreal cerrado y reabrió con C:\dev\vamps\editor.bat, confirmando compilación correcta. El código ahora informa errores de configuración y cancela el ataque antes de reproducirlo.

**Hecho — Codex por MCP:** ObjectTools.get_properties confirmó el montage asignado en /Game/Vamps/Data/DA_VampsCombatSettings. LogsToolset.GetLogCategories confirmó LogVampsAttack en el editor reabierto. No se guardaron ni alteraron assets.

## 2. Obtener el diagnóstico concreto

**Hecho — usuario:** realizó intentos de ataque en PIE después de recompilar y reabrir.

**Hecho — Codex por MCP:** LogsToolset.GetLogEntries confirmó el rechazo por falta de ANS_AttackTrace en Default [0,000000, 1,933333]. El editor siguió respondiendo y la consulta posterior indicó PIE detenido.

Recorrido reproducible: iniciar Play en el mapa habitual e intentar atacar una vez. Abrir Window > Developer Tools > Output Log y filtrar LogVampsAttack. También Codex puede leer esas entradas mediante LogsToolset.GetLogEntries.

El mensaje debe indicar el personaje, DA y causa. Si el problema está en un notify, identifica el montage, sección y sus tiempos. El ataque inválido debe rechazarse sin empezar la sección ni NA y sin cerrar el editor.

No hay herramienta MCP directa disponible para solicitar el input del ataque ni para consultar los notifies/secciones de este montage. Ese paso se deja al usuario; no se usa automatización de interfaz.

## 3. Corregir el asset

**Pendiente — usuario:** detener PIE y abrir /Game/Vamps/Animations/Executioner/AM_Executioner_Combo. Corregir la sección identificada:

1. Agregar ANS_AttackTrace en la pista Notifies del propio montage dentro de Default [0,000000, 1,933333], sobre el tramo de contacto del primer golpe. Cada sección necesita exactamente uno. Ponerlo solamente en las Animation Sequences de origen no satisface esta implementación.
2. Mantener inicio de sección < inicio de Attack Trace < fin de Attack Trace < fin de sección. Dejar preparación antes y recuperación después; el notify no puede comenzar o terminar justo en los límites de su sección.
3. Default es un nombre válido y renombrarlo no agrega el notify que falta. Los nombres son libres; todas las secciones cuentan como pasos de ataque. Solo eliminar una sección adicional si efectivamente está vacía y no representa un golpe.
4. Cada sección anterior a la última necesita un ANS_ComboWindow. Su inicio sigue al inicio de Attack Trace; su final sigue al fin de Attack Trace y queda antes del blend-out de su sección. El mensaje proporciona los límites efectivos si falla esa condición.
5. Guardar el montage y volver a intentar el ataque. No hace falta recompilar C++ por mover estos notifies. Continuar con la observación de cadenas y NA de la guía 024.

Codex no ajustó estos tiempos: deben corresponder al gesto de las animaciones elegidas por el usuario.
