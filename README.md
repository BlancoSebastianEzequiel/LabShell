```
# Lab shell
El próposito de este lab es el de desarrollar la funcionalidad mínima que caracteriza a un intérprete de comandos shell similar a lo que realizan bash, zsh, fish.

La implementación debe realizarse en C99 y POSIX.1-2008. (Estas siglas hacen referencia a la versión del lenguaje C utilizada y del estándar de syscalls Unix empleadas. Los versiones modernas del compilador GCC y de Linux cumplen con ambos requerimientos.)

Índice  
Esqueleto  

* Parte 1: Invocación de comandos
   * Búsqueda en $PATH ☆
   * Argumentos del programa ☆
   * Imprimir variables de entorno ☆
   * Resumen de la parte 1
* Parte 2: Invocación avanzada
   * Comandos built-in ☆
   * Variables de entorno adicionales ☆☆
   * Procesos en segundo plano ☆☆☆
   * Resumen de la parte 2
* Parte 3: Redirecciones
   * Flujo estándar ☆☆
   * Tuberías simples (pipes) ☆☆☆
   * Resumen de la parte 3
* Challenges promocionales
   * Pseudo-variables ★
   * Tuberías múltiples ★★
   * Segundo plano avanzado ★★★

## Esqueleto
Para que no tengan que implementar todo desde cero, se provee un esqueleto de shell. Éste tiene gran parte del parseo hecho, y está estructurado indicando con comentarios los lugares en donde deben introducir el código crítico de cada punto.

El esqueleto se encuentra en este repositorio. Pueden clonarlo mediante el comando git clone https://github.com/fisop/lab-shell-skel, o bien utilizar la interfaz de github y descargar una copia en .zip.

Se recomienda antes de empezar leer el código para entender bien cómo funciona, y qué hace cada una de las funciones. Particularmente recoendamos entender qué significa cada uno de los campos en los structs de types.h.

## Parte 1: Invocación de comandos

### Búsqueda en $PATH ☆

Los comandos que usualmente se utilizan, como los realizados en el lab anterior, están guardados (sus binarios), en el directorio /bin. Por este motivo existe una variable de entorno llamada $PATH, en la cual se declaran las rutas más usualmente accedidas por el sistema operativo (ejecutar: echo $PATH para ver la totalidad de las rutas almacenadas). Se pide agregar la funcionalidad de poder invocar comandos, cuyos binarios se encuentren en las rutas especificadas en la variable $PATH.

$ uptime
 05:45:25 up 5 days, 12:02,  5 users,  load average: ...
Pregunta: ¿cuáles son las diferencias entre la syscall execve(2) y la familia de wrappers proporcionados por la librería estándar de C (libc) exec(3)?
Función sugerida: execvp(3)

Argumentos del programa ☆
En esta parte del lab, vamos a incorporar a la invocación de comandos, la funcionalidad de poder pasarle argumentos al momento de querer ejecutarlos. Los argumentos pasados al programa de esta forma, se guardan en la famosa variable char* argv[], junto con cuántos fueron en int argc, declaradas en el main de cualquier programa en C.

$ df -H /tmp
Filesystem      Size  Used Avail Use% Mounted on
tmpfs           8.3G  2.0M  8.3G   1% /tmp
Imprimir variables de entorno ☆
Una característica de cualquier intérprete de comandos shell es la de expandir variables de entorno (ejecutar: env para una lista completa de las varibles de entorno definidas), como PATH, o HOME.

$ echo $TERM
xterm-16color
Se debe lograr que la shell expanda las variables de entorno antes de ejecutar el comando. Las variables de entorno a reemplazar se le indican a la shell mediante el caracter $.

Función sugerida: getenv(3)

Resumen de la parte 1
Al finalizar la parte 1 el shell debe poder:

Invocar programas y permitir pasarles argumentos
Expandir variables de entorno
Parte 2: Invocación avanzada
Comandos built-in ☆
Los comandos built-in nos dan la opurtunidad de realizar acciones que no siempre podríamos hacer si ejecutáramos ese mismo comando en un proceso separado. Éstos son propios de cada shell aunque existe un estándar generalizado entre los diferentes intérpretes, como por ejemplo cd, exit.

Es evidente que si cd no se realizara en el mismo proceso donde la shell se está ejecutando, no tendría el efecto deseado, ya que directorio actual se cambiaría pero en el hijo, y no en el padre que es lo que realmente queremos. Lo mismo se aplica a exit y a muchos comandos más (aquí se muestra una lista completa de los comando built-in que soporta bash).

Implementar:

cd - change directory (cambia el directorio actual)
exit - exits nicely (termina un terminal de forma linda)
pwd - print working directory (muestra el directorio actual de trabajo)
Pregunta: ¿entre cd y pwd, alguno de los dos se podría implementar sin necesidad de ser built-in? ¿por qué? ¿cuál es el motivo, entonces, de hacerlo como built-in? (para esta última pregunta pensar en los built-in como true y false)

Funciones sugeridas: chdir(3), exit(3)

### Variables de entorno adicionales ☆☆

En esta parte se va a extender la funcionalidad del shell para que soporte el poder incorporar nuevas variables de entorno a la ejecución de un programa. Cualquier programa que hagamos en C, por ejemplo, tiene acceso a todas las variables de entorno definidas mediante la variable externa environ (extern char** environ).
Se pide, entonces, la posibilidad de incorporar de forma dinámica nuevas variables, por ejemplo:

$ /usr/bin/env
--- todas las variables de entorno definidas hasta el momento ---

$ USER=nadie ENTORNO=nada /usr/bin/env | grep =nad
USER=nadie
ENTORNO=nada
Ayuda: luego de llamar a fork(2) realizar, por cada una de las variables de entorno a agregar, una llamada a setenv(3).

Pregunta: ¿por qué es necesario hacerlo luego de la llamada a fork(2) ?

En algunas de los wrappers de la familia de funciones de exec(3) (las que finalizan con la letra e), se les puede pasar un tercer argumento (o una lista de argumentos dependiendo del caso), con nuevas variables de entorno para la ejecución de ese proceso.

Supongamos, entonces, que en vez de utilizar setenv(3) por cada una de las variables, se guardan en un array y se lo coloca en el tercer argumento de una de las funciones de exec(3).

Responder (opcional):

¿el comportamiento es el mismo que en el primer caso? Explicar qué sucede y por qué.
Describir brevemente una posible implementación para que el comportamiento sea el mismo.
Función sugerida: setenv(3)

### Procesos en segundo plano ☆☆☆

Los procesos en segundo plano o procesos en el fondo, o background, son muy útiles a la hora de ejecutar comandos que no queremos esperar a que terminen para que la shell nos devuelva el prompt nuevamente. Por ejemplo si queremos ver algún documento .pdf o una imágen y queremos seguir trabajando en la terminal sin tener que abrir uno nuevo.

$ sleep 2 &
 [PID=2489]

$ ls /home
patricio
Sólo se pide la implementación de un proceso en segundo plano. No es necesario que se notifique de la terminación del mismo por medio de mensajes en la shell.

Detallar cúal es el mecanismo utilizado.
Resumen de la parte 2
Al finalizar la parte 2 el shell debe poder:

Invocar programas y permitir pasarles argumentos
Expandir variables de entorno
Disponer de los comandos built-in cd, pwd y exit
Ejecutar procesos en segundo plano (sin necesidad de avisar cuando terminan)
Ejecutar procesos con variables de entorno adicionales

## Parte 3: Redirecciones

### Flujo estándar ☆☆

La redirección del flujo estándar es una de las cualidades más interesantes y valiosas de un shell moderno. Permite, entre otras cosas, almacenar la salida de un programa en un archivo de texto para luego poder analizarla, como así también ejecutar un programa y enviarle un archivo a su entrada estándar. Existen, básicamente tres formas de redirección del flujo estándar:

Entrada y Salida estándares a archivos (<in.txt >out.txt)

Son los operadores clásicos del manejo de la redireccón del stdin y el stdout en archivos de entrada y salida respectivamente. Por ejemplo:

$ ls /usr
bin etc games include lib local sbin share src

$ ls /usr >out1.txt
$ cat out1.txt
bin etc games include lib local sbin share src

$ wc -w <out1.txt
10

$ ls -C /sys /noexiste >out2.txt
ls: cannot access '/noexiste': No such file or directory

$ cat out2.txt
/sys:
block  class  devices	 fs  kernel  module  power

$ wc -w <out2.txt
8
Se puede ver cómo queda implícito que cuando se utiliza el operador > se refiere al stdout y cuando se utiliza el < se refiere al stdin.

Error estándar a archivo (2>err.txt)

Es una de las dos formas de redireccionar el flujo estándar de error análogo al caso anterior del flujo de salida estándar en un archivo de texto. Por ejemplo:

$ ls -C /home /noexiste >out.txt 2>err.txt

$ cat out.txt
/home:
patricio

$ cat err.txt
ls: cannot access '/noexiste': No such file or directory
Como se puede observar, ls no informa ningún error al finalizar, como sí lo hacía en el ejemplo anterior. Su salida estándar de error ha sido redireccionada al archivo err.txt

Combinar salida y errores (2>&1)

Es la segunda forma de redireccionar el flujo estándar producido por errores en la ejecución de un programa. Su funcionamiento se puede observar a través del siguiente ejemplo:

$ ls -C /home /noexiste >out.txt 2>&1

$ cat out.txt
---????---
Investigar el significado de este tipo de redirección y explicar qué sucede con la salida de cat out.txt. Comparar con los resultados obtenidos anteriormente.
Challenge: investigar, describir y agregar la funcionalidad del operador de redirección >> y &>
Syscalls sugeridas: dup2(2), open(2)

### Tuberías simples (pipes) ☆☆☆

Al igual que la redirección del flujo estándar hacia archivos, es igual o más importante, la redirección hacia otros programas. La forma de hacer esto en una shell es mediante el operador | (pipe o tubería). De esta forma se pueden concatenar dos o más programas para que la salida estándar de uno se redirija a la entrada estándar del siguiente. Por ejemplo:

$ ls -l | grep Doc
drwxr-xr-x  7 patricio patricio  4096 mar 26 01:20 Documentos
Se pide, entonces, implementar pipes solamente entre dos comandos.

Investigar y describir brevemente el mecanismo que proporciona la syscall pipe(2), en particular la sincronización subyacente y los errores relacionados.
Syscalls sugeridas: pipe(2), dup2(2)

### Resumen de la parte 3

Al finalizar la parte 3 el shell debe poder:

Invocar programas y permitir pasarles argumentos
Expandir variables de entorno
Disponer de los comandos built-in cd, pwd y exit
Ejecutar procesos en segundo plano (sin necesidad de avisar cuando terminan)
Ejecutar procesos con variables de entorno adicionales
Redireccionar la entrada y salida estándar de los programas vía < y >
Concatenar la ejecución de dos programas con un pipe

## Challenges promocionales

### Pseudo-variables ★

Existen las denominadas variables de entorno mágicas, o pseudo-variables. Estas variables son propias del shell (no están formalmente en environ) y cambian su valor dinámicamente a lo largo de su ejecución. Implementar ? como única variable mágica (describir, también, su próposito).

$ /bin/true
$ echo $?
0

$ /bin/false
$ echo $?
1
Investigar al menos otras dos variables mágicas estándar, y describir su propósito.

### Tuberías múltiples ★★

Extender el funcionamiento de la shell para que se puedan ejecutar n comandos concatenados. Describir el diseño que se utilizó para poder realizarlo.

$ ls -l | grep Doc | wc
     1       9      64
### Segundo plano avanzado ★★★

$ sleep 2 &
PID=2489

$ sleep 3
<pasan tres segundos>

==> terminado: PID=2489 (sleep 2)
$
En otras palabras, se notifica de que ha terminado un proceso en segundo plano oportunísticamente, justo antes de imprimir un prompt nuevamente.

Preguntas:

Explicar detalladamente cómo se manejó la terminación del mismo.

¿Por qué es necesario el uso de señales?

Syscalls sugeridas: setpgid(2), sigaction(2), sigaltstack(2)
```
