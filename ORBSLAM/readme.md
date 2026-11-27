# Setup Guide: ORB-SLAM3 (`mono_webcam`) Remote Visualizer via XLaunch

Panduan ini menjelaskan alur instalasi sistem, konfigurasi jaringan, kompilasi software, eksekusi GUI ORB-SLAM3 dari Jetson Nano (`dalangans-desktop`) ke laptop Windows menggunakan *reverse SSH tunnel* dan XLaunch, serta perekaman video (`orb_slam_record.avi`) secara bersamaan untuk kebutuhan pipeline COLMAP pada proyek REVEAL.

---

## 1. Penyiapan SD Card dan Konfigurasi OS Jetson Nano

### 1.1 Burning image ke MicroSD Card

1. Unduh **NVIDIA JetPack SDK Image** resmi (JetPack 4.6.1 / Ubuntu 18.04 LTS) untuk Jetson Nano Developer Kit.
2. Gunakan aplikasi **BalenaEtcher** atau **Raspberry Pi Imager** di laptop Windows.
3. Masukkan MicroSD Card (minimal 32 GB, Class 10 / UHS-I) ke *card reader*.
4. Pilih file *image* `.img` / `.zip`, pilih *drive* MicroSD sebagai target, lalu klik **Flash!**.

### 1.2 Booting dan setup sistem awal

1. Masukkan MicroSD Card ke slot di bagian bawah modul NVIDIA Jetson Nano.
2. Hubungkan monitor via HDMI/DisplayPort, keyboard, mouse, serta catu daya 5 V yang teregulasi stabil (misalnya *buck converter* 5A yang diturunkan dari baterai 7.4V).
3. Ikuti *wizard* penyetelan awal Ubuntu 18.04:
   - Terima EULA.
   - Pilih bahasa (*English*) dan *layout* keyboard.
   - Atur zona waktu (*Asia/Jakarta*).
   - Buat akun pengguna:
     - **Username:** `dalangans`
     - **Hostname:** `dalangans-desktop`
     - **Password:** sesuai konfigurasi Anda.
4. Selesaikan proses *boot* hingga masuk ke desktop Ubuntu (L4T).

---

## 2. Konfigurasi Jaringan dan Remote SSH

### 2.1 Identifikasi IP (USB Device Mode & Wi-Fi)

Anda dapat terhubung ke Jetson Nano melalui dua jalur:
- **Koneksi USB Device Mode (kabel USB data langsung ke laptop Windows):** IP default Jetson Nano adalah `192.168.55.1`.
- **Koneksi Wi-Fi / LAN lokal:** Cek alamat IP di terminal Jetson Nano dengan perintah:

  ```bash
  ifconfig
   ```

   Contoh alamat IP:
   - Jetson Nano: `172.16.65.111`
   - Laptop Windows: `172.16.65.109`

### 2.2 Memastikan layanan SSH aktif

Jalankan perintah berikut di terminal Jetson Nano:

```bash
sudo apt update
sudo apt install openssh-server -y
sudo systemctl enable ssh
sudo systemctl start ssh
```

---

## 3. Instalasi Dependensi dan Build ORB-SLAM3

### 3.1 Instalasi library pendukung

Jalankan perintah berikut di terminal Jetson Nano:

```bash
sudo apt update && sudo apt install -y \
    build-essential cmake git pkg-config \
    libgtk2.0-dev libavcodec-dev libavformat-dev libswscale-dev \
    libeigen3-dev libboost-all-dev libssl-dev x11-apps
```

### 3.2 Build OpenCV dan Pangolin

Pastikan OpenCV (dengan modul videoio / GStreamer) dan Pangolin telah terpasang di sistem, serta repositori ORB-SLAM3 berada di direktori ~/SLAM/ORB_SLAM3.

### 3.3 Kompilasi ORB-SLAM3

Jalankan skrip build bawaan:

```bash
cd ~/SLAM/ORB_SLAM3
chmod +x build.sh
./build.sh
```

Untuk mengompilasi ulang target mono_webcam saja (gunakan -j2 agar penggunaan memori di Jetson Nano tetap aman):

```bash
cd ~/SLAM/ORB_SLAM3/build
make -j2 mono_webcam
```

Catatan: Berdasarkan konfigurasi build pada proyek ini, binary mono_webcam ter-output ke ~/SLAM/ORB_SLAM3/Examples/RGB-D/mono_webcam.

### 3.4 Konfigurasi parameter kamera (`cam_config.yaml`)

Sesuaikan berkas ~/SLAM/ORB_SLAM3/cam_config.yaml dengan matriks intrinsik kamera USB (320 × 240):

```yaml
%YAML:1.0
File.version: "1.0"
Camera.type: "PinHole"
Camera.fx: 260.03
Camera.fy: 260.14
Camera.cx: 150.56
Camera.cy: 107.00
Camera.k1: 0.0
Camera.k2: 0.0
Camera.p1: 0.0
Camera.p2: 0.0
Camera.width: 320
Camera.height: 240
Camera.fps: 30
Camera.RGB: 1
```

---

## 4. Eksekusi GUI via XLaunch

Metode ini meneruskan tampilan GUI Pangolin 3D dari Jetson Nano ke layar Windows melalui reverse SSH tunnel ke port X11 (`6000`).

### 4.1 Persiapan di laptop Windows

1. Tutup semua instance VcXsrv / XLaunch yang sedang berjalan melalui Task
   Manager.
2. Buka XLaunch dari Start Menu Windows dan atur konfigurasi berikut:
   - **Display settings:** pilih **Multiple windows**, isi **Display number**
     dengan `0`, lalu klik **Next**.
   - **Client startup:** pilih **Start no client**, lalu klik **Next**.
   - **Extra settings:** hapus centang **Native opengl** untuk menghindari
     kendala kelambatan atau glitch rendering OpenGL.
   - Hapus centang **Disable access control**.
   - Klik **Next**, lalu **Finish**.
3. Buka Command Prompt (CMD) dan pastikan port X11 (`6000`) berstatus
   `LISTENING`:

   ```cmd
   netstat -an | findstr 6000
   ```

### 4.2 Membuka reverse SSH tunnel dari CMD Windows

Jalankan perintah SSH berikut di CMD Windows dan biarkan terminal ini tetap
terbuka:

```cmd
ssh -R 6020:127.0.0.1:6000 -o ServerAliveInterval=15 -o ExitOnForwardFailure=yes dalangans@192.168.55.1
```

```cmd
ssh -R 6020:127.0.0.1:6000 -o ServerAliveInterval=15 -o ExitOnForwardFailure=yes dalangans@172.16.65.111
```

Perintah ini memetakan port `6020` di Jetson Nano (display `:20.0`) ke port
`6000` XLaunch di Windows (display `:0.0`).

### 4.3 Pengujian X11 dan eksekusi `mono_webcam`

Di sesi terminal Jetson Nano yang terhubung melalui SSH, atur environment
display dan OpenGL indirect:

```bash
export DISPLAY=localhost:20.0
export LIBGL_ALWAYS_INDIRECT=1
```

Uji tampilan visual X11:

```bash
xeyes
```

Jika jendela `xeyes` muncul di layar Windows, tutup jendela tersebut dan
jalankan ORB-SLAM3:

```bash
cd ~/SLAM/ORB_SLAM3
./Examples/RGB-D/mono_webcam Vocabulary/ORBvoc.txt cam_config.yaml
```

---
## 5. Perekaman Video Bersama ORB-SLAM3 (`orb_slam_record.avi`) untuk COLMAP

Agar perekaman video berjalan bersamaan dengan mono_webcam menggunakan satu kamera tanpa konflik akses perangkat, penulisan frame (`cv::VideoWriter`) ditempatkan di dalam loop utama pembacaan frame pada `~/SLAM/ORB_SLAM3/Examples/Monocular/mono_webcam.cc` (sebelum `SLAM.TrackMonocular(im, timestamp)`).

### 5.1 Alur edit, build ulang, dan ambil hasil rekaman

1. Buat backup sebelum mengedit file sumber:
   ```cmd
   cp ~/SLAM/ORB_SLAM3/Examples/Monocular/mono_webcam.cc ~/SLAM/ORB_SLAM3/Examples/Monocular/mono_webcam.cc.bak
   ```
2. Tambahkan objek `cv::VideoWriter` untuk menulis ke `orb_slam_record.avi` (`320x240`, `30 FPS`, `MJPG`) di dalam loop pembacaan frame `cap >> im`, lalu panggil `video_out.release()` saat keluar dari loop.
3. Kompilasi ulang:
   ```cmd
   cd ~/SLAM/ORB_SLAM3/build
   make -j2 mono_webcam
   ```
4. Jalankan program dari `~/SLAM/ORB_SLAM3`, lalu hentikan dengan `Ctrl+C` (atau tutup jendela viewer dengan bersih) setelah selesai merekam agar file video tertutup sempurna.
5. Salin file `orb_slam_record.avi` ke direktori lokal Windows (`D:\Univ\7th Semester\Despro\ORBSLAM`) melalui CMD Windows (sesuaikan IP dengan koneksi aktif Anda, misalnya `192.168.55.1`):
   ```cmd
   scp dalangans@192.168.55.1:~/SLAM/ORB_SLAM3/orb_slam_record.avi "D:\Univ\7th Semester\Despro\ORBSLAM"
   ```

## 6. Troubleshooting

| Masalah / error | Penyebab utama | Solusi |
| --- | --- | --- |
| `Display localhost:20.0 Refused Connection` | XLaunch belum aktif, Disable access control masih tercentang, atau reverse SSH tunnel terputus. | Pastikan XLaunch aktif, Disable access control tidak dicentang, dan sesi `ssh -R 6020:127.0.0.1:6000 ...` masih tersambung. |
| `Camera Failed to Open` | Kamera USB tidak terdeteksi atau sedang digunakan proses lain. | Cek `/dev/video*` dan pastikan perangkat kamera tidak sedang diakses aplikasi lain. |
| Pangolin framebuffer crash | Kegagalan rendering OpenGL langsung. | Pastikan `export LIBGL_ALWAYS_INDIRECT=1` sudah dijalankan di terminal sebelum program dimulai. |
