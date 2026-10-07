# 006 — Golpe de puño con hitbox y trazas

Código y documentación. Los assets quedan pendientes para el usuario según la [guía](editor.md).

Se reemplaza toda la lógica de arma del combate propio por una esfera sobre un socket del puño. Se mantiene un solo ataque y las dos reglas: guardia cancela al agresor sin daño; sin guardia se resta vida. IA, controles, HUD y lock-on siguen disponibles.

## Código y relaciones

Rutas relativas a `Source/vamps/`, con headers en Public e implementaciones en Private.

| Archivo / clase | Cambio |
|---|---|
| `Combat/FistHitboxComponent.h/.cpp` — nuevo | USphereComponent adjunto al socket FistHit del skeletal mesh. Radio inicial 12 cm; barre entre posiciones consecutivas durante el notify y registra receptores para evitar daño repetido |
| `Combat/WeaponTraceComponent.h/.cpp` — eliminado | Se retiran los dos sockets del arma, el muestreo de la hoja y la dependencia de StaticMeshComponent |
| `Characters/VampsDuelCharacter.h/.cpp` | Retira Weapon y WeaponTrace; crea FistHitbox adjunto a Mesh. Reinicio y fin de juego detienen sus trazas |
| `Combat/Abilities/GA_Attack.cpp` | Comprueba HasFistSocket y reinicia receptores de FistHitbox. Finalización, blend-out y cancelación detienen la nueva hitbox |
| `Animation/ANS_AttackTrace.cpp` | Conserva el mismo notify y comprobación de instancia de montage; ahora inicia/detiene FistHitbox |
| `AI/DuelAIController.h` | AttackDistance pasa de 150 a 100 cm como punto inicial para el menor alcance del puño |

La esfera tiene NoCollision y no genera overlaps: su volumen se usa en SweepMultiByObjectType contra Pawn. La primera muestra consulta el puño en su posición actual; las siguientes barren desde la muestra anterior. Se ignora al dueño y se conserva la consulta Visibility entre personajes para descartar receptores detrás de paredes. Al bloquearse un golpe, la cancelación deshabilita las trazas y no se procesan más contactos de ese ataque.

El Tick de la hitbox está desactivado fuera del tramo marcado por Attack Trace. No se agregan decisiones al Tick del personaje. La forma de barrido usa el radio escalado de la esfera; el socket determina su centro. BeginPlay alinea el componente al socket configurado y avisa en Output Log si falta. Sin socket no se activa el ataque.

## Conexiones y documentación

`/Game/Duel/Blueprints/BP_DuelCharacter` hereda FistHitbox y conserva AttackMontage/AttackDamage. El usuario debe crear FistHit en el skeleton que use Mesh y seleccionar su animación de puño en `/Game/Duel/Animations/AM_BasicAttack`. El nombre de esa animación y sus tiempos concretos no fueron proporcionados; el notify se coloca visualmente sobre el tramo de contacto.

No se crean sockets, montages ni assets desde código. Tampoco se agregan efectos, habilidades o componentes de daño adicionales. Se actualizan juego/combate funcional y técnico, roadmap e índice. Las guías 004 y 005 conservan su historial con aviso de que la configuración de arma fue reemplazada por 006.

## Verificación y límites

Revisión estática de activación, notify, barrido, deduplicación, bloqueo y reinicio. API de radio y forma contrastada con SphereComponent.h de UE 5.8. Se aplicó clang-format a los siete archivos C++ afectados. `git diff --check` no encontró errores de whitespace. Los 110 enlaces locales de archivos resuelven y no hay reportes sueltos. La búsqueda en Public/Private no encuentra Weapon, WeaponTrace, TraceBase ni TraceTip.

No se ejecutan tests automatizados. No se compiló ni se ejecutó PIE: no hay herramientas MCP directas de Unreal disponibles en esta sesión. Quedan pendientes la compilación completa, la migración de Blueprint y las comprobaciones manuales de la guía. Content y Config no se modifican.

El barrido aproxima el movimiento entre muestras y no reconstruye tramos completos perdidos durante hitches. La esfera golpea la colisión Pawn del receptor; no se añade una hitbox por hueso del defensor. La distancia de IA y el radio son valores iniciales para ajustar a la animación elegida.
