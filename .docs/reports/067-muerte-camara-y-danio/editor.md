# Muerte, cámara y daño en Unreal

**Queda para el usuario.** Cerrar Unreal, ejecutar `C:\dev\vamps\build.bat` y reabrir con `editor.bat` para verificar la migración por MCP. Los perfiles ya están respaldados y guardados.

## 1. Capturar la configuración antes de editar el daño

**Hecho — usuario.** Abrió el proyecto con `editor.bat` y dejó PIE detenido. Se confirmó que MCP estaba disponible.

**Hecho — Codex por MCP.** `ObjectTools.search_subclasses` y `AssetTools.find_assets` identificaron los cuatro perfiles existentes. `ObjectTools.list_properties/get_properties` capturó las 30 propiedades de cada DA y los defaults de personaje. `AssetTools.save_assets` guardó exclusivamente esos cuatro paquetes y `is_dirty` confirmó que quedaron guardados. [Respaldo previo a la edición](danio-antes.json).

## 2. Compilar y reabrir tras la migración

**Pendiente — usuario.** Cerrar Unreal, ejecutar `C:\dev\vamps\build.bat` y abrir el proyecto otra vez con `editor.bat` al terminar la compilación. Dejar MCP disponible con PIE detenido. La compilación limpia carga el nuevo campo del DA y retira las propiedades reflejadas anteriores.

**Pendiente — Codex por MCP.** Releer los DAs tras la carga limpia y comparar la configuración completa con la copia previa. Restaurar únicamente diferencias y comprobar **Damage → Attack Damage = 12** en los cuatro perfiles. Guardar los assets afectados y releerlos para verificar el resultado.

## 3. Comprobar en juego

**Pendiente — usuario.** En `/Game/Vamps/Maps/L_CharacterBasic`, observar una muerte normal y una por contacto bloqueado. Pulsar R y comprobar que termina la pose de muerte, se recupera la vida y vuelve el movimiento y la colisión.

**Pendiente — usuario.** En `/Game/Vamps/Maps/L_LocalMatch`, comprobar que la cámara conserva posición y rotación al morir, aun con WASD y mouse, y vuelve a seguir al jugador al reaparecer. Probar una baja que cierre la ronda y verificar que completa su animación. Juntar varios combatientes cerca de la cámara y acercarse también a una pared para distinguir el barrido frente a personajes y geometría. Detener PIE al terminar.
