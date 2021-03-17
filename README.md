# General framework for a sensors network on campus. 


### to do list
* [x] Phase 1- general architect: connect acceleromter (GY-521 (MPU6050)) to RPi4 and other potential boards (ESP32- LilyGo with sim7000G module)
* [x] Phase 1- general architect: connect sensing devices to Azure IoT Hub
* [x] Phase 1- general architect: Save telemetry data in a SQL database on Azure 
* [ ] Phase 1- general architect: connect 5G module
* [ ] Phase 1- general architect: connect more sensors
* [x] Phase 1- general architect: add an architect diagram
* [ ] Phase 1- general architect: Discuss 2-3 potential use cases in natural assets
* [x] Phase 1- general architect: GDPR, ethical/ data AI approach: fitler data on Edge device before uploading data to Azure

* [ ] Phase 2- use-case 1 architect: custom/general architect
* [ ] Phase 2- use-case 1 architect: obtain use-case specific sensors

* [ ] Phase 3- use-case 1 architect: Link this IoT system to other systems (see: custom vision in  repo)
* [ ] Phase 3- use-case 1 architect: architect optimzations


## General overview - Telemetry (custom vision in another repo):
\
![image output](rpi_azure.png)


# Trying code out:
1. clone repo
2. install dependencies (where relevant)
2. run code specific for each board 
3. ask for connection string: connection strings are linked to our own project (and are typically unshared outside members of the team)
4. set up endpoint through IoT Hub or stream/filter data upload through stream analytics
5. create a SQL database 



## Setup on RPi4 - GY-521 (MPU6050):
\
![image output](rpi_1.jpg)
![image output](rpi_2.jpg)


## Data uploaded to IoT Hub, passed through Stream Analytics:
\
![image output](stream_analytics.png)


## Telemetry data in Azure SQL Database:
\
![image output](Azure_SQL.png)


## Setting up Device Twin in Azure:
\
![image output](device_twin.png)



## Setting up Device Twin in Azure:
\
![image output](/Esp32/Device/tree_dynamics_copy.jpg)