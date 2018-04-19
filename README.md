# Lab shell

El prÃ³posito de este *lab* es el de desarrollar la funcionalidad mÃ­nima que caracteriza a un intÃ©rprete de comandos *shell* similar a lo que realizan `bash`, `zsh`, `fish`.

La implementaciÃ³n debe realizarse en C99 y POSIX.1-2008. *(Estas siglas hacen referencia a la versiÃ³n del lenguaje C utilizada y del estÃ¡ndar de syscalls Unix empleadas. Los versiones modernas del compilador GCC y de Linux cumplen con ambos requerimientos.)*

Enlaces recomendados:

  - el [rÃ©gimen de cursada](../regimen.md), donde se explica la notaciÃ³n â˜†/â˜…
  - las [instrucciones de entrega](../entregas.md) en papel.


## Ãndice
{:.no_toc}

* TOC
{:toc}


## Esqueleto

Para que no tengan que implementar todo desde cero, se provee un esqueleto de shell. Ã‰ste tiene gran parte del parseo hecho, y estÃ¡ estructurado indicando con comentarios los lugares en donde deben introducir el cÃ³digo crÃ­tico de cada punto.

El esqueleto se encuentra en [este repositorio](https://github.com/fisop/lab-shell-skel). Pueden clonarlo mediante el comando `git clone https://github.com/fisop/lab-shell-skel`, o bien utilizar la interfaz de github y descargar una copia en .zip.

Se recomienda antes de empezar leer el cÃ³digo para entender bien cÃ³mo funciona, y quÃ© hace cada una de las funciones. Particularmente recoendamos entender quÃ© significa cada uno de los campos en los structs de `types.h`.

## Parte 1: InvocaciÃ³n de comandos

### BÃºsqueda en *$PATH* â˜†

Los comandos que usualmente se utilizan, como los realizados en el *lab* anterior, estÃ¡n guardados (sus binarios), en el directorio */bin*. Por este motivo existe una variable de entorno llamada *$PATH*, en la cual se declaran las rutas mÃ¡s usualmente accedidas por el sistema operativo (ejecutar: `echo $PATH` para ver la totalidad de las rutas almacenadas). Se pide agregar la funcionalidad de poder invocar comandos, cuyos binarios se encuentren en las rutas especificadas en la variable *$PATH*.

```bash
$ uptime
 05:45:25 up 5 days, 12:02,  5 users,  load average: ...
```

- _<u>Pregunta:</u>_ Â¿cuÃ¡les son las diferencias entre la *syscall* `execve(2)` y la familia de *wrappers* proporcionados por la librerÃ­a estÃ¡ndar de *C* (*libc*) `exec(3)`?

**FunciÃ³n sugerida:**  `execvp(3)`




### Argumentos del programa â˜†

En esta parte del lab, vamos a incorporar a la invocaciÃ³n de comandos, la funcionalidad de poder pasarle *argumentos* al momento de querer ejecutarlos. Los argumentos pasados al programa de esta forma, se guardan en la famosa variable **char* argv[]**, junto con cuÃ¡ntos fueron en **int argc**, declaradas en el *main* de cualquier programa en *C*.

```bash
$ df -H /tmp
Filesystem      Size  Used Avail Use% Mounted on
tmpfs           8.3G  2.0M  8.3G   1% /tmp
```

### Imprimir variables de entorno â˜†

Una caracterÃ­stica de cualquier intÃ©rprete de comandos *shell* es la de expandir variables de entorno (ejecutar: `env` para una lista completa de las varibles de entorno definidas), como  **PATH**, o **HOME**.

```bash
$ echo $TERM
xterm-16color
```

Se debe lograr que la shell expanda las variables de entorno antes de ejecutar el comando. Las variables de entorno a reemplazar se le indican a la shell mediante el caracter `$`.

**FunciÃ³n sugerida:** `getenv(3)`


### Resumen de la parte 1

Al finalizar la parte 1 el shell debe poder:
- Invocar programas y permitir pasarles argumentos
- Expandir variables de entorno


## Parte 2: InvocaciÃ³n avanzada

### Comandos _built-in_ â˜†

Los comandos _built-in_ nos dan la opurtunidad de realizar acciones que no siempre podrÃ­amos hacer si ejecutÃ¡ramos ese mismo comando en un proceso separado. Ã‰stos son propios de cada _shell_ aunque existe un estÃ¡ndar generalizado entre los diferentes intÃ©rpretes, como por ejemplo `cd`, `exit`. 

Es evidente que si `cd` no se realizara en el mismo proceso donde la _shell_ se estÃ¡ ejecutando, no tendrÃ­a el efecto deseado, ya que directorio actual se cambiarÃ­a pero en el hijo, y no en el padre que es lo que realmente queremos. Lo mismo se aplica a `exit` y a muchos comandos mÃ¡s ([aquÃ­](https://www.gnu.org/software/bash/manual/bashref.html#Shell-Builtin-Commands) se muestra una lista completa de los comando _built-in_ que soporta _bash_).

_<u>Implementar</u>_:

- `cd` - _change directory_ (cambia el directorio actual)
- `exit` - exits nicely (termina un terminal de forma _linda_)
- `pwd` - _print working directory_ (muestra el directorio actual de trabajo)

_<u>Pregunta</u>_: Â¿entre `cd` y `pwd`, alguno de los dos se podrÃ­a implementar sin necesidad de ser _built-in_? Â¿por quÃ©? Â¿cuÃ¡l es el motivo, entonces, de hacerlo como _built-in_? (para esta Ãºltima pregunta pensar en los _built-in_ como `true` y `false`)

**Funciones sugeridas:** `chdir(3)`, `exit(3)`


### Variables de entorno adicionales â˜†â˜†

En esta parte se va a extender la funcionalidad del *shell* para que soporte el poder incorporar nuevas variables de entorno a la ejecuciÃ³n de un programa. Cualquier programa que hagamos en C, por ejemplo, tiene acceso a todas las variables de entorno definidas mediante la variable externa *environ* (`extern char** environ`).[^noinc]

[^noinc]: No es necesario realizar el _include_ de ningÃºn header para hacer uso de esta variable.

Se pide, entonces, la posibilidad de incorporar de forma dinÃ¡mica nuevas variables, por ejemplo:

```bash
$ /usr/bin/env
--- todas las variables de entorno definidas hasta el momento ---

$ USER=nadie ENTORNO=nada /usr/bin/env | grep =nad
USER=nadie
ENTORNO=nada
```

- _<u>Ayuda:</u>_ luego de llamar a `fork(2)` realizar, por cada una de las variables de entorno a agregar, una llamada a `setenv(3)`.

  _<u>Pregunta</u>:_ Â¿por quÃ© es necesario hacerlo luego de la llamada a `fork(2)` ?

- En algunas de los _wrappers_ de la familia de funciones de `exec(3)` (las que finalizan con la letra _e_), se les puede pasar un tercer argumento (o una lista de argumentos dependiendo del caso), con nuevas variables de entorno para la ejecuciÃ³n de ese proceso. 

  Supongamos, entonces, que en vez de utilizar `setenv(3)` por cada una de las variables, se guardan en un array y se lo coloca en el tercer argumento de una de las funciones de `exec(3)`.

  _<u>Responder (opcional):</u>_

  - Â¿el comportamiento es el mismo que en el primer caso? Explicar quÃ© sucede y por quÃ©.
  - Describir brevemente una posible implementaciÃ³n para que el comportamiento sea el mismo.

**FunciÃ³n sugerida:** `setenv(3)`


### Procesos en segundo plano â˜†â˜†â˜†

Los procesos en segundo plano o procesos en el fondo, o *background*, son muy Ãºtiles a la hora de ejecutar comandos que no queremos esperar a que terminen para que la *shell* nos devuelva el *prompt* nuevamente. Por ejemplo si queremos ver algÃºn documento *.pdf* o una imÃ¡gen y queremos seguir trabajando en la terminal sin tener que abrir uno nuevo.

```bash
$ sleep 2 &
 [PID=2489]

$ ls /home
patricio
```

SÃ³lo se pide la implementaciÃ³n de un proceso en segundo plano. No es necesario que se notifique de la terminaciÃ³n del mismo por medio de mensajes en la _shell_. 

- Detallar cÃºal es el mecanismo utilizado.

### Resumen de la parte 2

Al finalizar la parte 2 el shell debe poder:
- Invocar programas y permitir pasarles argumentos
- Expandir variables de entorno
- Disponer de los comandos built-in _cd_, _pwd_ y _exit_
- Ejecutar procesos en segundo plano (sin necesidad de avisar cuando terminan)
- Ejecutar procesos con variables de entorno adicionales


