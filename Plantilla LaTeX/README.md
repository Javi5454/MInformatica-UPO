# Plantilla LaTeX — Máster en Ingeniería Informática (IA), UPO

Plantilla para prácticas, actividades, memorias y trabajos del curso **2026/2027**, con la identidad corporativa de la Universidad Pablo de Olavide.

| Fichero | Qué es |
|---|---|
| `upo-master.cls` | La clase: estilos, portada, cabecera y pie, cajas, código, algoritmos y la **declaración de uso de IA** |
| `memoria.tex` | Plantilla para memorias largas (portada completa + índice) |
| `actividad.tex` | Plantilla para entregas cortas (cabecera compacta, ejercicios numerados) |
| `bibliografia.bib`, `unsrt-es.bst` | Bibliografía de ejemplo y estilo bibliográfico en español |
| `img/logo-upo.png` | Logo de la UPO (versión principal horizontal) |

## Uso

1. Copia `upo-master.cls`, `unsrt-es.bst`, `img/logo-upo.png` y `memoria.tex` o `actividad.tex` en la carpeta de la entrega (o usa `\rutaLogo{...}` para apuntar al logo de esta carpeta).
2. Rellena los datos del preámbulo (`\asignatura`, `\tipodocumento`, `\title`, ...).
3. **Revisa en cada entrega las líneas `\herramientaIA{...}`.**
4. Compila con `latexmk -pdf memoria.tex` (o pdfLaTeX → BibTeX → pdfLaTeX ×2). Funciona también en Overleaf y con XeLaTeX/LuaLaTeX.

### Opciones de la clase

- `cabecera`: sin página de portada, sólo un bloque de título arriba de la primera página.
- `sinindice`: sin índice.
- `twoside`: maquetación a doble cara para imprimir.

### Entornos y comandos útiles

`concepto{Título}`, `aviso`, `nota`, `enunciado`, `\ejercicio[título]` + `respuesta`, `\captura{fichero.png}{pie}`, `definicion`, `teorema`, `ejemplo`, estilos de código `python` y `bash`, `algorithm` en español, y atajos `\R`, `\E`, `\argmin`, `\argmax`, `\vect{x}`.

## Declaración de uso de IA (siempre presente)

- `\herramientaIA{Herramienta}{Modelo}{Finalidad}`: una línea por herramienta. Si no hay ninguna, el documento declara que no se ha usado IA.
- `\usoIA{...}`: texto libre opcional con más detalle.
- `\declaracionIA`: coloca la declaración. **Si se te olvida, la clase la añade automáticamente al final** (con un aviso en el log).

## Identidad corporativa

Colores del [Manual de Identidad Corporativa de la UPO](https://www.upo.es/comunicacion/mic-upo/colores-UPO/):

| Color | Pantone | CMYK | RGB | HEX | En LaTeX |
|---|---|---|---|---|---|
| Azul | 281 C | 100/72/0/38 | 0/55/114 | `#003772` | `upoazul` |
| Amarillo | 123 C | 0/27/94/0 | 252/193/0 | `#FCC100` | `upoamarillo` |

El logo es la versión principal horizontal, disponible en el [apartado de marca de la UPO](https://www.upo.es/comunicacion/mic-upo/marca-UPO/). Su uso se rige por el *Reglamento Regulador del Uso de la Imagen Institucional* de la UPO (BUPO nº 11).
