% =====================================================================
%  upo-master.cls
%  Clase LaTeX para prácticas, actividades, memorias y trabajos del
%  Máster Universitario en Ingeniería Informática (Especialidad en
%  Inteligencia Artificial) - Universidad Pablo de Olavide, 2026/2027.
%
%  Funciona con pdfLaTeX, XeLaTeX y LuaLaTeX (y en Overleaf).
%  La declaración de uso de IA se incluye SIEMPRE: si no se llama a
%  \declaracionIA en el documento, se añade automáticamente al final.
% =====================================================================
\NeedsTeXFormat{LaTeX2e}
\ProvidesClass{upo-master}[2026/11/22 Plantilla Master Ingenieria Informatica UPO]

% ---------- Opciones de la clase ----------
% portada   : portada completa en página propia (por defecto)
% cabecera  : sin página de portada, sólo un bloque de título compacto
%             (útil para actividades cortas de 1-3 páginas)
% sinindice : no genera el índice
% oneside / twoside : se pasan a article
\newif\ifupo@portada \upo@portadatrue
\newif\ifupo@indice  \upo@indicetrue
\DeclareOption{portada}{\upo@portadatrue}
\DeclareOption{cabecera}{\upo@portadafalse}
\DeclareOption{sinindice}{\upo@indicefalse}
\DeclareOption*{\PassOptionsToClass{\CurrentOption}{article}}
\ProcessOptions\relax
\LoadClass[11pt,a4paper]{article}

% ---------- Codificación y fuentes según el motor ----------
\RequirePackage{iftex}
\ifPDFTeX
  \RequirePackage[utf8]{inputenc}
  \RequirePackage[T1]{fontenc}
  \RequirePackage{lmodern}
\else
  \RequirePackage{fontspec}
\fi
\RequirePackage[spanish,es-noshorthands,es-tabla]{babel}
\RequirePackage{microtype}
\RequirePackage{csquotes}

% ---------- Paquetes generales ----------
\RequirePackage[margin=2.5cm,headheight=14pt]{geometry}
\RequirePackage{graphicx}
\RequirePackage[table]{xcolor}
\RequirePackage{amsmath,amssymb,amsthm}
\RequirePackage{booktabs}
\RequirePackage{tabularx}
\RequirePackage{array}
\RequirePackage{enumitem}
\RequirePackage{caption}
\RequirePackage{subcaption}
\RequirePackage{float}
\RequirePackage{listings}
\RequirePackage[most]{tcolorbox}
\RequirePackage{tikz}
\usetikzlibrary{arrows.meta,positioning,fit,backgrounds,shapes.geometric}
\RequirePackage{fancyhdr}
\RequirePackage{titlesec}
\RequirePackage{lastpage}
\RequirePackage{etoolbox}
\RequirePackage{algorithm}
\RequirePackage{algpseudocode}
\RequirePackage{url}
\RequirePackage[hidelinks,pdfusetitle]{hyperref}
\RequirePackage{bookmark}

\setlength{\emergencystretch}{3em}
\setlength{\parskip}{0.4em}
\captionsetup{font=small,labelfont={bf,color=upoazul}}

% ---------- Colores ----------
% Colores corporativos oficiales (Manual de Identidad Corporativa UPO):
%   Azul     Pantone 281 C · CMYK 100/72/0/38 · RGB 0/55/114  · #003772
%   Amarillo Pantone 123 C · CMYK 0/27/94/0   · RGB 252/193/0 · #FCC100
\definecolor{upoazul}{RGB}{0,55,114}
\definecolor{upoamarillo}{RGB}{252,193,0}
\definecolor{upogris}{RGB}{90,90,90}
\definecolor{codigofondo}{RGB}{245,245,245}
\definecolor{codigocomentario}{RGB}{0,128,0}
\definecolor{codigoclave}{RGB}{0,0,170}
\definecolor{codigocadena}{RGB}{163,21,21}

% ---------- Datos del documento ----------
% Se rellenan en el preámbulo del .tex con los comandos de abajo.
\newcommand{\upo@asignatura}{Asignatura}
\newcommand{\upo@tipo}{Práctica}
\newcommand{\upo@titulo}{Título del documento}
\newcommand{\upo@subtitulo}{}
\newcommand{\upo@autor}{Nombre del alumno}
\newcommand{\upo@correo}{}
\newcommand{\upo@profesor}{}
\newcommand{\upo@fecha}{\today}
\newcommand{\upo@grupo}{}
\newcommand{\upo@cortohead}{}

\newcommand{\asignatura}[1]{\renewcommand{\upo@asignatura}{#1}}
\newcommand{\tipodocumento}[1]{\renewcommand{\upo@tipo}{#1}}
\renewcommand{\title}[1]{\renewcommand{\upo@titulo}{#1}}
\newcommand{\subtitulo}[1]{\renewcommand{\upo@subtitulo}{#1}}
\renewcommand{\author}[1]{\renewcommand{\upo@autor}{#1}}
\newcommand{\correo}[1]{\renewcommand{\upo@correo}{#1}}
\newcommand{\profesor}[1]{\renewcommand{\upo@profesor}{#1}}
\renewcommand{\date}[1]{\renewcommand{\upo@fecha}{#1}}
\newcommand{\grupo}[1]{\renewcommand{\upo@grupo}{#1}}
\newcommand{\tituloCorto}[1]{\renewcommand{\upo@cortohead}{#1}}

% Datos fijos del máster
\newcommand{\upoUniversidad}{Universidad Pablo de Olavide}
\newcommand{\upoMaster}{Máster Universitario en Ingeniería Informática}
\newcommand{\upoEspecialidad}{Especialidad en Inteligencia Artificial}
\newcommand{\upoCurso}{Curso 2026/2027}
\newcommand{\upoModalidad}{Modalidad online}

% Para usar los datos dentro del texto
\newcommand{\elAutor}{\upo@autor}
\newcommand{\laAsignatura}{\upo@asignatura}
\newcommand{\elTitulo}{\upo@titulo}

% ---------- Títulos de sección ----------
\titleformat{\section}{\Large\bfseries\color{upoazul}}{\thesection}{0.8em}{}[{\color{upoamarillo}\titlerule[1.2pt]}]
\titleformat{\subsection}{\large\bfseries\color{upoazul}}{\thesubsection}{0.8em}{}
\titleformat{\subsubsection}{\normalsize\bfseries}{\thesubsubsection}{0.8em}{}
\titlespacing*{\section}{0pt}{1.6em}{0.9em}

% ---------- Cabecera y pie ----------
\pagestyle{fancy}
\fancyhf{}
\fancyhead[L]{\small\color{upogris}\upo@asignatura}
\fancyhead[R]{\small\color{upogris}\ifx\upo@cortohead\@empty\upo@tipo\else\upo@cortohead\fi}
\fancyfoot[L]{\small\color{upogris}\upo@autor}
\fancyfoot[R]{\small\color{upogris}\thepage{} / \pageref*{LastPage}}
\renewcommand{\headrulewidth}{0.8pt}
\renewcommand{\footrulewidth}{0.4pt}
\let\upo@headrule\headrule
\let\upo@footrule\footrule
\renewcommand{\headrule}{{\color{upoamarillo}\upo@headrule}}
\renewcommand{\footrule}{{\color{upoazul}\upo@footrule}}
\fancypagestyle{plain}{%
  \fancyhf{}%
  \fancyfoot[R]{\small\color{upogris}\thepage{} / \pageref*{LastPage}}%
  \renewcommand{\headrulewidth}{0pt}%
}

% ---------- Logo ----------
% Marca oficial UPO (versión principal horizontal, a dos colores),
% descargada de https://www.upo.es/comunicacion/mic-upo/marca-UPO/
% Por defecto se busca en img/logo-upo.{pdf,png,jpg}. Si el documento
% está en otra carpeta, indica la ruta sin extensión en el preámbulo:
%   \rutaLogo{../../Plantilla LaTeX/img/logo-upo}
% Si no existe, la portada se genera igualmente sin logo.
\newcommand{\upo@rutalogo}{img/logo-upo}
\newcommand{\rutaLogo}[1]{\renewcommand{\upo@rutalogo}{#1}}
\newcommand{\upo@logo}[1]{%
  \IfFileExists{\upo@rutalogo.pdf}{\includegraphics[width=#1]{\upo@rutalogo.pdf}}{%
  \IfFileExists{\upo@rutalogo.png}{\includegraphics[width=#1]{\upo@rutalogo.png}}{%
  \IfFileExists{\upo@rutalogo.jpg}{\includegraphics[width=#1]{\upo@rutalogo.jpg}}{}}}}

% ---------- Portada ----------
\newcommand{\upo@portadacompleta}{%
  \begin{titlepage}
    % Franjas corporativas (azul + amarillo) en el margen izquierdo
    \begin{tikzpicture}[remember picture, overlay]
      \fill[upoazul] (current page.north west) rectangle ([xshift=1.1cm]current page.south west);
      \fill[upoamarillo] ([xshift=1.1cm]current page.north west) rectangle ([xshift=1.4cm]current page.south west);
    \end{tikzpicture}%
    \centering
    \upo@logo{0.55\textwidth}\par
    \vspace*{1.2cm}
    {\large\color{upoazul}\bfseries \upoMaster\par}
    \vspace{0.1cm}
    {\large\itshape\color{upogris} \upoEspecialidad\par}
    \vspace{2.2cm}
    {\color{upoamarillo}\rule{\textwidth}{2pt}\par}
    \vspace{0.5cm}
    {\large\color{upogris}\upo@asignatura\par}
    \vspace{0.4cm}
    {\Huge\bfseries\color{upoazul} \upo@titulo\par}
    \ifx\upo@subtitulo\@empty\else
      \vspace{0.4cm}{\Large \upo@subtitulo\par}
    \fi
    \vspace{0.4cm}
    {\color{upoamarillo}\rule{\textwidth}{2pt}\par}
    \vspace{0.4cm}
    \colorbox{upoazul}{\large\bfseries\color{white}\strut\ \upo@tipo\ }\par
    \vfill
    \begin{tabular}{>{\bfseries}r l}
      Autor: & \upo@autor \\
      \ifx\upo@correo\@empty\else Correo: & \href{mailto:\upo@correo}{\nolinkurl{\upo@correo}} \\ \fi
      \ifx\upo@grupo\@empty\else Grupo: & \upo@grupo \\ \fi
      \ifx\upo@profesor\@empty\else Profesorado: & \upo@profesor \\ \fi
      Fecha: & \upo@fecha \\
    \end{tabular}
    \vspace{1cm}

    {\color{upogris}\upoCurso{} · \upoModalidad\par}
  \end{titlepage}}

\newcommand{\upo@portadacompacta}{%
  \thispagestyle{plain}%
  \begin{tcolorbox}[enhanced, colback=upoazul!4, colframe=upoazul,
      boxrule=0.8pt, arc=2pt, left=10pt, right=10pt, top=8pt, bottom=8pt]
    \begin{minipage}[c]{0.36\textwidth}\upo@logo{\linewidth}\end{minipage}\hfill
    \begin{minipage}[c]{0.6\textwidth}\raggedleft\small
      \textbf{\color{upoazul}\upoMaster}\\
      \textit{\upoEspecialidad}\\
      \upoCurso{} · \upoModalidad
    \end{minipage}
    \par\vspace{6pt}{\color{upoamarillo}\rule{\linewidth}{1.5pt}}\par\vspace{4pt}
    {\color{upogris}\upo@asignatura{} --- \upo@tipo\par}
    {\LARGE\bfseries\color{upoazul} \upo@titulo\par}
    \ifx\upo@subtitulo\@empty\else{\large \upo@subtitulo\par}\fi
    \vspace{6pt}
    {\small \textbf{Autor:} \upo@autor
      \ifx\upo@correo\@empty\else{} (\href{mailto:\upo@correo}{\nolinkurl{\upo@correo}})\fi
      \ifx\upo@profesor\@empty\else\quad\textbf{Profesorado:} \upo@profesor\fi
      \hfill \upo@fecha\par}
  \end{tcolorbox}
  \vspace{0.5em}}

\renewcommand{\maketitle}{%
  \hypersetup{pdftitle={\upo@titulo}, pdfauthor={\upo@autor}, pdfsubject={\upo@asignatura}}%
  \ifupo@portada
    % Numeración aparte para la portada (evita destinos duplicados en hyperref)
    \pagenumbering{Alph}%
    \upo@portadacompleta
    \pagenumbering{arabic}%
    \ifupo@indice\tableofcontents\newpage\fi
  \else
    \upo@portadacompacta
    \ifupo@indice\tableofcontents\vspace{1em}\fi
  \fi}

% ---------- Entornos de apoyo ----------
\newtcolorbox{concepto}[1]{colback=upoazul!5, colframe=upoazul,
  fonttitle=\bfseries, title={#1}, breakable}
\newtcolorbox{aviso}[1][Ojo]{colback=upoamarillo!12, colframe=upoamarillo!85!black,
  coltitle=black, fonttitle=\bfseries, title={#1}, breakable}
\newtcolorbox{nota}[1][Nota]{colback=upoazul!3, colframe=upoazul!50,
  fonttitle=\bfseries, title={#1}, breakable}
\newtcolorbox{enunciado}[1][Enunciado]{colback=gray!6, colframe=upogris,
  fonttitle=\bfseries, title={#1}, breakable}

% Ejercicios/preguntas numerados (para actividades tipo cuestionario)
\newcounter{ejercicio}
\newcommand{\ejercicio}[1][]{%
  \refstepcounter{ejercicio}%
  \subsection*{\color{upoazul}Ejercicio \theejercicio\ifx\relax#1\relax\else: #1\fi}%
  \addcontentsline{toc}{subsection}{Ejercicio \theejercicio\ifx\relax#1\relax\else: #1\fi}}
\newenvironment{respuesta}{\par\noindent\textbf{Respuesta.}\ }{\par}

% Teoremas y definiciones en español
\theoremstyle{definition}
\newtheorem{definicion}{Definición}[section]
\newtheorem{ejemplo}{Ejemplo}[section]
\theoremstyle{plain}
\newtheorem{teorema}{Teorema}[section]
\newtheorem{proposicion}[teorema]{Proposición}
\newtheorem{lema}[teorema]{Lema}

% Atajos matemáticos habituales en IA
\newcommand{\R}{\mathbb{R}}
\newcommand{\N}{\mathbb{N}}
\newcommand{\E}{\mathbb{E}}
\DeclareMathOperator*{\argmin}{arg\,min}
\DeclareMathOperator*{\argmax}{arg\,max}
\newcommand{\vect}[1]{\boldsymbol{#1}}

% ---------- Imágenes ----------
% \captura[opciones]{fichero}{pie}: muestra img/<fichero> si existe;
% si no, un recuadro con el nombre para acordarse de añadirla.
\newcommand{\captura}[3][width=0.8\textwidth,height=0.4\textheight,keepaspectratio]{%
  \begin{figure}[htbp]\centering
  \IfFileExists{img/#2}{\includegraphics[#1]{img/#2}}{%
    \fbox{\parbox[c][5cm][c]{0.8\textwidth}{\centering\ttfamily img/\detokenize{#2}}}}%
  \caption{#3}\label{fig:#2}
  \end{figure}}

% ---------- Código ----------
\lstset{
  basicstyle=\ttfamily\small,
  backgroundcolor=\color{codigofondo},
  commentstyle=\color{codigocomentario}\itshape,
  keywordstyle=\color{codigoclave}\bfseries,
  stringstyle=\color{codigocadena},
  numberstyle=\tiny\color{upogris},
  frame=single, rulecolor=\color{gray!40},
  breaklines=true, columns=fullflexible, keepspaces=true,
  showstringspaces=false, tabsize=4, captionpos=b,
  literate={á}{{\'a}}1 {é}{{\'e}}1 {í}{{\'i}}1 {ó}{{\'o}}1 {ú}{{\'u}}1
           {Á}{{\'A}}1 {É}{{\'E}}1 {Í}{{\'I}}1 {Ó}{{\'O}}1 {Ú}{{\'U}}1
           {ñ}{{\~n}}1 {Ñ}{{\~N}}1 {ü}{{\"u}}1 {¿}{{?`}}1 {¡}{{!`}}1
}
\renewcommand{\lstlistingname}{Código}
\renewcommand{\lstlistlistingname}{Índice de códigos}
\lstdefinestyle{python}{language=Python, numbers=left}
\lstdefinestyle{bash}{language=bash}

% ---------- Algoritmos en español ----------
\floatname{algorithm}{Algoritmo}
\renewcommand{\listalgorithmname}{Índice de algoritmos}
\algrenewcommand\algorithmicrequire{\textbf{Entrada:}}
\algrenewcommand\algorithmicensure{\textbf{Salida:}}
\algrenewcommand\algorithmicrepeat{\textbf{repetir}}
\algrenewcommand\algorithmicuntil{\textbf{hasta que}}
\algrenewcommand\algorithmicwhile{\textbf{mientras}}
\algrenewcommand\algorithmicfor{\textbf{para}}
\algrenewcommand\algorithmicforall{\textbf{para todo}}
\algrenewcommand\algorithmicdo{\textbf{hacer}}
\algrenewcommand\algorithmicif{\textbf{si}}
\algrenewcommand\algorithmicthen{\textbf{entonces}}
\algrenewcommand\algorithmicelse{\textbf{si no}}
\algrenewcommand\algorithmicend{\textbf{fin}}
\algrenewcommand\algorithmicreturn{\textbf{devolver}}
\algrenewcommand\algorithmicfunction{\textbf{función}}
\algrenewcommand\algorithmicprocedure{\textbf{procedimiento}}

% =====================================================================
%  DECLARACIÓN DE USO DE INTELIGENCIA ARTIFICIAL (obligatoria)
% =====================================================================
%  En el preámbulo:
%    \herramientaIA{Herramienta}{Modelo/versión}{Para qué se ha usado}
%       (una línea por herramienta; si no se declara ninguna, la
%        declaración indica que NO se ha usado IA generativa)
%    \usoIA{Texto libre con el detalle del uso}   (opcional)
%  En el cuerpo, donde se quiera (normalmente antes de la bibliografía):
%    \declaracionIA
%  Si se olvida \declaracionIA, se inserta automáticamente al final.
% ---------------------------------------------------------------------
\newif\ifupo@iadeclarada \upo@iadeclaradafalse
\newif\ifupo@iaherramientas \upo@iaherramientasfalse
\newcommand{\upo@iafilas}{}
\newcommand{\upo@iatexto}{}

\newcommand{\herramientaIA}[3]{%
  \upo@iaherramientastrue
  \g@addto@macro\upo@iafilas{#1 & #2 & #3 \\}}
\newcommand{\usoIA}[1]{\renewcommand{\upo@iatexto}{#1}}

\newcommand{\declaracionIA}{%
  \ifupo@iadeclarada\else
  \global\upo@iadeclaradatrue
  \clearpage
  \phantomsection
  \section*{Declaración de uso de inteligencia artificial}
  \addcontentsline{toc}{section}{Declaración de uso de inteligencia artificial}
  \ifupo@iaherramientas
    En la elaboración de este trabajo se han utilizado las siguientes
    herramientas de inteligencia artificial generativa:

    \begin{center}
    \small
    \begin{tabularx}{\textwidth}{>{\raggedright\arraybackslash}p{0.2\textwidth}
                                 >{\raggedright\arraybackslash}p{0.2\textwidth}
                                 >{\raggedright\arraybackslash}X}
      \toprule
      \rowcolor{upoazul!12}\textbf{Herramienta} & \textbf{Modelo / versión} & \textbf{Finalidad} \\
      \midrule
      \upo@iafilas
      \bottomrule
    \end{tabularx}
    \end{center}

    \ifx\upo@iatexto\@empty\else\upo@iatexto\par\fi

    La IA se ha utilizado como herramienta de apoyo. Todo el contenido
    generado con su ayuda ha sido revisado, contrastado y validado por el
    autor, y los resultados experimentales, capturas, datos y código
    ejecutado corresponden al trabajo realizado por el propio autor.
  \else
    El autor declara que en la elaboración de este trabajo \textbf{no se
    han utilizado} herramientas de inteligencia artificial generativa para
    producir su contenido.
    \ifx\upo@iatexto\@empty\else\par\upo@iatexto\fi
  \fi

  \begin{tcolorbox}[colback=upoazul!5, colframe=upoazul, boxrule=0.6pt,
      arc=2pt, fonttitle=\bfseries, title={Declaración de responsabilidad}]
    Yo, \textbf{\upo@autor}, alumno del \upoMaster{} (\upoEspecialidad) de
    la \upoUniversidad, declaro que este documento es fruto de mi trabajo,
    que he indicado de forma veraz y completa el uso que he hecho de
    herramientas de inteligencia artificial y que he citado debidamente
    todas las fuentes consultadas. Asumo la \textbf{plena responsabilidad}
    sobre su contenido ---exactitud técnica, corrección del código y de los
    resultados, idoneidad de las fuentes y originalidad--- y sobre el
    cumplimiento de la normativa académica de la \upoUniversidad.

    \vspace{0.8em}
    \upo@fecha \hfill Firmado: \upo@autor
  \end{tcolorbox}
  \fi}

% Red de seguridad: si el documento no la incluye, se añade al final.
\AtEndDocument{%
  \ifupo@iadeclarada\else
    \ClassWarningNoLine{upo-master}{No se llamo a \string\declaracionIA;
      se ha insertado automaticamente al final del documento}%
    \declaracionIA
  \fi}

\endinput
