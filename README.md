# Smart Alarm Clock with Fall Detection (ThreadX RTOS)

[cite_start]A professional-grade embedded system project developed on the **Renesas SK-S7G2** platform using **e2 studio**[cite: 2, 25].

## 🚀 Key Features
* [cite_start]**Real-time Clock & Alarm:** Managed via RTC driver with high precision and power-loss retention[cite: 45, 46].
* [cite_start]**Health Monitoring:** Integrated **MPU6050 IMU** to detect falls (threshold > 3.5g) using vector magnitude calculation[cite: 28, 95, 96].
* [cite_start]**Environment Sensing:** Real-time temperature display with **Mutex-protected** data integrity[cite: 37, 82].
* [cite_start]**Interactive UI:** A 16-bit RGB565 display powered by **GUIX**, featuring a custom state machine for time/alarm configuration[cite: 110, 52, 53].

## 🛠️ Technical Implementation
* [cite_start]**OS:** ThreadX RTOS with 3 independent threads: Main, Sensor, and Clock & Alarm[cite: 23, 34].
* [cite_start]**Communication:** Utilized **Queues** for fall alerts, **Semaphores** for interrupt synchronization, and **Mutexes** for shared sensor data[cite: 37, 38, 43, 107].
* [cite_start]**Fall Detection Algorithm:** Calculated using the total acceleration vector magnitude[cite: 95]:  
  $$\text{Total Acc} = \sqrt{ax^2 + ay^2 + az^2}$$
* [cite_start]**Optimization:** Resolved a critical UI freezing issue by implementing a software frequency divider (10ms sensor sampling vs 200ms UI refresh)[cite: 101, 104, 143].
* **Resource Efficiency:** Optimized RAM footprint to **84%** by fine-tuning hardware buffers and heap allocation.

## 📈 Performance & Evaluation
[cite_start]Successfully passed all verification tests[cite: 145]:
- [cite_start]Time rollover (23:59:59 -> 00:00:00)[cite: 145].
- [cite_start]Fall detection immediate response[cite: 145].
- [cite_start]Alarm auto-stop logic (1-minute timer)[cite: 145].

## 📺 Demo Video
[cite_start]Watch the system in action here: [YouTube Link](https://youtu.be/BSEao_yOBUo)[cite: 147].

## 📸 Hardware & System Preview

### 1. Physical Implementation
[cite_start]The system runs on the **Renesas SK-S7G2** development kit, featuring a 2.4-inch QVGA display and integrated sensors[cite: 25, 29].
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
[cite_start]The project follows a clean, modular structure with dedicated threads for core logic, sensor data acquisition, and UI management[cite: 34].
<p align="center">
  <img src="https://github.com/user-attachments/assets/ac964d92-68cc-44ad-8451-1c645c8924af" width="400" alt="Project Structure">
  <br>
  <i>Figure 3: Modular directory structure separating GUI, hardware drivers, and RTOS thread logic.</i>
</p>
