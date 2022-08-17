Elaborado por: Brandon Nicolas Bohorquez Muñoz 104618021313 <bbohorquez@unicauca.edu.co>
               Juan Fernando Abella Montenegro <jabella@unicauca.edu.co>

Se desarrollaron dos programas en C que implementan la funcionali-
dad para enviar y recibir archivos a través de la red. Uno de los dos extremos
actuará como cliente, y el otro como servidor. Quien recibe conexiones (el ser-
vidor) esperará por comandos del cliente, para transferir archivos desde o hacia
el servidor. Los archivos en el servidor se almacenarán en un directorio llamado "files".
Los comandos soportados son:
get ARCHIVO: Transfiere un archivo desde el servidor hacia el cliente. El
cliente deberá enviar un primer mensaje al servidor con la solicitud (el
nombre del archivo a transferir), a lo cual el servidor responderá con un
mensaje que contiene la información del archivo (por ejemplo, el tamaño
en bytes), seguido del contenido del archivo. Si el archivo no existe, se
deberá informar al cliente el error, y no se enviará nada a continuación
de este mensaje.



Pasos para la ejecucion:

debe ubicarse en la carpeta de nuestro proyecto:

../parcial_2_bbohorquez_jabella

y ejecutar el siguiente comando:

make

despues debe ingresar a la carpeta del servidor

cd servidor

y ejecutar el siguiente comando:

./server Aqui_El_Puerto

despues debera abrir otra terminal e ingresar a la carpeta servidor y ejecutar el siguiente comando:

./client Aqui_Va_La_IP_Del_Server Aqui_va_el_puerto_que_escuchar_el_server

recordando que si se esta ejecutando el cliente y el servidor en la mista maquina la ip es : 127.0.0.1


COMANDOS DISPONIBLES PARA EL CLIENTE:



put ARCHIVO: Transfiere un archivo desde el cliente hacia el servidor. El
cliente deberá enviar un mensaje al servidor con la solicitud (el nombre
del archivo a transferir y su tamaño en bytes), seguido del contenido del
archivo. El servidor recibirá el mensaje, y con la información suministra-
da, creará el archivo, leerá el contenido del socket y lo escribirá en el
archivo (nuevo o existente).

exit: El cliente deberá enviar un mensaje al servidor indicando que se va
a cerrar la conexión.

Se permite la comunicación entre un servidor y múltiples clientes
clientes, por lo cual en el servidor se crean y administran múltiples hilos,
uno por cada conexión. Se debe tener en cuenta que varios clientes pueden
tratar de enviar el mismo archivo al servidor. Si se presenta esta situación, el
servidor deberá rechazar el envío.

Tanto el servidor como el cliente deberán terminar cuando se reciba la señal
SIGTERM O SIGINT

