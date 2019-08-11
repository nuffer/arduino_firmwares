import thingspeak
import json
import matplotlib.pyplot as plt

ch = thingspeak.Channel(614816,api_key='TJO9VK7CO3M5V9BA')
results = ch.get({'results': 100})


results = json.loads(results)

data = results['feeds']

time_A = []
Pressure_A = []
Temperature_A = []

time_B = []
Pressure_B = []
Temperature_B = []

time_C = []
Pressure_C = []
Temperature_C = []


for i in data:
    t = i['entry_id']

    pa = i['field1']
    ta = i['field2']
    if pa is not None:
        time_A.append(t)
        Pressure_A.append(pa)
        Temperature_A.append(ta)

    pb = i['field3']
    tb = i['field4']
    if pb is not None:
        time_B.append(t)
        Pressure_B.append(pb)
        Temperature_B.append(tb)

    pc = i['field5']
    tc = i['field6']
    if pc is not None:
        time_C.append(t)
        Pressure_C.append(pc)
        Temperature_C.append(tc)

plt.plot(time_A,Pressure_A,'.-')
plt.plot(time_B,Pressure_B,'.-')
plt.plot(time_C,Pressure_C,'.-')
plt.show()



plt.plot(time_A,Temperature_A,'.-')
plt.plot(time_B,Temperature_B,'.-')
plt.plot(time_C,Temperature_C,'.-')
plt.show()

