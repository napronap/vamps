# Editor — Reinicio opcional de impactos

**Para las animaciones de un solo impacto no hay que cambiar nada.** Solo las secciones que deban volver a habilitar impactos de un arma necesitan el nuevo AN. La compilación y el registro de clase quedaron verificados por MCP; la colocación de notifies en el montage queda al usuario porque no hay una herramienta MCP directa dedicada a esa edición.

## 1. Compilación y disponibilidad

**Hecho — Codex por MCP.** LiveCodingToolset.LiveCodingToolset.CompileLiveCoding devolvió Success y aplicó un parche real. ObjectTools.list_properties/get_properties/get_class confirmaron /Script/vamps.AN_ResetWeaponHits, WeaponIndices como array de enteros, default [0] y preview desactivado. La configuración del DA coincidió antes y después, incluidos sus adicionales de 0,5 s. No se requiere una edición estructural del DA, reiniciar ni reasignar Blueprints o el AnimBP.

## 2. Configuración del montage

**Pendiente — usuario, solo para animaciones que necesiten varios impactos.**

1. Abrir el AttackMontage asignado en el DA del personaje. La referencia leída en esta iteración fue /Game/Vamps/Animations/Executioner/AM_Executioner_Combo.
2. Mantener un ANS_AttackTrace por sección. No agregar otro estado de trazas para el segundo golpe.
3. En una pista de Notifies del montage, hacer clic derecho en el momento que debe habilitar el siguiente impacto y elegir Add Notify → Reset Weapon Hits / AN_ResetWeaponHits. Debe estar estrictamente después del inicio y antes del final de AttackTrace; no colocarlo en el mismo instante que sus extremos. Usar el instante de la animación real, sin copiar un frame ilustrativo.
4. Seleccionar el AN y configurar Combat → Weapon Indices en Details: [0] para Weapons[0], [1] para Weapons[1], [0, 1] para ambas. El AN nuevo empieza en [0]. Consultar el orden real del array Weapons del DA; cambiar ese orden exige revisar los índices del montage. No dejar el array vacío. El DA leído tiene una sola arma, por lo que solo 0 es válido con esa configuración.
5. Agregar más AN únicamente cuando se necesiten más reinicios. En las secciones sin AN se mantiene una aplicación por víctima y arma hasta el siguiente paso. Guardar solo el montage editado.

El AN reinicia registros, no apaga las otras armas ni cierra trazas entre golpes. Tampoco borra que el paso ya conectó para continuar el combo. Si el arma seleccionada sigue tocando al receptor cuando se rearma, puede volver a impactar en la siguiente muestra válida.

## 3. Observación en PIE

**Pendiente — usuario.** Abrir el escenario de combate ya configurado y filtrar Output Log por LogVampsCombat.

- Sin AN, comprobar que el contacto sostenido de un arma no repita daño durante la sección y que el próximo paso pueda volver a impactar.
- En una sección con AN [0], comprobar que esa arma pueda impactar otra vez después del notify y que la continuación siga decidiéndose al cerrar Combo Window. Un reinicio por sí solo no debe contar como acierto.
- Cuando se configure un personaje con dos armas, comprobar que ambas impacten individualmente y que un AN [0] solo rearme la primera; [0, 1] debe rearmar ambas. No se configuró un segundo arma como parte de esta entrega.

No se declara observada una repetición real de daño por la sola compilación o inspección de propiedades. El paso 3 de guardia sigue separado de esta implementación.
