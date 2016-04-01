#!python3
from http.server import HTTPServer, BaseHTTPRequestHandler

import serial,time, sys


address_port = ('localhost', 50100)

class Data:
     connection = None
     #serial_port = "com3"

class Handler(BaseHTTPRequestHandler):
  
    def do_GET(self):

        # skip over the / in the command
        cmd = self.path[1:]
        # create a list containing the command and all of its parameters
        cmd_list = cmd.split('/')

        result = ''
        
        if  cmd.startswith("poll"):
            try:
                Data.connection.reset_input_buffer()
                string = Data.connection.readline().decode() 
                temp_list = string.strip().split(',')

                value = []
                for i in temp_list:
                    value.append(i.split(':')[1])

                result = 'NX ' + value[0] + '\r\n'
                result = result + 'NY ' + value[1] + '\r\n'
                result = result + 'NI ' + value[2] + '\r\n'
                result = result + 'SX ' + value[3] + '\r\n'
                result = result + 'SY ' + value[4] + '\r\n'
                result = result + 'SI ' + value[5] + '\r\n'
                result = result + 'BX ' + value[6] + '\r\n'
                result = result + 'BY ' + value[7] + '\r\n'
                result = result + 'BI ' + value[8] + '\r\n'
                result = result + 'BR ' + value[9] + '\r\n'
                result = result + 'BT ' + value[10] + '\r\n'
                
                #print(result)
                 
            except:
                pass

            
        http_response = "HTTP/1.1 200 OK" + '\r\n'
        http_response += "Content-Type: text/html; charset=utf-8" + '\r\n'
        http_response += "Content-Length" + str(len(result)) + '\r\n'
        http_response += "Access-Control-Allow-Origin: *" + '\r\n'
        http_response += '\r\n'

        if result != '':
            http_response += str(result) + '\r\n'

        self.wfile.write(http_response.encode('utf-8'))
        

#print("連接序列埠 : ", Data.serial_port)
print("連接序列埠 : ", sys.argv[1] )
#Data.connection = serial.Serial(Data.serial_port, 9600)
Data.connection = serial.Serial(sys.argv[1], 115200)

server = HTTPServer(address_port, Handler)
print ('啟動 -- 伺服程式 :' , address_port )
server.serve_forever()
        
