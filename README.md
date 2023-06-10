# TicTacToe

#Bogdan Budura, Cassidy Kevorkian-Mielly and Sebastian Simon worked together towards the completion of this project

This is a Tic-Tac-Toe game for multiplayer hosting  which can host a total of 5 games (two player per game) where a game is created on a first come first served basis. It is hosted on the port of the computer running the server executable.

Features:
- The server can host up to a fixed number of games, with 2 clients per game.

- Each game is handled by a separate thread.

- A flag is used to indicate whether one of the players has made a move.

-simultaneous sending from different threads is prevented with mutex lock.

-If the client does an incorrect move (out of bounds, incorrect string format, move on an already played position) he is informed and can try again.


Running the game:


First, in both cases of server or client fetch the executables by running the command:
make

On terminal in the appropriate directory.


Then, run the server on your in a socket on a <port_number> (larger than 5000) by running the command:

./server <port_number>

Then, on a different computer (or not) the clients can connect by entering in the terminal:

 ./client <IP> <port_number>

With the same <port_number> as above and the <IP> of the server (IP on which the computer running the server is attached to).

Then, the clients can each play by entering their moves in a:

<row> <column>

Where <row> and <column> are integers 0 and 2. The first player of a game received the symbol X and second player O.

Code details:


Server Code (server.c):

The server code initialises the necessary variables and sets up the network socket. It listens for incoming messages from clients and processes them accordingly. 

-The main function initialises the game threads and continuously receives messages from clients.

- The "check_game_validity" function checks the validity of a game move and updates the game state. It also checks if there is a winner or a draw.

- The "play_game" function is executed in a separate thread for each game. It knows when a move was done by a player by checking the flag associated with the game  found in the shared variable: game_update. It then accordingly sends a message to the client concerned to both clients in case of draw or win. It has a mutex lock on the send to make sure two different threads are not simultaneously sending a message as the socket is a shared resource.

- The "find_address" function finds the client address..

- Other helper functions, such as "zero_game_boards", "init_game_update", "init_game_threads", "send_welcome", "debug_message", and "printByteByByte", assist in various tasks within the server.

Client Code (client.c):

The client constantly receives information from the server, processes it and acts accordingly. The main functions are the following:

-tic_tac_toe contains the main logic of the program - constantly receives messages from the server and looks at the type of message, assigning the task to a helper function for each type of message in a switch

-get_move performs the read from the standard input of the client for the move, in the case in which the move is not valid it prompts the client again. It sends the server the ultimate legal move.

-print_board responds to an FYI message by printing to the standard output the state of the board so the user will have all the information for his/her move

-printBytebyByte is a debugging function that prints BytebyByte if DEBUG==1

Please refer to the code files for more details on the implementation.
