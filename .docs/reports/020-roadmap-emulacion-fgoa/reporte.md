# 020 — Plan de emulación FGOA

Continuación vigente: [021 — Solicitud compartida y fases de una acción](../021-solicitud-compartida/reporte.md). Conserva el cierre de la etapa 0 registrado aquí.

## Resultado

Se reescribe [el roadmap](../../roadmap.md) como una secuencia de entregas con dependencias, decisiones previas y criterios observables de cierre. Distingue emulación local del combate (hito A), base con partida y red completas (hito B) y extensiones Vamps posteriores.

No hubo cambios de código, clases, Blueprints, assets ni configuración de Unreal por Codex en esta iteración. La etapa 0 queda cerrada por confirmación del usuario; el setup reproducible de cámara/input sigue en [019](../019-camara-lockon/editor.md).

## Cambios

- `.docs/roadmap.md`: registra el punto de partida comprobado en fuentes y reportes 018–019; prioriza consolidar contacto/cámara y luego botón compartido/fases, cadena/NA, resultados, movilidad, presentación, equipos, red y calibración. Cada etapa tiene estado y condición de cierre. El avance se controla en ese mismo archivo, con fecha y referencia al reporte al cerrar etapas.
- `.docs/README.md`: actualiza la entrada al plan y enlaza esta iteración, conservando 019 como último cambio de implementación.
- Reporte y guía 020: registran el alcance documental y el cierre de 0 por el usuario. Las guías 013, 018 y 019 enlazan ese cierre; se conservan sus comprobaciones históricas y se actualizan los pendientes cubiertos por 0.

Se aplicó la skill [game-feature-docs](../../../.agents/skills/game-feature-docs/SKILL.md). Se contrastaron alcance/exclusiones de juego, reglas de combate/jugador, extensiones y análisis 016. La lectura de `AVampsCharacter`, `UGA_Attack`, `UGA_Guard`, hitbox, controller, IA, GameMode, lock-on y manager confirmó la diferencia entre la base existente y las tareas pendientes. No se tomó el texto anterior del prototipo como estado actualizado de cámara/selección.

## Verificación y continuación

Se revisaron coherencia de dependencias, cobertura del alcance y enlaces locales de los documentos modificados. No se ejecutaron tests ni operaciones de Unreal por Codex en esta edición.

## Cierre de la etapa 0

**Hecho — usuario, 15 de octubre de 2026.** El usuario confirmó «punto 0 esta OK». Se registra como aceptación de la etapa completa: compilación fuera de Live Coding, revisión del mapa, cámara/encuadre, WASD/F, selección con varios rivales y contacto/daño del montage. No se atribuye a Codex esta comprobación manual ni se inventa un log de compilación, una captura o el estado actual de guardado del mapa.

La confirmación cierra los pendientes de esa etapa; no aprueba la guardia antigua como emulación ni adelanta cámara de acciones, fade, cadenas o NA. El próximo trabajo es la etapa 1: resolver vigencia/consumo de solicitud defensiva y arbitraje de contactos simultáneos antes de implementar el botón compartido y las fases.
