#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define COUNT_OF(array) (sizeof(array) / sizeof(array[0]))

typedef struct {
    char* name;
    int bonusHealth;//Sağladığı ekstra sağlık
    int bonusStrength;
    char* type;// item'in türü
    int damage;
    int defense;
    int durability;//dayanıklılık
}Item;
typedef struct {
    int health;
    int strength;
    Item** inventory;//eşyaları tutacak olan dinamik array
    int inventoryCapacity;
    int inventorySize;
    int level;//Yaratik kestik�e level atlama sistemi level atlayinca da inventoryCapacity artacak o y�zden inventory arrayini dinamik yaptim
     int coin; //Default olarak her player coin'e sahip olur ve yaratık kestikçe veya savaş kazandıkça coin kazanır ve satış mağazasına gidip item alabilir
}Player;

// Forward declaration varsa ve Room pointer kullanıyorsanız:
struct Room;

// Ancak `Room` yapısını tam olarak kullanan bir fonksiyon veya struct'ta, tam tanımlama gerekli.
typedef struct Room {
    char* name;
    char* description;
    struct Room* north;
    struct Room* south;
    struct Room* east;
    struct Room* west;
    Item** items;
    int itemCapacity;
    int itemCount;
} Room;

typedef struct {
    char* name;//yaratığın  adı
    int health;//yaratığın canı
    int strength;//yaratığın gücü
} Creature;

//benim istediğim şey farklı türde yaratıklar olsun herbirinin statları farklı olsun tekdüze olmasın diye bunu istiyorum 
//ve karakterimiz level atladıkça bazılarını yenebilsin yani herkesi kesebilecek şekilde olmasın
void addItemToRoom(Room* room, Item* item);
void dropItem(Player* player, const char* itemName);
void freeItem(Item* item);
void equipItem(Player* player, const char* itemName);
void move(Player* player, Room** currentRoom, const char* direction);
void fight(Player* player, Creature* creature,int isTraining);
void look(Room* currentRoom);
void levelUp(Player* player);
void listInventory(Player* player);
void saveGame(Player* player, const char* filename);
void loadGame(Player* player, const char* filename);
Creature* createCreature(const char* creatureType);
void buyItemFromMarket(Player* player, Room* marketRoom);
Creature* randomCreateCreature();


Item* createItem(const char* name, int bonusHealth, int bonusStrength, const char* type, int damage, int defense, int durability) {
    Item* item = (Item*)malloc(sizeof(Item));
    if (!item) {
        printf("Bellek ayrımı başarısız oldu!\n");
        exit(1);
    }
    item->name = _strdup(name);
    item->bonusHealth = bonusHealth;
    item->bonusStrength = bonusStrength;
    item->type = _strdup(type);
    item->damage = damage;
    item->defense = defense;
    item->durability = durability;
    return item;
}

Creature* createCreature(const char* creatureType) {
    // 0: Goblin, 1: Orc, 2: Dragon
    Creature* creature = malloc(sizeof(Creature));
    if (!creature) {
        printf("Bellek ayırımı başarısız oldu.\n");
    }

    if (strcmp(creatureType,"goblin")==0){
        creature->name = _strdup(creatureType);
        creature->health = 20;
        creature->strength = 5;
       }
    
    else if (strcmp(creatureType, "orc")==0) {
        creature->name = _strdup(creatureType);
        creature->health = 45;
        creature->strength = 15;
    }
    else if (strcmp(creatureType, "dragon")==0) {
        creature->name = _strdup(creatureType);
        creature->health = 75;
        creature->strength = 30;
    }
    else if (strcmp(creatureType, "minnion")==0) {
        creature->name = _strdup(creatureType);
        creature->health = 2;
        creature->strength = 1;
    }
    else if (strcmp(creatureType, "boss") == 0) {
        creature->name = _strdup(creatureType);
        creature->health = 175;
        creature->strength = 85;
    }
    return creature;
}
Creature* randomCreateCreature() {
    // Oluşturmak istediğiniz yaratık türleri:
    const char* creatureTypes[] = { "goblin", "orc", "dragon", "minnion", "boss" };
    int numTypes = 5;

    // Rastgele bir indeks seç
    int randomIndex = rand() % numTypes;

    // Seçilen tipe göre yaratık oluştur
    return createCreature(creatureTypes[randomIndex]);
}



Item* randomItem() {
    int type = rand() % 2; // 0: Weapon, 1: Armor
    if (type == 0) return createItem("Kılıç", 0, 10, "Weapon", 15, 0, 5);
    return createItem("Zırh", 20, 0, "Armor", 0, 10, 8);
}


void initializePlayer(Player* player) {
    player->health = 100; // Başlangıç sağlığı
    player->strength = 10; // Başlangıç saldırı gücü
    player->inventoryCapacity = 3;
    player->inventorySize = 0;
    player->inventory = (Item**)malloc(player->inventoryCapacity * sizeof(Item*));
    if (!player->inventory) {
        printf("Bellek ayrımı başarısız oldu!\n");
        exit(1);
    }
    player->level = 1;
    player->coin = 100;

    for (int i = 0; i < 3; i++) {
        player->inventory[i] = NULL;
    }
}

void addItem(Player* player, Item* item) {
    if (player->inventorySize < player->inventoryCapacity) {
        player->inventory[player->inventorySize++] = item;
        printf("%s envantere eklendi.\n", item->name);
    }
    else {
        printf("Envanter dolu! Yeni item eklenemiyor.\n");
    }
}
void equipItem(Player* player, const char* itemName) {
    for (int i = 0; i < player->inventorySize; i++) {
        if (strcmp(player->inventory[i]->name, itemName) == 0) {
            if (player->inventory[i]->durability <= 0) {
                printf("%s kullanılamaz, çünkü dayanıklılığı sıfır!\n", itemName);
                return;
            }

            if (strcmp(player->inventory[i]->type, "Weapon") == 0) {
                player->strength += player->inventory[i]->bonusStrength;
                printf("%s kuşanıldı! Yeni saldırı gücü: %d\n", itemName, player->strength);
                printf("Silah hasarı: %d\n", player->inventory[i]->damage);
            }
            else if (strcmp(player->inventory[i]->type, "Armor") == 0) {
                player->health += player->inventory[i]->bonusHealth;
                printf("%s kuşanıldı! Yeni sağlık: %d\n", itemName, player->health);
                printf("Zırh savunması: %d\n", player->inventory[i]->defense);
            }
            else {
                printf("%s kuşanıldı, ancak etkisi yok.\n", itemName);
            }

            player->inventory[i]->durability--; // Dayanıklılığı azalt
            if (player->inventory[i]->durability <= 0) {
                printf("%s kırıldı ve kullanılamaz hale geldi!\n", itemName);
                dropItem(player, itemName);
            }
            return;
        }
    }
    printf("Bu eşya envanterde bulunamadı.\n");
}


void dropItem(Player* player, const char* itemName) {
    for (int i = 0; i < player->inventorySize; i++) {
        if (strcmp(player->inventory[i]->name, itemName) == 0) {
            freeItem(player->inventory[i]);
            for (int j = i; j < player->inventorySize - 1; j++) {
                player->inventory[j] = player->inventory[j + 1];
            }
            player->inventory[--player->inventorySize] = NULL;
            printf("%s envanterden çıkarıldı.\n", itemName);
            return;
        }
    }
    printf("Bu eşya envanterde bulunamadı.\n");
}

void useItem(Player* player, const char* itemName) {
    for (int i = 0; i < player->inventorySize; i++) {
        if (strcmp(player->inventory[i]->name, itemName) == 0) {
            if (strcmp(player->inventory[i]->type, "Consumable") == 0) {
                player->health += player->inventory[i]->bonusHealth;
                printf("%s kullanıldı! Sağlık artırıldı: %d\n", itemName, player->health);
                dropItem(player, itemName); // Kullanımdan sonra eşya silinir
            }
            else {
                printf("Bu eşya kullanılamaz. Kuşanmayı deneyin.\n");
            }
            return;
        }
    }
    printf("Bu eşya envanterde bulunamadı.\n");
}

void listInventory(Player* player) {
    if (player->inventorySize == 0) {
        printf("Envanter boş.\n");
        return;
    }
    printf("Envanteriniz:\n");
    for (int i = 0; i < player->inventorySize; i++) {
        printf("- %s (Bonus Sağlık: %d, Bonus Güç: %d, Hasar: %d, Savunma: %d, Dayanıklılık: %d)\n",
            player->inventory[i]->name,
            player->inventory[i]->bonusHealth,
            player->inventory[i]->bonusStrength,
            player->inventory[i]->damage,
            player->inventory[i]->defense,
            player->inventory[i]->durability);
    }
}



// Eşyayı Serbest Bırak
void freeItem(Item* item) {
    if (item) {
        free(item->name);
        free(item->type);
        free(item);
    }
   
}

void levelUp(Player* player) {
    player->level++;
    player->health += 40;
    player->inventoryCapacity += 2;
    player->strength += 10;
    player->inventory = (Item**)realloc(player->inventory, player->inventoryCapacity * sizeof(Item*));
    if (!player->inventory) {
        printf("Bellek genişletmesi başarısız oldu.\n");
        exit(1);
    }
    printf("Seviye atlandı! Yeni seviye: %d, yeni envanter kapasitesi: %d, Yeni Can:%d, Yeni Güç:%d\n",
        player->level, player->inventoryCapacity, player->health,player->strength);
}
void freeInventory(Player* player) {
    for (int i = 0; i < player->inventorySize; i++) {
        free(player->inventory[i]->name);
        free(player->inventory[i]);
    }
    free(player->inventory);
}

Room* createRoom(const char* name, const char* description) {
    Room* room = (Room*)malloc(sizeof(Room));
    if (!room) {
        printf("Bellek ayrımı başarısız oldu!\n");
        exit(1);
    }
    room->name = _strdup(name);
    room->description = _strdup(description);
    room->north = room->south = room->east = room->west = NULL;
    room->items = NULL;
    room->itemCapacity = 0;
    room->itemCount = 0;
    return room;
}






Room* createDynamicRoom(const char* name, const char* description, int playerLevel) {
    Room* room = createRoom(name, description);

    // Odadaki rastgele eşya
    if (rand() % 2 == 0) {
        addItemToRoom(room, randomItem());
    }

    return room;
}


void addItemToRoom(Room* room, Item* item) {
    if (room->itemCount < room->itemCapacity) {
        room->items[room->itemCount++] = item;
    }
    else {
        printf("Oda dolu, yeni eşya eklenemiyor!\n");
    }
}
void freeRoom(Room* room) {
    free(room->name);
    free(room->description);

    for (int i = 0; i < room->itemCount; i++) {
        freeItem(room->items[i]);
    }
    free(room->items);

    free(room);
}


void connectRooms(Room* room1, Room* room2, const char* direction) {
    if (strcmp(direction, "north") == 0) {
        room1->north = room2;
        room2->south = room1;
    }
    else if (strcmp(direction, "south") == 0) {
        room1->south = room2;
        room2->north = room1;
    }
    else if (strcmp(direction, "east") == 0) {
        room1->east = room2;
        room2->west = room1;
    }
    else if (strcmp(direction, "west") == 0) {
        room1->west = room2;
        room2->east = room1;
    }
}
void move(Player* player, Room** currentRoom, const char* direction) {
    Room* nextRoom = NULL;

    if (strcmp(direction, "north") == 0) nextRoom = (*currentRoom)->north;
    else if (strcmp(direction, "south") == 0) nextRoom = (*currentRoom)->south;
    else if (strcmp(direction, "east") == 0) nextRoom = (*currentRoom)->east;
    else if (strcmp(direction, "west") == 0) nextRoom = (*currentRoom)->west;

    if (nextRoom) {
        *currentRoom = nextRoom;
        printf("Yeni oda: %s\n", (*currentRoom)->name);

        // Oda boşsa eşya ekle
        if ((*currentRoom)->itemCount == 0) { // Sadece boş odalarda
            if (rand() % 2 == 0) {
                Item* randomItemInstance = randomItem();
                addItemToRoom(*currentRoom, randomItemInstance);
                printf("Odada bir eşya buldunuz: %s\n", randomItemInstance->name);
            }
           
        }
    }
    else {
        printf("Bu yöne gidemezsiniz!\n");
    }
}



void look(Room* currentRoom) {
    printf("Su an: %s\n", currentRoom->name);
    printf("Nerede olduğunu mu merak ediyorsun?\n%s", currentRoom->description);
    if (currentRoom->itemCount > 0) {
        printf("Odada bulunan esyalar:\n");
        for (int i = 0; i < currentRoom->itemCount; i++) {
            printf("- %s\n", currentRoom->items[i]->name);

        }
    }
    else {
        printf("Bu oda bos.\n");
    }
}
void fight(Player* player, Creature* creature,int isTraining) {
    printf("Savaş başladı! Rakibiniz: %s\n", creature->name);
    while (player->health > 0 && creature->health > 0) {
        printf("1. Saldır\n2. Savun\nSeçim yapın: ");
        int choice;
        scanf("%d", &choice);

        if (choice == 1) { // Saldır
            creature->health -= player->strength;
            printf("Saldırdınız! %s'nin kalan sağlığı: %d\n", creature->name, creature->health);
        }
        else if (choice == 2) { // Savun
            int reducedDamage = creature->strength / 2;
            player->health -= reducedDamage;
            printf("Savundunuz! Aldığınız hasar azaltıldı. Kalan sağlığınız: %d\n", player->health);
        }
        else {
            printf("Geçersiz seçim!\n");
            continue;
        }

        if (creature->health > 0) { // Yaratık saldırır
            player->health -= creature->strength;
            printf("%s size saldırdı! Kalan sağlığınız: %d\n", creature->name, player->health);
        }
    }

    if (player->health <= 0) {
        if (isTraining) {
            // Antrenmanda kaybetme durumu
            player->health = 10;
            printf("Antrenmanda yenildiniz ama bu gerçek bir savaş değildi. Biraz dinlenip tekrar deneyebilirsiniz.\n");
        }
        else {
            // Normal savaşta kaybetme
            printf("Yenildiniz! Oyun sona erdi.\n");
            exit(0);
        }
    }
    else {
        // Oyuncu kazandıysa
        printf("%s yendiniz!\n", creature->name);
        levelUp(player);
        
         }
    }
void saveGame(Player* player, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        printf("Dosya açılamadı.\n");
        return;
    }

    fprintf(file, "%d %d %d %d %d\n", player->health, player->strength, player->level, player->inventorySize,player->coin);
    for (int i = 0; i < player->inventorySize; i++) {
        fprintf(file, "%s %d %d %s %d %d %d\n",
            player->inventory[i]->name,
            player->inventory[i]->bonusHealth,
            player->inventory[i]->bonusStrength,
            player->inventory[i]->type,
            player->inventory[i]->damage,
            player->inventory[i]->defense,
            player->inventory[i]->durability);
    }

    fclose(file);
    printf("Oyun kaydedildi!\n");
}
void loadGame(Player* player, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Dosya açılamadı.\n");
        return;
    }

    freeInventory(player); // Eski envanteri temizle

    fscanf(file, "%d %d %d %d %d\n", &player->health, &player->strength, &player->level, &player->inventorySize, &player->coin);
    player->inventoryCapacity = player->inventorySize + 2; // Yeni kapasiteyi ayarla
    player->inventory = malloc(player->inventoryCapacity * sizeof(Item*));
    for (int i = 0; i < player->inventorySize; i++) {
        char name[50], type[20];
        int bonusHealth, bonusStrength, damage, defense, durability;
        fscanf(file, "%s %d %d %s %d %d %d\n",
            name,  // 'name' için boyut bilgisi
            &bonusHealth,
            &bonusStrength,
            type, // 'type' için boyut bilgisi
            &damage,
            &defense,
            &durability);
        player->inventory[i] = createItem(name, bonusHealth, bonusStrength, type, damage, defense, durability);
    }

    fclose(file);
    printf("Oyun yüklendi!\n");
}
void trainingSession(Player* player) {
    int choice;
    printf("Rastgele bir antrenman mı istersin(1) yoksa Yaratıkları Seçedebilirsin(2)\n");
    scanf("%d", &choice);
    switch (choice) {
    case 1: {
        Creature* dummy = randomCreateCreature();
        printf("Antrenman başlıyor. Bu savaş size XP kazandırır.\n");
        fight(player, dummy,1);

        free(dummy->name);
        free(dummy);

        break; 
        
        }
    case 2: {
        printf("Hangi yaratıkla savaşmak istersiniz?\n");
        printf("1. Minnion\n2. Goblin\n3. Orc\n4. Boss\n");
        int subchoice;
        scanf("%d", &subchoice);

        Creature* dummy = NULL;

        switch (subchoice) {
        case 1:
            dummy = createCreature("minnion");
            break;
        case 2:
            dummy = createCreature("goblin");
            break;
        case 3:
            dummy = createCreature("orc");
            break;
        case 4:
            dummy = createCreature("boss");
            break;
        default:
            printf("Geçersiz seçim. Minnion yaratıldı.\n");
            dummy = createCreature("minnion");
            break;
        }
        printf("Antrenman başlıyor. Bu savaş size XP kazandırır.\n");
        fight(player, dummy,1);

        free(dummy->name);
        free(dummy);

        if (player->health <= 0) {
            player->health = 10;
            printf("Antrenmanda yenildiniz ama bu gerçek bir savaş değildi. Biraz dinlenip tekrar deneyebilirsiniz.\n");
        }
        else {
            printf("Antrenmanı tamamladınız. XP kazandınız.\n");
        }
    }
         
    }
    

   
}
void startTournament(Player* player) {
    printf("Turnuva başlıyor! Sırasıyla Goblin, Orc ve Boss ile savaşacaksınız.\n");

    const char* creatures[] = { "goblin", "orc", "boss" };
    int numRounds = 3;

    for (int i = 0; i < numRounds; i++) {
        Creature* c = createCreature(creatures[i]);
        printf("\n%d. Raunt: %s ile savaşıyorsunuz!\n", i + 1, c->name);
        fight(player, c,0);

        free(c->name);
        free(c);

        if (player->health <= 0) {
            printf("Turnuvada elendiniz!\n");
            return;
        }
    }

    // Eğer buraya geldiysek tüm rauntlar başarıyla geçildi demektir.
    player->coin += 50;
    printf("Tebrikler! Turnuvayı kazandınız!\n Kazanılan Altın:50-----Mevcut Altın:%d",player->coin);
    
    player->level+=4;
    printf("Seviye atladınız! Yeni seviye: %d\n", player->level);
    
}
// Market odası menüsünde case 1 (Eşya Satın Al) için fonksiyon:
void buyItemFromMarket(Player* player, Room* marketRoom) {
    if (marketRoom->itemCount == 0) {
        printf("Markette hiç eşya yok!\n");
        return;
    }

    // Market eşyalarını ve fiyatlarını göster:
    // Altın Kılıç: index 0, fiyat 50
    // Demir Zırh: index 1, fiyat 80
    // Bu örnekte marketteki 2 eşya olduğunu varsayıyoruz, marketItemPrices dizisini global tanımladığınızı varsayalım.
    int marketItemPrices[2] = { 50, 80 };

    printf("Market Eşyaları:\n");
    for (int i = 0; i < marketRoom->itemCount; i++) {
        printf("%d. %s - Fiyat: %d coin\n", i + 1, marketRoom->items[i]->name, marketItemPrices[i]);
    }

    printf("Hangi eşyayı satın almak istersiniz? (1-%d, iptal için 0): ", marketRoom->itemCount);
    int choice;
    scanf("%d", &choice);

    if (choice == 0) {
        printf("Satın alma iptal edildi.\n");
        return;
    }

    if (choice < 1 || choice > marketRoom->itemCount) {
        printf("Geçersiz seçim.\n");
        return;
    }

    int itemIndex = choice - 1;
    int itemPrice = marketItemPrices[itemIndex];

    if (player->coin < itemPrice) {
        printf("Yeterli coin'iniz yok! Coininiz: %d, Fiyat: %d\n", player->coin, itemPrice);
        return;
    }

    // Coin yeterli, envantere ekleyelim:
    if (player->inventorySize >= player->inventoryCapacity) {
        printf("Envanteriniz dolu! Eşyayı satın alamazsınız.\n");
        return;
    }

    // Eşyayı kopyalıyoruz çünkü marketteki eşyayı direk player envanterine eklersek
    // marketteki eşyayı da silmemiz gerekir. Ya da market eşyasını direk player'a verebiliriz
    // sonra marketItemPrices yapısına gerek yok. Burada basitçe direk veriyoruz.

    // Player envanterine ekle
    addItem(player, marketRoom->items[itemIndex]);

    // Marketten o eşyayı çıkar:
    for (int i = itemIndex; i < marketRoom->itemCount - 1; i++) {
        marketRoom->items[i] = marketRoom->items[i + 1];
    }
    marketRoom->itemCount--;

    player->coin -= itemPrice;
    printf("Satın alma başarılı! Kalan coin: %d\n", player->coin);
}





void gameLoop(Player* player, Room* startingRoom) {
    Room* currentRoom = startingRoom;
    int gameRunning = 1; // Oyun devam ediyor mu kontrolü

    printf("\nHoş geldiniz, maceranız başlıyor!\n");
    while (gameRunning) {
        printf("\nMevcut konumunuz: %s\n", currentRoom->name);
        if (strcmp(currentRoom->name,"Ormanlık Alan")==0) {//Ormanlık alandaykenki menü
            printf("1. Hareket Et\n2. Bak\n3. Envanteri Listele\n4. Oyunu Kaydet\n5. Oyunu Yükle\n6. Oyundan Çık\n7. Seviyene,Canına ve Altınına Bak\n");
            printf("Seçim yapın: ");


            int choice;
            scanf("%d", &choice);

            switch (choice) { 

            case 1: { // Hareket Et
                printf("Hangi yönde hareket edeceksiniz? (north, south, east, west):\n North:Arena,South:Ev,East:Antrenman Odası,West:Market\n");

                char direction[10];
                scanf("%s", direction);
                move(player, &currentRoom, direction);

                   break;
            }
            case 2: { // Odada Bak
                look(currentRoom);
                break;
            }
            case 3: { // Envanteri Listele
                listInventory(player);

                printf("1. Kuşan\n2. Kullan\n3.Çıkış\nSeçiminizi yapın: ");
                int subChoice;
                scanf("%d", &subChoice);
                switch (subChoice) {
                case 1: {
                    printf("Hangi eşyayı kullanmak istiyorsunuz? ");
                    char itemName[20];
                    getchar(); // önceki satırdan kalan '\n' almak için
                    fgets(itemName, sizeof(itemName), stdin);
                    itemName[strcspn(itemName, "\n")] = 0; // sonda kalan \n kaldır
                    equipItem(player, itemName);
                    break;
                }
                case 2: {
                    printf("Hangi eşyayı kullanmak istiyorsunuz? ");
                    char itemName[20];
                    getchar(); // önceki satırdan kalan '\n' almak için
                    fgets(itemName, sizeof(itemName), stdin);
                    itemName[strcspn(itemName, "\n")] = 0; // sonda kalan \n kaldır
                    useItem(player, itemName);
                    break;
                }
                case 3: break;
                default:
                    printf("Geçersiz seçim");


                    break;
                }
                break;
                

                
            }
            case 4: {
                printf("Oyununuzu kaydetmek için bir dosya adı girin: ");
                char filename[50];
                scanf("%s", filename);
                saveGame(player, filename);
                break;
            }
            case 5: {
                printf("Kaydedilmiş oyunu yüklemek için bir dosya adı girin: ");
                char filename[50];
                scanf("%s", filename);
                loadGame(player, filename);
                break;
            }
            case 6: {
                printf("Oyundan çıkılıyor. Görüşmek üzere!\n");
                gameRunning = 0;
                break;
            }
            case 7: {
                printf("Seviye:%d-------Can:%d--------Coin:%d", player->level, player->health,player->coin);
                break;
            }


            default:
                printf("Geçersiz seçim, tekrar deneyin.\n");
                break;
            }

        }
        else if (strcmp(currentRoom->name, "Market") == 0) {
            printf("1.Eşya Satın Al 2.Ormanlık Alana Dön\n3.Odaya Bak\n4. Envanteri Listele\n5. Oyunu Kaydet\n6. Oyunu Yükle\n7. Oyundan Çık\n8. Seviyene Canına ve Altınına Bak\n ");
            printf("Seçim yapın: ");
            
            int choice;
            scanf("%d", &choice);
            switch (choice) {
            case 1: {
            
                buyItemFromMarket(player, currentRoom);
                break;
            
            }//Eşya satın alma işlemleri
            
            
            case 2: {
                currentRoom = startingRoom;
                break;
            
            } //Ormanlık Alana geri dönme    
            
            
            case 3: { // Odada Bak
                look(currentRoom);
                break;
            }
            case 4: { // Envanteri Listele
                listInventory(player);

                printf("1. Kuşan\n2. Kullan\n3.Çıkış\nSeçiminizi yapın: ");
                int subChoice;
                scanf("%d", &subChoice);
                switch (subChoice) {
                case 1: {
                    printf("Hangi eşyayı kullanmak istiyorsunuz? ");
                    char itemName[20];
                    getchar(); // önceki satırdan kalan '\n' almak için
                    fgets(itemName, sizeof(itemName), stdin);
                    itemName[strcspn(itemName, "\n")] = 0; // sonda kalan \n kaldır
                    equipItem(player, itemName);
                    break;
                }
                case 2: {
                    printf("Hangi eşyayı kullanmak istiyorsunuz? ");
                    char itemName[20];
                    getchar(); // önceki satırdan kalan '\n' almak için
                    fgets(itemName, sizeof(itemName), stdin);
                    itemName[strcspn(itemName, "\n")] = 0; // sonda kalan \n kaldır
                    useItem(player, itemName);
                    break;
                }
                case 3: break;
                default:
                    printf("Geçersiz seçim");


                    break;
                }
                break;
            }
            case 5: {
                printf("Oyununuzu kaydetmek için bir dosya adı girin: ");
                char filename[50];
                scanf("%s", filename);
                saveGame(player, filename);
                break;
            }
            case 6: {
                printf("Kaydedilmiş oyunu yüklemek için bir dosya adı girin: ");
                char filename[50];
                scanf("%s", filename);
                loadGame(player, filename);
                break;
            }
            case 7: {
                printf("Oyundan çıkılıyor. Görüşmek üzere!\n");
                gameRunning = 0;
                break;
            }
            case 8: {
                printf("Seviye:%d-------Can:%d--------Coin:%d", player->level, player->health, player->coin);
            }


            default:
                printf("Geçersiz seçim, tekrar deneyin.\n");
                break;
            }
            }
        else if (strcmp(currentRoom->name, "Arena") == 0) {
            printf("1.Turnuvaya katıl\n2.Ormanlık Alana Dön\n3.Odaya Bak\n4. Envanteri Listele\n5. Oyunu Kaydet\n6. Oyunu Yükle\n7. Oyundan Çık\n8. Seviye,Can ve Altınına Bak\n");
            printf("Seçim yapın: ");

            int choice;
            scanf("%d", &choice);
            switch (choice){
            case 1: {
                startTournament(player);
                break;
            }//turnuva işlemleri
            
            case 2: {
                currentRoom = startingRoom;
                break;

            } //Ormanlık Alana geri dönme    


            case 3: { // Odada Bak
                look(currentRoom);
                break;
            }
            case 4: { // Envanteri Listele
                listInventory(player);

                printf("1. Kuşan\n2. Kullan\n3.Çıkış\nSeçiminizi yapın: ");
                int subChoice;
                scanf("%d", &subChoice);
                switch (subChoice) {
                case 1: {
                    printf("Hangi eşyayı kullanmak istiyorsunuz? ");
                    char itemName[20];
                    getchar(); // önceki satırdan kalan '\n' almak için
                    fgets(itemName, sizeof(itemName), stdin);
                    itemName[strcspn(itemName, "\n")] = 0; // sonda kalan \n kaldır
                    equipItem(player, itemName);
                    break;
                }
                case 2: {
                    printf("Hangi eşyayı kullanmak istiyorsunuz? ");
                    char itemName[20];
                    getchar(); // önceki satırdan kalan '\n' almak için
                    fgets(itemName, sizeof(itemName), stdin);
                    itemName[strcspn(itemName, "\n")] = 0; // sonda kalan \n kaldır
                    useItem(player, itemName);
                    break;
                }
                case 3: break;
                default:
                    printf("Geçersiz seçim");


                    break;
                }
                break;
            }
            case 5: {
                printf("Oyununuzu kaydetmek için bir dosya adı girin: ");
                char filename[50];
                scanf("%s", filename);
                saveGame(player, filename);
                break;
            }
            case 6: {
                printf("Kaydedilmiş oyunu yüklemek için bir dosya adı girin: ");
                char filename[50];
                scanf("%s", filename);
                loadGame(player, filename);
                break;
            }
            case 7: {
                printf("Oyundan çıkılıyor. Görüşmek üzere!\n");
                gameRunning = 0;
                break;
            }
            case 8: {
                printf("Seviye:%d-------Can:%d--------Coin:%d", player->level, player->health, player->coin);
                break;
            }


            default:
                printf("Geçersiz seçim, tekrar deneyin.\n");
                break;
            }
            
            }
        else if (strcmp(currentRoom->name, "Ev") == 0) {
                printf("1.Ailenle vakit geçir\n2.Ormanlık Alana Dön\n3.Odaya Bak\n4. Envanteri Listele\n5. Oyunu Kaydet\n6. Oyunu Yükle\n7. Oyundan Çık\n8. Seviye,Can ve Altınına Bak\n");
                printf("Seçim yapın: ");

                int choice;
                scanf("%d", &choice);
                switch (choice) {
                case 1: {
                    // Ev odasına girince
                    printf("Ailen seni sıcak bir gülümsemeyle karşılıyor. Sağlığın tamamen yenilendi.\n");
                    player->health = 100; // Tam can
                    break;
                }//Ev işlemleri

                case 2: {
                    currentRoom = startingRoom;
                    break;

                } //Ormanlık Alana geri dönme    


                case 3: { // Odada Bak
                    look(currentRoom);
                    break;
                }
                case 4: { // Envanteri Listele
                    listInventory(player);

                    printf("1. Kuşan\n2. Kullan\n3.Çıkış\nSeçiminizi yapın: ");
                    int subChoice;
                    scanf("%d", &subChoice);
                    switch (subChoice) {
                    case 1: {
                        printf("Hangi eşyayı kullanmak istiyorsunuz? ");
                        char itemName[20];
                        getchar(); // önceki satırdan kalan '\n' almak için
                        fgets(itemName, sizeof(itemName), stdin);
                        itemName[strcspn(itemName, "\n")] = 0; // sonda kalan \n kaldır
                        equipItem(player, itemName);
                        break;
                    }
                    case 2: {
                        printf("Hangi eşyayı kullanmak istiyorsunuz? ");
                        char itemName[20];
                        getchar(); // önceki satırdan kalan '\n' almak için
                        fgets(itemName, sizeof(itemName), stdin);
                        itemName[strcspn(itemName, "\n")] = 0; // sonda kalan \n kaldır
                        useItem(player, itemName);
                        break;
                    }
                    case 3: break;
                    default:
                        printf("Geçersiz seçim");


                        break;
                    }
                    break;
                }
                case 5: {
                    printf("Oyununuzu kaydetmek için bir dosya adı girin: ");
                    char filename[50];
                    scanf("%s", filename);
                    saveGame(player, filename);
                    break;
                }
                case 6: {
                    printf("Kaydedilmiş oyunu yüklemek için bir dosya adı girin: ");
                    char filename[50];
                    scanf("%s", filename);
                    loadGame(player, filename);
                    break;
                }
                case 7: {
                    printf("Oyundan çıkılıyor. Görüşmek üzere!\n");
                    gameRunning = 0;
                    break;
                }
                
                case 8: {
                    printf("Seviye:%d-------Can:%d--------Coin:%d", player->level, player->health, player->coin);
                    break;
                }
                

                default:
                    printf("Geçersiz seçim, tekrar deneyin.\n");
                    break;
                }

                }
        else if (strcmp(currentRoom->name, "Antrenman Odası") == 0) {
                    printf("1.Antrenman Yap\n2.Ormanlık Alana Dön\n3.Odaya Bak\n4. Envanteri Listele\n5. Oyunu Kaydet\n6. Oyunu Yükle\n7. Oyundan Çık\n8. Seviye,Can ve Altınına Bak\n");
                    printf("Seçim yapın: ");

                    int choice;
                    scanf("%d", &choice);
                    switch (choice) {
                    case 1: {
                        trainingSession(player);
                        break;
                    }//Antrenman işlemleri

                    case 2: {
                        currentRoom = startingRoom;
                        break;

                    } //Ormanlık Alana geri dönme    


                    case 3: { // Odada Bak
                        look(currentRoom);
                        break;
                    }
                    case 4: { // Envantere listele
                        listInventory(player);

                        printf("1. Kuşan\n2. Kullan\n3.Çıkış\nSeçiminizi yapın: ");
                        int subChoice;
                        scanf("%d", &subChoice);
                        switch (subChoice) {
                        case 1: {
                            printf("Hangi eşyayı kullanmak istiyorsunuz? ");
                            char itemName[20];
                            getchar(); // önceki satırdan kalan '\n' almak için
                            fgets(itemName, sizeof(itemName), stdin);
                            itemName[strcspn(itemName, "\n")] = 0; // sonda kalan \n kaldır
                            equipItem(player, itemName);
                            break;
                        }
                        case 2: {
                            printf("Hangi eşyayı kullanmak istiyorsunuz? ");
                            char itemName[20];
                            getchar(); // önceki satırdan kalan '\n' almak için
                            fgets(itemName, sizeof(itemName), stdin);
                            itemName[strcspn(itemName, "\n")] = 0; // sonda kalan \n kaldır
                            useItem(player, itemName);
                            break;
                        }
                        case 3: break;
                        default:
                            printf("Geçersiz seçim");


                            break;
                        }
                        break;
                    }
                    case 5: {
                        printf("Oyununuzu kaydetmek için bir dosya adı girin: ");
                        char filename[50];
                        scanf("%s", filename);
                        saveGame(player, filename);
                        break;
                    }
                    case 6: {
                        printf("Kaydedilmiş oyunu yüklemek için bir dosya adı girin: ");
                        char filename[50];
                        scanf("%s", filename);
                        loadGame(player, filename);
                        break;
                    }
                    case 7: {
                        printf("Oyundan çıkılıyor. Görüşmek üzere!\n");
                        gameRunning = 0;
                        break;
                    }
                    case 8: {
                        printf("Seviye:%d-------Can:%d--------Coin:%d", player->level, player->health, player->coin);
                        break;
                    }


                    default:
                        printf("Geçersiz seçim, tekrar deneyin.\n");
                        break;
                    }
                    
                    }


                    // Oyuncunun sağlık durumunu kontrol et
                    if (player->health <= 0) {
                        printf("Sağlığınız tükendi. Oyun sona erdi.\n");
                        gameRunning = 0;
                    }
                  
         }
    




        
       
    }
    




int main() {
    srand((unsigned)time(NULL));//random sayı üretsin diye antrenmanda randomCreature üretilsin diye
    Player player;
    initializePlayer(&player);

    Room* room = createRoom("Ormanlık Alan", "Yoğun, sisli bir atmosferle kaplı ormanda ağaç gövdeleri gizemli işaretlerle dolu, dallar hafif bir rüzgârda esrarengiz bir fısıltıyla titreşir.Şu anda balta girmemiş bir ormandasın.\n"); //ormandayiz
    Room* roomSouth = createRoom("Ev", "Evine hoşgeldin.Burada seni sıcacık bir aile bekliyor çocukların ailen seni görmek için sabırsızlanıyor.");
    Room* roomWest = createRoom("Market","Burası tüm ihtiyaçlarını karşılayabileceğin bir Market.");
    Room* roomNorth = createRoom("Arena", "Burası tüm gladyatörlerin çıktığı zengin olduğu yer dilediğin gibi her zaman senin için bir dövüş ayarlanabilir.");
    Room* roomEast = createRoom("Antrenman Odası", "Her gladyatörün antrenman yaptığı bir yer vardır burada dilediğin gibi canavarlarla antrenman yapabilirsin");
    connectRooms(room, roomNorth, "north");//Ana odadan north'a doğru gidince roomNorth odası
    connectRooms(room, roomWest, "west");
    connectRooms(room, roomSouth, "south");
    connectRooms(room, roomEast, "east");
    // createRoom'dan sonra itemCapacity'yi artırın, sonra:
    roomWest->itemCapacity = 10;
    roomWest->items = malloc(sizeof(Item*)* roomWest->itemCapacity);
    roomWest->items[0] = createItem("Altın Kılıç", 0, 15, "Weapon", 20, 0, 5);
    roomWest->items[1] = createItem("Demir Zırh", 30, 0, "Armor", 0, 15, 10);
    roomWest->itemCount = 2;



    gameLoop(&player, room);

    freeRoom(room);
    freeRoom(roomSouth);
    freeRoom(roomEast);
    freeRoom(roomWest);
    freeRoom(roomNorth);
   
    freeInventory(&player);
    return 0;
}
