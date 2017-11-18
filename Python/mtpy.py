import zmq
import json

context = zmq.Context()
socket = context.socket(zmq.REP)
socket_url = "tcp://0.0.0.0:5555"
socket.bind(socket_url)

print("Listening on {0}".format(socket_url))

while True:
    message = socket.recv()
    # print("Received request: ", message.decode('utf-8'))
    socket.send_string("OK")
    message_dict = json.loads(message.decode('utf-8'))
    print(message_dict)
