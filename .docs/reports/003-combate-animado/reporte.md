# 003 — Combate animado y acciones explícitas

La base vigente es [004 — Un ataque y una guardia](../004-hit-y-guard/reporte.md), con su [guía del editor](../004-hit-y-guard/editor.md). Este documento conserva el estado de una iteración anterior.

**Fecha:** 2026-10-05. **Alcance:** C++ y documentación. No se modificaron assets, configuración del proyecto ni el editor. La preparación de assets y la comprobación jugable quedan en la [guía](editor.md).

## Resultado

Se retiraron el botón contextual, buffer, parry, volumen ofensivo frontal y `UDuelCombatSubsystem`. Ataque y guardia se solicitan por separado; sus ventanas proceden de animaciones. El arma detecta contactos mediante sockets y barridos entre poses. NA conserva su cooldown independiente.

No se agregó implementación online. Las habilidades siguen siendo ServerOnly; el daño solo se resuelve con autoridad. La reproducción local de montages todavía no implementa predicción, RPC ni sincronización entre clientes.

## Código y responsabilidades

Las rutas siguientes son relativas a `Source/vamps/Public/` y `Source/vamps/Private/`; cada clase modificada tiene su par .h/.cpp salvo donde se indica.

| Archivos / clases | Cambio y conexión |
|---|---|
| `Characters/VampsDuelCharacter` | Solicitudes explícitas, componentes nativos Weapon/WeaponTrace, recepción de defensa/daño. Movimiento se aplica desde RequestMove; Tick conserva orientación. Despacha ventanas a la habilidad propietaria del montage |
| `Player/DuelPlayerController` | Dos Input Actions Boolean y mapping transitorios, LMB/RMB. Move aplica dirección desde Triggered; PlayerTick conserva cámara |
| `AI/DuelAIController` | Timer de decisión propio; Tick aplica únicamente movimiento. Solicita las mismas acciones que el jugador. Modo nativo inicial Passive |
| `Combat/Abilities/DuelGameplayAbility` | Reproducción normal y limpieza de montage. Identifica cada reproducción para ignorar callbacks/notifies anteriores |
| `Combat/Abilities/GA_Attack` | Gatings, combo inmediato, ventanas animadas y un contacto por objetivo/golpe. Sin reloj ni avance central |
| `Combat/Abilities/GA_Guard` | Guardia desde reposo o respuesta al daño, cobertura de un contacto, repetición tras bloqueo y NA al salir |
| `Combat/Abilities/GA_HitReaction` | Respuesta marcada por notify y finalización por montage; sin HitLock/HitResponse numéricos |
| `Combat/Abilities/GA_NA` | Timer cancelable, vencimiento y pausa reemplazable. HUD consulta tiempo restante sin polling de habilidades |
| `Combat/Data/CombatConfig.h` | Elimina tiempos de fases, buffer, parry, alcance frontal y acercamiento automático. Añade sockets y radio de trazas; conserva secciones existentes, montages, daño, NA y cobertura |
| `Combat/CombatRules` | Solo geometría de cobertura; elimina buffer y reloj auxiliar |
| `Animation/ANS_DuelWindow` — nuevo | Notify State nativo con AttackActive, Combo, GuardActive y HitResponse; sin estado compartido entre personajes |
| `Combat/WeaponTraceComponent` — nuevo | Barridos por muestras de la hoja, después de refrescar huesos, únicamente durante la ventana activa; obstáculos y debug geométrico |
| `UI/DuelHUD.cpp` | Controles separados y distinción entre preparación defensiva, cobertura y espera de continuación |
| `Combat/DuelCombatSubsystem.h/.cpp` — eliminados | Se retiran reloj global, avance de habilidades/IA y arbitraje central de contactos |

Los demás módulos de atributos, efectos, GameMode y AnimInstance mantienen sus conexiones. No se copió lógica ni Blueprints del ejemplo Third Person.

## Decisiones visibles

- Cada pulsación actúa ahora o se rechaza; no hay ejecución diferida.
- La continuación inicia inmediatamente el siguiente golpe dentro de Combo, después de cerrar AttackActive.
- Atacar no concede defensa automática; el cruce de ataques no convierte al receptor en guardia.
- Guardia necesita su propio montage con ventana, también desde reposo.
- El alcance lo define el arma. Se retira el acercamiento artificial de windup; la posición se ajusta con movimiento.
- El primer golpe conserva giro inicial limitado; las continuaciones no corrigen orientación.
- Las nueve secciones existentes se conservan para facilitar la migración, con enlaces a velocidad normal. No dictan ventanas ni escalan duraciones.
- Sin mesh/sockets/secciones/montage válidos, se rechaza la acción. Sin montage de reacción, el daño ocurre pero no se mantiene una reacción bloqueada.
- La cancelación y el reinicio limpian ventanas y trazas; NA limpia su timer. Señales de otra instancia de montage no alteran la acción actual.

## Blueprints y assets

No se crearon ni guardaron .uasset. Las rutas comprobadas en disco son:

- `/Game/Duel/Blueprints/BP_DuelCharacter`: conserva padre VampsDuelCharacter y recibe los componentes nativos nuevos. Falta equipar el arma.
- `/Game/Duel/Blueprints/BP_DuelPlayerController`: conserva padre DuelPlayerController. SharedAction desaparece; las otras cinco acciones siguen asignadas. Attack/Guard se crean por código.
- `/Game/Duel/Blueprints/BP_DuelGameMode`: continúa usando los Blueprints propios y la configuración compartida.
- `/Game/Duel/Data/DA_DuelCombat`: conserva referencias y secciones; requiere revisión de campos nuevos y montages de guardia/reacción.
- `/Game/Duel/Animations/AM_DuelAttack`: necesita los notifies nuevos. No se alteraron sus secciones, clips ni slot.
- `/Game/Duel/Animations/ABP_Duel`, `BS_Duel` y `AS_DuelAttack1/2/3`: permanecen sin modificaciones.
- `/Game/Duel/Input/IMC_Duel` y `IA_DuelShared`: el mapping antiguo se puede retirar durante la configuración; el código ya no lo enlaza.
- `/Game/Duel/Maps/L_Duel`: escenario de revisión manual, sin cambios.

Los assets de arma, guardia y reacción de esta guía son destinos a preparar, no resultados ya creados.

## Documentación y organización

Se actualizaron combate funcional/técnico, juego funcional/técnico, roadmap, convenciones C++, índice y ambos AGENTS.md. Los entregables ahora son `.docs/reports/<NNN-iteracion>/reporte.md` y `editor.md`.

Se trasladaron los dos pares previos a `001-preparacion-mvp` y `002-mvp-duelo`, conservando su contenido histórico y corrigiendo enlaces relativos. Ambos advierten que sus pasos de combate fueron reemplazados por esta iteración. No hay reportes Markdown sueltos en reports.

## Verificaciones y límites

- Revisión de código y firmas contra headers/fuentes locales de UE 5.8: branching points, instancia de montage, secciones y ciclo de habilidades GAS.
- Revisión de interrupciones, deduplicación, recepción de ventanas por instancia, reset y limpieza de timers.
- Se aplicó clang-format 20 a los 24 archivos C++ modificados/nuevos; revisión de diferencias sin errores de whitespace. Se comprobaron 78 enlaces locales de archivos, sin destinos faltantes; reports no contiene Markdown suelto.
- No se ejecutaron tests automatizados.
- No se compiló ni se ejecutó PIE: esta sesión no expone herramientas MCP directas de Unreal. No hubo resultado Live Coding, ni NoChanges ni parche aplicado.
- La incorporación/eliminación de tipos reflejados y componentes requiere compilación en frío y reapertura antes de revisar los Blueprints. Instrucciones en la guía.

El muestreo entre poses es lineal y depende de los frames disponibles; no reconstruye una ventana completa saltada durante un hitch. Falta comprobar cobertura real, paredes y ritmo de combo con el arma/animaciones configuradas. La evaluación de huesos fuera de pantalla está preparada en código; el dedicado y la replicación permanecen sin validar.
