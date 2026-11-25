# Setup Guide: ORB-SLAM3 (`mono_webcam`) Remote Visualizer via XLaunch

Panduan ini menjelaskan alur instalasi sistem, konfigurasi jaringan, kompilasi
software, dan eksekusi GUI ORB-SLAM3 dari Jetson Nano ke laptop Windows
menggunakan remote tunnel XLaunch.

---

## 1. Penyiapan SD Card dan Konfigurasi OS Jetson Nano

### 1.1 Burning image ke MicroSD Card

1. Unduh **NVIDIA JetPack SDK Image** resmi (JetPack 4.6.1 / Ubuntu 18.04 LTS)
   untuk Jetson Nano Developer Kit.
2. Gunakan aplikasi **BalenaEtcher** atau **Raspberry Pi Imager** di laptop
   Windows.
3. Masukkan MicroSD Card (minimal 32 GB, Class 10 / UHS-I) ke card reader.
4. Pilih file image `.img` / `.zip`, pilih drive MicroSD sebagai target, lalu
   klik **Flash!**.

### 1.2 Booting dan setup sistem awal

1. Masukkan MicroSD Card ke slot di bagian bawah modul NVIDIA Jetson Nano.
2. Hubungkan monitor via HDMI/DisplayPort, keyboard, mouse, serta catu daya
   (5 V via Micro-USB atau Barrel Jack 5 V terkalibrasi).
3. Ikuti wizard penyetelan awal Ubuntu 18.04:
   - Terima EULA.
   - Pilih bahasa (*English*) dan layout keyboard.
   - Atur zona waktu (*Asia/Jakarta*).
   - Buat akun pengguna:
     - **Username:** `dalangans`
     - **Hostname:** `dalangans-desktop`
     - **Password:** sesuai konfigurasi Anda.
4. Selesaikan proses boot hingga masuk ke desktop Ubuntu (L4T).

---

## 2. Konfigurasi Jaringan dan Remote SSH

### 2.1 Penyetelan Wi-Fi dan identifikasi IP

1. Hubungkan Jetson Nano ke jaringan Wi-Fi lokal.
2. Buka terminal di Jetson Nano dan cek alamat IP:

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

Pastikan OpenCV (versi 3.x / 4.x dengan modul `videoio` / GStreamer) dan
Pangolin (0.6) telah terpasang di sistem.

Tempatkan repositori ORB-SLAM3 di direktori `~/SLAM/ORB_SLAM3`.

### 3.3 Kompilasi ORB-SLAM3

Jalankan skrip build bawaan:

```bash
cd ~/SLAM/ORB_SLAM3
chmod +x build.sh
./build.sh
```

Kompilasi executable `mono_webcam` secara khusus:

```bash
cd ~/SLAM/ORB_SLAM3/build
make -j2 mono_webcam
```

### 3.4 Konfigurasi parameter kamera (`cam_config.yaml`)

Sesuaikan berkas `cam_config.yaml` di direktori `~/SLAM/ORB_SLAM3` dengan
matriks intrinsik kamera USB Anda (320 × 240):

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

Metode ini meneruskan tampilan GUI Pangolin 3D dari Jetson Nano ke layar Windows
tanpa VNC dan tanpa bergantung pada X11 forwarding otomatis dari `ssh.exe` yang
dapat menghalangi pengaturan `$DISPLAY`.

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

## 5. Troubleshooting

| Masalah / error | Penyebab utama | Solusi |
| --- | --- | --- |
| `Display localhost:20.0 Refused Connection` | XLaunch belum aktif atau tunnel terputus. | Pastikan XLaunch aktif di system tray Windows dan sesi SSH tunnel port `6000` → `6020` di CMD belum terputus. |
| `Camera Failed to Open` | Kamera USB tidak terdeteksi atau sedang digunakan proses lain. | Cek `/dev/video*` dan pastikan perangkat kamera tidak sedang diakses aplikasi lain. |
| Pangolin framebuffer crash | Kegagalan rendering OpenGL langsung. | Pastikan `export LIBGL_ALWAYS_INDIRECT=1` sudah dijalankan di terminal sebelum program dimulai. |
