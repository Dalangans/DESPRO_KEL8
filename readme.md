# 🤖 REVEAL — Real-Time Exploration & Visual Environment Analysis for Rescue Operations

[![System OS](https://img.shields.io/badge/OS-Ubuntu%2018.04%20%2F%20Linux-orange?logo=ubuntu)](https://ubuntu.com/)
[![Algorithm](https://img.shields.io/badge/SLAM-ORB--SLAM3-blue)](https://github.com/UZ-SLAMLab/ORB_SLAM3)
[![Reconstruction](https://img.shields.io/badge/3D%20Mapping-COLMAP-green)](https://colmap.github.io/)
[![Hardware Support](https://img.shields.io/badge/Hardware-NVIDIA%20Jetson%20Nano%204GB-76B900?logo=nvidia)](https://developer.nvidia.com/embedded/jetson-nano-developer-kit)

Official repository for **Desain Proyek 1 & 2 (Kelompok 28)** — Department of Electrical Engineering, Universitas Indonesia.

> **REVEAL** is a disaster-response robotic framework designed to navigate GPS-denied indoor environments, tracking its real-time trajectory while streaming data for offline high-fidelity 3D scene reconstruction.

---

## 📌 Project Overview & Architecture

In post-disaster Search and Rescue (SAR) scenarios, structural instability and loss of GPS signals severely hamper human entry. REVEAL decouples spatial understanding into a two-path data pipeline:
1. **Real-Time On-Board Path (Real-Time SLAM):** Real-time localization, camera intrinsic calibration, and visual tracking using **ORB-SLAM3**.
2. **Offline Base-Station Path (Photogrammetry):** Photogrammetry and dense 3D point-cloud mapping using **COLMAP**.

```
[ Camera Feed ] ───► [ Camera Calibration Matrix (K) ]
                              │
               ┌──────────────┴──────────────┐
               ▼                             ▼
   [ Real-Time Tracking ]         [ Sequential Frames ]
       (ORB-SLAM3)                      (COLMAP)
               │                             │
   Real-Time Trajectory (x,y,z)    Offline 3D Reconstruction
```

---

## 🚀 Team Responsibilities & Module Division

Project REVEAL divides system development into 5 specialized technical modules:

### 🔹 Person A — 3D Reconstruction & Photogrammetry (COLMAP)
- [x] **Pipeline COLMAP:** Offline dense point-cloud & 3D mesh generation pipeline setup.
- [x] **Dataset Validation:** Reconstruction experiments on benchmark datasets (*South Building*) and local room environments (*Gedung S UI*).
- [x] **Model Rendering:** Feature matching optimization for high-density visual point clouds.

### 🔹 Person B — Real-Time Positioning & Navigation Stack (ORB-SLAM3 - Nabiel Harits Utomo)
- [x] **SLAM Stack Build:** Native compilation and dependency management of ORB-SLAM3 (`Pangolin`, `DBoW2`, `g2o`, `OpenCV`).
- [x] **Simulation Verification:** Benchmarking monocular tracking performance against the **TUM Freiburg1_xyz** dataset.
- [x] **Camera Calibration:** Automated OpenCV checkerboard calibration routines for intrinsic matrix ($K$) and distortion coefficients ($k_1, k_2, p_1, p_2$).
- [x] **Embedded Deployment:** Tuning `.yaml` configs and frame rates for **NVIDIA Jetson Nano (4GB LPDDR4)** on-board execution.

### 🔹 Person C — System Architecture, Communication & Literature Review
- [x] **System Architecture:** Designing edge-to-base station communication block diagrams and network hierarchy.
- [x] **Literature Synthesis:** Academic review of state-of-the-art visual SLAM and rescue robotics frameworks.
- [x] **Data Transmission:** Planning bandwidth-efficient image stream transfers from robot to base station.

### 🔹 Person D — Hardware Engineering & Power Systems
- [x] **Mechanical Design:** Sizing and assembly design of the robot chassis, drive mechanisms, and camera mounts.
- [x] **Power Distribution:** Circuit regulation for 5V/4A Jetson Nano power delivery and motor driver power management.
- [x] **BOM & Budgeting:** Component procurement, cost optimization (~Rp 2 Million target), and bill-of-materials management.

### 🔹 Person E — Embedded Control & Software Pipeline Integration
- [x] **Data Pipeline Design:** Flowcharting dual-path real-time (SLAM) and offline (COLMAP) data synchronization.
- [x] **Embedded Middleware:** Interfacing microcontrollers, motor drivers, and low-level actuation loops with the high-level OS.
- [x] **System Integration:** Coordinating real-time trajectory output parsing to support navigation control.

---

## ⚙️ Prerequisites & Dependencies

Before building the repository, ensure your environment meets the following requirements:

### Operating System
- Ubuntu 18.04 / 20.04 LTS (x86_64 or ARM64/aarch64)

### Core Libraries
- **OpenCV** $\ge$ 3.2.0 (`python3-opencv` / C++ library)
- **Pangolin** (OpenGL display and GUI)
- **Eigen3** $\ge$ 3.1.0
- **g2o** & **DBoW2** (Included in ORB-SLAM3 thirdparty builds)

---

## 🛠️ Quickstart & Usage

### 1. Repository Setup
```bash
# Clone this repository
git clone [https://github.com/](https://github.com/)<your-username>/REVEAL-ORBSLAM3-Stack.git
cd REVEAL-ORBSLAM3-Stack
```

### 2. Camera Calibration (Intrinsic Matrix Extraction)
To capture calibration samples using a physical webcam or connected sensor:
```bash
# Run photo acquisition script (press SPACE to save, ESC to exit)
python3 scripts/take_photos.py

# Execute calibration processing
python3 scripts/cameraCalibration.py
```
*The resulting camera matrix ($K$) will be saved to `config/Custom_Robot.yaml`.*

### 3. Run ORB-SLAM3 Simulation (TUM Dataset Example)
```bash
# Execute Monocular SLAM
./Examples/Monocular/mono_tum Vocabulary/ORBvoc.txt Examples/Monocular/TUM1.yaml PATH_TO_SEQUENCE_FOLDER
```

---

## 🗓️ Roadmap & DesPro 2 Milestones

| Timeline | Phase | Focus Deliverables | Status |
| :--- | :--- | :--- | :--- |
| **Aug (W1-W4)** | **DesPro 1** | Software stack compilation, simulation validation (TUM), & proposal defense. | ✅ Completed |
| **Sep (W1-W4)** | **Calibration & Setup** | Camera calibration with physical robot sensor & configuration tuning. | 🟡 In Progress |
| **Oct (W1-W4)** | **Hardware Assembly** | Mounting sensor suite & mini PC to physical robot chassis. | ⏳ Scheduled |
| **Nov (W1-W4)** | **System Integration** | On-board deployment to NVIDIA Jetson Nano & real-time trajectory tracking. | ⏳ Scheduled |
| **Dec (W1-W4)** | **Final Testing** | Field test in simulated disaster environment & trajectory export to COLMAP. | ⏳ Scheduled |

---

## 👥 Team & Academic Supervision

- **Institution:** Department of Electrical Engineering, Universitas Indonesia (FTUI)
- **Course:** Desain Proyek 1 & 2 (2026) — Kelompok 28
- **Project Lead / Software Stack (Person B):** Nabiel Harits Utomo
- **Team Members:**
  - **Person A:** 3D Reconstruction & Photogrammetry Lead
  - **Person B:** Real-Time SLAM & Positioning Lead
  - **Person C:** System Architecture & Communication Lead
  - **Person D:** Hardware & Power Systems Lead
  - **Person E:** Embedded Control & Data Pipeline Lead
