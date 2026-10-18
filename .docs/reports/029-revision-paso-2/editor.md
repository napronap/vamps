# Editor — Revisión del paso 2

El AN elegido se implementó después en [030 — Guía de reinicio de impactos](../030-reinicio-impactos/editor.md). La comparación y los estados de esta guía corresponden a la revisión previa.

**No quedan operaciones de editor necesarias para esta entrega.** Es una revisión documental y de fuentes: no requiere compilar, reiniciar Unreal, guardar assets ni cambiar montages. No hubo operaciones realizadas por MCP ni una nueva sesión de PIE.

El [reporte](reporte.md) distingue el ajuste recomendado para cerrar 2 con múltiples impactos de las reglas defensivas que se deben resolver en 3. No se debe agregar un segundo ANS_AttackTrace al montage actual como solución: la validación y la transición de fase todavía lo rechazan.

La comparación posterior incluye la alternativa de un AN de reinicio selectivo por arma. Sigue siendo diseño: todavía no existe ese notify ni soporte para varias ventanas por sección. No hay nuevos pasos de configuración para el usuario.

Las observaciones opcionales del código actual siguen en [028 — Guía del editor](../028-contraste-fgoa/editor.md); no se repiten como pendientes nuevos ni se declaran realizadas. No se modificó el estado del roadmap.

Si se encarga la ampliación de impactos y afecta la clase del DA o sus structs, primero corresponde inventariar y guardar su configuración real por MCP según AGENTS.md. La captura histórica de 027 no sustituye esa lectura. Los pasos y valores concretos de una futura migración se documentarán con su implementación.
