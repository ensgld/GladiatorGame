My goal in this game was to create various areas where the player could perform different actions, and to use pointers and data structures to facilitate transitions between rooms while introducing scenario-based changes in certain attributes. To achieve this, I defined a Player entity along with its capabilities and characteristics, including the locations they can travel to and the activities they can perform upon arrival.
For example, I began the game’s narrative by placing the character in a “Forest Area,” from which they can freely move to other locations: the Arena, Home, Market, and Training Room. Each area is designed to offer unique experiences and influences on the player. In the Training Room, the player can practice and improve their skills; in the Market, they can purchase new items with gold; at Home, they can rest and recover; and in the Arena, they can participate in tournaments. Winning a tournament grants the player certain rewards, contributing to a simple underlying storyline.
Regarding the Market, I implemented a system that allows players to buy specific equipment using gold. They can then access their inventory to use these items whenever they wish. Of course, the player can choose not to use an item even after purchasing it. Each item provides different benefits, such as boosting strength or increasing health, thus adding variety and strategic depth to the game.
Finally, I introduced a Creature structure to create diverse types of enemies. In the Training Room, players can battle these creatures to prepare for tournaments. They can either choose specific creatures or opt for a randomly generated one, thereby enhancing the variety of encounters. Each creature type comes with distinct stats, encouraging the player to develop strategies and improve their skills.
In summary, the game is composed of different areas, activities, items, and creatures, all working together to offer a dynamic and engaging player experience.


1. Key Structures
Item: Represents items in the game with properties such as bonus health, bonus strength, damage, defense, and durability.
Player: Represents the player's attributes including health, strength, a dynamic inventory array, level, and coins.
Room: Represents rooms in the game. Each room has a name, description, connections to other rooms (north, south, east, west), and an array to hold items.
Creature: Represents enemy creatures with attributes such as name, health, and strength.

2. Game Flow
Game Initialization (main function):
Initialize Player:
initializePlayer sets the player's initial stats such as health, strength, inventory capacity, and starting coins.
Create Rooms:
The game starts in the "Forest Area" (main room).
Other rooms include:
House: Restores health.
Market: Allows buying items.
Arena: Hosts battles with progressively difficult enemies in a tournament.
Training Room: Allows the player to fight creatures for XP without dying.
Connect Rooms:
The rooms are connected using the connectRooms function:
North: Arena
South: House
West: Market
East: Training Room
Initialize Market Items:
Two initial items are added to the market:
Golden Sword and Iron Armor.
Start the Game Loop:
The gameLoop function begins, where the player interacts with the game.

Game Loop (gameLoop function):
The player can choose actions depending on the current room. The choices are dynamic based on the room.
1. Forest Area:
Move: Travel to another room (north, south, east, west).
Look: See items in the current room.
Inventory: List items, equip weapons/armor, or use consumables.
Save/Load: Save progress to a file or load an existing save.
Show Stats: Display health, level, and coins.
Exit: Quit the game.
2. Market Room:
Buy Items: Purchase items from the market using coins.
Return to Forest Area: Move back to the starting room.
Other options: Look around, manage inventory, save/load the game, or exit.
3. Arena:
Tournament: Fight a sequence of enemies (Goblin, Orc, Boss). Winning earns coins and levels up the player.
Return to Forest Area: Move back to the starting room.
4. House:
Rest: Fully restores the player's health.
Return to Forest Area.
5. Training Room:
Train: Fight creatures to gain XP without the risk of dying. Creatures can be random or chosen by the player.
Return to Forest Area.

Core Mechanisms
Combat System (fight function):
The player can choose to Attack or Defend:
Attack: Reduces the enemy's health by the player's strength.
Defend: Reduces incoming damage by half.
The enemy counterattacks if it is still alive.
If the player's health drops to 0:
In training mode, health resets to 10.
In real combat, the game ends.
Inventory Management:
Items can be:
Equipped (e.g., weapons increase strength, armor increases health).
Used (e.g., consumables that restore health).
Dropped when broken (durability reaches 0).
The inventory size increases as the player levels up.
Leveling Up:
Winning fights or completing training sessions increases the player's level.
Leveling up grants:
Increased health
Increased strength
Expanded inventory capacity
Save and Load:
The player can save the game progress to a file (saveGame) and reload it later (loadGame).

Randomized Items and Creatures:
Random Items: Generated with randomItem (e.g., Sword, Armor).
Random Creatures: Generated with randomCreateCreature (e.g., Goblin, Orc, Dragon, Boss).
Enes Geldi 21050111035



