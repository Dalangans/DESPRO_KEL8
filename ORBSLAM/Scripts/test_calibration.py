import numpy as np
import cv2

# Memuat data kalibrasi yang sudah disimpan sebelumnya
with np.load('calibration_data.npz') as data:
    mtx = data['mtx']
    dist = data['dist']

cap = cv2.VideoCapture(0)
cap.set(cv2.CAP_PROP_FRAME_WIDTH, 320)
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 240)

print("[INFO] Uji kalibrasi aktif. Arahkan kamera ke objek bergaris lurus.")
print("[INFO] Tekan 'q' di jendela video untuk keluar.")

while cap.isOpened():
    ret, frame = cap.read()
    if not ret:
        break

    h, w = frame.shape[:2]
    
    # Menghitung matriks kamera baru yang dioptimalkan untuk koreksi distorsi
    newcameramtx, roi = cv2.getOptimalNewCameraMatrix(mtx, dist, (w, h), 1, (w, h))

    # Melakukan proses undistort pada frame
    dst = cv2.undistort(frame, mtx, dist, None, newcameramtx)

    # Menggabungkan video asli (kiri) dan hasil undistort (kanan) dalam satu layar
    combined = np.hstack((frame, dst))

    cv2.imshow('Kiri: Asli | Kanan: Undistorted', combined)

    if cv2.waitKey(30) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
