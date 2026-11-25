import numpy as np
import cv2

CHECKERBOARD = (14, 14)
square_size = 0.0135

criteria = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 30, 0.001)

# Membuat titik objek yang aman
objp = np.zeros((CHECKERBOARD[0] * CHECKERBOARD[1], 3), np.float32)
for i in range(CHECKERBOARD[1]):
    for j in range(CHECKERBOARD[0]):
        objp[i * CHECKERBOARD[0] + j, :2] = np.array([j, i], np.float32) * square_size

objpoints = []
imgpoints = []

cap = cv2.VideoCapture(0)
cap.set(cv2.CAP_PROP_FRAME_WIDTH, 320)
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 240)

print("[INFO] Jendela kamera dibuka. Tekan 's' untuk ambil sampel, 'q' untuk keluar.")
img_count = 0

while cap.isOpened():
    ret, frame = cap.read()
    if not ret:
        break

    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    ret_corners, corners = cv2.findChessboardCorners(gray, CHECKERBOARD, None)

    if ret_corners:
        corners2 = cv2.cornerSubPix(gray, corners, (11, 11), (-1, -1), criteria)
        cv2.drawChessboardCorners(frame, CHECKERBOARD, corners2, ret_corners)

    cv2.imshow('Kamera POV (Tekan s / q)', frame)

    key = cv2.waitKey(30) & 0xFF
    if key == ord('s') and ret_corners:
        objpoints.append(objp)
        imgpoints.append(corners2)
        img_count += 1
        print(f"Berhasil ambil sampel ke-{img_count}")
    elif key == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()

if len(objpoints) > 0:
    ret, mtx, dist, rvecs, tvecs = cv2.calibrateCamera(objpoints, imgpoints, gray.shape[::-1], None, None)
    np.savez("calibration_data.npz", mtx=mtx, dist=dist, rvecs=rvecs, tvecs=tvecs)
    print("\n[INFO] Kalibrasi selesai! Data tersimpan di calibration_data.npz")
    print("Matriks Kamera:\n", mtx)
