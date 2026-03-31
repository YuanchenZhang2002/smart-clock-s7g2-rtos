# Smart Alarm Clock with Fall Detection (ThreadX RTOS)

A professional-grade embedded system project developed on the **Renesas SK-S7G2** platform using **e2 studio**.

## 🚀 Key Features
**Real-time Clock & Alarm:** Managed via RTC driver with high precision and power-loss retention.
**Health Monitoring:** Integrated **MPU6050 IMU** to detect falls (threshold > 3.5g) using vector magnitude calculation.
**Environment Sensing:** Real-time temperature display with **Mutex-protected** data integrity.
**Interactive UI:** A 16-bit RGB565 display powered by **GUIX**, featuring a custom state machine for time/alarm configuration.

## 🛠️ Technical Implementation
**OS:** ThreadX RTOS with 3 independent threads: Main, Sensor, and Clock & Alarm.
**Communication:** Utilized **Queues** for fall alerts, **Semaphores** for interrupt synchronization, and **Mutexes** for shared sensor data.
**Fall Detection Algorithm:** Calculated using the total acceleration vector magnitude:  
  $$\text{Total Acc} = \sqrt{ax^2 + ay^2 + az^2}$$
**Optimization:** Resolved a critical UI freezing issue by implementing a software frequency divider (10ms sensor sampling vs 200ms UI refresh).
* **Resource Efficiency:** Optimized RAM footprint to **84%** by fine-tuning hardware buffers and heap allocation.

## 📈 Performance & Evaluation
Successfully passed all verification tests:
- Time rollover (23:59:59 -> 00:00:00).
- Fall detection immediate response.
- Alarm auto-stop logic (1-minute timer).

## 📺 Demo Video
Watch the system in action here: [YouTube Link](https://youtu.be/BSEao_yOBUo).

## 📸 Hardware & System Preview

### 1. Physical Implementation
The system runs on the **Renesas SK-S7G2** development kit, featuring a 2.4-inch QVGA display and integrated sensors.
<p align="center">
  <img src="https://github.com/user-attachments/assets/fe77126d-7901-466f-8007-3fd3c6530abf" width="450" alt="Hardware Running">
  <br>
  <i>Figure 1: Real-time Clock & UI running on the Cortex-M4 target.</i>
</p>

### 2. Optimized Resource Footprint
One of the key technical challenges was managing the limited SRAM. By reconfiguring hardware buffers and the heap size, RAM usage was successfully optimized to 84%.
<p align="center">
  <img src="https://github.com/user-attachments/assets/6c575f53-70d1-4efb-9603-ab4719ade630" width="600" alt="Memory Usage">
  <br>
  <i>Figure 2: Memory usage analysis showing 543KB used out of 640KB.</i>
</p>

### 3. Modular Software Architecture
The project follows a clean, modular structure with dedicated threads for core logic, sensor data acquisition, and UI management.
<p align="center">
  <img src="https://github.com/user-attachments/assets/ac964d92-68cc-44ad-8451-1c645c8924af" width="400" alt="Project Structure">
  <br>
  <i>Figure 3: Modular directory structure separating GUI, hardware drivers, and RTOS thread logic.</i>
</p>
