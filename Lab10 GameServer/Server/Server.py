# server2 - Improved version (from the code we wrote in class)

# Improvement: This version sends the game state as a dictionary
#              that includes the list of player dictionaries.
#              The dictionary is serialized using msgpack and can
#              easily have other elements added.
#
# Note: UDP is used and there is no reliability mechanism coded.

import socket
import pygame
import random
import msgpack
import time

global currentTime               #start off with current time

class Player(object):
    # This object stores the player info within the server
    def __init__(self, ip_addr, port, name="unknown", init_x=0.0, init_y=0.0):
        self.ip_addr = ip_addr
        self.port = port
        self.x = init_x
        self.y = init_y
        self.name = name
        self.size = 15
        self.speed = 1.0
        self.color = (random.randint(100, 255), random.randint(100, 255), random.randint(100, 255))
        self.alive = 0.0
        self.keys_down = []

    def dump_dict(self):
        '''dump_dict() - returns a dictionary of the player object's attributes.'''
        temp_dict = {}
        temp_dict["x"] = self.x
        temp_dict["y"] = self.y
        temp_dict["name"] = self.name
        temp_dict["size"] = self.size
        temp_dict["color"] = self.color
        temp_dict["alive"] = self.alive
        return temp_dict

    def load_dict(self, temp_dict):
        '''load_dict() - Sets the player object's attributes from the dictionary provided.'''
        self.x = temp_dict["x"]
        self.y = temp_dict["y"]
        self.name = temp_dict["name"]
        self.size = temp_dict["size"]
        self.color = temp_dict["color"]
        self.alive = temp_dict["alive"]

    def move(self):
        if 'a' in self.keys_down:
            self.x -= self.speed
        if 'd' in self.keys_down:
            self.x += self.speed
        if 'w' in self.keys_down:
            self.y -= self.speed
        if 's' in self.keys_down:
            self.y += self.speed
        if self.x < 0:
            self.x = 0
        if self.x > 639:
            self.x = 639
        if self.y < 0:
            self.y = 0
        if self.y > 479:
            self.y = 479

    def render(self):
        pygame.draw.circle(ds, self.color, (int(self.x), int(self.y)), int(self.size), 0)
        pygame.draw.circle(ds, (0, 0, 0), (int(self.x - self.size * 0.3), int(self.y - self.size * 0.3)),
                           int(self.size * 0.2), 0)
        pygame.draw.circle(ds, (0, 0, 0), (int(self.x + self.size * 0.3), int(self.y - self.size * 0.3)),
                           int(self.size * 0.2), 0)
        pygame.draw.line(ds, (0, 0, 0), (int(self.x - self.size * 0.3), int(self.y + self.size * 0.3)),
                         (int(self.x + self.size * 0.3), int(self.y + self.size * 0.3)), int(self.size * 0.2))


def get_net_message():
    """If a datagram is available, get it and return it, otherwise return None."""
    # TODO: refactor to use a list of events encoded using masgpack?
    try:
        message, address = serverSocket.recvfrom(1024)
    except:
        return None, None
    message = message.decode('utf-8')
    return message, address


def send_net_message_client(message, client_addr):
    """simply sends a message to the client address specified."""
    serverSocket.sendto(message, client_addr)


def process_net_message(message, address):
    """process incoming messages from clients."""
    if message[0] == '<' and message[-1] == '>':
        message = message[1:-1]
        if ":" in message:
            command, data = message.split(":")
        else:
            command = message
            data = None

        if command == "JOIN":
            print("added player to player list:", data, address)
            ip_address, port = address
            active_player_dict[str(address)] = Player(ip_address, port, data, random.randint(0, 639),
                                                      random.randint(0, 479))
        elif command == "QUIT":
            print("player removed from player list:", address)
            del active_player_dict[str(address)]
        elif command == "KD":
            data = chr(int(data))
            if data not in active_player_dict[str(address)].keys_down:
                active_player_dict[str(address)].keys_down.append(data)
        elif command == "KU":
            data = chr(int(data))
            if data in active_player_dict[str(address)].keys_down:
                active_player_dict[str(address)].keys_down.remove(data)
        elif command == "keepAlive":
            data = int(data)
            if active_player_dict[str(address)].alive > 0: #time for player to be alive is not zero
                active_player_dict[str(address)].alive = data
                currentTime = time.time()
    else:
        print("invalid message.")


# Initialization
pygame.init()

# load the window
ds = pygame.display.set_mode((640, 480))
pygame.display.set_caption("server")

# set up server socket and bind
serverSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
serverSocket.setblocking(0)                                         # set to non-blocking
server_addr = ("", 12000)                                           # address and port of server
serverSocket.bind(server_addr)

# player_dict is used to store active clients under a key that is based upon their IP and port.
active_player_dict = {}

game_running = True

print("Server is running.")
# game loop
while game_running:
    # getting events / sending events
    currentTime = time.time()
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            game_running = False
        if event.type == pygame.KEYDOWN:
            if event.key == pygame.K_ESCAPE:
                game_running = False

    while True:
        message, address = get_net_message()
        if message == None:
            break
        process_net_message(message, address)

    for player_id in active_player_dict.keys():                 # update game state
        active_player_dict[player_id].move()

    for player_id in active_player_dict.keys():                 #adjust time alive
        if active_player_dict[player_id].alive > 0:
            active_player_dict[player_id].alive -= (time.time() - currentTime)


                                                                # create list of player dictionaries
    player_dict_list = []
    for player_id in active_player_dict.keys():
        player_dict_list.append(active_player_dict[player_id].dump_dict())

    game_state_dict = {}                                        # create game state dictionary
    game_state_dict["player_dict_list"] = player_dict_list      # put player list into the dictionary
    # Note: other things could be added to the game-state dictionary too.
    game_state_str = msgpack.dumps(game_state_dict)             # create serialized game state


    for player in active_player_dict.values():                  # send the game state to all clients
        send_net_message_client(game_state_str,
                                (player.ip_addr, player.port))


    pygame.draw.rect(ds, (0, 0, 0), (0, 0, 640, 480), 0)        # clear screen


    for player_id in active_player_dict.keys():                 # render the game state
        active_player_dict[player_id].render()

    pygame.display.update()

print("Server is shutting down.")
pygame.display.quit()