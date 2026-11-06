# Fundido por End Recovery en Unreal

**Queda para el usuario.** En `L_LocalMatch`, probar un ataque aislado y un combo de Lancer para observar el cierre, el fundido y NA. Los tiempos de los notifies se revisan en la pista del montage.

## 1. Compilar

**Hecho — Codex por MCP.** Con PIE detenido, `LiveCodingToolset.LiveCodingToolset.CompileLiveCoding` compiló y aplicó el parche. Para reproducir la compilación desde Unreal, detener PIE y pulsar **Compile** con Live Coding habilitado.

## 2. Configurar los montages de ataque

**Hecho — Codex por MCP.** `AssetTools.find_assets` y `ObjectTools.get_properties` identificaron las referencias `Attack Montage` de los cuatro perfiles.

| Perfil | Montage |
| --- | --- |
| Lancer | `/Game/Vamps/Animations/Impaler/AM_Impaler_Combo` |
| Dual Swords | `/Game/Vamps/Animations/Reaver/AM_Reaver_combo` |
| Executioner | `/Game/Vamps/Animations/Executioner/AM_Executioner_Combo` |
| Sword and Shield | `/Game/Vamps/Animations/Warden/AM_Warden_Combo` |

**Hecho — Codex por MCP.** `ObjectTools.set_properties` desactivó `bEnableAutoBlendOut` y `AssetTools.save_assets` guardó los cuatro montages. Para repetirlo en otro montage de ataque, abrir **Asset Details → Blend Option**, desmarcar **Enable Auto Blend Out** y guardar. `Blend Out` define la duración del fundido que inicia el cierre. Los cuatro montages conservan su valor de 0,25 s.

## 3. Comprobar el cierre

**Pendiente — usuario.** Abrir `AM_Impaler_Combo` y comprobar que `End Recovery` en `Default` está después del fin de `Attack Trace` y `Combo Window`, y antes de `1,333333 s`. Con los tiempos del error anterior, `1,173398 s` es válido. Para el resto de los golpes se aplica el mismo orden con los límites de su propia sección. Los montages requieren un `End Recovery` por sección; consultar el reporte para el resto de los requisitos.

1. Iniciar PIE en `/Game/Vamps/Maps/L_LocalMatch`, seleccionar Lancer y pulsar START. Hacer un ataque aislado. Al marcador deben empezar el fundido y NA. Comprobar que vuelve el movimiento y empieza el regreso de la cámara al plano general.
2. Conectar un golpe y pulsar dentro de `Combo Window`. Debe reproducirse la sección siguiente sin iniciar NA entre golpes. Dejar que el último golpe ejecutado llegue a su marcador y comprobar el cierre.
3. Ante un rechazo, buscar `LogVampsCombat` en Output Log. El diagnóstico indica el montage o la sección y el requisito incumplido. Detener PIE al terminar.
