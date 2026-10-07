# 007 — Uso de scripts de build y editor

No hay operaciones de assets pendientes por esta revisión. Los scripts conservan su contenido original; las correcciones recomendadas están en el [reporte](reporte.md).

1. **Hecho — usuario:** probar `build.bat` y `editor.bat`. El usuario confirmó que funcionan; Codex realizó únicamente revisión estática.
2. **Pendiente — usuario:** antes de un build externo completo, cerrar Unreal y ejecutar `build.bat` desde la raíz. El target es `vampsEditor Win64 Development`. En su estado actual solicita dos builds por la llamada incluida en `vars.bat`.
3. **Pendiente — usuario:** abrir mediante `editor.bat` o `editor.bat -log` para añadir la ventana de log. Actualmente este script solicita primero un build y abre UE aunque ese build falle. `start` retorna sin esperar al cierre del editor; su éxito no confirma que el proyecto haya cargado.
4. Para cambios compatibles con Live Coding mientras UE está abierto, Codex debe usar `LiveCodingToolset.LiveCodingToolset.CompileLiveCoding` y comprobar si hubo parche real o `NoChanges`. No se realizó esta operación durante la revisión.
