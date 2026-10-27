# Editor — Selector Executioner, START y revancha

No quedan operaciones pendientes para cerrar 6.2. El usuario confirmó compilación y funcionamiento el 2026-10-27. La implementación no requirió assets, mappings ni asignaciones nuevas. Esta guía conserva los pasos reproducibles; Codex no realizó una nueva compilación ni observación por MCP.

## 1. Cargar el módulo actualizado

**Hecho — usuario, compilación confirmada.** El usuario informó que compiló y probó con éxito. Para reproducir la carga desde disco: detener Play y cerrar Unreal; en `C:/dev/vamps`, ejecutar `build.bat` y esperar `Result: Succeeded`; abrir con `editor.bat`. Los scripts usan Unreal 5.8 y el target vampsEditor, Win64 Development. No se presume que el usuario utilizó estos comandos concretos ni que Codex los ejecutó.

El header del GameMode incorpora Selecting y una propiedad de instancia; el HUD incorpora el dibujo de selección. El arranque limpio carga las clases con su disposición actual. No se afirma una compilación por Live Coding ni se reutiliza un Success/NoChanges anterior como validación de estos cambios.

## 2. Conservar y comprobar conexiones

**Hecho — Codex por MCP en 051, sin relectura en 055.** Mapa, Blueprints y referencias de Executioner quedaron configurados y guardados según la [guía 051](../051-partida-local/editor.md). No volver a crear ni sustituir esos assets para habilitar el selector.

**Hecho — usuario, comprobación funcional general.** El flujo cargó y funcionó tras compilar. Para reproducir la comprobación de conexiones, abrir `/Game/Vamps/Maps/L_LocalMatch`: World Settings conserva GameMode Override `BP_VampsMatchGameMode`, con DefaultPawnClass BP_VampsCharacter_Player, BotCharacterClass BP_VampsCharacter, PlayerControllerClass BP_VampsMatchPlayerController y HUDClass VampsCombatTestHUD. Si una reconstrucción posterior informa errores del Blueprint, abrirlo y compilar para revisar su diagnóstico. No hay propiedades nuevas de catálogo que asignar ni cambios requeridos en DA_VampsCombatSettings. La confirmación funcional no sustituye una captura nueva de propiedades por MCP.

## 3. Selección e inicio

**Hecho — usuario, 2026-10-27.** Funcionamiento confirmado de forma general. Pasos reproducibles en Play, L_LocalMatch:

1. Ver SELECT CHARACTER, EXECUTIONER y START con cursor visible. Al comenzar no hay selección; START está gris e inactivo. Esperar más de 5 s: no debe comenzar la ronda, aparecer el reloj ni moverse/atacar los participantes.
2. Pulsar EXECUTIONER: aparece SELECTED y START queda activo. La selección no inicia la partida.
3. Pulsar START una vez: desaparece el menú, empieza la preparación de 5 s y luego la ronda. No aparece otro diálogo. Los cuatro participantes siguen siendo Executioner, con la configuración y recursos de 6.1. Clicks repetidos no reinician la preparación.
4. Comprobar movimiento/combate y F9 en la ronda. El HUD y los recursos funcionan como antes. Si se pulsa F9 mientras se está en selección, el selector sigue siendo la vista visible.

## 4. Respawn y otra partida

**Hecho — usuario, 2026-10-27.** Confirmación general del ciclo. Para reproducirlo, comprobar durante la ronda que muerte/respawn conservan personaje, recursos y retorno de cámara. Al terminar:

1. El resultado conserva PLAY AGAIN y sus estadísticas.
2. Pulsar PLAY AGAIN: vuelve SELECT CHARACTER con EXECUTIONER seleccionado y START activo. Se recupera el cursor y termina la cámara libre si estaba presente. El menú no inicia una cuenta atrás por sí solo.
3. Pulsar START: otra preparación de 5 s, puntos/kills/muertes/rachas a cero, posiciones y recursos restaurados, sin pose de muerte retenida. No hay que seleccionar otra vez ni confirmar en otra pantalla.
4. Detener y volver a iniciar Play: la sesión nueva empieza sin selección. No hay guardado entre sesiones.

Revisar también el selector con el tamaño de viewport usado para jugar: botones/textos centrados, sin paneles de combate superpuestos. Si aparece MATCH UNAVAILABLE, revisar LogVampsCombat y resolver la referencia/participante/controller indicado; no se sustituye por otra configuración para continuar.

El ciclo 6.2 queda cerrado por la confirmación del usuario. No se afirma una captura independiente de cada escenario, resolución o error de configuración. La próxima entrega es el paso 7 de red; no hay tests automatizados que crear o ejecutar para este cierre.
