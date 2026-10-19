# 039 — Simplificación de movilidad

Control direccional actualizado en [040](../040-giro-direccional-dash/reporte.md): Shift con input gira al iniciar el dash; la configuración del DA y los dos montages de esta iteración se conservan. [Guía vigente del giro](../040-giro-direccional-dash/editor.md).

## Alcance y resultado — 2026-10-19

Implementa GA_Dash con root motion y dos montages; limita approach al primer ataque y retira el permiso de evasión por impacto final. Sustituye esas partes de [037](../037-movilidad-combate/reporte.md) y las propuestas de [038](../038-estado-paso-4/reporte.md). Las observaciones de gameplay siguen pendientes en la [guía](editor.md).

El dash frontal usa el frente corporal al ejecutarse, sin giro adicional hacia WASD ni historial direccional. Sin input, orienta el cuerpo hacia el target y usa el clip de retirada para alejarse. Sin target/dirección horizontal válida, rechaza con diagnóstico. El recorrido y la duración proceden del asset: ya no se imponen 350 cm/0,3 s ni se exponen multiplicadores nuevos. Los dos clips duran 1 s y conservan su velocidad autorada.

Se preservan sprint, costes/recuperación, ausencia de invulnerabilidad, prioridad de contactos y continuación, y restricciones de Recovery/NA y reacciones. Guard termina por su calendario existente; no se libera por bloquear un supuesto remate ni por un marcador nuevo.

## Código y relaciones

Rutas relativas a Source/vamps, con headers en Public y cpp en Private:

| Archivos/clases | Cambio y responsabilidad |
|---|---|
| Combat/Abilities/GA_Dash.h y .cpp, nuevos | Habilidad nativa InstancedPerActor/ServerOnly otorgada junto a GA_Attack. Selecciona uno de los dos montages, valida precondiciones, ejecuta PlayMontageAndWait con escala/velocidad 1, cobra solo tras iniciar reproducción y limpia el estado al terminar/interrumpirse. Usa el cierre de locomoción existente antes de empezar. |
| Characters/VampsCharacter.h y .cpp | Mantiene intención Shift, presencia de input y solicitud pendiente del paso; el resolver activa la GA tras contactos/continuación. Elimina dirección pendiente y permiso especial de Guard. Ataque nuevo, contacto, muerte y reset cancelan el dash; soltar Shift termina intención de sprint sin truncar el dash. Da acceso interno a GA_Dash mediante friend, sin getters/setters adicionales para sus datos internos. |
| Combat/VampsCombatMovementComponent.h y .cpp | Retira selección de sectores, reproducción y desplazamiento programado del dash. Permite su root motion y conserva colisión del CharacterMovement. Mantiene approach limitado y alineación/empuje de reacciones. BeginCombatStep solo inicia approach cuando ComboStep=1. |
| Combat/Data/VampsCombatSettings.h y .cpp | Reemplaza array de ocho montages por ForwardDashMontage/BackwardDashMontage; elimina DashDistance/DashDuration. Extrae ApproachDistance/Duration/StopDistance del struct de cada paso al DA. Cada ComboSteps conserva solo alineación, empuje y duración de reacción. Conserva validaciones no fatales de escalares. |
| Combat/VampsCombatState.h | Sustituye bGuardDashReady por bIsDashing, compartido con animación y HUD. |
| Combat/VampsCombatResolver.h y .cpp | Retira bFinalImpact, índices de arma/subimpacto de la entrega de contacto y búsqueda adicional del remate. Conserva clasificación de contactos y orden contactos → continuación → dash → locomoción. |
| Combat/VampsWeapon.h y .cpp | Elimina contador usado solo para identificar el remate. Conserva mapas de víctimas por arma y su reset mediante AN_ResetWeaponHits para multihit. |
| Combat/Abilities/GA_Attack.cpp | Retira validación de índices finales y parámetros de approach por sección. Conserva validación de trazas, ventanas y reinicios. |
| Player/VampsPlayerController.cpp | Comunica presencia de input actual; movimiento WASD mantiene su referencia a cámara. Ya no transforma una dirección adicional para seleccionar dash. |
| Animation/VampsAnimInstance.cpp y UI/VampsCombatTestHUD.cpp | Leen bIsDashing del estado de combate y retiran la señal FINAL GUARD. |

Formateador aplicado a todos los fuentes propios modificados. El conjunto de archivos C++ afectados, incluyendo la GA nueva, pasa de 3663 a 3599 líneas: reducción neta de 64. No se introduce lógica del ejemplo Third Person, infraestructura de red ni compatibilidad de campos retirados.

## Assets y Blueprints

- Modificados /Game/Vamps/Animations/Executioner/AM_Executioner_Dash_Fw y AM_Executioner_Dash_Bw: conservan DefaultSlot, rango de segmento 0–1 s, RateScale=1, Auto Blend Out y blends 0,06/0,08 s. Sus segmentos apuntan a clips propios.
- Agregados /Game/Vamps/Animations/Executioner/A_Executioner_Dash_Fw y A_Executioner_Dash_Bw, duplicados de los Dash_Fw_RM/Dash_Bw_RM de GreatSwordAnimV1. Enable Root Motion=true y Root Motion Root Lock=AnimFirstFrame. No se modifica el pack fuente.
- Eliminados por MCP los seis AM_Executioner_Dash_FR/R/BR/BL/L/FL tras confirmar ausencia de referenciadores. La búsqueda final devuelve únicamente Bw/Fw. [Resultados](montages-eliminados.json).
- Modificado /Game/Vamps/Data/DA_VampsCombatSettings: referencias frontal/retirada asignadas; approach inicial 250 cm/0,25 s/130 cm. Energía y sprint conservan 100, coste dash 25, sprint 20/s a 700 cm/s y recuperación 25/s tras 1 s.
- BP_VampsCharacter_Player y BP_VampsCharacter conservan sus clases propias derivadas de AVampsCharacter; reciben la GA nativa desde BeginPlay. ABP_Executioner continúa usando el slot existente con RootMotionFromMontagesOnly. Inputs, controllers, BlendSpace, mapa y graphs no se modifican.

[Lectura actual de montages y clips](animaciones-mcp.json).

## Preservación del DA y compilación

ObjectTools.search_subclasses y AssetTools.find_assets buscaron clase/derivadas en todas las raíces: un único DA. Se capturaron todas sus propiedades, armas, transforms, montages y tres pasos en [data-assets-antes.json](data-assets-antes.json), y se guardó antes de editar tipos reflejados.

LiveCodingToolset.CompileLiveCoding aplicó un parche [Success](live-coding-1.json). La recarga sustituyó ComboSteps por defaults en memoria; [captura](data-assets-live-coding.json). Ese estado no se guardó.

**Hecho — usuario:** cierre sin guardar DA/Blueprints recargados, build Development Editor/Win64 y reapertura, confirmado en la conversación. Codex no ejecutó ese build ni el reinicio.

Después del arranque limpio se releyó el DA: los tres pasos originales seguían en disco. Se aplicó el [objetivo migrado](data-assets-objetivo.json), preservando orden/cantidad y cada valor no eliminado por la tarea. La [lectura final](data-assets-despues.json) tiene cero diferencias contra ese objetivo, sin referencias transitorias. Se guardó solo el DA afectado; is_dirty=false. Las animaciones/montages ya se habían guardado antes de compilar y se verificaron después del reinicio.

La comprobación MCP final devolvió [NoChanges](live-coding-final.json), que indica ausencia de cambios para compilar; no representa otro parche.

## Comprobaciones y pendientes

- Revisión de continuidad, contactos, restricciones, cancelaciones y root motion; búsqueda sin usos residuales de FinalImpact, GuardDashReady, ImpactIndex, array direccional ni distancia/duración del dash. git diff --check sin errores.
- PIE iniciado por MCP en /Game/Vamps/Maps/L_CharacterBasic. Ambos personajes recibieron GA_Attack y GA_Dash, estaban vivos/Idle, con energía 100/100, bCanDash=true y bIsDashing=false. LogVampsCombat sin errores/warnings en la consulta. [Captura](pie-estado.json). PIE detenido y confirmado false.
- No se simularon inputs ni se ejecutaron tests automatizados. La lectura confirma concesión/arranque y configuración, no el recorrido efectivo del root motion, blending, combate ni sprint en ejecución.
- Queda la [observación manual](editor.md#5-observar-el-comportamiento) de cápsula/mesh, dash frontal y retirada, sprint, approach solo inicial y cierre/reacciones. No se cierra el paso 4 únicamente por compilar.
- No se modifican especificaciones funcionales/técnicas ni el roadmap; esta implementación y guía prevalecen para la configuración de movilidad sobre las instrucciones de 037/038.
