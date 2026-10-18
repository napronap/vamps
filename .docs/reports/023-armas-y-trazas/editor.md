# 023 — Configurar armas y trazas

Para el momento de salto entre secciones, aplicar [024 — Cierre de Combo Window](../024-cierre-ventana-combo/editor.md).

**Pendiente para el usuario:** calibrar arma/agarre con sus animaciones y observar contactos y duales del paso 5. La compilación completa/reapertura ya fue realizada por el usuario. Codex completó la asignación del DA, compiló y guardó los Blueprints y verificó creación, enganche y limpieza de las armas en PIE.

Esta guía reemplaza el componente y socket del puño usados en [022](../022-cadena-y-na/editor.md). Los montages, notifies, cadena y NA mantienen esa guía. La reacción de bloqueo queda para el paso 3 del roadmap.

## 1. Compilación y arranque limpio

**Hecho — Codex por MCP:** CompileLiveCoding compiló y aplicó parches reales con Success. Se eliminaron los archivos de FistHitboxComponent, se creó AVampsWeapon y se amplió el DA con FVampsWeaponSettings.

**Hecho — usuario:** realizó la compilación completa y reapertura, confirmadas con «listo». ObjectTools había detectado el campo WeaponClass del DA referido a /Engine/Transient.LIVECODING_VampsWeapon_0. Tras reabrir, Codex verificó Class@/Script/vamps.VampsWeapon. Recorrido reproducible para eliminar la referencia transitoria y cargar los cambios nativos:

1. Guardar las animaciones y el trabajo propio. Cerrar Unreal.
2. Ejecutar C:\dev\vamps\build.bat con Unreal cerrado y esperar compilación correcta.
3. Abrir C:\dev\vamps\editor.bat y avisar a Codex.

Esta compilación completa/reapertura no tiene herramienta MCP directa; no se sustituyó por terminal ni automatización de interfaz. La configuración de armas vacía se rechaza explícitamente; el DA actual ya tiene su entrada asignada.

## 2. Blueprint por arma

**Hecho — Codex por MCP:** BlueprintTools.create creó /Game/Vamps/Blueprints/Weapons/BP_VampsGreatSword con clase base /Script/vamps.VampsWeapon. ObjectTools configuró sus componentes, BlueprintTools.compile_blueprint completó con warnings_as_errors=true y AssetTools confirmó save_assets=true e is_dirty=false.

Para reproducirlo o crear otra arma:

1. Content Browser → Vamps/Blueprints/Weapons → Blueprint Class → All Classes → VampsWeapon. Crear el Blueprint específico del arma. No derivar de clases ni Blueprints del pack de animaciones.
2. Abrir el Blueprint. La jerarquía nativa es WeaponRoot → WeaponMesh → TraceStart y TraceEnd. WeaponRoot representa el punto de agarre. Asignar la Static Mesh en WeaponMesh y acomodar su transform relativo para que el agarre coincida con ese origen.
3. Colocar TraceStart al comienzo de la zona que golpea y TraceEnd en la punta. Ambos se editan en el viewport del Blueprint como Scene Components. No agregar una Box Collision ni sockets en el arma para estas trazas.
4. Class Defaults → Weapon → Trace Radius: grosor de la detección. El valor se expresa en cm del actor y se escala con su transform de montaje. Dejar NoCollision y Generate Overlap Events desactivado en WeaponMesh.
5. Compile y Save. El Blueprint contiene presentación y parámetros; la detección común ya vive en C++.

Valores iniciales guardados en BP_VampsGreatSword:

| Propiedad | Valor |
|---|---|
| WeaponMesh → Static Mesh | /Game/GreatSwordAnimV1/DEMO/StaticMesh/Sword_10 |
| WeaponMesh → Location | (60,0,0) |
| WeaponMesh → Rotation | Pitch=90, Yaw=0, Roll=0 |
| WeaponMesh → Scale | (30,30,30) |
| TraceStart → Location, relativa a WeaponMesh | (0,0,0.8) |
| TraceEnd → Location, relativa a WeaponMesh | (0,0,-3.239877) |
| Trace Radius | 8 cm |

**Pendiente — usuario:** ajustar esos valores con el arma y las animaciones definitivas. Son una configuración inicial sobre Sword_10, no una calibración de alcance aprobada.

## 3. Array del DataAsset

**Hecho — Codex por MCP:** se consultaron los sockets de /Game/Characters/Mannequins/Meshes/SKM_Manny_Simple. HandGrip_R pertenece a hand_r y HandGrip_L a hand_l. No se editaron Manny ni su AnimBP.

**Hecho — Codex por MCP después del paso 1:** ObjectTools.set_properties asignó en /Game/Vamps/Data/DA_VampsCombatSettings → Combat → Weapons una entrada:

- Weapon Class: BP_VampsGreatSword.
- Attach Socket: HandGrip_R.
- Relative Transform: Location=(0,0,0), Rotation=(0,0,0), Scale=(1,1,1).

Se conservaron Attack Montage, Hit Reaction Montage, NA Recovery Duration y Combo Steps. AssetTools.save_assets confirmó guardado, ObjectTools releyó la entrada y los demás datos e is_dirty=false confirmó el paquete limpio. En esta configuración inicial ambos personajes siguen compartiendo el mismo DA y usan una greatsword.

Para un personaje con duales, configurar dos entradas con el Blueprint correspondiente a cada espada; pueden repetir la misma clase. La primera se monta en HandGrip_R y la segunda en HandGrip_L, con su ajuste de montaje propio. Cada entrada crea un actor distinto y conserva un registro de impactos independiente. La segunda arma no reduce el daño de la primera.

## 4. Personajes, ventana y limpieza

**Hecho — Codex por MCP después del paso 1:** BP_VampsCharacter y BP_VampsCharacter_Player compilaron con warnings_as_errors=true. ActorTools.get_components confirmó ausencia de FistHitbox en ambos CDO y ObjectTools confirmó su Combat Settings=DA_VampsCombatSettings en PIE. También compilaron ABP_VampsCharacter y BP_VampsGreatSword sin errores ni advertencias. Arma, personajes y DA se guardaron con is_dirty=false. No se agregó un AC de armas al personaje.

El personaje crea sus armas desde el DA durante BeginPlay. ANS_AttackTrace habilita simultáneamente las armas equipadas durante Contacts y las detiene en AttackEnd. BeginCombatStep limpia cada registro al entrar en otra sección. El montaje no necesita un notify distinto por arma. La creación desde el DA se observa al jugar; editar el DA no crea actores de preview en el Blueprint del personaje.

## 5. Comprobaciones en PIE

**Hecho — Codex por MCP, comprobación de arranque:** StartPIE en L_CharacterBasic confirmó una instancia distinta de BP_VampsGreatSword en Weapons de cada personaje. get_parent_component confirmó WeaponRoot adjunto a su CharacterMesh0 correspondiente, con configuración HandGrip_R en el DA. Ambos personajes estaban en Idle y bCanDealDamage=false; ambas armas tenían TraceRadius=8 y bStartWithTickEnabled=false. Tras StopPIE, find_actors no encontró armas. El log no mostró asserts ni errores de Blueprint/PIE; solo una sesión MCP vencida al reconectar. PIE quedó detenido.

Owner y AttachSocketName son privados y no se pueden releer con ObjectTools. La asociación se verificó con Weapons y get_parent_component, y el socket se verificó en la configuración usada por el código nativo. La comprobación no acredita todavía el agarre visual ni contactos de ataque.

**Pendiente — usuario:** con los montages preparados según [022](../022-cadena-y-na/editor.md), observar:

1. Una espada toca al mismo rival durante varios frames: aplica una sola vez AttackDamage en esa sección. Puede alcanzar también a otro rival.
2. La siguiente sección puede volver a golpear al mismo enemigo. Un salto no traza desde la posición de la sección anterior. Fuera de Contacts no hay daño.
3. Con duales, cada espada puede golpear al mismo enemigo en la misma sección: dos impactos abiertos aplican dos veces AttackDamage. Si solo una conecta, aplica una vez. No hacen falta dos pulsaciones para habilitar la continuación: conecta cualquiera de las dos.
4. El arma propia y la otra espada no reciben daño ni interfieren con la consulta. La pared conserva el filtro existente; la mesh del arma no empuja físicamente a las cápsulas.
5. Observar golpes rápidos y giros con las armas largas. Revisar que TraceStart/End cubran el filo y que TraceRadius corresponda al grosor elegido. La detección interpola entre poses evaluadas; no reconstruye poses omitidas de la animación.
6. R durante Contacts y cambio de modo de IA: detener trazas, limpiar los registros y evitar daño tardío del intercambio anterior. La nueva acción puede volver a golpear. Detener PIE y comprobar ausencia de armas sobrantes.

No hay comprobación de impactos/duales aprobada todavía; las compilaciones y la configuración del Blueprint no sustituyen esa observación.
