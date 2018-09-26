# client2 - Improved version (from the code we wrote in class)

# Improvement: This version gets the game state as a dictionary
#              that includes the list of player dictionaries.
#              The dictionary is deserialized using msgpack.
#
# Note: UDP is used and there is no reliability mechanism coded.


import socket
import pygame
import msgpack
import random
import time


class PlayerClient(object):
    # This object stores the player info for client players
    def __init__(self, name="unknown", init_x=0.0, init_y=0.0):
        self.x = init_x
        self.y = init_y
        self.name = name
        self.size = 15
        self.speed = 1.0
        self.color = (255, 255, 255)
        self.alive = 2.0

    def dump_dict(self):
        temp_dict = {}
        temp_dict["x"] = self.x
        temp_dict["y"] = self.y
        temp_dict["name"] = self.name
        temp_dict["size"] = self.size
        temp_dict["color"] = self.color
        temp_dict["alive"] = self.alive
        return temp_dict

    def load_dict(self, temp_dict):
        self.x = temp_dict["x"]
        self.y = temp_dict["y"]
        self.name = temp_dict["name"]
        self.size = temp_dict["size"]
        self.color = temp_dict["color"]
        self.alive = temp_dict["alive"]

    def render(self):
        pygame.draw.circle(ds, self.color, (int(self.x), int(self.y)), int(self.size), 0)
        pygame.draw.circle(ds, (0, 0, 0), (int(self.x - self.size * 0.3), int(self.y - self.size * 0.3)),
                           int(self.size * 0.2), 0)
        pygame.draw.circle(ds, (0, 0, 0), (int(self.x + self.size * 0.3), int(self.y - self.size * 0.3)),
                           int(self.size * 0.2), 0)
        pygame.draw.line(ds, (0, 0, 0), (int(self.x - self.size * 0.3), int(self.y + self.size * 0.3)),
                         (int(self.x + self.size * 0.3), int(self.y + self.size * 0.3)), int(self.size * 0.2))


def send_net_message(message):
    """simply sends a message to the server address."""
    # TODO: refactor to use a list of events encoded using masgpack?
    clientSocket.sendto(message.encode('utf-8'), server_addr)


def get_net_message():
    """If a datagram is available, get it and return it, otherwise return None."""
    try:
        message, address = clientSocket.recvfrom(1024)
    except:
        return None, None

    return message, address


def process_net_message(message, address):
    """process incoming messages from server."""
    global game_state_dict
    global player_list

    # deserialize the game state dictionary
    game_state_dict = msgpack.loads(message, raw=False)

    # pull the list of player dictionaries out of the game state dictionary
    player_dict_list = game_state_dict["player_dict_list"]

    # clear the player list
    player_list = []
    # iterate through the player dictionary list and create the local list of player instances from this list.
    for player_dict in player_dict_list:
        new_player = PlayerClient()
        new_player.load_dict(player_dict)
        player_list.append(new_player)

    # Note: other things could be pulled from the game-state dictionary too.


# Initialization
pygame.init()
ds = pygame.display.set_mode((640, 480))
pygame.display.set_caption("client")

clientSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
clientSocket.setblocking(0)  # set to non-blocking
server_addr = ("127.0.0.1", 12000)  # address and port of server

# send a join message
player_name = "Thomas"
send_net_message("<JOIN:" + player_name + ">")

game_state_dict = {}
player_list = []

currentTime = time.time()
prcentPacketsToDrop = 10

game_running = True
# game loop
while game_running:
    # getting events / sending events
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            game_running = False
        if event.type == pygame.KEYDOWN:
            if event.key == pygame.K_ESCAPE:
                game_running = False
            else:
                if random.randint(0, prcentPacketsToDrop) == 1:         # 10% drop rate
                    while random.randint(0, prcentPacketsToDrop) != 1:  # 10% drop rate
                        None
                        #send_net_message("<KD:" + str(event.key) + ">")
                    send_net_message("<KD:" + str(event.key) + ">")
        if event.type == pygame.KEYUP:
            while random.randint(0, prcentPacketsToDrop) != 1:             # 10% drop rate
                None
                #send_net_message("<KU:" + str(event.key) + ">")
            send_net_message("<KU:" + str(event.key) + ">")

    if (time.time() - currentTime) > 0.5:
        while random.randint(0, prcentPacketsToDrop) != 1:  # 10% drop rate
            None
            #send_net_message("<keepAlive:" + str(2) + ">")             #Send Keep alive
        send_net_message("<keepAlive:" + str(2) + ">")                  # Send Keep alive
        currentTime = time.time()                                       #reset time

    # process all messages
    while True:
        message, address = get_net_message()
        if message == None:
            break
        process_net_message(message, address)

    # clear screen
    pygame.draw.rect(ds, (0, 0, 0), (0, 0, 640, 480), 0)

    # render the game state
    for player in player_list:
        player.render()

    # render the game state
    pygame.display.update()

send_net_message("<QUIT>")

pygame.display.quit()