# Lectura y comprobaciones en el editor

**No quedan operaciones de editor necesarias para recibir esta documentación.** Los MP4 y la guía pueden abrirse directamente. Las observaciones siguientes son opcionales para contrastar la revisión estática con el proyecto en ejecución; no se marcaron como realizadas ni condicionan la entrega.

Esta iteración no crea ni cambia assets, por lo que no requiere recompilar, activar plugins, reasignar clases, guardar Blueprints o migrar DA. No hubo pasos hechos por MCP. Se buscaron capacidades directas y no había editor Unreal accesible; la consulta de sesiones conectadas tampoco encontró una sesión.

## 1. Configuración actual

**Pendiente — usuario, sólo para contrastar los valores vivos.**

1. En Content Browser abrir `/Game/Vamps/Data/DA_VampsCombatSettings` y registrar las referencias de Weapons, AttackMontage, HitReactionMontage, NARecoveryDuration y todas las entradas de ComboSteps, incluidos transforms y adicionales. No sustituir sus valores por defaults de C++.
2. La captura de027 tenía un arma `BP_VampsGreatSword`, socket `HandGrip_R`, transform identidad; AttackMontage `/Game/Vamps/Animations/Executioner/AM_Executioner_Combo`; NA0,5s; alineación/empuje130/100,130/100,150/250; adicionales0; HitReactionMontage=None. Es referencia histórica para comparar, no una instrucción de restauración.
3. Abrir el montage referenciado y observar secciones, ANS_AttackTrace, ANS_ComboWindow y playback rate sin modificarlos. La validación C++ vigente exige un AttackTrace por sección. Agregar otro sin cambiar el modelo de código provoca rechazo de configuración; no completa soporte multihit.
4. En el Blueprint del personaje, observar qué AnimBP usa su Mesh. En el AnimBP, revisar el uso de `bIsGuarding`: C++ lo alimenta desde disponibilidad `bCanDefend`, no desde una reacción efectiva. No asumir que una pose visible demuestra que existe Guard como estado de combate.

Resultado útil: captura actual de referencias/valores y uso del AnimBP, que permita distinguir efectos configurados de las limitaciones de la ruta C++. Si alguna ruta difiere, registrar la referencia real; no crear un asset alternativo para continuar.

## 2. Contraste del prototipo actual

**Pendiente — usuario, observación manual.** Usar la escena de combate ya configurada, sin cambiar GameMode o assets para esta revisión. Abrir Output Log y filtrar `LogVampsCombat` para identificar configuración rechazada antes de atribuirla al combate.

| Observación en PIE | Predicción que se contrasta |
|---|---|
| Pulsar y luego mantener el botón de combate. | La ruta propia enlaza Started: mantener no genera por sí solo un nuevo estado mantenido de defensa. La configuración de triggers del asset requiere lectura actual. |
| Bloquear mientras el personaje ejecuta su acción. | La ruta bloqueada no solicita Guard ni cancela el montage de ataque; puede conservar contactos activos. Confirmar con la animación/DA presentes. |
| Intentar caminar tras el empuje breve, antes de que venza la restricción ofensiva. | El input de movimiento no consulta ese plazo. Puede haber movimiento aunque no pueda empezar ataque; root motion/configuración pueden afectar lo visible. |
| Recibir un bloqueo desde un rival distinto del objetivo. | El resolver cambia selección; la rama de guardia omite Alignment/FacingGoal. Distinguir giro de cámara de giro corporal. |
| Observar el tiempo entre bloqueo y nueva ofensiva. | Está calculado con duración completa de sección+adicional, no con datos de una reacción defensiva por subgolpe. No comparar segundos con el42 nativo sin conversión medida. |

No se afirma que estas observaciones ya hayan reproducido errores. No se pide configurar multihits, dos armas o varios atacantes para esta entrega. Cuando exista esa implementación, los casos concretos de [la guía](analisis.md#intercambios-que-deben-poder-explicarse-al-observar-la-implementación) servirán para observarla manualmente.

## 3. Antes de una futura modificación

**Pendiente — Codex por MCP, sólo si se autoriza e inicia una implementación posterior.** Si se cambian `UVampsCombatSettings` o structs serializados, buscar todos sus DA y derivadas, guardar sus valores y assets por MCP y releerlos tras compilar/reabrir, según [AGENTS.md](../../../AGENTS.md). La captura histórica no sustituye ese paso. Si MCP no expone una propiedad necesaria, documentar la operación manual antes de hacer el cambio que pueda perderla.

**Pendiente — usuario, decisión de diseño futura.** Resolver qué diferencias del apartado13 de la guía se conservan respecto de FGOA. No hay cambios de especificaciones o gameplay esperando confirmación en esta iteración.
