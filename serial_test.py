################
###############
## find the specific port a device is connected to:
# $ ls -l /dev/serial/by-id
###############
###############
###############
###############

# parts of the code adapted from Dr. John Lui's code (sdi_12_reading_v1_0.py)


# ejri

# Notes:
# conencting the devices through a port by id is recommended to prevent connections to wrong ports (GSM ports, etc.)
# each sensor needs to have its own address, individually changed using: 
# ################### defining each sensor to its own unique address. default is 0, so change the other connected ones.
    # ser.write(sdi_12_address + b"A1!") # Data logger command.
    # Request to the sensor to change its address from 1 to a new address of 0.
    # sdi_12_line = ser.readline()
    # sdi_12_line = sdi_12_line[:-2]  # remove \r and \n
    # teros_address_info_1="Sensor address converted to 1:"+str(sdi_12_line.decode('UTF-8'))
    # print(teros_address_info_1)
# read sensor individually 

import serial.tools.list_ports
import serial
import time
import re


def read_soil():
    # Store the device name to open port with later in the script.
    port_device = "/dev/serial/by-id/usb-FTDI_FT232R_USB_UART_AC00Z2TL-if00-port0"

    ser = serial.Serial(
        port=port_device,
        baudrate=9600,
        parity=serial.PARITY_EVEN,
        stopbits=serial.STOPBITS_ONE,
        timeout=10,
        bytesize=serial.SEVENBITS,
    )
    time.sleep(2.5)  # delay at least 1 second delay of the adapter.

    ser.write(
        b"?!"
    )  # Data logger command. Request for a response from any sensor listening on the data line.
    sdi_12_line = ser.readline()
    sdi_12_line = sdi_12_line[:-2]  # remove \r and \n since [0-9]$ has trouble with \r
    m = re.search(b"[0-9a-zA-Z]$", sdi_12_line)  # having trouble with the \r
    sdi_12_address = m.group(0)  # find address
    teros_address = "Sensor address:" + str(sdi_12_address.decode("UTF-8").strip())
    print(teros_address)
    # print("\nSensor address:", sdi_12_address.decode("utf-8"))


    ser.write(b"1I!")  # Data logger command. Request information from sensor 1.
    sdi_12_line = ser.readline()
    sdi_12_line = sdi_12_line[:-2]  # remove \r and \n
    teros_address_info = "Sensor 1 info:" + str(sdi_12_line.decode("UTF-8"))
    print(teros_address_info)
    # print("Sensor info:", sdi_12_line.decode("utf-8"))

    ser.write(b"1M!")  # Data logger command. Request information from sensor 1.
    sdi_12_line = ser.readline()
    sdi_12_line = ser.readline()
    ser.write(b"1D0!")  # Data logger command. Request all data from sensor 1.
    sdi_12_line = ser.readline()
    sdi_12_line = sdi_12_line[:-2]  # remove \r and \n
    teros_reading_1 = "Sensor " + str(sdi_12_line.decode("UTF-8"))
    print(teros_reading_1)
    teros_1 = [x for x in teros_reading_1.split("+")]
    print(teros_1)
    # print("Sensor reading:", sdi_12_line.decode("utf-8"))

    ser.write(b"0I!")  # Data logger command. Request information from sensor 0.
    sdi_12_line = ser.readline()
    sdi_12_line = sdi_12_line[:-2]  # remove \r and \n
    teros_address_info = "Sensor 0 info:" + str(sdi_12_line.decode("UTF-8"))
    print(teros_address_info)
    # print("Sensor info:", sdi_12_line.decode("utf-8"))

    ser.write(b"0M!")  # Data logger command. Request information from sensor 0.
    sdi_12_line = ser.readline()
    sdi_12_line = ser.readline()
    ser.write(b"0D0!")  # Data logger command. Request all data from sensor 0.
    sdi_12_line = ser.readline()
    sdi_12_line = sdi_12_line[:-2]  # remove \r and \n
    teros_reading_0 = "Sensor " + str(sdi_12_line.decode("UTF-8"))
    print(teros_reading_0)
    teros_0 = [x for x in teros_reading_0.split("+")]
    print(teros_0)
    # print("Sensor reading:", sdi_12_line.decode("utf-8"))

    ser.close()


read_soil()