import cv2
import numpy as np
import serial, time #arduino comunicacion

arduino = serial.Serial('COM3', 9600)
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

# Guardar la imagen de referencia
cv2.imwrite('referencia.jpg', ref_frame)

# Convertir la imagen de referencia a escala de grises y suavizar
ref_gray = cv2.cvtColor(ref_frame, cv2.COLOR_BGR2GRAY)
ref_gray = cv2.GaussianBlur(ref_gray, (21, 21), 0)

# Variable para ajustar el valor de similitud
similarity_threshold = 0.05  # Ajusta este valor para cambiar la sensibilidad

while True:
    # Capturar frame por frame
    ret, frame = cap.read()

    if not ret:
        print("Error al recibir frames.")
        break

    # Convertir el frame actual a escala de grises y suavizar
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    gray = cv2.GaussianBlur(gray, (21, 21), 0)

    # Calcular la diferencia absoluta entre el frame actual y la referencia
    frame_delta = cv2.absdiff(ref_gray, gray)
    thresh = cv2.threshold(frame_delta, 25, 255, cv2.THRESH_BINARY)[1]

    # Dilatar la imagen umbralizada para rellenar agujeros y encontrar contornos
    thresh = cv2.dilate(thresh, None, iterations=2)
    contours, _ = cv2.findContours(thresh.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

    # Verificar si hay cambios significativos en la imagen
    non_zero_pixels = cv2.countNonZero(thresh)
    if non_zero_pixels > (thresh.size * similarity_threshold):
        arduino.write(b"C")
        time.sleep(2)

        # Leer la respuesta de Arduino
        respuesta = arduino.readline().strip()
        print(respuesta)

    # Mostrar el frame con la diferencia
    cv2.imshow('Detección de Cambios (ESP32-CAM)', frame)

    # Salir con la tecla 'q'
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# Liberar la captura y cerrar todas las ventanas
cap.release()
cv2.destroyAllWindows()
arduino.close()
