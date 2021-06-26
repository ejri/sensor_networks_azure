import concurrent.futures
# import keyboard
import subprocess
import threading
import time
from timeit import default_timer as timer

# from pynput.keyboard import Key, Controller

# keyboard = Controller()  # Create the controller

# function to input (q + enter)  while Alexa is running to quit application
# after 10 seconds  (time_to_stop) is specified in running thread function
def start_timing(time_to_stop):
    stay_connected = True
    start = timer()
    while stay_connected == True:
        end = timer()
        time_difference = end - start
        if time_difference > time_to_stop:
            # input('q')
            # keyboard.press("q")
            # keyboard.release("q")
            time.sleep(0.2)
            # keyboard.press(Key.enter)
            # keyboard.release(Key.enter)
            time.sleep(0.2)
            stay_connected = False
            break

    return "Alexa ran out of time."


def start_alexa():
    subprocess.call("bash /home/pi/startsample.sh", shell=True)


with concurrent.futures.ThreadPoolExecutor() as executer:
    f1 = executer.submit(start_alexa)
    # time_to_stop = 30 seconds
    f2 = executer.submit(start_timing, 30)
    print(f2.result())
