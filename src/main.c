//=================================================================
// Copyright 2026 Georgia Tech.  All rights reserved.
// The materials provided by the instructor in this course are for
// the use of the students currently enrolled in the course.
// Copyrighted course materials may not be further disseminated.
// This file must not be made publicly available anywhere.
//=================================================================

#include "buzz.h"
#include "globals.h"
#include "map.h"
#include "graphics.h"

/// #define statements for any constants that I am using 
#define DEBUG 0
#define FRAME_MS 100
#define WIN_SCORE 50 
#define VISIBLE_COLS 11
#define VISIBLE_ROWS 9

//advanced features (Directinoal Sprites)
#define BUZZ_DIR_RIGHT 0
#define BUZZ_DIR_UP 1
#define BUZZ_DIR_DOWN 2

/////////////////////////////////////////////////////////////////////////////////////////

// FUNCTION PROTOTYPES

int get_action(GameInputs inputs);
int update_game(int action);
int get_object();
void init_main_map();
void update_stingers();
void draw_game();

//I added the below ones makes the code look simpler and cleaner instead of writing down all code in the main func.
static int any_input(const GameInputs* inputs); 
static void wait_for_release();
static void wait_for_press_then_release();
static void show_start_screen(); 
static void show_countdown();
static void show_end_screen(int won);
static int scroll_period_from_speed(int speed);

//advanced feature function prototype for shattering
static void shatter_effect(int k, int l);

//advanced feature function prototype for music
static void music_task(void *pvParameters);

//advanced feature function prototype for POLLEN
static void draw_pollen(int u, int v);

/////////////////////////////////////////////////////////////////////////////////////////

// GLOBAL VARS

Buzz buzz; // Initialize the Buzz player

// LCD screen object
uLCD_4DGL myLCD;

// Speaker object and its associated timer
sound speaker;
gptimer_handle_t gptimer = NULL;

Buttons buttons;    // Initialize the push buttons
Nav_Switch myNav;   // Initialize the navigation switch

//For advanded features leaderboard
int leaderboard_scores[3] = {0,0,0}; // Array to hold the top 3 scores for the leaderboard

//Advanced Feature Game Start Difficulty Selection
int difficulty = 1; // Default difficulty level (1: Easy, 2: Medium)

//advanced feature directional sprites
int buzz_direction = BUZZ_DIR_RIGHT; // Variable to track the current direction of the buzz for directional sprites (0: right, 1: up, 2: down)
/////////////////////////////////////////////////////////////////////////////////////////
// This part of code I initilaze the function PROTOTYPES just showing what those functions does 

static int any_input(const GameInputs* inputs) //any control is currently being pressed.
{
    return inputs->up || inputs->down || inputs->left || inputs->right || inputs->center || inputs->b0 || inputs->b1 || inputs->b2 || inputs->b3;
}

static void wait_for_release() //Wait until all controls are released.

{
    GameInputs inputs; // Initialize a GameInputs struct to hold the current state of inputs

    do {
        read_inputs(&inputs, &buttons, &myNav); // Read the current state of inputs using the read_inputs function
        vTaskDelay(pdMS_TO_TICKS(20)); // Delay for 20 milliseconds 
    } while (any_input(&inputs)); // Continue looping until no inputs are detected, indicating that all controls have been released
}

static void wait_for_press_then_release() //Wait until any control is pressed, then wait for all controls to be released.
{
    GameInputs inputs; // Initialize a GameInputs struct to hold the current state of inputs

    while (1) {
        read_inputs(&inputs, &buttons, &myNav);
        if (any_input(&inputs)) { // If any input is detected, break out of the loop to proceed
            vTaskDelay(pdMS_TO_TICKS(20)); //
            wait_for_release(); // After detecting a press, call wait_for_release to wait until all controls are released before proceeding
            return;
        }
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}
    // You can remove this start screen section and get right into the game if you'd like
    // This is the text that prints on the start screen
static void show_start_screen()    
{
    uLCD_cls(&myLCD);
    uLCD_locate(&myLCD, 0, 0);
    uLCD_puts(&myLCD, "Buzz Blitz!\n", 0);
    //Leaderboard display on the start screen for advanced features
    char score_str[40];
    sprintf(score_str, "1:%d 2:%d 3:%d\n", leaderboard_scores[0], leaderboard_scores[1], leaderboard_scores[2]);

    uLCD_text_color(&myLCD, WHITE);
    uLCD_puts(&myLCD, "High Scores:\n", 0);
    uLCD_puts(&myLCD, score_str, 0);

    // Advanced feature example: Difficulty selection using B0 and B2 buttons on the start screen
    // Note: After win or loose, my game not coming here going straight to show_end_screen, so I put this difficulty selection in the start screen, so player can select difficulty before start playing again after win or lose. Can not change again, needs to restrat
    // Note continue: I also added the leaderboard to show_end_screen that way you can see your previous runs scores when you win or lose and decide if you want to try again to beat the scores. Here always it will be 0,0,0.
    uLCD_text_color(&myLCD, YELLOW);
    uLCD_puts(&myLCD, "B1: Play EASY\n", 0);
    uLCD_puts(&myLCD, "B2: Play HARD\n", 0);
    uLCD_text_color(&myLCD, WHITE);
    uLCD_puts(&myLCD, "B3: Controls\n", 0);

    uLCD_text_color(&myLCD, WHITE);
    uLCD_puts(&myLCD, "Press any button\n", 0);
    uLCD_puts(&myLCD, "to start.\n", 0);
}

static void show_countdown() // Show a simple 3-2-1 countdown before the game starts
{
    char msg[4]; // Buffer to hold the countdown number as a string

    for (int i = 3; i >= 1; i--) { // Countdown from 3 to 1
        uLCD_cls(&myLCD);
        sprintf(msg, "%d", i);
        uLCD_locate(&myLCD, 7, 6);
        uLCD_puts(&myLCD, msg, 0);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    uLCD_cls(&myLCD);
}

static void show_end_screen(int won) // Show a simple end screen with "You Win!" or "Game Over!" based on the game outcome, and prompt to restart
{
    uLCD_cls(&myLCD);
    uLCD_locate(&myLCD, 0, 0);
    if (won) {
        uLCD_puts(&myLCD, "You Win!\n", 0);
    } else {
        uLCD_puts(&myLCD, "You LOST! Game Over!\n", 0);
    }
    //Leaderboard display on the start screen for advanced features
    uLCD_text_color(&myLCD, WHITE);
    uLCD_puts(&myLCD, "High Scores:\n", 0);
    char score_str[40];
    sprintf(score_str, "1. %d\n2. %d\n3. %d\n\n", leaderboard_scores[0], leaderboard_scores[1], leaderboard_scores[2]);
    uLCD_puts(&myLCD, score_str, 0);
    
    uLCD_puts(&myLCD, "Press any button\n", 0);
    uLCD_puts(&myLCD, "to restart.\n", 0);
}

static int scroll_period_from_speed(int speed) // Given the buzz's current speed level, determine how many milliseconds should be between each scroll of the map. Higher speeds should result in faster scrolling (smaller periods).
{
    switch (speed) { //Changing speed / scroll by moving left or right
        case 3: return 2;
        case 2: return 4;
        default: return 6;
    }
}

//Advancded feature = background music task prototype
static void music_task(void* pvParameters) {
    int song_notes[] = {440, 392, 494, 523};  // Frequencies for A4, G4, B4, C5 notes to create a simple melody
    int song_durations[] = {300, 300, 300, 600}; // Durations for each note in milliseconds
    int song_length = sizeof(song_notes) / sizeof(song_notes[0]); // Calculate the number of notes in the song based on the size of the array

    tone(&speaker, &gptimer, song_notes[0]); // Start the music immediately when the task is created
    sound_on(&speaker, &gptimer); // Ensure the sound is on
    while(1) {

        for (int i = 0; i < song_length; i++) { // Loop through each note in the song
            tone(&speaker, &gptimer, song_notes[i]); // Play the current note using the tone function, passing the speaker object, timer handle, and the frequency of the note to be played
            vTaskDelay(pdMS_TO_TICKS(song_durations[i])); // Delay for the duration of the current note before moving on to the next one
        }
    }
}   

static void shatter_effect(int k, int l) // Advanced feature: Shatter effect when Buzz dies. This function will create a simple shattering animation at the given (u,v) location on the LCD.
{
    int local_x = k - get_map_start(); // Convert global map x-coordinate to local screen coordinate by subtracting the starting x-coordinate of the visible map area.

    if(local_x < 0 || local_x >= VISIBLE_COLS || l < 0 || l >= VISIBLE_ROWS) {
        return; // If the coordinates are out of bounds, exit the function
    }
    int u = local_x *11 +3; // Convert map coordinates to LCD pixel coordinates (assuming each tile is 11x11 pixels and adding an offset for centering)
    int v = l *11 +15; // Convert map coordinates to LCD pixel coordinates (assuming each tile is 11x11 pixels and adding an offset for centering)

    //Frame 1: Small Crack
    uLCD_filled_rectangle(&myLCD, u, v, u + 10, v + 10, WHITE);
    vTaskDelay(pdMS_TO_TICKS(120));

    // Frame 2: bigger crack
    uLCD_filled_rectangle(&myLCD, u, v, u + 10, v + 10, 0x8B4513);
    uLCD_line(&myLCD, u + 1, v + 2, u + 6, v + 6, WHITE);
    uLCD_line(&myLCD, u + 6, v + 6, u + 9, v + 3, WHITE);
    vTaskDelay(pdMS_TO_TICKS(120));

    // Frame 3: burst/explosion
    uLCD_filled_rectangle(&myLCD, u, v, u + 10, v + 10, BLACK);
    uLCD_line(&myLCD, u + 1, v + 1, u + 9, v + 9, YELLOW);
    uLCD_line(&myLCD, u + 9, v + 1, u + 1, v + 9, YELLOW);
    uLCD_filled_circle(&myLCD, u + 5, v + 5, 2, RED);
    vTaskDelay(pdMS_TO_TICKS(120));
}
//Advanced Feature: POLLEN
static void draw_pollen(int u, int v)
{   
    if(u<0 || u <0 || v>= VISIBLE_ROWS) {
        return; // If the coordinates are out of bounds, exit the function
    }

    if (get_current(u,v) == NULL)
    {
        add_pollen(u,v); // Add pollen to the map data structure at the specified coordinates (u,v) using the add_pollen function
    }
}
/**
 * Program entry point! This is where it all begins.
 * This function is for all the parts of the game. Most of your
 * implementation should be elsewhere - this holds the game loop, and should
 * read like a road map for the rest of the code.
 */
void app_main(void) {
    //////////////////////////////////////////////////////////////////////////////////////

    // INITIALIZE HARDWARE
    configure_sound(&speaker, &gptimer);
    xTaskCreate(music_task, "background_music", 2048, &speaker, 5, NULL); // Create a task for the background music, passing the speaker object as a parameter

    led_strip_handle_t rgb_led = configure_led(LED_GPIO_PIN); // Configure the RGB LED
    led_strip_clear(rgb_led);
    led_strip_set_pixel(rgb_led, 0, 0x80, 0, 0  ); // First sign of life for app -- turn on LED as 50% red
    led_strip_refresh(rgb_led); // Refresh strip to send data

    init_Buttons(&buttons, PB0_GPIO_PIN, PB1_GPIO_PIN, PB2_GPIO_PIN, PB3_GPIO_PIN);
    init_Nav_Switch(&myNav, NAV_UP_GPIO_PIN, NAV_DOWN_GPIO_PIN, NAV_LEFT_GPIO_PIN, NAV_RIGHT_GPIO_PIN, NAV_CENTER_GPIO_PIN);

    esp_rom_delay_us(1000000); // Delay to allow time to open serial monitor
    esp_rom_printf("%s", "\nStarting...\n"); // Unless the serial monitor is connected very quickly,

    // Initialize the uLCD -- nothing will be visible for three seconds!!
    // Do not call any uLCD functions until uLCD_init and uLCD_reset are complete
    uLCD_init(&myLCD, UART_NUM_1, LCD_RESET_GPIO_PIN, LCD_TX_GPIO_PIN, LCD_RX_GPIO_PIN);
    esp_rom_printf("\nResetting LCD (requires 3 seconds to complete)\n");
    uLCD_reset(&myLCD);
    if (DEBUG) printf("LCD reset! \n");
    led_strip_set_pixel(rgb_led, 0, 0, 0, 0xFF  );// Give a blue flicker to show life after LCD reset
    led_strip_refresh(rgb_led); // Refresh strip to send data

    // Clear the LCD screen
    uLCD_cls(&myLCD);
    if (DEBUG) printf("LCD screen cleared!\n");

    // Change the LCD baud rate to 115200 for faster drawing
    // Note that there are limitations on the LCD side for max baud rate. Even at 115200,
    // the effective speed is about 40000 baud due to processing delays in the LCD.
    uLCD_baudrate(&myLCD, 115200);
    if (DEBUG) printf("LCD Baudrate changed!\n");

    uLCD_locate(&myLCD, 0, 0);
    if (DEBUG) { // In debug mode, pause 2 seconds to see some text on LCD before demo starts
        uLCD_puts(&myLCD, "Wait...\r\n", 0);
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
    led_strip_set_pixel(rgb_led, 0, 0, 0x7F, 0 ); // Finished initialization -- LED turns 50% green
    led_strip_refresh(rgb_led); // Refresh strip to send data

    //////////////////////////////////////////////////////////////////////////////////////
    // You can remove this start screen section and get right into the game if you'd like
    // This is the text that prints on the start screen
    show_start_screen();

    while(1){ //Advanced feature: This loop allows the player to select difficulty and view controls before starting the game. It will only break out of the loop to start the game once they have made a selection.
        GameInputs menu_inputs; // Initialize a GameInputs struct to hold the current state of inputs for the menu
        read_inputs(&menu_inputs, &buttons, &myNav); // Read the current state of inputs using the read_inputs function

        int menu_action = get_action(menu_inputs); // Implement a get_menu_action function similar to get_action that checks for B0, B2, and B3 presses and returns a corresponding value (e.g., B0 for easy, B2 for hard, B3 for controls)

        if (menu_action == B1) { 
            difficulty = 1; // Set difficulty to Easy
            uLCD_cls(&myLCD);
            uLCD_locate(&myLCD, 1, 6); // Set cursor near the center
            uLCD_text_color(&myLCD, GREEN);
            uLCD_puts(&myLCD, "DIFFICULTY: EASY", 0);
                
            wait_for_release(); 
            vTaskDelay(pdMS_TO_TICKS(1500)); // Show the selected difficulty for 1.5 seconds before starting the game

            break; // Exit the loop to start the game
        } else if (menu_action == B2) {
            difficulty = 2; // Set difficulty to Hard
            uLCD_cls(&myLCD);
            uLCD_locate(&myLCD, 1, 6); // Set cursor near the center
            uLCD_text_color(&myLCD, GREEN);
            uLCD_puts(&myLCD, "DIFFICULTY: HARD", 0);
                
            wait_for_release(); 
            vTaskDelay(pdMS_TO_TICKS(1500)); // Show the selected difficulty for 1.5 seconds before starting the game
            
            break; // Exit the loop to start the game
        } else if (menu_action == B3) { // If the player presses B3, show the controls screen
            uLCD_cls(&myLCD);
            uLCD_locate(&myLCD, 0, 0);
            uLCD_text_color(&myLCD, GREEN);
            uLCD_puts(&myLCD, "CONTROLS:\n\n", 0);
            uLCD_text_color(&myLCD, WHITE);
            uLCD_puts(&myLCD, "Right/Left: Move/Speed\n", 0);
            uLCD_puts(&myLCD, "Center: Shoot\n", 0);
            uLCD_puts(&myLCD, "B1: Pause Game\n\n", 0);
            uLCD_text_color(&myLCD, YELLOW);
            uLCD_puts(&myLCD, "Press any button\nto return.", 0);
            
            wait_for_release(); // Wait for them to let go of B3
            wait_for_press_then_release(); // Wait for any button to go back
            show_start_screen(); // Redraw the start menu
        }
        vTaskDelay(pdMS_TO_TICKS(20));
    }
    wait_for_press_then_release();
    // This loop is for the start screen
    // While the user has not provided an input, the game should remain on the start screen
    while (1) {
        GameInputs inputs; // Initialize a GameInputs struct to hold the current state of inputs
        TickType_t t = xTaskGetTickCount(); // Variable to hold the current tick count for timing control & This way resulted much faster
        int won = 0; // Variable to track if the player has won the game

        // 0. Initialize the buzz and maps
        // TODO: implement buzz_init, maps_init(), and init_main_map() function
        // in buzz.c, map.c, and main.c respectively:
        buzz_init(&buzz);
        init_main_map();
        show_countdown();
        srand((unsigned)time(NULL)); // this line sets the random seed & doing it before buzz_init and init_main_map() so any rand() calls in setup use a fresh seed 

        // Main game loop
        while (1) { 
            // 1. Read inputs
            //TODO: implement read_inputs() function in hardware.c:
            read_inputs(&inputs, &buttons, &myNav);

            // 2. Determine action (move, act, menu, etc.)
            //TODO: implement get_action() function:
            int action = get_action(inputs);

            //Advanced feature example: Pause functionality using B1 button
            if(action == B1) { // If the player presses the B1 button, we will pause the game until they press it again
                uLCD_text_color(&myLCD, RED); // Change text color to red for the pause message (optional)
                uLCD_locate(&myLCD, 5, 4); // Set the cursor to the middle of the screen for the pause message
                uLCD_puts(&myLCD, "PAUSED", 0); // Display a "PAUSED" message on the screen when the game is paused

                wait_for_release(); // Wait for all buttons to be released before unpausing
                wait_for_press_then_release(); // Wait for any button press to resume the game
                show_countdown(); // Show the countdown again when resuming the game
            }
            // 3. Update game
            //TODO: implement update_game() function:
            int result = update_game(action);
            (void)result; // This line is to avoid an "unused variable" warning since result is not used yet Note: only result; was resulting complining error since it thinks it's an expression instead of a variable, so we can cast to void to explicitly ignore it for now

             // Update the upper status bar with current score, lives, energy, and speed info as desired

            uLCD_filled_rectangle(&myLCD, 0, 0, 127, 8, BLACK);
            char str[40];
            sprintf(str, "S:%d L:%d E:%d V:%d", buzz.score, buzz.lives, buzz.attack_energy, buzz.speed);
            uLCD_text_color(&myLCD, WHITE); //Customize the text color as desired (optional)
            uLCD_locate(&myLCD, 0, 0); // Set the cursor to the top-left corner of the screen
            uLCD_puts(&myLCD, str, 0); // Print the status bar text on the LCD

            // 3b. Check for game over based on result
            // and if so, handle game over or win accordingly
            // Check for game over conditions
            if (buzz.lives <= 0) {
                break;
            }
            // Check for win condition
            if (buzz.score >= WIN_SCORE) {
                won = 1;
                break;
            }
            // TODO: implement this here or as a new function.
            // 3c. If game is not over, then update stingers' positions
            update_stingers();

            // 4. Draw the game (mostly provided but some TODOs inside)
            draw_game();

            
            // Enforce a fixed 100ms loop period regardless of processing time
            vTaskDelayUntil(&t, pdMS_TO_TICKS(FRAME_MS));
        }
        //After Game is over, check if the score is high enough to be on the leaderboard and update the leaderboard scores accordingly
        if(buzz.score > leaderboard_scores[0]) {
            leaderboard_scores[2] = leaderboard_scores[1]; // Move the old 2nd place score down to 3rd place
            leaderboard_scores[1] = leaderboard_scores[0]; // Move the old 1st place score down to 2nd place
            leaderboard_scores[0] = buzz.score; // Set the new 1st place score to the player's score
        } else if (buzz.score > leaderboard_scores[1]) {
            leaderboard_scores[2] = leaderboard_scores[1]; // Move the old 2nd place score down to 3rd place
            leaderboard_scores[1] = buzz.score; // Set the new 2nd place score to the player's score
        } else if (buzz.score > leaderboard_scores[2]) {
            leaderboard_scores[2] = buzz.score; // Set the new 3rd place score to the player's score
        }

        show_end_screen(won); // Show the end screen with win/lose message
        wait_for_press_then_release(); // Wait for the player to press a button to restart the game
    }
}

/**
 * Given the game inputs, determine what kind of update needs to happen.
 * Possible return values are defined below.
 * Get Actions from User (pushbuttons, and nav_switch)
 * Based on push button and navigation switch inputs, determine which action
 * needs to be performed (may be no action).
 */
int get_action(GameInputs inputs)
{
    // TODO: Implement
    // 1. Check your action and menu button inputs and return the corresponding action value
    // 2. Check for your navigation switch inputs and return the corresponding action value
    // If no button is pressed, just return no action value
    static GameInputs prev; // Static variable to hold the previous state of inputs for edge detection
    int action = NO_ACTION; // Initialize the action variable to NO_ACTION, which will be returned if no inputs are detected

    // Check for navigation switch inputs and return corresponding action values
    if (inputs.up) action = GO_UP; 
    else if (inputs.down) action = GO_DOWN;
    else if (inputs.left && !prev.left) action = GO_LEFT;
    else if (inputs.right && !prev.right) action = GO_RIGHT;
    else if (inputs.center && !prev.center) action = CENTER_PRESSED;
    else if (inputs.b1 && !prev.b1) action = B1;
    else if (inputs.b2 && !prev.b2) action = B2;
    else if (inputs.b3 && !prev.b3) action = B3;

    prev = inputs; // Update the previous inputs state to the current state 
    return action; // Return the detected action
}

/**
 * This function is called by update game to check when the buzz collects an object.
 * We update the buzz and game conditions as needed based on the object type.
 */
int get_object()
{
    // 1. Get item at current location
    MapItem* item = get_current(buzz.loc_x, buzz.loc_y);
    // 2. Initialize a return value variable to denote the item type we got
    int return_value = NO_RESULT;
    // 3. Check if item is valid and is a nectar
    if (item && item->type == NECTAR) {
        //    3a. if so, increment the score
        //    3b. Remove the nectar item
        //    3c. Spawn a new nectar item at a random empty location
        buzz.score += 5; // Increment the score by 5 for collecting nectar
        buzz.attack_energy += 3; // Increment attack energy by 3 for collecting nectar (you can adjust this value as desired)
        return_value = GOT_OBJ; // Set return value to GOT_OBJ to indicate that we collected an object
        map_erase(buzz.loc_x, buzz.loc_y); // Remove the nectar item from the map after collecting it

        for (int tries = 0; tries < 20; tries++) { // Try up to 20 times to find a random empty location to spawn the new nectar
            int rand_x = (rand() % 20) + get_map_start(); // Random x coordinate within the visible map range
            int rand_y = rand() % VISIBLE_ROWS; // Random y coordinate within the visible map range
            if (get_current(rand_x, rand_y) == NULL) { // Check if the randomly selected location is empty (no item present)
                add_nectar(rand_x, rand_y); // If the location is empty, add a new nectar item at that location
                break; // Exit the loop after successfully adding a new nectar
            }
        }
    // 4. Else, check if the item is a valid heal or poison item
    //    4a. Handle this accordingly, updating necessary variables for the buzz struct
    //    4b. If consumable, remove the heal/poison item and set the return value variable to GOT_OBJ
    // 5. Erase the "old" version of your character based on previous location and return the return value variable
    //    Note that erasing the old version does not need to strictly happen here; it can be done elsewhere instead
    } else if (item && item->type == HEAL) {
        buzz.lives += 1;
        return_value = GOT_OBJ;
        map_erase(buzz.loc_x, buzz.loc_y);
    } else if (item && item->type == POISON) {
        buzz.lives -= 1;
        return_value = GOT_OBJ;
        map_erase(buzz.loc_x, buzz.loc_y);
    }

    return return_value;
}

/**
 * Update the game state based on the user action. For example, if the user
 * requests GO_UP, then this function should determine if that is possible by
 * consulting the map, and update buzz's position accordingly.
 */
int update_game(int action)
{
    // TODO: Implement
    // 1. Append a new random map panel to keep the scrolling map populated
    //    1a. Do this only if necessary, hint: use diff_map_start_end()
    //    1b. Consider also that you may want to respawn items that go out of frame
    //    as we scroll, depending on your desired game feel
    static int scroll_timer = 0;
    int action_result = NO_RESULT;

    if (diff_map_start_end() < 18) {
        append_random_panel();
    }
    // 2. Update the previous buzz position to the current position
    buzz.loc_x_prev = buzz.loc_x;
    buzz.loc_y_prev = buzz.loc_y;

    // 3. Check if action is to go up
    //      3a. Get item north of the current buzz position
    //      3b. Update the current buzz position upwards
    //      3c. Check if the item exists and item is walkable
    //          2c1. Otherwise, update buzz position back to its original position
    //               and return no result.
    //      3d. Check if the buzz gets an object by calling another function
    //      3e. Finally, return moved since the buzz has moved
    if (action == GO_UP && buzz.loc_y > 0) { //3. Going North (ensure we don't go out of bounds)
        MapItem* north_item = get_north(buzz.loc_x, buzz.loc_y);
        if (north_item == NULL || north_item->walkable) {
            draw_pollen(buzz.loc_x, buzz.loc_y); // Advanced feature: Leave a pollen trail when moving
            buzz.loc_y -= 1;
            buzz_direction = BUZZ_DIR_UP;
            get_object();
            action_result = MOVED;
        }
    } else if (action == GO_DOWN && buzz.loc_y < map_height() - 1) { //4. Going South (ensure we don't go out of bounds)
        MapItem* south_item = get_south(buzz.loc_x, buzz.loc_y);
        if (south_item == NULL || south_item->walkable) {
            draw_pollen(buzz.loc_x, buzz.loc_y); // Advanced feature: Leave a pollen trail when moving
            buzz.loc_y += 1;
            buzz_direction = BUZZ_DIR_DOWN;
            get_object();
            action_result = MOVED;
        }
    } else if (action == GO_LEFT) { //4. Going West: means we are slowing down, so we decrease speed by 1 (to a minimum of 1)
        if (buzz.speed > 1) {
            buzz.speed--;
        }
    } else if (action == GO_RIGHT) { //4. Going East: means we are speeding up, so we increase speed by 1 (to a maximum of 3)
        if (buzz.speed < 3) {
            buzz.speed++;
        }
    } else if (action == CENTER_PRESSED && buzz.attack_energy > 0) { //5. Other actions 
        MapItem* front_item = get_east(buzz.loc_x, buzz.loc_y);
        buzz.attack_energy--;

        if (front_item != NULL && !front_item->walkable) {
            shatter_effect(buzz.loc_x + 1, buzz.loc_y);// Show shatter effect at the location of the item being attacked
            map_erase(buzz.loc_x + 1, buzz.loc_y);
            buzz.score += 5;
            action_result = MOVED;
        } else if (front_item == NULL) {
            add_stinger(buzz.loc_x + 1, buzz.loc_y);
            action_result = MOVED;
        }
    }
    // 4. Else check if action is to go left, right, and down
    //    NOTE: Repeat same general steps as in go up 

    // 5. Else check for other actions (like button presses) and handle accordingly  
    //    You can do game over checks here as well depending on your design choices
    
    // If no action was given, then we simply return no result
    scroll_timer++; // Increment the scroll timer on each call to update_game, which happens every frame

    if (scroll_timer >= scroll_period_from_speed(buzz.speed)) {
        int steps;

        scroll_timer = 0; // Reset the scroll timer
        increment_map_start(); // Advance the visible map window one column to the right

        steps = (buzz.loc_x < get_map_start() + 5) ? 2 : 1;
        for (int i = 0; i < steps; i++) {
            MapItem* front_item = get_east(buzz.loc_x, buzz.loc_y);
            if (front_item != NULL && !front_item->walkable) {
                break;
            }
            draw_pollen(buzz.loc_x, buzz.loc_y); // Advanced feature: Leave a pollen trail when moving
            buzz.loc_x += 1;
            buzz_direction = BUZZ_DIR_RIGHT;
            buzz.score += 1;
            get_object();
        }

        if (buzz.loc_x < get_map_start()) {
            buzz.lives = 0;
            return GAME_OVER;
        }

        action_result = MOVED;
    }
    // NOTE: The map scrolls automatically. Each call to update_game should check
    // whether it is time to advance the map (call increment_map_start()) and
    // move buzz forward with it. You may use a cycle counter, a timer, or any
    // other approach to control the scroll rate relative to loop iterations. The 
    // purpose of this is so that it is possible to have movements within the game 
    // that are independent of the map scrolling.

    return action_result;
}

/**
 * This function is called once per iteration of the game loop to update the
 * positions of stingers on the map and handle their interactions with other items.
 */
void update_stingers() {
    // 1. Iterate over all visible map tiles
    // 2. If there is a stinger, erase old stringer
    // 3. Move stinger to the right if there's nothing there
    // 4. If there's a non-walkable item to the right, destroy it and increment score
    for (int i = VISIBLE_COLS - 1; i >= 0; i--) { // Iterate from right to left to avoid issues with moving stingers while iterating
        for (int j = 0; j < VISIBLE_ROWS; j++) { // Iterate over all rows
            int x = get_map_start() + i; // Calculate the actual x coordinate on the map based on the visible column index and the current map start
            int y = j; // The y coordinate is simply the row index since the visible rows start from 0

            MapItem* item = get_current(x, y); // Get the current item at this map location
            if (item && item->type == STINGER) { // 2. If there is a stinger at this location
                MapItem* right_item = get_east(x, y); 
                if (right_item == NULL) { //3. If there's nothing to the right of the stinger, move the stinger to the right
                    map_erase(x, y);
                    add_stinger(x + 1, y);
                } else if (!right_item->walkable) { //4. If there's a non-walkable item to the right of the stinger, destroy that item and increment the score
                    shatter_effect(x + 1, y); // Show shatter effect at the location of the item being destroyed by the stinger
                    map_erase(x + 1, y);
                    map_erase(x, y);
                    buzz.score += 5;
                }
            }
        }
    }
}

/**
 * Entry point for frame drawing. This should be called once per iteration of
 * the game loop. This draws all tiles on the screen, followed by the status bars.
 */
void draw_game() {
    // Iterate over all visible map tiles
    for (int i = 0; i < VISIBLE_COLS; i++) { // Iterate over visible columns from left to right
        for (int j = 0; j < VISIBLE_ROWS; j++) { // Iterate over visible rows from top to bottom
            int x = get_map_start() + i; //     Calculate the actual x coordinate on the map based on the visible column index and the current map start
            int y = j; // The y coordinate is simply the row index since the visible rows start from 0
            // Compute u,v coordinates for drawing
            int u = i * 11 + 3;
            int v = j * 11 + 15;

            // If the current tile is the buzz's location, draw the buzz
            if (x == buzz.loc_x && y == buzz.loc_y) {
                draw_buzz_direction(u, v, buzz_direction); // Draw the buzz with directional sprites based on the current direction
                continue;
            }

            // Get the current item at this tile and set the draw function accordingly
            MapItem* curr_item = get_current(x, y);
            if (curr_item != NULL && curr_item->draw != NULL) {
                curr_item->draw(u, v); // If there is an item at this location, call its draw function pointer to draw it
            } else {
                draw_nothing(u, v); // If there is no item at this location, draw the default background tile (nothing)
            }
        }
    }
    // Update the upper and lower status bars with current info

    draw_upper_status();
    draw_lower_status();
}

/**
 * Initialize the main world map with walls and other items.
 */
void init_main_map() {
    // Define the 4 panels that will be randomly appended to the map as it scrolls
    // You can modify these panels as you see fit, but make sure they maintain a walkable path
    // 1s represent walls and 0s represent empty space
    bool panels[6][MAP_PANEL_HEIGHT][MAP_PANEL_WIDTH] =
    {
        {
            {1,1,1,1,1,1,1,0,0,1,1,1,1,1,1},
            {0,1,1,1,1,1,0,0,0,1,1,1,1,1,0},
            {0,0,1,1,1,0,0,0,0,0,1,1,1,1,0},
            {0,0,0,0,0,0,0,0,0,0,0,0,1,1,0},
            {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,1,1,0,0,0,0,0,0},
            {0,0,0,0,0,1,1,1,1,1,0,0,0,0,0},
            {0,0,0,1,1,1,1,1,1,1,1,0,0,0,0},
            {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
        },
        {
            {0,0,0,0,0,0,0,0,0,0,1,1,1,1,1},
            {0,0,0,0,0,0,0,0,0,0,1,1,1,1,1},
            {0,0,0,0,0,0,0,0,0,0,1,1,1,1,1},
            {1,1,1,1,1,0,0,0,0,0,1,1,1,1,1},
            {1,1,1,1,1,0,0,0,0,0,1,1,1,1,1},
            {1,1,1,1,1,0,0,0,0,0,1,1,1,1,1},
            {1,1,1,1,1,0,0,0,0,0,0,0,0,0,0},
            {1,1,1,1,1,0,0,0,0,0,0,0,0,0,0},
            {1,1,1,1,1,0,0,0,0,0,0,0,0,0,0}
        },
        {
            {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
            {0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
            {0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
            {1,1,1,1,1,1,1,1,0,0,0,0,0,0,1},
            {1,1,1,1,1,1,1,1,0,0,0,0,0,0,1},
            {0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
            {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
            {1,1,1,1,1,1,1,1,1,1,1,1,1,0,0}
        },
        {
            {1,1,1,1,1,0,0,0,0,0,1,1,1,1,1},
            {1,1,0,0,0,0,0,0,0,0,0,0,0,1,1},
            {1,0,0,0,0,0,0,1,0,0,0,0,0,0,1},
            {0,0,0,0,0,0,1,1,1,0,0,0,0,0,0},
            {0,0,0,0,0,1,1,1,1,1,0,0,0,0,0},
            {0,0,0,0,0,0,1,1,1,0,0,0,0,0,0},
            {1,0,0,0,0,0,0,1,0,0,0,0,0,0,1},
            {1,1,0,0,0,0,0,0,0,0,0,0,0,1,1},
            {1,1,1,1,1,0,0,0,0,0,1,1,1,1,1}
        },
        {
            {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
            {1,1,1,0,0,0,0,0,0,0,0,0,1,1,1},
            {0,0,0,0,0,1,1,0,1,1,0,0,0,0,0},
            {0,0,0,0,0,1,1,0,1,1,0,0,0,0,0},
            {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
            {0,1,1,1,0,0,0,0,0,0,0,1,1,1,0},
            {0,0,0,0,0,0,1,1,1,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
            {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
        },
        {
            {1,1,1,1,0,0,0,0,0,0,0,1,1,1,1},
            {0,0,0,0,0,0,1,1,1,0,0,0,0,0,0},
            {0,0,0,0,0,0,1,0,1,0,0,0,0,0,0},
            {0,1,1,1,0,0,1,0,1,0,0,1,1,1,0},
            {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
            {0,1,1,1,0,0,1,0,1,0,0,1,1,1,0},
            {0,0,0,0,0,0,1,0,1,0,0,0,0,0,0},
            {0,0,0,0,0,0,1,1,1,0,0,0,0,0,0},
            {1,1,1,1,0,0,0,0,0,0,0,1,1,1,1}
        }
    };

    map_init();
    // 1. Add the panels to the map panels
    for (int i = 0; i < 6; i++) {
        add_map_panel(panels[i]);
    }
    // 2. TODO: Append two random panels to the map to start
    append_random_panel();
    append_random_panel();
    append_random_panel();


    for (int y = 0; y < VISIBLE_ROWS; y++) { // Erase any items in the initial visible map area to start with a clean slate (you can modify this if you want some starting items right away)
        for (int x = 0; x < VISIBLE_COLS; x++) { // Iterate over the initial visible columns and rows
            map_erase(x, y); // Erase any item at this location to ensure the starting area is clear of items (you can choose to spawn some items here instead if you want)
        }
    }
    // 3. TODO: Add some heal, nectar, and poison items
    int nectar_left = (difficulty==1) ? 5 : 2; // Set the number of nectar items to spawn based on difficulty (more for easy, less for hard)
    int heal_left = (difficulty==1) ? 3 : 1; // Set the number of heal items to spawn based on difficulty (more for easy, less for hard)
    int poison_left = (difficulty==1) ? 2 : 6; // Set the number of poison items to spawn based on difficulty (less for easy, more for hard)

    //Unlike get_object() where we know the exact location of the item we want to interact with, here we want to randomly spawn a certain number of each item type within the visible map area, so we are using a while loop 
    while (nectar_left > 0 || heal_left > 0 || poison_left > 0) { //Making sure we add the desired number of each item type and stay within the visible map area
        int rand_x = (rand() % 20) + get_map_start(); // Random x coordinate within the visible map range
        int rand_y = rand() % VISIBLE_ROWS; // Random y coordinate within the visible map range

        if (get_current(rand_x, rand_y) == NULL) {
            if (nectar_left > 0) {
                add_nectar(rand_x, rand_y);
                nectar_left--;
            } else if (heal_left > 0) {
                add_heal(rand_x, rand_y);
                heal_left--;
            } else if (poison_left > 0) {
                add_poison(rand_x, rand_y);
                poison_left--;
            }
        }
    }

    if (DEBUG) {
        print_map(); // Print the map to the console for testing
    }
}
