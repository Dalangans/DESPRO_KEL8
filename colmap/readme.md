# Alur Rekonstruksi 3D dengan COLMAP dan MeshLab

Dokumen ini merangkum alur pengolahan video menjadi model 3D menggunakan
COLMAP dan MeshLab. Dokumentasi lengkap, termasuk foto dan hasil pengujian,
tersedia di [Google Docs](#dokumentasi-lengkap).

## Alur Kerja

1. **Rekam objek atau area** menggunakan webcam atau kamera robot.
2. **Ekstrak frame** dari video menggunakan Python dan OpenCV. Gunakan interval
   awal setiap 5 frame, lalu sesuaikan berdasarkan hasil rekonstruksi.
3. **Siapkan proyek COLMAP** dengan membuat proyek baru, memilih folder frame
   sebagai sumber gambar, dan membuat file database.
4. **Ekstrak fitur** dengan model kamera `OPENCV`; biarkan parameter kamera
   kosong atau default bila belum memiliki kalibrasi khusus.
5. **Cocokkan fitur antar-frame**:
   - Untuk frame dari video berurutan, pilih **Sequential** dan aktifkan
     `quadratic_overlap`.
   - Untuk foto yang tidak berurutan, pilih **Exhaustive**.
6. **Jalankan rekonstruksi sparse (SfM)**. Pada opsi rekonstruksi, gunakan
   `merge/complete_max_reproj_error` sebesar `8.00 px`,
   `ignore_two_view_tracks` dalam keadaan nonaktif, `abs_pose_max_error`
   sebesar `16–20 px`, dan `abs_pose_min_num_inliers` sebesar `15`.
7. **Jalankan rekonstruksi dense (MVS)** pada folder workspace kosong secara
   berurutan: **Undistortion → Stereo → Fusion**. Hasil point cloud biasanya
   tersimpan sebagai `fused.ply`.
8. **Bersihkan point cloud di MeshLab** dengan membuka `fused.ply` dan
   menghapus titik-titik liar (*outlier*) yang bukan bagian objek. Penghapusan
   dapat dilakukan manual atau menggunakan filter **Select Outliers**.
9. **Hitung normal permukaan** melalui **Compute Normals for Point Sets**.
   Gunakan `Number of neighbors` sebesar `16` sebagai nilai awal.
10. **Bentuk mesh** menggunakan **Surface Reconstruction: Ball Pivoting**.
    Gunakan `Pivoting Ball Radius` otomatis (`0`), `Clustering radius` `20%`,
    dan `Angle Threshold` `90°` sebagai nilai awal.
11. **Ekspor model** melalui **Export Mesh As...** ke format `.ply` atau `.obj`.

## Panduan Singkat Pengambilan Video

- Rekam dengan gerakan melengkung yang perlahan dan stabil, serta sertakan
  perpindahan posisi kamera; jangan hanya berputar di tempat.
- Arahkan kamera sedikit menunduk (sekitar 30–45°) dan jaga objek tetap terlihat
  dari beberapa sudut.
- Pilih area dengan tekstur, variasi warna, dan sudut atau bentuk yang jelas.
- Hindari permukaan polos, objek mengilap atau transparan, pantulan kaca,
  perubahan gerak mendadak, pencahayaan gelap, dan gambar blur.
- Usahakan pencahayaan terang dan merata.

Parameter di atas adalah nilai awal pengujian dan dapat disesuaikan dengan
dataset. Periksa log COLMAP untuk memastikan kamera berhasil terdaftar sebelum
melanjutkan ke tahap dense reconstruction.

## Dokumentasi Lengkap

[Buka dokumentasi lengkap di Google Docs](https://docs.google.com/document/d/1KMDZIZ8AzL2EVKxRRsk-u-RHI7P2eDsmt60KuB8Gvr8/edit?usp=sharing)