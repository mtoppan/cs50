# CS50 final project
 
## An reinforcement learning algorithm that learns to play a game optimally through trial and error
Reinforcement learning is a type of machine learning that learns what *actions* to take in any given situation (instead of calculating a probability as some other machine learning methods do).   It involves an *agent* that observes the *state* of its environment, then chooses an action to take from a finite set of possible actions based on the state.  The environment then "*rewards*" the agent based on its choice of action.  Over many iterations, the agent learns which actions are beneficial in each state and which are detrimental.  [Sutton and Barto](https://mitpress.mit.edu/9780262039246/reinforcement-learning/) have a great book that describes reinforcement learning in detail -- highly recommended if you'd like to learn more.
 
Many people believe humans and animals learn this way.  A baby takes an action, such as kicking a wall, gets a negative reward (pain), and learns to not kick the wall.  
 
We will use the idea of reinforcement learning to create a program that learns to play the popular card game **21** (also known as blackjack) in an optimal manner.  Your program will play hundreds, maybe thousands, of hands of 21 and learn which actions work well (e.g., the program wins), and which do not (e.g., the program looses).  On the last day of class we will have a tournament where each team's program will play the game together and we will ultimately crown a CS50 champion.
 
## The game
The game of 21 is relatively simple.  Its is played with a [standard deck of 52 playing cards](https://en.wikipedia.org/wiki/Standard_52-card_deck#:~:text=A%20standard%2052%2Dcard%20deck,cards%2C%20from%20one%20to%20ten.).  Each card has a suit of either clubs, spades, diamonds or hearts and has a rank, a number (2 through 10), Jack, Queen, King or Ace. In total there are four suits, where each suit has one card with each of the 13 possible ranks, for a total of 52 (= 4 * 13) cards in the deck.  
 
 A dealer deals each player one card from the deck face up, deals themself a card face down, then dealer then deals each player a second card face up, and themself a second card also face up.  At the end of the dealing, each player has two cards face up, the dealer has one card face down and one card face up.  The value of the face down card is initially unknown to the players and the dealer.
 
### Objective
The goal of the game is for the player to get as close to 21 points **without going over**.  Points are calculated as follows:
- each numbered card (2 through 10) has the number of points on the card (suit does not matter).  For example, the Seven of Diamonds counts for 7 points
- each "face card" (Jack, Queen, King) counts for 10 points
- each Ace counts for either 1 or 11 points (whichever is best for the player, the choice of either 1 or 11 can change as the game progresses).
 
 
After the dealer first deals the cards, each player will have two cards with a total number of points between 2 (got two Aces, each worth 1 point) and 21 (10 or face card, plus Ace worth 11). 
 
### Actions
After receiving the first two cards, the player then decides on one of two possible actions: 
1. HIT -- take another card to increase their point total 
2. STAND -- take no more cards.  
 
(Note: the [real game of 21](https://bicyclecards.com/how-to-play/blackjack/) also has a SPLIT action, our simplified version will not have SPLITs.  We will also not "[count cards](https://en.wikipedia.org/wiki/MIT_Blackjack_Team)").  
The player can choose to keep taking cards (HIT) until they decide to stop (STAND) or they exceed 21 points.  If they exceed 21 points, the player looses immediately and is said to BUST.
 
Once all players have taken cards until they STAND or BUST, if any players have not busted, the dealer reveals with face-down card and then decides to HIT or STAND.  The dealer must continue to HIT if their total points are 16 or fewer.  The dealer must STAND if their point total is 17 or more.  If the dealer has an Ace, they must STAND if their total with the Ace counting as 11 is over 16 but below 22.  For example, the dealer STANDs if they have an Ace and a 7 for a total of 18.  If the dealer would BUST if the Ace counted for 11, the dealer can count the Ace as 1 and can continue to HIT. For example, imagine the dealer has a Five of Clubs and a 6 of Spades initially (11 points), so the dealer must HIT, but  gets an Ace of Diamonds. In this case if the Ace counts as 11 the dealer would have 22.  The dealer can count the Ace as 1 and continue to HIT until they exceed 16 points or BUST. If the dealer BUSTs, any remaining non-busted players WIN.  If the dealer does not BUST, any non-busted player with more points than the dealer WINS, non-busted players with fewer points than the dealer LOOSE.  If the dealer and player have the same number of points, the player is said to PUSH.
 
 
 
### Outcomes
The outcome of the game from the player's point of view is:
- WIN - the player has 21 or fewer points and player has more points than the dealer or the dealer BUSTs
- BUST - the player has more than 21 points
- LOOSE - the player BUSTS or player has fewer points than a non-busted dealer
- PUSH - neither the player nor the dealer bust, and they have the same number of points.
 
There is a summary:
 
|              | **Dealer**  ||
| :--          | :-- | :-- |
|**Player**    | **BUST** | **Not BUST** |
|BUST          | LOOSE | LOOSE |
|Not BUST      | WIN   | WIN Player > dealer|
|              |       | LOOSE Player < dealer |
|              |       | PUSH Player == dealer |
 
The process of each player taking actions to HIT or STAND, then the dealer taking similar actions until there is an outcome for each player is called a *game* or *round* of 21.  The deck of cards is shuffled before each game.
 
### Strategy
Sometimes it is beneficial for the player to STAND with a relatively small number of points, depending on the dealer's cards.  For example, suppose the player has a total of 12 points and the dealer's face up card is a 6.  Because the player's point total is much smaller than 21, it may seem obvious they should HIT.  If they do HIT, a 10 or face card would cause them to BUST but any other card would increase their points and still remain below 21.  Experienced players typically assume the dealer's face-down card is worth 10 points (since the player can only see the value of one of the dealer's two cards when they decide to HIT or STAND, they cannot be sure of the dealer's true point total).  If the face-down card is worth 10, then the dealer would have to HIT the 16, so any card greater than 5 would cause the dealer to BUST -- very likely to happen and the player's 12 would win.  Observing the dealer's card can lead a smart player to behave differently than if they simply only looked at their own cards.
 
In a real game, many players sit at a table and play, but each player is playing against the dealer, not against other players (e.g., multiple players can win).  On the last day of class we have a tournament where each team plays against my dealer program.  See [tournament below](#tournament).
 
 
## Reinforcement learning
As noted above, reinforcement involves an agent (your program) evaluating the *state* of the environment (the player's cards and one of the dealer's cards) and choosing to take an *action* (HIT or STAND) based on the state.  The agent will then receive a *reward* (WIN, BUST, LOOSE, or PUSH) based on the action chosen (see [Outcomes](#outcomes) above). 
 
### State
The state the agent observes is comprised of the cards the player has, as well as the face-up card the deal holds.  The agent does not know the true state of the dealer's hand because it does not know the dealer's face-down card.  We can think of the state as an integer tuple of (player's points, dealer's points).  
 
 
### Actions
The agent will take an action based the state it observes.  In our case the action is either to HIT (take another card) or STAND (stop taking cards).  If the player does not exceed 21 points, they can choose to keeping hitting until they either BUST or decide to STAND.  The dealer decides to HIT or STAND after all players either BUST or STAND.  
 
### Rewards
The player either WINs, LOOSEs, BUSTs, or PUSHes as [described above](#outcomes).  If player WINs, a common reward is +1 point.  If the player LOOSEs or BUSTs, a common reward is -1 point.  If the player PUSHes (player's points equal the dealer's points), a common reward is 0 points.  
 
### Choosing an action
Your program (the agent) should take the action with highest *average* reward based on the [current state](#state).  You can create a table of all possible states and keep track of the average reward received when taking each HIT or STAND action in each state.  After playing many games (perhaps many thousands) this average should approach the true value of taking each action in each state.  This table is typically called Q in the literature.  In our case, we can create a two dimensional table to hold all possible states, and add an extra dimension for each possible action for a three-dimensional table. This table stores the average reward received after taking each action in each state.  For example, you might create a three-dimensional array that looks like this:
 
```c
//Q holds the average reward for taking action given player and dealer points
float Q[max_player_points][max_dealer_points][number_actions];
```
 
To use the table, the agent observes the number of points the player currently has, as an example say it is 12, as well as the number of points the dealer has with their face-up card, say it is 6.  The agent can look up in the table the average reward with Q[12][6][HIT] and Q[12][6][STAND].  To choose the optimal action, the agent then chooses the action with the highest average reward points.
 
Although the agent might choose a smart action in one state, it might get unlucky in one game and still loose (e.g., the dealer happens to get better cards in that particular game).  Decision science researchers often say that you can make a good decision and have a bad outcome (likewise, you can make a bad decision and have a good outcome).  Due to the Law of Large Numbers, however, as the number of training games played increases and the agent makes a decision in the same state many times, the average reward observed for each state/action pair will approach the true average.  It is important, therefore that the agent tries each state/action pair many times to converge to the true average reward.  This is why reinforcement learning plays a large number of games.
 
Note: in some states there is an obvious best action (e.g., do not HIT if the player already has 21 points, taking another card cannot help, it can only hurt!)  Your program should always choose the best action in those circumstances.  In other cases, it should determine the best action after playing many games.
 
<a id="update"></a>
### Updating the average reward
Your program will need to track the average reward for every possible state and for every possible action in that state.  Here we only have two actions: HIT or STAND.  Typically we calculate an average reward by keeping a list of all prior rewards, looping over that list and summing the rewards, then dividing by the number of rewards.  If we took that approach, we would need to track the outcome of every action decision taken over thousands of games -- very memory inefficient and time consuming to compute.  An easier way to update the average is to keep a *count* of the number of times each action has been tried in a state in a second three-dimensional matrix and use the following formula:
 
```c
//Q is the average reward for taking an action in a state (state is player and dealer points)
//Q_count is number of times an action has been tried in a state 
//max_player_points: maximum player points
//max_dealer_points: maximum dealer points
//number_actions: number of actions the player can take (2 here, HIT or STAND)
//initialize to zero
float Q[max_player_points][max_dealer_points][number_actions] = {0}; 
int Q_count[max_player_points][max_dealer_points][number_actions] = {0}; 
 
//player_points: sum of points on player's cards
//dealer_points: point value of dealer's face up card
//action: 0 = HIT, 1 = STAND
//reward: +1 for WIN, -1 for LOOSE or BUST, 0 for PUSH
void update_Q(int player_points, int dealer_points, int action, int reward) { 
   Q_count[player_points][dealer_points][action] += 1; //increment count
   Q[player_points][dealer_points][action] += (1/(float)Q_count[player_points][dealer_points][action]) 
                                           * (reward - Q[player_points][dealer_points][action]) //update reward
}
```
 
Note: many reinforcement learning approaches take a more sophisticated approach to updating Q.  The approach in this section will suffice for the final project.
 
 
### Giving proper credit
A player may end up hitting several times before deciding to STAND.  Remember, each action was based on a different state (player's points, dealer's) points, so the average reward each state/action pair should be updated after the round ends and player receives a reward.  For example, consider the dealer's face up card is a 8 and a player's hand progresses as follows:
 
|Hand    			| Points    | Decision  |
|:--     			| :--       | :--       |
| Start
|Five of diamonds	|	5  		| 	        |
|Six of clubs		| 11        | HIT       |     
|Three of hearts	| 14		| HIT       |
|Seven of spades        | 21        | STAND     |
 
Assume the player WINs and thus gets a +1 reward.  Each of those states and actions (11,8,HIT), (14,8,HIT), (21,8,STAND) should be updated with the +1 reward because they all contributed to the win.  Your program will need to update Q for each state/action pair taken in a round.
 
### Training vs. playing
A classic problem arises in reinforcement learning which involves *exploration* -- trying new actions in a given state to see if those action are better than the current best action vs. *exploitation* -- choosing the best action in a given state.  As noted above, trying an action in a particular state may not lead to the expected outcome (e.g., player might make a smart move but still loose or might make a dumb decision but still win).  Your algorithm should play many games (you decide how many) in a training mode where it tries random actions in each state and calculates the average reward.  You should be able to save your Q table and read it back it to continue training later.  You should also have a play mode where your algorithm reads in the Q table and always chooses the best action according to the table built during training.  This mode will be important for the end of class tournament.
 
<a id="tournament"></a>
## Tournament
We will have a tournament on the last day of class where your programs will play many games.  I'll provide a dealer program that will accept up to six players (your programs) and will deal cards to each player.  Your program will then respond with its HIT/STAND decisions and my dealer program will decide if you WIN, LOOSE, BUST, or PUSH each game.  Because we'll have about 18 teams, the first round will be three tables of six teams.  After many games, the top two teams from each table that win the most games will advance to the final round.  The teams that advance to the final round will again play many games and an ultimate winner will crowned.  If all algorithms play optimally, the winner will be chosen by luck...
 
## Implementation
You should write both a dealer program and a player program.  We will use TCP/IP sockets to communicate between the programs, similar to [PS-6 of CS10](https://cs.dartmouth.edu/~tjp/cs10/PS-6.html), but without the multi-threading.  Also see our reading for this week on [sockets]({{site.siteurl}}/reading/sockets) for more information about communications between programs.  Additionally, I found [this site](https://www.geeksforgeeks.org/socket-programming-cc/) helpful.   
 
 
See the diagram below for how messages will be passed between programs to play the game.  Make sure you follow the message passing exactly as diagrammed or your program will not work in the tournament.  I'll provide a pre-compiled dealer program that you can use to test your player program.
 
### Dealer
I will write a dealer program for the tournament, but you should create one for training.  The dealer shall:
- run from the command line
- set up a server socket listening for clients (players) to connect (see [this server example](server.c)) 
- once a client connects (you need only handle one client, mine will handle up to six players), pass messages back and forth over socket as shown in diagram below, and
    - create a deck of 52 cards for each game
    - shuffle the deck
    - deal cards to the player by sending messages with the card suit and rank as a string (e.g., "Seven of Hearts")
    - receive HIT/STAND decisions from the client
    - calculate the game's result (WIN, LOOSE, BUST, or PUSH) and send a message to the client
    - reset and play again (you decide on how many games to play)
- send a QUIT message to the client when done
 
Your dealer program need only accommodate a single player.  My dealer for the tournament will accommodate up to six players.
 
### Player
You should create a player program that connects to the dealer program over a TCP/IP socket and passes messages to play the game according to the diagram below.  The player program shall:
- run from the command line taking the player's name, server's IP address, and PORT number as parameters 
- connect to the server using a socket (see [this client example](client.c))
- have a training mode where it plays many games with the dealer program, choosing random actions
   - must be able to write its Q table to disk and read it back
   - must be able to continue training after reading the Q table from disk
- have a play mode where it makes optimal decisions based on what it learned during training
   - reads Q table written to disk during training
   - uses table to make optimal decisions
 
Note: you can get the server's IP address by running this command from a terminal connected to the server:
```
wget -O - -q https://checkip.amazonaws.com
```
 
**IMPORTANT:** Because multiple teams may be using plank at the same time, your team should use a PORT = 8080 + your team number.  That way you will not hear messages from other teams' programs.
 
 
### Message passing
Follow this protocol to play the game:
 
![Message Passing](MessagePassing.png)
 
Notes:
- JOIN \<player name\>: player asks to join game, player name should not have spaces (use underscore for spaces e.g., team_one_is_here)
- BEGIN: to keep dealer and player in sync, if a player gets a BEGIN message, they should reset for a new game (e.g., discard any cards)
- CARD: dealer sends player a card, rank will be strings "Two" through "Ten", "Jack", "Queen", "King", or "Ace", suit will be "Diamonds", "Hearts", "Clubs" or "Spades" (e.g., "CARD Nine of Hearts")
- DEALER: dealer tells player the Rank of Suit of dealer's face-up card (e.g., "DEALER Ace of Clubs")
- DECISION: dealer asks the player to make a decision (either HIT or STAND)
- HIT or STAND: player tells the dealer their decision based on player's cards and dealer's face-up card (repeat until STAND or BUST)
- RESULT: dealer tells the player if they WIN, LOOSE, BUST, or PUSH (followed by BEGIN if playing multiple rounds)
- QUIT: dealer tells player to quit
 
Each message should be null terminated.
 
 
 
## Assignment
 
One person on the team should accept the [assignment](https://classroom.github.com/a/V6nv7-OP) and then add other team members to the repo.
 
In addition to your code written to implement the dealer and the player, provide the following documentation as discussed in class:
- REQUIREMENTS.md
- DESIGN.md
- IMPLEMENTATION.md
- TESTING.md
 
Also include a short write up on how many games you used to train your player.  Discuss why you choose to stop when you did.
 
### Submission
 
Add/commit all the code and ancillary files required to build and test your solution to a `project` branch.
 
To submit, read the [Lab submission instructions]({{site.siteurl}}/git/submit).
 
 
## Extra credit
Implement a text-based graphical user interface (GUI) using [ncurses](https://dev.to/tbhaxor/introduction-to-ncurses-part-1-1bk5).  A well-written GUI interface will earn up to 10 extra credit points.