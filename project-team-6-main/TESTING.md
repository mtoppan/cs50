# CS50 BlackJack
## Testing Specification
 
### Cards
 
The `cardstest.c` is a unit test driver for the cards module. It checks each function making sure that when properly used, they perform as should, and when improperly used, they also perform as should. Additionally, by using the cards functions in the dealer and player modules, the functionality and edgecases of the cards module functions were tested.
 
To test, simply `make test`.
See `testing.out` for details of testing and an example test run.
 
To test with valgrind, `make valgrind`.
 
### Network
The network module contains a `server.c` and `client.c` to create and test out a client-server connection. It is a simple version used to confirm that the client and server could be set up and closed and could pass messages to each other.
 
To test, compile with `make` in the network folder and open two terminals. First, run `./server` in one terminal, and then run `./client` in the other.
 
To test with valgrind, run `valgrind --leak-check=full --show-leak-kinds=all ./server` and connect a client to it in the same manner as above. You can also run valgrind with these flags on `./client` as it connects to a server.
 
### Player
 
As `player` functions largely in tandem with `dealer`, the individual functions may be tested in checks within a non-networked setting by disabling the `runGame` function and individually testing subsections of the code. However, to test it as a whole it is best seen in practice alongside the dealer. 
 
To test, setup the `dealer` in a separate terminal with the following command:
 
```bash
./dealer-given 1 2 8086
```
...then do the following to locate your own IP address:
```bash
wget -O - -q https://checkip.amazonaws.com
```
 
Then run the player in the original terminal with the following command, replacing IP with the result of the above:
 
```bash
./player team_6 <IP> 6
```
 
To test with valgrind, follow the above but accompany the command with `myvalgrind` as follows:
 
```bash
myvalgrind ./player team_6 <IP> 6
```
 
### Dealer
 
The `dealertest.c` is a simplified player/client that communicates with the `dealer.c` via network. It connects to the dealer server by sending a JOIN message, receives messages from the dealer about the two hands, and makes a hardcoded decision to HIT or STAND. It stops playing when it receives a QUIT message from the dealer.
 
To test, run `./dealer numGames portNumber` first. The dealer will open the server for connection. 
Then, run `./dealertest` from another command line window to have the client join the game.
 
To test with valgrind, run `myvalgrind ./dealer numGames portNumber` & repeat steps above.