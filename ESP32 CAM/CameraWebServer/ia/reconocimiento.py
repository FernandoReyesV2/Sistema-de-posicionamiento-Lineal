import cv2
import serial
import time

arduino = serial.Serial('COM4', 9600, timeout=1)
time.sleep(2)

# URL de la cámara IP (ESP32-CAM configurada como punto de acceso)
url = 'http://192.168.4.1:81/stream'  # Ajusta la URL si es necesario

# Configurar la conexión con la cámara IP específica
cap = cv2.VideoCapture(url)

if not cap.isOpened():
    print("Error al abrir la cámara IP en la red WiFi.")
    exit()

# Capturar una imagen de referencia
ret, ref_frame = cap.read()
if not ret:
    print("Error al capturar la imagen de referencia.")
    cap.release()
    exit()

# Convertir la imagen de referencia a escala de grises
ref_gray = cv2.cvtColor(ref_frame, cv2.COLOR_BGR2GRAY)

# Variable para ajustar el valor de similitud
similarity_threshold = 0.02  # Ajusta este valor para cambiar la sensibilidad

# Bandera para controlar el tiempo de espera
esperar = False

while True:
    # Capturar frame por frame
    ret, frame = cap.read()

    if not ret:
        print("Error al recibir frames.")
        break

    # Convertir el frame actual a escala de grises
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

    if not esperar:
        # Calcular la diferencia absoluta entre el frame actual y la referencia
        frame_delta = cv2.absdiff(ref_gray, gray)

        # Verificar si hay cambios significativos en la imagen
        _, thresh = cv2.threshold(frame_delta, 30, 255, cv2.THRESH_BINARY)
        non_zero_pixels = cv2.countNonZero(thresh)
        if non_zero_pixels > (thresh.size * similarity_threshold):
            comando = b"C\n"  # Envía un comando con formato
            arduino.write(comando)
            time.sleep(0.5)  # Agrega un delay entre comandos

            # Procesa la respuesta de Arduino
            respuesta = arduino.readline().strip()
            if respuesta == b"C":
                print("Comando ejecutado correctamente")
            else:
                print("Error al ejecutar comando:", respuesta)

            # Establecer la bandera para esperar
            esperar = True
            # Guardar el tiempo en que se detectó el cambio
            tiempo_cambio = time.time()

    else:
        # Esperar 2 segundos después de detectar un cambio
        if time.time() - tiempo_cambio >= 5:
            print("Tiempo finalizado")
            esperar = False  # Reiniciar la comparación

    # No mostrar el frame en una ventana (comentar esta línea)
    cv2.imshow('Detección de Cambios (ESP32-CAM)', frame)

    # Salir con la tecla 'q'
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# Liberar la captura y cerrar todas las ventanas
cap.release()
cv2.destroyAllWindows()
arduino.close()
