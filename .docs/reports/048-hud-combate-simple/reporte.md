# 048 — HUD de combate simple y alternancia de debug

La presentación Canvas queda reemplazada por [058 — Widgets UMG](../058-ui-widgets/reporte.md), conservando la información y la alternancia F9.

Cierre posterior en [053](../053-cierre-local-y-documentacion/reporte.md): compilación acreditada en 051 y funcionamiento confirmado por el usuario el 2026-10-27. Los pendientes descritos aquí conservan el estado histórico de esta implementación.

Continuación de planificación: [049 — Revisión del paso 6](../049-revision-paso-6/reporte.md), con [guía de pendientes](../049-revision-paso-6/editor.md). El HUD de esta iteración es la base de presentación; 049 no cambia su código.

## Resultado

Agrega una vista de combate con barras y texto sobre el HUD nativo existente. Es la vista inicial; F9 alterna con el debug en builds de desarrollo. El cambio de vista solo afecta el dibujo, sin reiniciar combate, modificar selección ni pausar la IA.

Implementación terminada en fuentes, pendiente de compilación y observación en Unreal: esta sesión no dispone de herramientas MCP directas del editor. No se declara cerrado el paso 5 por revisión de código. La [guía](editor.md) reúne los pasos necesarios.

## Archivos y responsabilidades

| Archivos en Source/vamps | Cambio |
|---|---|
| Public/UI/VampsCombatTestHUD.h y Private/UI/VampsCombatTestHUD.cpp | Mantienen el dibujo de debug y agregan la vista de combate, barras compartidas, estado del jugador/target, lista de rivales y direcciones. Un booleano privado, inicialmente false, controla la presentación de debug. No se agrega configuración reflejada. |
| Public/Player/VampsCombatTestPlayerController.h y Private/Player/VampsCombatTestPlayerController.cpp | Enlazan F9 mediante Enhanced Input BindDebugKey, como el F7 existente. El callback valida el HUD y solicita alternancia con diagnóstico no fatal si la clase no coincide. El binding y callback se excluyen de Shipping. |

El HUD lee `UVampsAttributeSet` y `FVampsCombatState`; no calcula daño, permisos ni resultados. `UVampsLockOnComponent` sigue siendo la única fuente de selección y orden. El controller solo entrega la orden de presentación.

## Presentación

- Panel del jugador abajo a la izquierda y del objetivo arriba a la derecha: HP, carga de guardia y energía en barras separadas. Muestran ataque, recuperación, dash/sprint, reacción con tiempo restante, stun y baja. NA conserva su contador aunque coincida con movilidad o reacción; no aparece READY mientras existe NA.
- La línea LAST muestra el último contacto recibido: HIT, BLOCK o GUARD BREAK, daño y CRITICAL cuando corresponde. Es historial explícito, no una animación ni un estado de guardia permanente. El reset usa el estado de combate existente y limpia ese historial.
- Lista vertical de rivales vivos en el orden usado por F, con numeración, `>` para selección, distancia horizontal en metros y flecha relativa al yaw de cámara. NEAR significa hasta 15 m, un umbral visual inicial propio, independiente del alcance del arma. No se ordena por distancia ni se cambia la selección. Si las filas exceden el alto disponible, la página sigue al seleccionado.
- Un rival que selecciona al jugador y está en Preparation o Contacts genera `! INCOMING` rojo y una flecha roja alrededor del centro. El aviso existe desde preparación y no requiere daño ni bloqueo. Las flechas centrales recorren todos los rivales, incluso fuera de la página visible; no predicen que el contacto vaya a alcanzar ni alteran el resolver. En posiciones horizontales coincidentes se conserva `!` sin inventar una dirección.
- La vista de combate escala conjuntamente texto, barras y posiciones respecto de 1280×720. Sin objetivo muestra NO TARGET; la lista y las amenazas siguen disponibles. El debug conserva sus paneles y controles anteriores, con la indicación F9 para volver.

La alternancia dura mientras exista esa instancia de HUD y se conserva al usar R. Una nueva sesión empieza en combate. En Shipping no se enlaza F9 ni aparece su indicación.

## Assets y conexiones

No se crean ni modifican assets, Blueprints, Data Assets, structs de configuración, mapas, materiales, input actions ni mappings. `/Game/Vamps/Blueprints/BP_VampsCombatTestGameMode` conserva el HUD nativo `VampsCombatTestHUD` y `/Game/Vamps/Blueprints/BP_VampsCombatTestPlayerController`, heredero del controller modificado. Estas conexiones fueron acreditadas en 045; no se releyeron mediante MCP en esta sesión.

El cambio no requiere sustituir HUDClass ni configurar referencias nuevas. Se conserva la lógica propia sobre AHUD y el controller propio; no se reutiliza lógica de Third Person ni de sus variantes. No se incorpora puntuación/portador porque pertenecen al ciclo de partida del paso 6.

## Verificaciones y pendientes

- Revisados el flujo de alternancia, el estado inicial, las ramas Shipping, selección ausente, rivales muertos/destruidos, orden de F, paginación, avisos fuera de página y compatibilidad de reacción/NA. El dibujo anterior de debug se conserva en su rama.
- Contrastadas firmas de DrawText/DrawRect/DrawLine y BindDebugKey con los headers del motor 5.8 instalado; revisados nombres nuevos contra AHUD, AActor, APlayerController, AController y la jerarquía UObject.
- clang-format aplicado a los cuatro fuentes modificados. git diff --check sin errores de espacios.
- No se compila ni se ejecuta PIE: no está disponible `LiveCodingToolset.LiveCodingToolset.CompileLiveCoding` ni otro MCP directo de Unreal. No se ejecuta un build mediante terminal para suplirlo. No hay un resultado Success o NoChanges nuevo.
- No se agregan ni ejecutan tests automatizados, no se automatiza UI y no se guardan assets. La revisión estática no certifica el aspecto renderizado, el binding en juego ni una compilación exitosa.

Queda el build con arranque limpio y la observación de [editor.md](editor.md). Se agregan este reporte, su guía y enlaces desde el índice y la revisión 047. Roadmap y especificaciones funcionales/técnicas permanecen sin editar.
