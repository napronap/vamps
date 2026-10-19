# Editor — Cámara sin target

Guía histórica, sustituida por [044 — Seguimiento libre y F7](../044-seguimiento-libre-f7/editor.md). El comando de consola siguiente fue retirado.

**Para el usuario:** el parche ya está aplicado. Iniciar Play y ejecutar en la consola del juego `vamps.Debug.DisablePlayerLockOn 1`. Esto permite observar la cámara sin selección manteniendo los enemigos. Ejecutar `vamps.Debug.DisablePlayerLockOn 0` para volver al seguimiento con lock-on. Falta comprobar la activación del modo y la sensación lateral en juego.

## 1. Preparación y compilación

**Hecho — Codex por MCP.** EditorAppToolset.IsPIERunning confirmó que PIE estaba detenido antes de editar/compilar. Se revisó código y referencia nativa conservada; no se editaron ni guardaron assets.

**Hecho — Codex por MCP.** LiveCodingToolset.LiveCodingToolset.CompileLiveCoding aplicó un parche [Success](live-coding.json), no NoChanges. [Log UBT](build.log) finalizado Succeeded. No se ejecutó build completo ni reinicio; no se guardaron assets durante la recarga.

**Hecho — Codex por MCP, parcial.** SearchCVars confirmó el modo con valor inicial false. StartPIE arrancó L_CharacterBasic; se leyeron manager/selección normales y LogVampsCombat no devolvió errores/warnings. StopPIE terminó la sesión e IsPIERunning confirmó false. [Estado](pie-estado.json). No se activó la variable ni se ejecutó WASD por herramientas; la comprobación del apartado 2 sigue siendo manual.

## 2. Activar el modo de prueba

**Pendiente — usuario.** En `/Game/Vamps/Maps/L_CharacterBasic`:

1. Iniciar Play y abrir la consola de Unreal con su tecla configurada.
2. Ejecutar `vamps.Debug.DisablePlayerLockOn 1`. El panel existente debe mostrar LOCK ON: None, conservando los rivales de su lista. F y los bloqueos no deben seleccionar mientras el modo siga activo.
3. Mover lateralmente con A/D, detenerse, invertir y hacer diagonales. El cuerpo gira con la locomoción; la cámara conserva una referencia independiente y acompaña la traslación con su filtro existente.
4. Repetir con Shift más dirección y sprint; probar también ataque al aire. Sin target real, Shift sin dirección conserva su rechazo existente: el sujeto virtual de cámara no habilita la retirada ni approach hacia un enemigo.
5. Ejecutar `vamps.Debug.DisablePlayerLockOn 0`. Se readquiere un rival elegible y la cámara transiciona al seguimiento real. Repetir la alternancia mientras el personaje mire hacia un lateral y durante encuadre de combate.
6. Repetir sin rivales elegibles y con R: al arrancar/resetear sin target se toma la referencia inicial del controller. El giro corporal posterior no arrastra la cámara.

La variable es global de desarrollo, inicialmente desactivada y no existe en Shipping. Durante la sesión del editor conserva su valor hasta cambiarlo o cerrar el proceso, incluso al detener/reiniciar Play. Dejarla en 0 al terminar.

No hay nuevas Input Actions ni Blueprints que asignar. Las herramientas descubiertas permiten buscar CVars pero no escribirlas directamente; este paso es manual, sin automatización de interfaz.

## 3. Límites de comprobación

Compilar y leer estado no aprueba la sensación de movimiento ni reproduce inputs reales. No se agregan ni ejecutan tests automatizados. Las observaciones anteriores quedan pendientes hasta que se realicen.
