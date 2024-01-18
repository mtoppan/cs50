# CS50 BlackJack
## Team 6
Julia Gottschalk: juliagott, 
Helen Liu: hxliu02, 
Macy Toppan: mtoppan, 
Alexander Ye: alex-ye-7, 

### Cards, Network, Dealer and Player

For this project we created four modules. The cards and network modules are used by the player and dealer modules. For information on any of their `usage, implementation, assumptions, files, compilation, or testing` check out their respective README.md files.

The overall process was to first define these four modules and consider how they would interact with each other. This allowed us to create functions in the cards and network modules that the player and dealer modules would be able to export. Makefiles were then created for each module to make compiling easier. From there, we implemented the details of each function in a .c file. The next step was to test these different modules. Each module was tested slightly differently and the details can be found in the TESTING.md file. Once the functionality was confirmed, a README.md file to explain the modules and process in more detail.