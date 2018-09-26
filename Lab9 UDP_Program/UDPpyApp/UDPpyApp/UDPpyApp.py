#CLIENT

import socket
import random
import pygame

#////////////////////////////////////////////////////////////////////////////////////
#Function
def send_net_message(message):
    clientSocket.sendto(message.encode('utf-8'),server_addr)
    print("sent: ",message)
#End Function
#////////////////////////////////////////////////////////////////////////////////////

pygame.init()

#Create Window
displaySurface = pygame.display.set_mode((600,400))
pygame.display.set_caption("client")

#Create Socket
clientSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
clientSocket.setblocking(0)       #Non blocking
server_addr = ("127.0.0.1",12000) #(Address, Port)

#send Joing message
player_name = "myName"
send_net_message("<JOIN:"+player_name+">") #Tell server Im joining

#Loop
running = True
while(running):

    #Event Handling
    for event in pygame.event.get():

        if event.type == pygame.QUIT:
            running = False

        if event.type == pygame.KEYDOWN:
            if event.key == pygame.K_ESCAPE:
                running = False
            else:
                send_net_message("<KD:"+str(event.key)+">")

        if event.type == pygame.KEYUP:
            send_net_message("<KU:"+str(event.key)+">")

    pygame.display.update()

send_net_message("<QUIT>") #Tell server im quiting

#Exit
pygame.display.quite()

#////////////////////////////////////////////////////////////////////////////////////
#////////////////////////////////////////////////////////////////////////////////////
#////////////////////////////////////////////////////////////////////////////////////
#////////////////////////////////////////////////////////////////////////////////////
#////////////////////////////////////////////////////////////////////////////////////

#server
#class
class Player(object):
    def __init__(self, IP_address, port, name ="N/A", initX = 0, initY = 0):
        self.port = port
        self.IP_address = IP_address
        self.name = name
        self.x = initX
        self.y = initY
        self.dx = 0
        self.dy = 0
        self.speed = 1.0
        self.size = 15
        self.color = (random.randint(0,255), random.randint(0,255), random.randint(0,255))
        self.keyDown = []
    def dumps(self):
        tempDict = {}
        tempDict["x"] = self.x
        tempDict["y"] = self.y
        tempDict["dx"] = self.dx
        tempDict["dy"] = self.dy
        tempDict["speed"] = self.speed
        tempDict["size"] = self.size
        tempDict["color"] = self.color
        temp_string = msgpack.dumps(tempDict)
        return temp_string
    def load(self):
        tempDict = msgpack.loads(tempDict)
        self.x = tempDict["x"]
        self.y = tempDict["y"]
        self.dx = tempDict["dx"]
        self.dy = tempDict["dy"]
        self.speed = tempDict["speed"]
        self.size = tempDict["size"]
        self.color = tempDict["color"]
    def move(self):
        if 'a' in self.keyDown:
            if self.x > 0:
                self.x -= self.speed
        if 'd' in self.keyDown:
            if self.x < displaySurface.get.width():
                self.x += self.speed
        if 's' in self.keyDown:
            if self.y > 0:
                self.y -= self.speed
        if 'w' in self.keyDown:
            if self.y < displaySurface.get.height():
                self.y += self.speed
    def render(self):
        pygame.draw.circle(displaySurface,self.color,(int(self.x), int(self.y)),int(self.size),0)
#End class
#////////////////////////////////////////////////////////////////////////////////////
#Function
def get_NetMessage(void):
    try:
        message,address = serverSocket.recvfrom(1472) #Full DataGram Len
    except:
        return None, None   #nothing coming in

    message = message.decode('utf-8')               #Decode message
    print("Message: "+message+" Address: "+address) #print message and address
    return message, address                         #Get sent items
#End Function
#////////////////////////////////////////////////////////////////////////////////////
#Function
def message_Processor(message, address):
    if message[0] == '<' and message[-1] == '>':    #Strip message of brackets
        message = message[1:-1]
        if ':' in message:
            com,data = message.split(':')
        else:
            com = message
            data = None

        print("Message: "+message+" Command: "+com+" Data: "+data)
        if com == "JOIN":
            playerList[str(address)] = Player(address[0],address[1],random.randint(0,displaySurface.get.height()),random.randint(0,displaySurface.get.width()))
            print("Player added to list:",data, address)
        elif com == "QUIT":
            del playerList[str(address)]
            print("Player removed from list:",data, address)
        elif com =="KD":
            data = chr(int(data))
            if data not in playerList[str(address)].keyDown:
                playerList[str(address)].keyDown.append(data)
        elif com == "KU":
            data = chr(int(data))
            if data in playerList[str(address)].keyDown:
                playerList[str(address)].keyDown.remove(data)
    else:
        print("invalid message",message,address)
#End Function
#////////////////////////////////////////////////////////////////////////////////////

#Initialization
pygame.init()

#Create Window
displaySurface = pygame.display.set_mode((600,400))
pygame.display.set_caption("server")

#Create Socket
serverSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
serverSocket.setblocking(0) #Non blocking
server_addr = ("",12000) #(Address, Port)
serverSocket.bind(server_addr)

playerList = {}

#Loop
running = True
print("ServerRunning")
while(running):

    #Event Handling
    for event in pygame.event.get():

        if event.type == pygame.QUIT:
            running = False

        if event.type == pygame.KEYDOWN:
            if event.key == pygame.K_ESCAPE:
                running = False

    message,address = get_NetMessage()
    if message != None:
        message_Processor(message,address)

    pygame.draw.rect(displaySurface,(0,0,0),(0,0,600,400),0)
    #update Game state
    for player_id in playerList.keys():
        playerList[player_id].move()
        playerList[player_id].render()

    pygame.display.update()

print("Shutting Down Server")
#Exit
serverSocket.close()
pygame.display.quite()
