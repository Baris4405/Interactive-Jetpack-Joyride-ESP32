// ============================================
// The Map class file
//
// Copyright 2026 Georgia Tech. All rights reserved.
// The materials provided by the instructor in this course are for
// the use of the students currently enrolled in the course.
// Copyrighted course materials may not be further disseminated.
// This file must NOT be made publicly available anywhere.
//==================================================================


#include "map.h"

// #include "globals.h"
// #include "graphics.h"
#define MHF_NBUCKETS 97

/**
 * The Map structure. This holds a HashTable for all the MapItems, along with
 * values for the width and height of the Map.
 */
struct Map {
    HashTable* items;
    int w, h;
};

#define NUM_MAPS 1
#define MAX_MAP_PANELS 10
#define BLANK_WIDTH 5

static Map maps[NUM_MAPS];
static int active_map;
static int map_x_start;
static int map_x_end;
static bool map_panels[MAX_MAP_PANELS][MAP_PANEL_HEIGHT][MAP_PANEL_WIDTH]; // Track which panels have been generated
static int map_panel_used = 0; // Track how many panels have been used

/**
 * The first step in HashTable access for the map is turning the two-dimensional
 * key information (x, y) into a one-dimensional unsigned integer.
 * This function should uniquely map (x,y) onto the space of unsigned integers.
 */
static unsigned XY_KEY(int X, int Y) {
    
    unsigned int ux = X;
    unsigned int uy = Y;
    // TODO: Implement 
    // Hint: Think about how you accessed location in Project 1
    // 1. Return the 1-D xy key
    return(ux << 16) | (uy & 0xFFFF);
}

/**
 * This is the hash function actually passed into createHashTable. It takes an
 * unsigned key (the output of XY_KEY) and turns it into a hash value (some
 * small non-negative integer).
 */
unsigned map_hash(unsigned key) {
    
    // TODO: Implement 
    // Hint: what math operator can be used as an easy way to hash a key?
    // 1. Use MHF_NBUCKETS as part of your hash function
    // 2. Return the hashed key
    return key % MHF_NBUCKETS;
}

/**
 * Initializes the map, using a hash_table, setting the width and height.
 */
void map_init() {
    // TODO: Implement
    // 1. Set width & height for your map
    // 2. Create hashtable for its items
    Map* map = get_active_map();

    if(map->items) {
        // If items already exists, free it before creating a new one
        destroyHashTable(map->items);
    }
    active_map = 0;
    map_x_start = 0;
    map_x_end = 0;
    map_panel_used = 0;


    map->w = 1024; // Example width, can be changed
    map->h = MAP_PANEL_HEIGHT;  // Example height, can be lower
    map->items = createHashTable(map_hash, MHF_NBUCKETS);
}

/**
 * Adds the given panel to map's list of panels.
 */
void add_map_panel(bool panel[MAP_PANEL_HEIGHT][MAP_PANEL_WIDTH]) {
    // TODO: Implement
    // 1. Add the panel to the map panels data structure (map_panels)
    // 2. Update any variables you need to keep track of for the panels (map_panel_used)
    if(map_panel_used < MAX_MAP_PANELS) {
        for (int i = 0; i < MAP_PANEL_HEIGHT; i++) {
            for (int j = 0; j < MAP_PANEL_WIDTH; j++) {
                map_panels[map_panel_used][i][j] = panel[i][j];
            }
        }
        map_panel_used++;
    }
}

/**
 * Adds a random panel to the end of the map.
 */
void append_random_panel() {
    // TODO: Implement
    // 1. Randomly select a panel from the map panels data structure (map_panels)
    // 2. Randomly decide whether to flip the panel horizontally or vertically (or not at all)
    // 3. Add the panel to the end of the map with the decided alteration
    // 4. Add blank space after the panel
    // 5. Update any variables you need to keep track of for the map end
    int r = rand() % map_panel_used; // Randomly select a used panel
    bool flip_h = rand() % 2; // Randomly decide to flip horizontally
    bool flip_v = rand() % 2; // Randomly decide to flip vertically
    int panel_start = map_x_end;
    // Add the panel to the end of the map with the decided alteration
    for(int i = 0; i < MAP_PANEL_HEIGHT; i++) {
        for(int j = 0; j < MAP_PANEL_WIDTH; j++) {
            int src_i = flip_h ? (MAP_PANEL_HEIGHT - 1 - i) : i;
            int src_j = flip_v ? (MAP_PANEL_WIDTH - 1 - j) : j;

            int dest_x = panel_start + j;
            int dest_y = i; // Vertical flip will be handled in the source panel indexing

            
            if (map_panels[r][src_i][src_j]) {
                add_wall(dest_x, dest_y, HORIZONTAL, 1); // Example: add a wall, can be changed based on panel design
            }
        }
    }
    // for (int tries = 0; tries < 20; tries++) { // Try up to 20 times to place an item in the new panel
    //     int item_x = panel_start + (rand() % MAP_PANEL_WIDTH); // Random x coordinate within the new panel
    //     int item_y = rand() % MAP_PANEL_HEIGHT; // Random y coordinate within the new panel
    //     if (get_current(item_x, item_y) == NULL) { // Check if the location is empty
    //         int roll = rand() % 5; //   Randomly decide which item to place
    //         if (roll < 3) add_nectar(item_x, item_y); // 60% chance to add nectar
    //         else if (roll == 3) add_heal(item_x, item_y); // 20% chance to add heal
    //         else add_poison(item_x, item_y); // 20% chance to add poison
    //         break;
    //     }
    // }

    map_x_end += (MAP_PANEL_WIDTH + BLANK_WIDTH); // Update map end
}

 /**
 * Returns the leftmost column of the map currently being drawn on the screen.
 */
int get_map_start() {
    //  TODO: Implement
    // 1. Return the starting x coordinate of the map
    return map_x_start;
}

/**
 * Increments the starting x coordinate of the map.
 */
void increment_map_start() {
    // TODO: Implement
    // 1. Increment the starting x coordinate of the map (map_x_start)
    map_x_start++;
}

/**
 * Returns the distance between the starting and ending x coordinates of the map.
 */
int diff_map_start_end() {
    // TODO: Implement
    // 1. Return the distance between the starting and ending x coordinates of the map
    return map_x_end - map_x_start;
}

/**
 * Returns a pointer to the active map.
 */
Map* get_active_map() {
    return &maps[active_map];
}

/**
 * Sets the active map to map m, where m is the index of the map to activate.
 * Returns a pointer to the new active map.
 */
Map* set_active_map(int m) {
    active_map = m;
    return &maps[active_map];
}

/**
 * Returns the map m, regardless of whether it is the active map. This function
 * does not change the active map.
 */
Map* get_map(int m) {
    return &maps[m];
}

/**
 * Print the active map to the serial console.
 */
void print_map() {
    char lookup[] = {'W', 'D', 'P', 'A', 'K', 'N', 'H', 'S', 'B', 'O', 'Y'}; //Edited for Pollen since pollen is 10
    Map* map = get_active_map();
    printf("Current map start: %d, end: %d\n", get_map_start(), (get_map_start() + diff_map_start_end()) % map_width());    
    printf(" ");
    for (int i = get_map_start(); i < get_map_start() + diff_map_start_end(); i++) {
        printf("%d", (i%map_width())%10);
    }
    printf("\n");
    for(int j = 0; j < map->h; j++) {
        for (int i = get_map_start(); i < get_map_start() + diff_map_start_end(); i++) {
            if (i == get_map_start()) {
                printf("%d", j%10);
            }

            MapItem* item = (MapItem*)getItem(map->items, XY_KEY(i%map_width(), j));
            if (item && item->type < (int)sizeof(lookup)) printf("%c", lookup[item->type]);
            else printf(" ");
        }
        printf("\r\n");
    }
}

/**
 * Returns width of active map
 */
int map_width() {

    // TODO: Implement  
    // Hint 1 for this and below functions: use another function to get the current active map
    // Hint 2: check the struct of a map item
    return get_active_map()->w;
}

/**
 * Returns height of active map
 */
int map_height() {
    // TODO: Implement
    return get_active_map()->h;
}

/**
 * Returns the area of the active map
 */
int map_area() {
    
    // TODO: Implement  
    // HINT: You have the width and height to compute area
    return map_width() * map_height();
}

/**
 * Returns MapItem at current coordinate location
 */
MapItem* get_current(int x, int y) {
    
    // TODO: Implement  
    // 1. Get map item
    // 2. Return the item
    return getItem(get_active_map()->items, XY_KEY(x, y));
}

/**
 * Returns the MapItem immediately above the given location.
 */
MapItem* get_north(int x, int y) {
    
    // TODO: Implement
    // 1. Get map item 
    // 2. Return the item
    return getItem(get_active_map()->items, XY_KEY(x, y - 1));

}

/**
 * Returns the MapItem immediately below the given location.
 */
MapItem* get_south(int x, int y) {
    
    // TODO: Implement 
    // 1. Get map item
    // 2. Return the item
    return getItem(get_active_map()->items, XY_KEY(x, y + 1));
}

/**
 * Returns the MapItem immediately right the given location.
 */
MapItem* get_east(int x, int y) {
    
    // TODO: Implement 
    // 1. Get map item
    // 2. Return the item
    return getItem(get_active_map()->items, XY_KEY(x + 1, y));
}

/**
 * Returns the MapItem immediately left the given location.
 */
MapItem* get_west(int x, int y) {
   
    // TODO: Implement 
    // 1. Get map item
    // 2. Return the item
    return getItem(get_active_map()->items, XY_KEY(x - 1, y));
}

/**
 * Erase current location map item
 */
void map_erase(int x, int y) {
    
    // TODO: Implement  
    // 1. Get current location map item
    // 2. Free current location map item if it exists
    deleteItem(get_active_map()->items, XY_KEY(x, y));
}

/**
 * Adds a wall to the map starting at (x,y) in the given direction and of the given length.
 */
void add_wall(int x, int y, int dir, int len) {
    for(int i = 0; i < len; i++) {
        MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
        w1->type = WALL;
        w1->draw = draw_wall;
        w1->walkable = false;
        w1->data = NULL;
        unsigned key = (dir == HORIZONTAL) ? XY_KEY(x+i, y) : XY_KEY(x, y+i);
        void* val = insertItem(get_active_map()->items, key, w1);
        if (val) free(val); // If something is already there, free it
    }
}

/**
 * Adds a nectar item to the map at (x,y).
 */
void add_nectar(int x, int y) {
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = NECTAR;
    w1->draw = draw_nectar;
    w1->walkable = true;
    w1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}

/**
 * Adds the buzz character to the map at (x,y).
 */
void add_buzz(int x, int y) {
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = BUZZ;
    w1->draw = draw_buzz;
    w1->walkable = false;
    w1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}

/**
 * Adds a heal item to the map at (x,y).
 */
void add_heal(int x, int y) { // code to add a heal item
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = HEAL;
    w1->draw = draw_heal;
    w1->walkable = true;
    w1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}

/**
 * Adds a poison item to the map at (x,y).
 */
void add_poison(int x, int y) { // code to add a poison
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = POISON;
    w1->draw = draw_poison;
    w1->walkable = true;
    w1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}

/**
 * Adds a stinger item to the map at (x,y).
 */
void add_stinger(int x, int y) { // code to add a simple stinger
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = STINGER;
    w1->draw = draw_stinger;
    w1->walkable = true;
    w1->data = NULL; // Keep it simple, no destination data needed
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}

// Advanced feature: Adds a pollen item to the map at (x,y)
void add_pollen(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem)); // Allocate memory for a new MapItem
    w1->type = POLLEN; // Set the type of the MapItem to POLLEN, which should correspond to a unique integer defined in map.h
    w1->draw = draw_pollen_tile; // Set the draw function pointer to the draw_pollen function, which should be defined in graphics.c and declared in graphics.h
    w1->walkable = true; // Set walkable to true, allowing the player to move onto the pollen item and interact with it
    w1->data = NULL; // Set data to NULL for simplicity, but this could be used to store additional information about the pollen item if needed in the future

    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1); // Insert the new MapItem into the active map's hash table at the key corresponding to the (x,y) coordinates. The insertItem function should return a pointer to any existing item at that location, which we store in val.
    if (val != NULL) 
    {
        // If val is not NULL, it means there was already an item at that location. We free the memory for that existing item to prevent memory leaks before overwriting it with the new pollen item.
        free(val);
    }
}
