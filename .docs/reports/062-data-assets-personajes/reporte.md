# 062 — Data Assets por personaje

La locomoción continúa en [063 — BlendSpace configurable y ABP compartido](../063-locomocion-configurable/reporte.md), con [operaciones del editor](../063-locomocion-configurable/editor.md). Agrega el BS por DA conservando todos los parámetros y referencias de esta iteración.

## Resultado

Se renombró el DA existente como `/Game/Vamps/Data/DA_ExecutionerCombatSettings` y se crearon tres perfiles independientes de clase nativa `UVampsCombatSettings`: `DA_DualSwordsCombatSettings`, `DA_SwordAndShieldCombatSettings` y `DA_LancerCombatSettings`, todos en `/Game/Vamps/Data/`.

Executioner conserva todos sus valores, referencias, orden de arrays y transform del arma. Los tres nuevos tienen tres `ComboSteps`, parámetros iniciales según su arma, `Weapons` vacío y sus siete referencias de montage en `None`. El usuario creará y asignará los montages. Solo existe `BP_VampsGreatSword`; no se asignó ese mandoble a los otros perfiles.

No se modificaron fuentes C++, clases reflejadas, structs, GameModes, mapas, selector ni paquetes de animaciones importados por el usuario. No se requieren compilación ni reinicio para estos cambios de assets.

## Assets y conexiones

| Asset | Cambio y responsabilidad |
|---|---|
| `Data/DA_VampsCombatSettings` → `Data/DA_ExecutionerCombatSettings` | Configuración específica de Executioner; conserva `AM_Executioner_Combo`, reacciones, muertes, dash, `BP_VampsGreatSword`, socket `HandGrip_R` y sus valores calibrados. |
| `Data/DA_DualSwordsCombatSettings` | Perfil corto y ágil: menor restricción de ataque, menor consumo de movilidad, más críticos y menor defensa. |
| `Data/DA_SwordAndShieldCombatSettings` | Perfil defensivo: más capacidad de guardia, reducción de daño y recuperación de guardia; menor velocidad de sprint. |
| `Data/DA_LancerCombatSettings` | Perfil de alcance largo, mayor separación y empuje final, con recuperación de ataque algo más larga. |
| `Blueprints/BP_VampsCharacter` y `Blueprints/BP_VampsCharacter_Player` | Referencias actualizadas y guardadas hacia `DA_ExecutionerCombatSettings`; mantienen animación, arma, daño base 12 y vida inicial 100. |

Todas las rutas de la tabla son relativas a `/Game/Vamps/`. Las clases existentes siguen separando responsabilidades: `AVampsCharacter` consume el DA y equipa armas; `UGA_Attack` usa montage y steps; el resolver consume reacción, empuje y guardia; movilidad consume energía, sprint y acercamiento. El daño base y la vida están en el personaje, no en este DA: no se añadieron campos ni se cambiaron esos valores.

## Parámetros iniciales

Distancias en cm, duraciones en segundos. Los valores completos están en [valores-nuevos.json](valores-nuevos.json) y su lectura guardada en [verificacion-final.json](verificacion-final.json).

| Parámetro | Executioner, conservado | Dual Swords | Sword and Shield | Lancer |
|---|---|---|---|---|
| Alineación steps 1/2/3 | 130 / 130 / 150 | 90 / 90 / 100 | 110 / 110 / 120 | 190 / 200 / 220 |
| Empuje steps 1/2/3 | 100 / 100 / 300 | 40 / 50 / 180 | 60 / 70 / 220 | 80 / 100 / 280 |
| Reacción steps 1/2/3 | 1 / 1,3 / 1 | 0,35 / 0,4 / 0,6 | 0,5 / 0,55 / 0,7 | 0,55 / 0,6 / 0,8 |
| NA | 0,5 | 0,35 | 0,5 | 0,55 |
| Daño crítico / probabilidad % | 24 / 10 | 22 / 20 | 22 / 8 | 26 / 12 |
| Reducción de daño bloqueado / capacidad de guardia | 10 / 100 | 6 / 70 | 14 / 150 | 8 / 90 |
| Carga de guardia por impacto saliente | 25 | 18 | 24 | 28 |
| Sprint / coste de dash | 700 / 25 | 760 / 20 | 650 / 25 | 700 / 25 |
| Acercamiento distancia / duración / parada | 250 / 0,25 / 130 | 280 / 0,2 / 95 | 220 / 0,25 / 110 | 320 / 0,3 / 200 |

Los tres perfiles nuevos tienen empuje final mayor que los otros dos golpes y valores positivos en los campos que lo requieren. Son bases para ajustar junto con los futuros montages; todavía no son personajes jugables conectados al selector.

## GameModes y verificación

- Se descubrieron por MCP los assets de `VampsCombatSettings` y sus subclases. Solo existía el DA original. Se guardaron una [captura completa previa](antes.json) y el asset en disco antes de renombrarlo.
- `AssetTools.move`, `DataAssetTools.create` y `ObjectTools.set_properties` realizaron los cambios. `AssetTools.save_assets` confirmó el guardado de los cuatro DAs y los dos Blueprints afectados. La relectura completa de Executioner coincide exactamente con la captura previa; todos esos assets terminaron sin cambios pendientes de guardar.
- Las dependencias guardadas de ambos personajes apuntan directamente al nuevo nombre. El asset antiguo ya no está en el directorio de datos; no se añadieron redirects en configuración ni referencias al nombre antiguo en código/configuración.
- Los tres GameModes conservan sus propiedades consultadas, incluidas clases de pawn, controller y HUD; `BP_VampsCombatTestGameMode` conserva oponente/IA/spawns y `BP_VampsMatchGameMode` conserva bot, puntuación y duraciones. Comparación antes/después en [verificacion-final.json](verificacion-final.json).
- `L_CharacterBasic` conserva override `BP_VampsCombatTestGameMode`. PIE arrancó con jugador, oponente y sus armas, ambos usando el DA renombrado, vivos y en `Idle`: [lectura](pie-observacion.json).
- `L_LocalMatch` conserva override `BP_VampsMatchGameMode`; también sigue siendo mapa inicial y por defecto del proyecto. PIE arrancó en `Selecting` con cuatro participantes 2v2, vivos, equipados y usando Executioner: [lectura](verificacion-final.json).
- `BP_VampsGameMode` conserva el pawn jugador de Executioner y sus clases previas; se revisó su configuración, sin abrir otro mapa para ejecutarlo.
- Se detuvieron ambas sesiones PIE y se confirmó `IsPIERunning=false`. Quedó abierto `L_LocalMatch`, el mapa original. No se agregaron ni ejecutaron tests ni se automatizaron inputs.

## Pendientes del usuario

Crear y asignar montages y armas de los perfiles nuevos, y calibrar sus valores según [editor.md](editor.md). Los GameModes actuales siguen usando solo Executioner para conservar el flujo existente. El montage de Executioner conserva el pendiente de `End Recovery` de [061](../061-fin-recovery-notify/editor.md); estas verificaciones confirman arranque y referencias, no la ejecución completa de ataques.
