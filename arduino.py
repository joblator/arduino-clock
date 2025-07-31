import serial
import time
import pytz
from datetime import datetime
arduino = serial.Serial('COM3',9600,timeout=1)
timeZoneList = ["Asia/Tel_Aviv","Europe/Prague","America/New_York"]
time.sleep(2)
current_time = datetime.now(pytz.timezone(timeZoneList[2]))
byteTime = bytes(current_time.strftime("%H:%M:%S\n"),'utf-8')
print("hi")
arduino.write(byteTime)
arduino.close()
print(current_time.strftime("%H:%M:%S"))