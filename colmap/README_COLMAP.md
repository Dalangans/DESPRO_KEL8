# Alur Rekonstruksi 3D dengan COLMAP dan MeshLab

Subsistem 3D Mapping untuk proyek **REVEAL: Robot for Exploration &
Visualization in Emergency Affected Locations**. Dokumen ini merangkum alur
pengolahan video menjadi model 3D menggunakan Python (OpenCV), COLMAP, dan
MeshLab, beserta parameter terbaik hasil pengujian. Dokumentasi lengkap,
termasuk foto dan hasil pengujian, tersedia di
[Google Docs](#dokumentasi-lengkap).

## Alur Singkat

```
Video (webcam/kamera robot)
  → Ekstraksi frame (Python + OpenCV, step 5)
  → COLMAP: Feature Extraction → Matching → Sparse (SfM) → Dense (MVS)
  → MeshLab: bersihkan outlier → hitung normal → Ball Pivoting
  → Ekspor model (.ply / .obj)
```

## Persiapan

- Python dengan `opencv-python` (`pip install opencv-python`)
- [COLMAP](https://colmap.github.io/) (dense reconstruction butuh GPU NVIDIA/CUDA)
- [MeshLab](https://www.meshlab.net/)

## Alur Kerja

### 1. Rekam video

Ikuti [panduan pengambilan video](#panduan-pengambilan-video) di bawah.

### 2. Ekstrak frame

Gunakan skrip `stepfive.py`. Frame disimpan sebagai PNG (`frame_00000.png`,
`frame_00005.png`, ...) langsung ke satu folder yang siap dipilih di COLMAP.

```bash
python stepfive.py --video video4.mp4 --output Uji_Meja3/images --step 5
```

| Argumen    | Default             | Keterangan                          |
|------------|---------------------|-------------------------------------|
| `--video`  | (wajib)             | Path file video input               |
| `--output` | `Uji_Meja3/images`  | Folder tujuan frame                 |
| `--step`   | `5`                 | Simpan 1 frame setiap N frame       |

Gunakan `step 5` sebagai nilai awal (overlap antar-foto ≥ 80%), lalu sesuaikan
dengan kecepatan gerak kamera. Gerakan yang berhenti atau berbalik arah
menghasilkan frame redundan; potong bagian tersebut sebelum masuk COLMAP.

> Gunakan file video umum (`.mp4`/`.avi`), bukan rosbag. Rosbag berukuran besar
> dan bermasalah dependensinya di Windows. Jika perlu, ekstrak dengan FFmpeg.

### 3. Siapkan proyek COLMAP

`File → New project`. Buat file database baru (mis. `database.db`) dan pilih
folder frame sebagai folder gambar.

### 4. Ekstrak fitur

`Processing → Feature extraction`. Pilih camera model **`OPENCV`**, biarkan
parameter kamera kosong/default bila belum ada kalibrasi, lalu klik **Extract**.

### 5. Cocokkan fitur

`Processing → Feature matching`:

- Frame dari video berurutan: tab **Sequential**, aktifkan `quadratic_overlap`.
- Foto yang tidak berurutan: tab **Exhaustive**.

Pastikan proses selesai tanpa error.

### 6. Rekonstruksi sparse (SfM)

Atur `Reconstruction → Reconstruction options`:

| Tab           | Parameter                                | Nilai          |
|---------------|------------------------------------------|----------------|
| Triangulation | `merge/complete_max_reproj_error`        | `8.00 px`      |
| Triangulation | `ignore_two_view_tracks`                 | Nonaktif       |
| Registration  | `abs_pose_max_error`                     | `16.00–20.00 px` |
| Registration  | `abs_pose_min_num_inliers`               | `15`           |

Jalankan `Reconstruction → Start reconstruction`, lalu periksa log untuk
memastikan kamera berhasil terdaftar sebelum lanjut ke dense.

### 7. Rekonstruksi dense (MVS)

`Reconstruction → Dense reconstruction`. Pilih folder workspace kosong (mis.
`dense/`), lalu jalankan berurutan: **Undistortion → Stereo → Fusion**. Hasilnya
`fused.ply`.

### 8. Olah di MeshLab

1. `File → Import Mesh` → buka `fused.ply`. Cek jumlah *Vertices* di status bar.
2. **Bersihkan outlier**: blok titik melayang dengan *Select Vertices in a
   rectangular region*, lalu `Ctrl + Delete`. Atau otomatis:
   `Filters → Selection → Select Outliers`, lalu `Ctrl + Delete`.
3. **Hitung normal**: `Filters → Normals, Curvatures and Orientation →
   Compute Normals for Point Sets`, `Number of neighbors` = `16` (rentang 16–24).
4. **Bentuk mesh**: `Filters → Remeshing, Simplification and Reconstruction →
   Surface Reconstruction: Ball Pivoting`:
   - `Pivoting Ball Radius`: `0` (auto) sebagai nilai awal. Jika mesh
     berlubang-lubang, isi manual sekitar **1.5%–3.0%** (3.0% optimal pada
     Uji_Meja3).
   - `Clustering radius`: `20%`
   - `Angle Threshold`: `90°`
5. **Ekspor**: `File → Export Mesh As...` ke `.ply` atau `.obj`.

> **Jangan gunakan Screened Poisson** untuk area terbuka/parsial. Hasilnya
> menggelembung (*ballooning effect*). Poisson hanya cocok untuk objek tertutup
> 360° (watertight). Gunakan Ball Pivoting.

## Mode Automatic Reconstruction (hasil terpadat)

Untuk hasil paling rapat, gunakan `Reconstruction → Automatic reconstruction`:

- Data type: **Video frames**, **shared intrinsics** aktif
- **Image Quality: High**

Pada Uji_Meja3 mode ini meregistrasi seluruh **949 frame (54.184 sparse
points)** dan menghasilkan **3.631.076 vertices** pada `fused.ply`. Konfigurasi
manual sebelumnya hanya meregistrasi 334 frame (20.777 points).

Trade-off: pengaturan High memakan waktu sekitar **6–8 jam** untuk 949 frame
(≈ 7 jam 51 menit pada GPU laptop lokal). Gunakan **Medium** untuk uji cepat
dan simpan **High** untuk aset final.

## Alternatif: Pipeline Hybrid dengan Google Colab (GPU T4)

Undistortion dilakukan lokal, lalu dense stereo dijalankan di Colab (headless).

- Set `QT_QPA_PLATFORM=offscreen` agar COLMAP tidak crash karena GUI Qt/OpenGL.
- Struktur hasil undistort menyimpan `cameras.bin`, `images.bin`, `points3D.bin`
  di `sparse/0/`, sedangkan `patch_match_stereo` dan `stereo_fusion` mencari di
  `sparse/`. Salin dulu sebelum menjalankan keduanya:

  ```bash
  cp /content/DENSE/sparse/0/*.bin /content/DENSE/sparse/
  ```

  Tanpa ini muncul error `rigs, cameras, frames, images, points3D files do not
  exist at "/content/DENSE/sparse"`.

> Status: perbaikan path di atas masih divalidasi; keluaran `fused_colab.ply`
> belum terkonfirmasi tersimpan utuh. Evaluasi menunjukkan GPU laptop lokal
> sudah memadai, dan faktor penentu kualitas adalah mode Automatic
> Reconstruction dengan Image Quality High, bukan sekadar perpindahan ke cloud.

## Panduan Pengambilan Video

**Wajib**

- Area dengan sudut siku/geometris tegas, tekstur, dan variasi warna kontras.
- Gerakan kamera melengkung, perlahan, dan stabil.
- Kamera berpindah tempat (translasi nyata), bukan hanya berputar di tempat.
- Kamera sedikit menunduk (sekitar 30–45°); objek terlihat dari beberapa sudut.
- Pencahayaan terang dan merata.
- Jaga jalur robot di koridor dinding solid bertekstur.

**Hindari**

- Permukaan licin/mengkilap/transparan, kaca, cermin, dan pantulan lampu.
- Bidang polos: lantai polos, dinding putih, permukaan tanpa motif.
- Kamera mendongak hingga didominasi dinding putih atau plafon.
- Hentakan atau perubahan arah/sudut mendadak.
- Merekam melewati penghalang (mis. di luar dinding buku) yang memutus overlap
  latar belakang.
- Pencahayaan gelap dan gambar blur.
- Merekam objek dari jarak terlalu dekat (parallax/background mismatch).

## Hasil Pengujian

| Uji | Skenario | Frame terdaftar | Hasil | Catatan |
|-----|----------|-----------------|-------|---------|
| 1 | Kamar (dinding & lantai polos) | 2 | 26 titik | Minim gradien, SIFT gagal |
| 2 | Gelas bermotif (mengkilap, 360°) | 2 | < 50 titik | Pantulan & parallax |
| 3 | Tumbler (warna solid, simetris) | 2 | < 50 titik | Fitur tampak sama dari tiap sudut |
| 4 | Dataset internet | 5 | 328 titik | Wide baseline, lensa tak sinkron |
| 5 | Meja Belajar 1 | 116 | 21.086 sparse / 50.355 dense | Alas meja masih bolong |
| 6 | Kafe (Webcam 1) | 212 | 12.389 sparse / 3.694 vertices | Pantulan kaca merusak depth map |
| 7 | Meja Belajar 2 | 156 | 5.300 sparse / 12.286 vertices | Gerak lebih stabil |
| 8 | Meja Belajar 3 (Uji_Meja3) | 334 | 20.777 sparse / 63.603 vertices / 82.011 faces | Mesh BPA rapat dan detail |
| 9 | Meja dengan webcam (Webcam 2) | 124 | 14.091 sparse / 50.187 vertices / 23.002 faces | Mesh solid |
| 10 | Uji_Webcam3 (meja kafe) | 160 | 8.462 vertices / 794 faces | Linear solver failure, mesh berlubang |
| 11 | Uji_Meja3, Automatic Reconstruction High | 949 | 54.184 sparse / **3.631.076 vertices** | Hasil terbaik saat ini |

## Troubleshooting

| Gejala | Penyebab | Solusi |
|--------|----------|--------|
| Hanya 2–5 kamera terdaftar | Permukaan polos/mengkilap, gerak terlalu cepat | Rekam ulang sesuai panduan; perkecil `--step` |
| `W: Linear solver failure (Eigen failure)` | Frame redundan di titik balik, atau sudut berubah mendadak/latar terhalang | Potong frame redundan; jaga kecepatan translasi konstan |
| Titik dense sedikit pada permukaan polos/kaca | Depth map consistency gagal | Pilih area bertekstur; hindari kaca dan pantulan |
| Mesh BPA berlubang | `Pivoting Ball Radius` = 0 pada titik renggang | Set manual 1.5%–3.0% |
| Mesh menggelembung | Memakai Screened Poisson pada area terbuka | Gunakan Ball Pivoting |
| COLMAP crash di Colab (Qt/OpenGL) | Server tanpa layar | `QT_QPA_PLATFORM=offscreen` |

Parameter di atas adalah nilai awal hasil pengujian dan dapat disesuaikan
dengan dataset. Hasil dengan titik dense terbanyak menjadi penentu.

## Dokumentasi Lengkap

[Buka dokumentasi lengkap di Google Docs](https://docs.google.com/document/d/1KMDZIZ8AzL2EVKxRRsk-u-RHI7P2eDsmt60KuB8Gvr8/edit?usp=sharing)
