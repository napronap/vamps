# Editor — Barrido de caja para armas

1. **Pendiente — usuario.** Con Unreal cerrado, compilar `vampsEditor` en `Win64 Development` y volver a abrir el proyecto. El editor y su MCP estaban desconectados durante la limpieza de C++.
2. **Pendiente — usuario.** En los Blueprints de la lista, revisar los ajustes guardados de `TraceBox`. En **Shape → Box Extent**, cada valor define la mitad del tamaño en su eje. **Transform → Rotation** orienta la caja respecto de `WeaponRoot`. Mantener **Collision Enabled** en **No Collision**. `TraceStart` y `TraceEnd` fijan las dos posiciones del centro durante el barrido. La ubicación de `TraceBox` sirve de referencia visual. Compilar y guardar los Blueprints que requieran ajustes.
3. **Pendiente — usuario.** En `L_CharacterBasic`, probar un golpe a alcance y otro fuera de alcance con PIE. En `L_LocalMatch`, comprobar que un tajo puede alcanzar a dos rivales y que una pared bloquea el contacto. Confirmar que cada arma golpea una sola vez por objetivo hasta el siguiente reinicio de impactos. Detener PIE al terminar.

## Blueprints de arma

- `/Game/Vamps/Blueprints/Weapons/GreatSword/BP_VampsGreatSword`
- `/Game/Vamps/Blueprints/Weapons/Spear/BP_VampsSpear`
- `/Game/Vamps/Blueprints/Weapons/SwordAndShield/BP_VampsSword`
- `/Game/Vamps/Blueprints/Weapons/SwordAndShield/BP_VampsShield`
- `/Game/Vamps/Blueprints/Weapons/DualSwords/BP_VampsDualRH`
- `/Game/Vamps/Blueprints/Weapons/DualSwords/BP_VampsDualLH`
