# 028 — Contraste de FGO Arcade con Vamps

La [revisión 029](../029-revision-paso-2/reporte.md) evalúa qué conclusiones afectan al cierre del paso 2 y cuáles corresponden a 3–4; conserva esta investigación como evidencia y señala la diferencia con la independencia por arma elegida para Vamps.

18 de octubre de 2026. Se creó una referencia práctica para implementar con código propio los sistemas de FGOA y evaluar la dirección del prototipo actual. La explicación principal está en [analisis.md](analisis.md); [evidencia.md](evidencia.md) vincula videos, datos, funciones nativas y fuentes de Vamps.

## Resultado

La base técnica es aprovechable: GAS, resolver al final del tick, trazas de armas, movimiento propio, DA y separación de cámara/selección. La carencia central es una reacción defensiva separada, con identidad entrante, duración por impacto, fuente y permisos. No se recomienda empezar de cero ni construir un administrador global que ordene IDs para elegir ganadores.

El código actual consume `bCanDefend`, aplica duración de sección atacante+adicional y conserva el mayor vencimiento; no sustituye la ofensiva por Guard ni bloquea locomoción durante todo ese plazo. Un solo ANS por sección y memoria de víctimas por arma/sección impiden representar correctamente varios subgolpes de una acción. Son diferencias respecto de la referencia; parte de ellas corresponde a simplificaciones deliberadas de las iteraciones anteriores.

La guía corrige equivalencias entre ID/acción/hit/montage, defensa por click obligatorio, tiempo basado en longitud de clip, giro defensivo3/30 y ruptura deducida del agotamiento de recurso. Distingue lo nativo probado estáticamente, lo observado en video, las inferencias y las decisiones propias de Vamps. Conserva preguntas abiertas sobre empate exacto, cobertura universal, variantes de clocks y equivalencia de builds.

## Archivos y alcance

| Archivo agregado/modificado | Contenido |
|---|---|
| `analisis.md` | Modelo accesible, diagramas, recorridos, evaluación por clase, correcciones y orden recomendado de implementación. |
| `evidencia.md` | Catálogo de13 escenas y trazas N01–N09/C01–C10. |
| `evidencia/*.mp4` y `V*-secuencia-*.jpg` | Recortes reales con HUD/inputs y láminas temporales. |
| `evidencia/manifest.json` | Fuentes y recortes de video con hashes, FPS, rangos y fotogramas. |
| `evidencia/nativo/` y `trazas-nativas.json` | Subconjunto de36 funciones previamente exportadas, C/ASM/metadatos y procedencia. |
| Tres JSON de ejemplos | Marcas de guardia, orientación/subgolpes y auditoría de campos de ataque. |
| `evidencia/fuentes-vamps.json` | Hashes de42 fuentes revisados y rutas de sus copias preservadas en research. |
| `reporte.md` / `editor.md` | Resultado, límites y comprobaciones manuales pertinentes. |
| `.docs/README.md` | Enlace a la referencia y nueva iteración documental, manteniendo027 como última de gameplay. |

No hubo cambios de código, clases, Blueprints, Data Assets ni configuración.

El documento describe las responsabilidades y conexiones de las clases existentes; los nombres conceptuales recomendados no son clases creadas. Los assets citados —`/Game/Vamps/Data/DA_VampsCombatSettings`, `/Game/Vamps/Animations/Executioner/AM_Executioner_Combo` y arma greatsword— proceden de la captura histórica027. No se asignaron referencias ni se reconfiguraron animaciones.

## Verificación y límites

Se capturaron y revisaron los42 fuentes propios de Public/Private y se compararon sus hashes al cerrar. Se verificó que el `ago.exe` actual coincide con la investigación nativa. Los13 recortes se decodificaron completos:1080 fotogramas en total, con FPS y rangos correctos; se inspeccionaron láminas de las escenas clave. Se comprobaron los hashes de111 archivos nativos/datos y260 enlaces locales, sin errores. Los [resultados de integridad](evidencia/verificacion.json) y manifiestos permiten revisar esas comprobaciones.

La evidencia de video está recodificada sin audio y sin modificar velocidad ni contenido del frame. Las funciones nativas reutilizadas son pseudocódigo provisional acompañado de instrucciones; el informe distingue el detalle temporal que sólo aparece correctamente en ASM. No se repitieron inventarios o descompilaciones ya resueltos.

No hubo validación dinámica nueva de FGOA ni observación de Vamps en PIE. No se dispuso de MCP directo del editor; no se utilizó automatización de interfaz. No se compilaron cambios inexistentes ni se ejecutaron tests. Los valores actuales de assets y los comportamientos predichos desde C++ quedan pendientes de observación, según [editor.md](editor.md).

La comparación no declara terminada ninguna etapa del roadmap. La revisión de producto pendiente es decidir qué adaptaciones previas siguen vigentes ante el objetivo actual de fidelidad. La siguiente unidad técnica recomendada es una acción con dos ventanas y Guard renovable, conservando el núcleo existente.

Notas y copias de trabajo: [research/vamps-contrast-20261018-0838](<D:/fgo arcade/fgoac/research/vamps-contrast-20261018-0838/notes/AVANCE.md>).
