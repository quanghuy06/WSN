import paho.mqtt.client as mqtt
import pymysql
import json
from datetime import datetime
db = pymysql.connect("localhost", "huytq","Quanghuy@123","wsn")
print ("Running....")
cursor = db.cursor()

def on_connect(client, userdata, flags, rc):
    client.subscribe("DHT11")
    #client.subscribe("dataButton")
def on_message(client, userdata, msg):
    if msg.topic == "DHT11":
        val = json.loads(msg.payload)
        sql ="INSERT INTO esp8266(temp,hum,time) VALUES('%s','%s','%s')"%(str(val["temp"]),str(val["hum"]),str(datetime.now()))
        cursor.execute(sql)
        db.commit()
        print(val)
        print("Inserted")
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.username_pw_set(username="huytq",password="Quanghuy@123")
client.connect("172.20.10.13", 1883, 60)
try:
    client.loop_forever()
except KeyboardInterrupt:
    client.loop_stop()
    print ("Client Disconnect")
    db.close()
    print ("Close Database")


