---
name: game-feature-docs
description: Crear, modificar y revisar pares de documentación funcional y técnica de funcionalidades de este juego en .docs. Usar al especificar comportamiento, diseñar su implementación o refinar documentos existentes.
---

# Documentar funcionalidades del juego

Leer las [convenciones del proyecto](../../../.docs/AGENTS.md) y el [índice](../../../.docs/README.md). Abrir el par de documentos de la funcionalidad y solo las dependencias relevantes.

## Agregar o modificar

1. Elegir la carpeta por responsabilidad del juego. Reutilizar una existente si cubre la misma funcionalidad; anidar únicamente cuando ayude a separar responsabilidades.
2. Crear o actualizar `functional.md` y `technical.md`. Incorporar las decisiones proporcionadas directamente en el texto y llevar las preguntas abiertas al final del archivo que deba resolverlas.
3. En el funcional, describir las condiciones de entrada, acciones, cambios de estado y resultados observables que correspondan. Incluir ejemplos o criterios de aceptación cuando aclaren reglas difíciles; no repetirlas con otras palabras para llenar una plantilla.
4. En el técnico, conectar esas reglas con componentes, datos, flujo de ejecución, dependencias y motivos de diseño. Si existe implementación, inspeccionar los archivos relevantes antes de describirla. Referenciar las reglas funcionales en vez de transcribirlas.
5. Actualizar los enlaces y el índice cuando cambie la estructura. Al resolver una pregunta, revisar también sus consecuencias en el documento contraparte.

Si la funcionalidad solo tiene definida una parte, documentar esa parte y cerrar con las preguntas necesarias para continuar. No inventar clases, valores, decisiones de producto ni resultados de pruebas.

## Revisar

Buscar contradicciones entre comportamiento e implementación, resultados ambiguos, dependencias rotas y explicaciones duplicadas. Comprobar los casos relevantes para la funcionalidad: por ejemplo, inputs simultáneos en combate o pérdida del objetivo en una cámara que lo sigue; no aplicar un catálogo universal de casos.

Corregir redacción, organización y redundancias dentro del alcance solicitado. Si resolver un hallazgo exige elegir una regla de juego o arquitectura, formular la pregunta al final del documento pertinente. Priorizar las preguntas que desbloquean otras reglas y agrupar las que pertenecen a una misma decisión.

Comprobar que cada carpeta de funcionalidad conserva ambos archivos y que los enlaces locales resuelven. Informar de forma breve qué cambió y cuál es la siguiente pregunta que conviene resolver.
