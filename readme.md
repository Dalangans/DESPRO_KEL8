# 🤖 REVEAL — Robot for Exploration & Visualization in Emergency Affected Locations

[![System OS](https://img.shields.io/badge/OS-Ubuntu%2018.04%20%2F%2020.04-orange?logo=ubuntu)](https://ubuntu.com/)
[![JetPack SDK](https://img.shields.io/badge/JetPack-4.6%20(ARM64)-green?logo=nvidia)](https://developer.nvidia.com/embedded/jetpack)
[![Middleware](https://img.shields.io/badge/ROS-Noetic-blue?logo=ros)](https://www.ros.org/)
[![SLAM Algorithm](https://img.shields.io/badge/Real--Time%20SLAM-ORB--SLAM3-purple)](https://github.com/UZ-SLAMLab/ORB_SLAM3)
[![Reconstruction](https://img.shields.io/badge/3D%20Mapping-COLMAP%204.0.3-brightgreen)](https://colmap.github.io/)
[![Hardware](https://img.shields.io/badge/Compute-NVIDIA%20Jetson%20Nano%204GB-76B900?logo=nvidia)](https://developer.nvidia.com/embedded/jetson-nano-developer-kit)

Official repository for **Desain Proyek 1 & 2 (Kelompok 8)** — Department of Electrical Engineering, Universitas Indonesia (FTUI).

> **REVEAL** is a disaster-response teleoperated reconnaissance robotic framework designed for GPS-denied indoor environments. It streams visual and inertial data to track real-time robot trajectories (ORB-SLAM3) while logging data for offline high-fidelity 3D scene reconstruction (COLMAP) to assist Search and Rescue (SAR) teams during the critical **Golden Hour**.

---

## 📌 Project Overview & System Architecture

In structural collapses caused by seismic activity, conventional 2D cameras fail to capture spatial depth and distance scales, putting SAR personnel at high risk. REVEAL decouples spatial understanding into a two-path architecture:

1. **Real-Time On-Board Path (Visual SLAM):** Real-time localization, camera intrinsic calibration, and visual tracking using **ORB-SLAM3** displayed on RViz/Pangolin.
2. **Offline Base-Station Path (Photogrammetry):** Sequential frame extraction from `.bag` recordings and dense 3D point-cloud mapping using **COLMAP** and **MeshLab**.

```
                           [ REVEAL ROBOT UNIT ]
     ┌──────────────────────────────────────────────────────────────┐
     │  Camera RGB-D / Webcam (640x480 @ 30fps) + MPU6050 IMU       │
     │  Compute Board: NVIDIA Jetson Nano 4GB (Ubuntu / ROS Noetic) │
     └──────────────────────────────┬───────────────────────────────┘
                                    │  Wi-Fi Stream (ROS Topics / rosbag)
                                    ▼
                          [ BASE STATION LAPTOP ]
               ┌────────────────────┴────────────────────┐
               ▼                                         ▼
   [ Real-Time Tracking ]                    [ Sequential Frames ]
       (ORB-SLAM3)                                (COLMAP)
               │                                         │
   Real-Time Trajectory (x,y,z)            Offline Dense 3D Mesh
   (RViz / Pangolin Viewer)               (MeshLab / CloudCompare)
```

---

## 🛠️ Hardware & Software Specifications

### 🤖 Robot Unit Hardware
* **Compute Board:** NVIDIA Jetson Nano Developer Kit (4GB LPDDR4, 128-core Maxwell GPU)
* **Visual Sensor:** Webcam / RGB-D Camera (Logitech C270 HD, 640x480 @ 30 FPS)
* **Inertial Sensor:** MPU6050 6-Axis Accelerometer + Gyroscope (I²C interface)
* **Drive System:** 4WD Acrylic Chassis Kit + 4x TT DC Motor Gearbox + L298N Motor Driver
* **Power Supply:** 4x Li-Ion 14500 Baterai (7.4V Series) + DC-DC Step-Down Buck Converter (5V/4A via J48 Jack)
* **Lighting:** High-Brightness White LED Module (For low-light disaster rubble inspection)

### 💻 Base Station Hardware & Peripherals
* **Compute Laptop:** Intel Core i7, 16GB RAM, Dedicated NVIDIA GPU (CUDA Enabled)
* **Network Infrastructure:** TP-Link AC1200 Dual-Band Router (High Throughput)
* **Controller:** Wireless DualShock 3 (PlayStation Joystick via `joy_node`)

---

## 🚀 Team Responsibilities & Module Division

### 🔹 Aliya — 3D Reconstruction & Photogrammetry (COLMAP)
- [x] **Pipeline COLMAP Setup:** Configured SfM (Structure from Motion) and MVS (Multi-View Stereo) pipeline with CUDA acceleration.
- [x] **Dataset Validation:** Reconstruction experiments on public datasets (*South Building*, 128 photos) and internal environment datasets (*Gedung S UI*, 85 photos).
- [x] **Post-Processing:** Mesh filtering and Poisson surface reconstruction using **MeshLab**.

### 🔹 Nabiel — Real-Time Positioning & Visual SLAM Stack
- [x] **ORB-SLAM3 Stack Compilation:** Full native compilation of ORB-SLAM3 and dependencies (`Pangolin`, `DBoW2`, `g2o`, `OpenCV 3.2+`) on ARM64/x86 Ubuntu environments.
- [x] **Simulation Benchmarking:** Verified tracking stability against the **TUM Freiburg1_xyz** benchmark dataset.
- [x] **Camera Calibration Pipeline:** Automated OpenCV checkerboard calibration script to derive intrinsic matrix ($K$) and distortion parameters ($k_1, k_2, p_1, p_2$).
- [x] **Jetson Deployment Strategy:** Optimization of `.yaml` configuration files and feature extraction limits for real-time Jetson Nano execution.

### 🔹 Mirza — System Architecture, Network Infrastructure & Literature
- [x] **System Architecture Design:** Edge-to-base station network block diagrams and communication flowcharts.
- [x] **Literature Synthesis:** Comprehensive review of Visual-Inertial SLAM, EKF fusion, and PROSAC dynamic filtering.
- [x] **Network Optimization:** Wi-Fi bandwidth allocation for streaming ROS Topics and JPEG-compressed video feeds (~22 Mbps).

### 🔹 Sabil — Mechanical Engineering, Power Systems & Budgeting
- [x] **Hardware Assembly & CAD:** Mechanical chassis design, 3D printing custom casing, and camera mounting brackets.
- [x] **Power Management:** Custom Power Distribution Board (PDB) & 5V/4A DC step-down regulator for Jetson Nano.
- [x] **BOM Management:** Total component procurement and budget management targeting ~Rp 2.04 Million.

### 🔹 Audy — Embedded Control & Software Pipeline Integration
- [x] **Data Pipeline Sync:** Designed dual-path flowcharts linking real-time SLAM streaming with offline `rosbag` logging.
- [x] **Motor Actuation Middleware:** Custom ROS Node translation (`geometry_msgs/Twist` to PWM signals via `Jetson.GPIO`).
- [x] **Teleoperation Setup:** Integration of `joy_node` for wireless PlayStation DualShock 3 teleoperation.

---

## ⚙️ Prerequisites & Installation

### Operating System
* **Robot Unit:** Ubuntu 18.04 LTS (JetPack 4.6 for Jetson Nano ARM64)
* **Base Station:** Ubuntu 18.04 / 20.04 LTS (x86_64 CUDA Enabled)

### Core Software Stack
* **Middleware:** ROS Noetic / Melodic
* **Vision & SLAM:** OpenCV $\ge$ 3.2.0, Pangolin v0.6, Eigen3, ORB-SLAM3
* **3D Reconstruction:** COLMAP 4.0.3, MeshLab, CloudCompare

---

## 🛠️ Quickstart & Usage

### 1. Repository Clone
```bash
git clone [https://github.com/](https://github.com/)<your-username>/REVEAL-ORBSLAM3-Stack.git
cd REVEAL-ORBSLAM3-Stack
```

### 2. Camera Calibration (Intrinsic Matrix Extraction)
Capture calibration samples using the robot's webcam:
```bash
# Step 1: Capture checkerboard images (Press SPACE to capture, ESC to finish)
python3 scripts/take_photos.py

# Step 2: Calculate Camera Matrix K and Distortion Coefficients
python3 scripts/cameraCalibration.py
```
*The output parameters will automatically format into `config/Custom_Robot.yaml`.*

### 3. Launching ORB-SLAM3 Tracking (Simulation / TUM Dataset)
```bash
./Examples/Monocular/mono_tum Vocabulary/ORBvoc.txt Examples/Monocular/TUM1.yaml PATH_TO_TUM_DATASET_FOLDER
```

### 4. ROS Data Logging for COLMAP Pipeline
```bash
# Record all sensor topics into a ROS bag file
rosbag record -O reveal_session.bag /image_raw /imu/data /tf
```

---

## 🗓️ DesPro 2 Timeline & Milestones

| Code | Phase / Task | Key Deliverables | Schedule |
| :---: | :--- | :--- | :---: |
| **A** | **Procurement** | Jetson Nano 4GB, sasis 4WD, webcam, IMU MPU6050, & 5V/4A Buck converter. | Aug (W1-W4) |
| **B** | **Software Optimization** | Camera intrinsic calibration with OpenCV (`Custom_Robot.yaml`) & ORBvoc tuning. | Aug - Sep |
| **C** | **Hardware Assembly** | Mechanical mounting, 3D printed casing, & power distribution board (PDB). | Sep - Oct |
| **D** | **System Integration** | Deployment of ORB-SLAM3 to Jetson Nano & motor control via `Jetson.GPIO`. | Nov (W1-W4) |
| **E** | **Field Testing** | Indoor trajectory tracking test & exporting `.bag` image sequences to COLMAP. | Dec (W1-W2) |
| **F-G** | **Finalization** | Network latency tuning, final report preparation, and live rescue demonstration. | Dec (W3-W4) |

---

## 👥 Project Supervision & Team Members

* **Institution:** Department of Electrical Engineering, Universitas Indonesia (FTUI)
* **Course:** Desain Proyek 1 & 2 (2026) — Kelompok 8
* **Academic Advisor:** Alfan Prasekal, S.T., M.Sc.(DIC)

### Team Members:
* **Aliya Rizqiningrum Salamun** (NPM: 2306161813)
* **Audy Natalie Cecilia** (NPM: 2306266962)
* **Mirza Wisnu Armano** (NPM: 2306156901)
* **Muhammad Sabililhaq** (NPM: 2306202795)
* **Nabiel Harits Utomo** (NPM: 2306267044) 
