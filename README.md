# MyFlightController

<img width="998" height="568" alt="Screenshot 2026-10-27 022806" src="https://github.com/user-attachments/assets/27de6353-2f26-401a-ae41-a2943c7433db" />


So I made a custom flight controller which can be used for an RC Plane . it is based on STM32 and The Flight Controller  has a IMU's and a barometer for precise data collection. It has connectors for 2 Servos to control the wind / 2 ESCs and a reciever . It will work on a 12V battery. 

## Features
- Base on  STM32F401CBUx 
- Has IMU (MPU_6050)
- Has a Barometer (BMP280)
- Works on 12 V battery
- Has multiple voltage regulators for  (5v , 3.3v)

### BOM

| No. | Designation | Component / Value | Quantity per PCB | Price (USD) | JLCPCB Link |
|---:|---|---|---:|---:|---|
| 1 | C1, C3, C7 | 10uF Capacitor | 3 | $0.47 | [View](https://jlcpcb.com/partdetail/20411-CL10A106KP8NNNC/C19702) |
| 2 | C2, C4, C5, C8 | 0.1uF Capacitor | 4 | $0.16 | [View](https://jlcpcb.com/partdetail/YAGEO-CC0603KRX7R9BB104/C14663) |
| 3 | C9, C10 | 100nF Capacitor | 2 | $0.16 | [View](https://jlcpcb.com/partdetail/YAGEO-CC0603KRX7R9BB104/C14663) |
| 4 | C6 | 2.2uF Capacitor | 1 | $0.20 | [View](https://jlcpcb.com/partdetail/24366-CL10A225KO8NNNC/C23630) |
| 5 | C11, C12 | 12pF Capacitor | 2 | $0.14 | [View](https://jlcpcb.com/partdetail/39506-CL10C120JB8NNNC/C38523) |
| 6 | C13, C15, C16, C17 | 100pF Capacitor | 4 | $0.16 | [View](https://jlcpcb.com/partdetail/15530-CL10C101JB8NNNC/C14858) |
| 7 | D1, D2 | LED | 2 | $0.62 | [View](https://jlcpcb.com/partdetail/MEIHUA-MHT126UBCT/C55613448) |
| 8 | R1, R2 | 33 Ohm Resistor | 2 | $0.05 | [View](https://jlcpcb.com/partdetail/23867-0603WAF330JT5E/C23140) |
| 9 | R3 | 50K Resistor | 1 | $0.68 | [View](https://jlcpcb.com/partdetail/YAGEO-RT0603BRD0750KL/C861451) |
| 10 | R4 | 50K Resistor | 1 | $0.68 | [View](https://jlcpcb.com/partdetail/YAGEO-RT0603BRD0750KL/C861451) |
| 11 | R5, R6 | 1K Resistor | 2 | $0.03 | [View](https://jlcpcb.com/partdetail/21904-0603WAF1001T5E/C21190) |
| 12 | R7 | 1K Resistor | 1 | $0.03 | [View](https://jlcpcb.com/partdetail/21904-0603WAF1001T5E/C21190) |
| 13 | SW1 | DPDT Slide Switch | 1 | $1.05 | [View](https://jlcpcb.com/partdetail/55982-DSIC01LHGET/C54957) |
| 14 | U1 | AMS1117-3.3 Regulator | 1 | $0.39 | [View](https://jlcpcb.com/partdetail/Advanced_MonolithicSystems-AMS1117_33/C6186) |
| 15 | U2 | L7805 Regulator | 1 | $0.35 | [View](https://jlcpcb.com/partdetail/OSEN-L7805/C22378639) |
| 16 | U3 | MPU6050 Sensor | 1 | $41.99 | [View](https://jlcpcb.com/partdetail/TDKInvenSense-MPU6050/C24112) |
| 17 | U4 | BMP280 Sensor | 1 | $26.18 | [View](https://jlcpcb.com/partdetail/BoschSensortec-BMP280/C83291) |
| 18 | U5 | STM32F401CBU6 MCU | 1 | $17.20 | [View](https://jlcpcb.com/partdetail/STMicroelectronics-STM32F401CBU6/C118825) |
| 19 | Y1 | Crystal Oscillator | 1 | $0.37 | [View](https://jlcpcb.com/partdetail/2606-X49SM3579545MSD2SC1/C2237) |

## PCB Total Supply 

| Description | Price (USD) |
|---|---:|
| PCB Price | $4.00 |
| Setup Fee | $25.75 |
| Stencil | $8.27 |
| Panel | $0.00 |
| Large Size | $0.00 |
| Components (15 items) | $89.85 |
| Feeders Loading Fee | $20.15 |
| SMT Assembly | $0.91 |
| Hand-soldering Labor Fee | $3.61 |
| Manual Assembly | $0.30 |
| Packaging Fee | $0.50 |
| Shipping | $22.12 |
| **GRAND TOTAL** | **$175.46** |


### Schematic
<img width="1025" height="606" alt="Screenshot 2026-10-27 020426" src="https://github.com/user-attachments/assets/6baf4480-91b9-4835-9ee2-7628c4cb6a47" />


 ### PCB Design 

 <img width="1195" height="615" alt="Screenshot 2026-10-27 020403" src="https://github.com/user-attachments/assets/3851c1a3-d488-43a2-b151-8fdda0afccff" />

  ### 3D View 
<img width="998" height="568" alt="Screenshot 2026-10-27 022806" src="https://github.com/user-attachments/assets/efc79db0-7c6d-48a3-abcc-69da4578d353" />

<img width="955" height="580" alt="image" src="https://github.com/user-attachments/assets/1e57e487-e6ca-43b5-9170-4da139fd85db" />


