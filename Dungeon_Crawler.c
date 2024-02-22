#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <conio.h>


#define MAX_INVENTORY_SIZE 10
#define MAX_KEY_INVENTORY_SIZE 4

// Structs
struct Item {
    char name[50];
    int health;
    int damage;
};

struct Adventurer {
    char name[50];
    int health;
    struct Item inventory[MAX_INVENTORY_SIZE];
    struct Item keyInventory[MAX_KEY_INVENTORY_SIZE];
};

typedef struct RoomSection {
    struct Item items[3];
} RoomSection;

typedef struct Room {
    char name[50];
    RoomSection start;
    RoomSection middle;
    RoomSection end;
    struct Room* next;
} Room;

typedef struct Monster {
    char name[50];
    int health;
} Monster;

// Global Variables
int keysCollected = 0;
int inventoryCount = 0;

// Function prototypes
void clearScreen();
void displayMenu();
void displayCredits();
Room* createRoom( char* name);
void assignRandomItems(struct Item* itemArray, int numItems, RoomSection* section);
int inspectRoomItems( RoomSection* section);
int listInventory( struct Adventurer* player);
void displayGameOver();
int takeItem(struct Adventurer* player, RoomSection* section, int itemIndex);
void displayPlayerStatus( struct Adventurer* player, int numNodes);
void useItem(struct Adventurer* player, int itemIndex, Monster* enemy);
int startBattle(struct Adventurer* player, Monster* monster);
void displayGameScreen( Room* currentRoom,  RoomSection* currentSection,  struct Adventurer* player, int numNodes);
void processPlayerAction(struct Adventurer* player, Room** currentRoom, RoomSection** currentSection, int* numNodes);
void newGame();

// Room names
 char* roomNames[] = {
    "Enchanted Forest",
    "Ancient Ruins",
    "Dark Cavern",
    "Witch's Lair",
    "Haunted Mansion",
    "Mystic Temple",
    "Lost Crypt",
    "Eerie Swamp",
    "Shadowy Castle",
    "Goblin Hideout",
    "Fairy Glade",
    "Dragon's Den",
    "Elven Kingdom",
    "Grim Tomb",
    "Labyrinth of Illusions",
    "Vampire's Castle",
    "Werewolf Woods",
    "Ogre's Mountain",
    "Wizard's Tower",
    "Cursed Dungeon"
};
 int numRoomNames = sizeof(roomNames) / sizeof(roomNames[0]);

 //item names
struct Item itemArray[] = {
    {"Health Potion", 20, 0},
    {"Gold", 0, 0},
    {"Key", 0, 0},
    {"Shield", 10, 0},
    {"Sword", 0, 20},
    {"Amulet of Protection", 15, 0},
    {"Mystic Elixir", 30, 0},
    {"Enchanted Dagger", 0, 25},
    {"Scroll of Fireball", 0, 30},
    {"Ring of Healing", 25, 0},
    {"Wand of Lightning", 0, 35},
    {"Cloak of Shadows", 10, 10},
    {"Phoenix Feather", 10, 15},
    {"Bottled Starlight", 40, 0},
    {"Scepter of Power", 0, 40},
    {"Cursed Amulet", -20, -20}
};
 int numItems = sizeof(itemArray) / sizeof(itemArray[0]);
// monster names
Monster monsterArray[] = {
    {"Goblin", 30},
    {"Orc", 50},
    {"Dragon", 100},
    {"Witch", 40},
    {"Banshee", 45},
    {"Minotaur", 60},
    {"Cyclops", 70},
    {"Succubus", 50},
    {"Chimera", 85},
    {"Griffon", 60},
    {"Hydra", 90},

};
 int numMonsters = sizeof(monsterArray) / sizeof(monsterArray[0]);


// Clear console
void clearScreen() {
    system("cls");
}

void displayMenu() {
    clearScreen();
    printf("Dungeon Crawler\n");
    printf("1. New Game\n");
    printf("2. Credits\n");
    printf("3. Quit\n");
}

void displayCredits() {
    clearScreen();
    printf("Game created by Hrishikesh U\n");
    getch();
}

// Create a new room with a given name
Room* createRoom( char* name) {
    Room* newRoom = (Room*)malloc(sizeof(Room));
    strcpy(newRoom->name, name);

    for (int i = 0; i < 3; ++i) {
        struct Item noItem;
        strcpy(noItem.name, "n");
        newRoom->start.items[i] = noItem;
        newRoom->middle.items[i] = noItem;
        newRoom->end.items[i] = noItem;
    }

    newRoom->next = NULL;
    return newRoom;
}
// Assign random items to a room section
void assignRandomItems(struct Item* itemArray, int numItems, RoomSection* section) {
    if (numItems > 0) {
        int itemCount = rand() % 3 + 1;
        for (int i = 0; i < itemCount; ++i) {
            int randomIndex = rand() % numItems;
            section->items[i] = itemArray[randomIndex];
        }
    }
}
//inspect room
int inspectRoomItems( RoomSection* section) {
    printf("Section items:\n");

    int itemCount = 0;
    int itemNumber = 1;

    for (int i = 0; i < 3; ++i) {
        if (strlen(section->items[i].name) > 1) {
            printf("%d. %s\n", itemNumber, section->items[i].name);
            itemNumber++;
            itemCount++;
        }
    }

    if (itemCount > 0) {
        return 0;
    } else {
        printf("No items in this section.\n");
        return -1;
    }
}
//display player inventory
int listInventory( struct Adventurer* player) {
    if (inventoryCount != 0) {
        printf("Player Inventory:\n");
        for (int i = 0; i < MAX_INVENTORY_SIZE; ++i) {
            if (strlen(player->inventory[i].name) > 1) {
                printf("- %s\n", player->inventory[i].name);
            }
        }
        return 1;
    } else {
        printf("Player Inventory:\nEMPTY.\n");
        return 0;
    }
}
// display game over
void displayGameOver() {
    clearScreen();
    printf("Game Over! You were defeated in battle.\n");
    printf("\n\n\nPRESS ENTER TO CONTINUE\n");
    getch();
}
//Function to take item from room
int takeItem(struct Adventurer* player, RoomSection* section, int itemIndex) {
    if (itemIndex >= 0 && itemIndex < 3) {
        if (strlen(section->items[itemIndex].name) > 1) {
            if (strcmp(section->items[itemIndex].name, "Key") == 0) {
                for (int i = 0; i < MAX_KEY_INVENTORY_SIZE; ++i) {
                    if (strlen(player->keyInventory[i].name) == 0) {
                        strcpy(player->keyInventory[i].name, "Key");
                        for (int j = itemIndex; j < 3 - 1; ++j) {
                            section->items[j] = section->items[j + 1];
                        }
                        strcpy(section->items[2].name, "n");
                        printf("You took the key!\n");
                        keysCollected++;
                        break;
                    }
                }
            } else {
                if (inventoryCount < MAX_INVENTORY_SIZE) {
                    for (int i = 0; i < MAX_INVENTORY_SIZE; ++i) {
                        if (strlen(player->inventory[i].name) == 0) {
                            if (strlen(section->items[itemIndex].name) > 1) {
                                player->inventory[i] = section->items[itemIndex];
                                for (int j = itemIndex; j < 3 - 1; ++j) {
                                    section->items[j] = section->items[j + 1];
                                }
                                strcpy(section->items[2].name, "n");
                                printf("You took the %s!\n", player->inventory[i].name);
                                inventoryCount++;
                                break;
                            }
                        }
                    }
                } else {
                    printf("Your inventory is full. Cannot add more items.\n");
                }
            }
        } else {
            printf("No item at index %d to take.\n", itemIndex + 1);
        }
    } else {
        printf("Invalid item index.\n");
    }

    return 0;
}
//Display Player status
void displayPlayerStatus( struct Adventurer* player, int numNodes) {
    printf("|--------------------------------------------------------------------------------------------|\n");
    printf("|  %s\t Health: %d \t  Key Collected: %d \t  No of Dungeons: %d\t Inventory: %d/%d \t|\n",
           player->name, player->health, keysCollected, numNodes, inventoryCount, MAX_INVENTORY_SIZE);
    printf("|____________________________________________________________________________________________|\n");
}
// Use an item from the player's inventory
void useItem(struct Adventurer* player, int itemIndex, Monster* enemy) {
    if (itemIndex >= 0 && itemIndex < MAX_INVENTORY_SIZE) {
        struct Item* usedItem = &player->inventory[itemIndex];

        if (strlen(usedItem->name) > 0) {
            if (enemy != NULL) {
                if (usedItem->health > 0 && usedItem->damage == 0) {
                    if (player->health < 100) {
                        player->health += usedItem->health;
                        if (player->health > 100) {
                            player->health = 100;
                        }
                        printf("You used a %s and restored some health.\n", usedItem->name);
                        inventoryCount--;
                        for (int i = itemIndex; i < MAX_INVENTORY_SIZE - 1; ++i) {
                            player->inventory[i] = player->inventory[i + 1];
                        }
                        strcpy(player->inventory[MAX_INVENTORY_SIZE - 1].name, "n");
                        player->inventory[MAX_INVENTORY_SIZE - 1].health = 0;
                        player->inventory[MAX_INVENTORY_SIZE - 1].damage = 0;

                    } else {
                        printf("Your health is already full. You can't use a %s.\n", usedItem->name);
                    }
                } else if (usedItem->damage > 0) {
                    if (enemy->health > 0) {
                        int damageDealt = usedItem->damage;
                        enemy->health -= damageDealt;
                        printf("You used a %s and dealt %d damage to the %s.\n", usedItem->name, damageDealt, enemy->name);
                        inventoryCount--;
                        for (int i = itemIndex; i < MAX_INVENTORY_SIZE - 1; ++i) {
                            strcpy(player->inventory[i].name, player->inventory[i + 1].name);
                            player->inventory[i].health = player->inventory[i + 1].health;
                            player->inventory[i].damage = player->inventory[i + 1].damage;
                        }
                        strcpy(player->inventory[MAX_INVENTORY_SIZE - 1].name, "n");
                        player->inventory[MAX_INVENTORY_SIZE - 1].health = 0;
                        player->inventory[MAX_INVENTORY_SIZE - 1].damage = 0;

                    } else {
                        printf("You can't use this item for damage.\n");
                    }
                }
            } else {
                if (usedItem->health > 0) {
                    if (player->health < 100) {
                        player->health += usedItem->health;
                        if (player->health > 100) {
                            player->health = 100;
                        }
                        printf("You used a %s and restored some health.\n", usedItem->name);
                        for (int i = itemIndex; i < MAX_INVENTORY_SIZE - 1; ++i) {
                            strcpy(player->inventory[i].name, player->inventory[i + 1].name);
                            player->inventory[i].health = player->inventory[i + 1].health;
                            player->inventory[i].damage = player->inventory[i + 1].damage;
                        }
                        strcpy(player->inventory[MAX_INVENTORY_SIZE - 1].name, "n");
                        player->inventory[MAX_INVENTORY_SIZE - 1].health = 0;
                        player->inventory[MAX_INVENTORY_SIZE - 1].damage = 0;
                    } else {
                        printf("Your health is already full. You can't use a %s.\n", usedItem->name);
                    }
                } else {
                    printf("You can't use this item outside of battle.\n");
                }
            }
        } else {
            printf("No item at index %d to use.\n", itemIndex + 1);
        }
    } else {
        printf("Invalid item index.\n");
    }
}
// Start a battle between the player and a monster
int startBattle(struct Adventurer* player, Monster* monster) {
    printf("A wild %s has appeared!\n", monster->name);

    while (player->health > 0 && monster->health > 0) {
        printf("Your health: %d   %s's health: %d\n", player->health, monster->name, monster->health);
        printf("1. Attack     2. Escape\n");
        printf("3. Inventory\n");
        int choice;
        scanf("%d", &choice);
        getchar();

        if (choice < 1 || choice > 3) {
            printf("Invalid choice. Please enter a valid option (1, 2, or 3).\n");
        } else {
            if (choice == 1) {
                int playerDamage = rand() % 20 + 10;
                int monsterDamage = rand() % 15 + 5;

                monster->health -= playerDamage;
                printf("You dealt %d damage to %s!\n", playerDamage, monster->name);

                if (monster->health <= 0) {
                    return 1;
                }

                player->health -= monsterDamage;
                printf("%s dealt %d damage to you!\n", monster->name, monsterDamage);

                if (player->health <= 0) {
                    return -1;
                }
            } else if (choice == 2) {
                int escapeChance = rand() % 2;
                if (escapeChance == 0) {
                    printf("You successfully escaped from the battle.\n");
                    return 0;
                } else {
                    printf("Escape attempt failed! You must fight!\n");
                    int monsterDamage = rand() % 15 + 5;
                    player->health -= monsterDamage;
                    printf("%s dealt %d damage to you!\n", monster->name, monsterDamage);

                    if (player->health <= 0) {
                        return -1;
                    }
                }
            } else if (choice == 3) {
                int z = listInventory(player);
                if (z == 1) {
                    printf("Enter the number of the item to use: ");
                    int itemNumber;
                    scanf("%d", &itemNumber);
                    getchar();
                    useItem(player, itemNumber - 1, monster);
                    int monsterDamage = rand() % 15 + 5;
                    player->health -= monsterDamage;
                    printf("%s dealt %d damage to you!\n", monster->name, monsterDamage);
                } else {
                    printf("Two Flys fled away from your bag.\n");
                }
            } else {
                printf("Invalid choice.\n");
            }
        }
    }
    return 0;
}

// Display the game screen
void displayGameScreen( Room* currentRoom,  RoomSection* currentSection,  struct Adventurer* player, int numNodes) {
    displayPlayerStatus(player, numNodes);
    printf("|--------------------------------------------------------------------------------------------|\n");
    printf("You are in %s - %s section                                                       \n",
           currentRoom->name,
           currentSection == &currentRoom->start ? "Start" : currentSection == &currentRoom->middle ? "Middle" : "End");
    printf("|--------------------------------------------------------------------------------------------|\n");
    printf("\n");
    printf("|Press 'a' to advance, 'x' to inspect items, 'e' to take items, 'i' to list inventory,       |  \n|'u' to use item, 'q' to quit:                                                                | ");
    printf("\n|--------------------------------------------------------------------------------------------|\n");
}

// Process the player's action in the game
void processPlayerAction(struct Adventurer* player, Room** currentRoom, RoomSection** currentSection, int* numNodes) {
    char action;
    scanf(" %c", &action);
    printf("\n");
    switch (action) {
    case 'a':
        if (*currentSection == &(*currentRoom)->start) {
            *currentSection = &(*currentRoom)->middle;
        } else if (*currentSection == &(*currentRoom)->middle) {
            *currentSection = &(*currentRoom)->end;
        } else if (*currentSection == &(*currentRoom)->end) {
            int randomIndex = rand() % numRoomNames;
             char* nextRoomName = roomNames[randomIndex];

            Room* newRoom = createRoom(nextRoomName);
            assignRandomItems(itemArray, numItems, &newRoom->start);
            assignRandomItems(itemArray, numItems, &newRoom->middle);
            assignRandomItems(itemArray, numItems, &newRoom->end);
            (*currentRoom)->next = newRoom;
            *currentRoom = newRoom;

            *currentSection = &(*currentRoom)->start;
            (*numNodes)++;
        }
        printf("YOU ADVANCED TO THE NEXT ROOM\n");

        if (action == 'a' && currentSection != NULL) {
            int monsterChance = rand() % 4; // 25% chance of encountering a monster
            if (monsterChance == 0) {
                int randomMonsterIndex = rand() % numMonsters;
                Monster* encounteredMonster = &monsterArray[randomMonsterIndex];
                int battleResult = startBattle(player, encounteredMonster);
                if (battleResult == 1) {
                    printf("You defeated the %s!\n", encounteredMonster->name);
                } else if (battleResult == -1) {
                    printf("You were defeated by %s.\n", encounteredMonster->name);
                    displayGameOver();
                }
            }
        }
        break;
    case 'x':
        inspectRoomItems(*currentSection);
        break;
    case 'e': {
        int z = inspectRoomItems(*currentSection);
        if (z == -1) {
            printf("No items in this section.\n");
        } else {
            printf("Enter the number of the item to take (1, 2, 3) ");
            int itemNumber;
            scanf("%d", &itemNumber);
            getchar();
            if (itemNumber >= 1 && itemNumber <= 3) {
                int selectedIndex = itemNumber - 1;
                takeItem(player, *currentSection, selectedIndex);
            } else if (itemNumber == -1) {
                for (int i = 0; i < 3; ++i) {
                    takeItem(player, *currentSection, i);
                }
            } else {
                printf("Invalid item number.\n");
            }
        }
        break;
    }
    case 'i':
        listInventory(player);
        break;
    case 'u': {
        listInventory(player);
        printf("Enter the number of the item to use: ");
        int itemNumber;
        scanf("%d", &itemNumber);
        getchar();
        useItem(player, itemNumber - 1, NULL);
        break;
    }
    case 'q':

        printf("\nEXITING........");
        exit(0); // Quit the program
        break;
    default:
        printf("Invalid action.\n");
        break;
    }
}

// Start a new game
void newGame() {
    // Initialize the game variables
    clearScreen();
    struct Adventurer player = {"Player", 100};
    Room* currentRoom = createRoom("Mystic Woods");
    int numNodes = 1;
    RoomSection* currentSection = &currentRoom->start;

    do {
        displayGameScreen(currentRoom, currentSection, &player, numNodes);
        processPlayerAction(&player, &currentRoom, &currentSection, &numNodes);
        sleep(1);
        clearScreen();
    } while (keysCollected < 4 && player.health > 0);

    if (keysCollected == 4) {
        printf("Congratulations! You collected all the keys and won the game!\n");
        getch();
    } else if (player.health <= 0) {
        printf("Game Over! You were defeated in battle.\n");
        displayGameOver();
    }

    // Free memory for the linked list
    currentRoom = currentRoom->next; // Skip the first room
    while (currentRoom != NULL) {
        Room* temp = currentRoom;
        currentRoom = currentRoom->next;
        free(temp);
    }
}

int main() {
    int choice;
    srand(time(NULL));

    do {
        displayMenu();
        scanf("%d", &choice);
        getchar(); // Clear newline character

        switch (choice) {
        case 1:
            newGame();
            break;
        case 2:
            displayCredits();
            break;
        case 3:
            printf("Goodbye!\n");
            break;
        default:
            printf("Invalid choice.\n");
            break;
        }
    } while (choice != 3);

    return 0;
}

