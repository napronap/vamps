# Editor — HUD de combate simple

Guía histórica: compilación posterior en 051 y comprobación confirmada por el usuario en [053](../053-cierre-local-y-documentacion/editor.md). Los pasos siguientes son reproducibles; ya no son pendientes de cierre del paso 5. F9 ahora pertenece al controller común según la implementación de 051.

**Para el usuario:** compilar con el editor cerrado, reabrir y comprobar el HUD en `/Game/Vamps/Maps/L_CharacterBasic`. La vista de combate es inicial y F9 alterna con debug. No hay assets ni mappings nuevos que crear/asignar. Codex modificó los fuentes; no realizó operaciones del editor porque el MCP directo de Unreal no está disponible en esta sesión.

## 1. Compilar y reabrir

**Pendiente — usuario.** Detener Play y cerrar Unreal. En Rider/Visual Studio seleccionar el target `vampsEditor`, configuración **Development Editor**, plataforma **Win64**, y compilar el proyecto `C:/fran/vamps/vamps.uproject`. Esperar compilación exitosa y abrir nuevamente el proyecto. Se modificó el header del HUD y su tamaño nativo; el arranque limpio permite crear las instancias con su disposición actual.

No ejecutar Save All ni editar el DA para habilitar la interfaz: el cambio solo requiere cargar el módulo compilado. No se afirma haber aplicado un parche Live Coding ni realizado este build.

## 2. Comprobar conexiones existentes

**Pendiente — usuario.** Abrir `/Game/Vamps/Maps/L_CharacterBasic`. En World Settings comprobar el GameMode de prueba `/Game/Vamps/Blueprints/BP_VampsCombatTestGameMode`; sus defaults deben conservar HUD Class=`VampsCombatTestHUD` y Player Controller Class=`BP_VampsCombatTestPlayerController`, según la configuración previa. No sustituirlos por clases de Third Person ni de sus variantes.

F9 se enlaza en el controller nativo de pruebas mediante BindDebugKey; no necesita Input Action ni entrada en IMC_VampsCombatTest. El HUD lee los atributos y selección que ya usa el debug. Si faltan referencias requeridas, revisar el mensaje en pantalla o LogVampsCombat y corregir esa configuración antes de continuar.

## 3. Comprobar alternancia y datos

**Pendiente — usuario.** Iniciar Play y dar foco al juego:

1. Confirmar el panel PLAYER abajo a la izquierda y TARGET arriba a la derecha, con barras independientes de HP, GUARD y ENERGY. El debug anterior no aparece simultáneamente.
2. Pulsar F9 una vez: aparecen los paneles de debug existentes. Pulsar nuevamente: vuelve combate conservando vida, energía, selección, acciones y modo de IA. Mantener la tecla no debe alternar por repetición sostenida. F6, R y F7 mantienen sus funciones anteriores.
3. Atacar, bloquear y recibir daño: comprobar reacción, contador de NA, acumulación/ruptura de guardia y stun. LAST muestra únicamente un resultado ya resuelto; CRITICAL corresponde al último contacto crítico recibido por ese panel. Durante dash/sprint y NA simultáneos, ambos se presentan.
4. Usar F para cambiar de rival: `>` en la lista y TARGET deben referirse al mismo personaje que la cámara. Los rivales muertos dejan la lista. Si hay más filas que espacio, F recorre el mismo orden y la página sigue al seleccionado.
5. Pulsar F7 para quitar el target: aparece NO TARGET y desaparece su panel; PLAYER, lista y amenazas continúan. Pulsar F7 otra vez para readquirir.
6. Pulsar R en cada modo: restaura los datos de combate y conserva la vista elegida. Detener y volver a iniciar Play: empieza otra vez en combate.

## 4. Comprobar amenazas y escala

**Pendiente — usuario.** Con F6 elegir un modo ofensivo de la IA existente y observar su preparación antes del contacto:

1. Cuando un rival prepara un ataque seleccionando al jugador, su fila indica `! INCOMING` en rojo y aparece una flecha roja con `!` alrededor del centro. El aviso no necesita un bloqueo confirmado; debe terminar cuando ese rival deja Preparation/Contacts, muere o deja de seleccionar al jugador.
2. Moverse y cambiar el yaw de cámara: las flechas corresponden a delante/detrás/izquierda/derecha respecto de la vista. Un rival fuera de la página visible conserva su aviso central. Un ataque fallido puede tener aviso; no debe mostrarse como HIT/BLOCK en LAST por ese motivo.
3. Comprobar distancia en metros y NEAR al acercarse a 15 m o menos. Ese umbral solo afecta la etiqueta y no modifica daño, alcance, lock-on ni permisos.
4. Comparar la vista de combate en 1280×720 y otra resolución/aspecto disponible: textos y barras deben conservar separación y quedar dentro del viewport. Observar especialmente los contadores y la fila de amenaza.
5. Revisar Output Log con filtro LogVampsCombat: F9 debe registrar alternancia sin errores de clase. Detener Play y registrar cualquier problema visual o de compilación antes de dar por validado el cambio.
