#include <ncurses.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>
#include <sys/stat.h>
#include <pthread.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>


typedef struct{
    int x, y;
} Pos;

typedef struct{
    Pos player;
    Pos *items;
    int score;
    int highest;
    int gold;
    int hunger;
    int normal_food, magic_food , quality_food , rotenflesh;
    int Health_potin, Speed_potion , Damage_potion;   
    int magic_wand , arrows, dagger , sword , mace;
    char range_weapon_name[50];
    char melee_weapon_name[50];
    int range_weapon;
    int melee_weapon;
    time_t time_limit;
    time_t start_time;
    int speed ;
    int color;
    int games_started;
    int games_finished;
    char name[51];
    char password[51];
    char email[51];
} Game;

typedef struct{
    char signs;
    int flags;
    int layer;
    int health;
    int enemy;
    int count;
} Map;

typedef struct{
    char name[51];
    int score;
    int games_started;
    int games_finished;
} Leaderboard;

int l = 0;
int menu = 1 , menu_playing=0;
int welcome = 1;
int signin = 1;
int First_floor = 1,First_floor_playing=0;
int Second_floor = 1,Second_floor_playing=0;
int Third_floor = 1,Third_floor_playing=0;
int Forth_floor = 1,Forth_floor_playing=0;
int GAME_STATUS = 1;
int timer_1 = 0;
int timer_2 = 0;
int elapsed = 0;
int difficulty = 0;
int hunger = 0;
int damage = 0;
int healing = 0;
int healing_speed = 1;
int music_is_playing = 1;
int on_potions = 0;
int x1[6][10];
int y_1[6][10];
int x2[6][10];
int y2[6][10];
Map map[6][50][184];
Game g;
int dim1 =0, dim2 =0;
int dim3 =0, dim4=0;
int dimmaster;
int in_game = 1;

char *coin="⎈";
char *blackcoin="⛃";
char *playerr= "☭";
char *arroww="➳";
char *daggerr="🗡";
char *swordd="⚔";
char *wandd="⚚";
char *foodd="⛾";
char *potionn="⚱";
char *flag = "🏲";
char *apple = "♡";

void welcome_panel();
void draw_menu_border();
void back_ground_sky();

void get_player_info(Game *g);
void get_innitial_values();
void set_final_values();
void create_a_new_account(Game *g);
void get_username(Game *g,FILE *fptr);
void get_user_password(Game *g,FILE *fptr);
void get_user_email(Game *g,FILE *fptr);
bool check_username(char * name);
bool check_user_password(char * password);
void random_password_generator();
bool check_user_email(char * email);

void log_into_account(Game *g);
void get_username_from_loggin(Game *g,FILE *fptr);
void get_user_password_from_loggin(Game *g ,FILE *fptr);
bool check_username_from_loggin(char * name); 
void get_player_info(Game *g);
void password_from_loggin(Game *g,FILE *fptr);
bool check_user_password_from_loggin(Game *g,char * password);
int menu_page(Game *g);

void show_score_board();
void add_to_score_board();
void show_user_profile(Game *g);
void show_options_menu(Game *g);


void Set_game_difficulty();
void Set_colors();
int Set_player_color();
void Set_game_music();

void create_a_new_game();

void draw_hungerbar();
void draw_heathbar();
void draw_food_count();
void clear_massages();

void get_four_levels_rooms();
void get_hidden_rooms();
void generage_gold_int_map(int x1 , int x2 , int y_1 , int y2 ,int l);
void generage_food_int_map(int x1 , int x2 , int y_1 , int y2 ,int l);
void generage_pillars_int_map(int x1 , int x2 , int y_1 , int y2 ,int l);
void generage_traps_int_map(int x1 , int x2 , int y_1 , int y2 ,int l);
void generage_potions_int_map(int x1 , int x2 , int y_1 , int y2 ,int l);
void generage_weapons_int_map(int x1 , int x2 , int y_1 , int y2 ,int l);
void generage_enemies_int_map(int x1 , int x2 , int y_1 , int y2 ,int l);

void fill_the_map();
void handle_movement(int ch, Pos *p);
void output_massages(int massage);
void draw_player(Game g);
void draw_rooms();
void check_room_index();

void food_menu();
void magic_menu();
void weapon_menu();

void* playMusic(void* arg);
void* welcomeplayMusic(void* arg);
void* signinplayMusic(void* arg);
void* menuplayMusic(void* arg);
void check_what_to_play(int l);

void Check_winning_or_losing();


//*********************************************************************************
int main(){

    pthread_t musicThread;
    pthread_t welcomemusicThread;
    pthread_t signinmusicThread;
    setlocale(LC_ALL, "");

    initscr();

    srand(time(0));

    get_four_levels_rooms();
    fill_the_map();

    start_color();
    Set_colors();

    srand(time(0));
    noecho();
    curs_set(FALSE);
    keypad(stdscr, TRUE);



    pthread_create(&welcomemusicThread, NULL, welcomeplayMusic, NULL);    
    welcome_panel();
    pthread_join(welcomemusicThread, NULL);
 
    // pthread_create(&musicThread, NULL, playMusic, NULL);   
    // menu = 0;
    // First_floor=0;Second_floor=0;Third_floor=0;Forth_floor=0;
    // First_floor=1;    
    // pthread_join(musicThread, NULL);

    pthread_create(&signinmusicThread, NULL, signinplayMusic, NULL);   
    get_player_info(&g);
    pthread_join(signinmusicThread, NULL);

    get_innitial_values();

    while(menu_page(&g)){}
    g.start_time = time(NULL);

    
    check_what_to_play(l);
    while(GAME_STATUS){
        if(in_game)while(menu_page(&g)){};
        draw_menu_border();
        check_room_index();
        draw_rooms();
        draw_player(g);
        draw_hungerbar();
        draw_heathbar();

        draw_food_count();
        int ch = getch();
        handle_movement(ch, &g.player);
        // mvprintw(40 , 110 , "%d , %d" , COLS , LINES);
        refresh();
        clear_massages();
        Check_winning_or_losing();
    }
    Mix_CloseAudio();
    SDL_Quit();
    return 0;
}

//********************************************************************************







void welcome_panel(){
    back_ground_sky();
    draw_menu_border();
    for (int j = 11 ; j <= 28 ; j++){
        for (int i = 68 ; i <= 114 ; i++){
            mvprintw( j , i , " ");
        }
    }
    mvprintw(LINES / 2 - 10, COLS / 2 - 15, " ____                        ");
    mvprintw(LINES / 2 - 9, COLS / 2 - 15, "|  _ \\ ___   __ _ _   _  ___ ");
    mvprintw(LINES / 2 - 8, COLS / 2 - 15, "| |_) / _ \\ / _` | | | |/ _ \\");
    mvprintw(LINES / 2 - 7, COLS / 2 - 15, "|  _ | (_) | (_| | |_| |  __/");
    mvprintw(LINES / 2 - 6, COLS / 2 - 15, "|_| \\_\\___/ \\__, |\\__,_|\\___|");
    mvprintw(LINES / 2 - 5, COLS / 2 - 15, "            |___/            ");
    // mvprintw(LINES / 2 - 4, COLS / 2 - 15, "          By Parsa           ");
    mvprintw(LINES / 2 - 1, COLS / 2 - 15, "Press any key to countinue");
    noecho();
    keypad(stdscr, TRUE);
    getch();
    welcome = 0;
    return;
}

void get_innitial_values(){
    strcpy(g.melee_weapon_name,"Mace");
    g.melee_weapon = 0;
    g.range_weapon = 0; 
    g.hunger = 100;
    g.speed = 1;
    g.magic_wand=0 ;
    g.arrows=0;
    g.dagger=0;
    g.sword=0;
    g.mace=1;
    if(strcmp(g.name , "Guest") == 0){
        return;
    }
    FILE *fpt;
    char filepath[150];
    snprintf(filepath, sizeof(filepath), "%s/%s", g.name, "scores");
    fpt = fopen(filepath,"r");
    fscanf(fpt, "%d", &g.highest);
    fclose(fpt);
    snprintf(filepath, sizeof(filepath), "%s/%s", g.name, "started");
    fpt = fopen(filepath,"r");
    fscanf(fpt, "%d", &g.games_started);
    fclose(fpt);
    snprintf(filepath, sizeof(filepath), "%s/%s", g.name, "wons");
    fpt = fopen(filepath,"r");
    fscanf(fpt, "%d", &g.games_finished);
    fclose(fpt);
}            
void set_final_values(){
    if(strcmp(g.name , "Guest") == 0){
        return;
    }
    FILE *fpt;
    char filepath[150];
    snprintf(filepath, sizeof(filepath), "%s/%s", g.name, "scores");
    fpt = fopen(filepath,"w");
    fprintf(fpt, "%d", g.highest);
    fclose(fpt);
    snprintf(filepath, sizeof(filepath), "%s/%s", g.name, "started");
    fpt = fopen(filepath,"w");
    fprintf(fpt, "%d", g.games_started);
    fclose(fpt);
    snprintf(filepath, sizeof(filepath), "%s/%s", g.name, "wons");
    fpt = fopen(filepath,"w");
    fprintf(fpt, "%d", g.games_finished);
    fclose(fpt);   
}
void get_player_info(Game *g){
    draw_menu_border();

    const char *levels[] = {"Create a new account", "Log in", "Enter as a Guest"};
    int choice = 0;
    back_ground_sky();

    for (int j = 3 ; j <= 14 ; j++){
        for (int i = 12 ; i <= 48 ; i++){
            mvprintw( j , i , " ");
        }
    }
    while (1){
        draw_menu_border();
        mvprintw(1, 1, " ");
        for (int i = 0; i < 3; ++i)
        {
            if (i == choice)
                attron(A_REVERSE);
            mvprintw(7 + i, 20, "%s", levels[i]);
            if (i == choice)
                attroff(A_REVERSE);
        }

        int ch = getch();
        if (ch == KEY_UP)
            choice = (choice == 0) ? 2 : choice - 1;
        else if (ch == KEY_DOWN)
            choice = (choice == 2) ? 0 : choice + 1;
        else if (ch == 10)
            break;
    }

    switch (choice)
    {
    case 0:
        create_a_new_account(g);
        break;
    case 1:
        log_into_account(g);
        break;
    case 2:
        strcpy(g->name ,"Guest");
        strcpy(g->password , "None");
        strcpy(g->email , "None");
        break;
    }
    signin = 0;
    clear();
}

void draw_menu_border(){
    for (int x = 0; x < COLS; ++x)
    {
        mvprintw(0, x, "-");
        mvprintw(LINES - 1, x, "_");
    }
    for (int y = 0; y < LINES; ++y)
    {
        mvprintw(y, 0, "|");
        mvprintw(y, COLS - 1, "|");
    }
    refresh();
}

void create_a_new_account(Game *g){
    FILE *fptr;
    get_username(g , fptr);
    get_user_password(g , fptr);
    get_user_email(g , fptr);
}
void get_username(Game *g,FILE *fptr){
    clear();
    draw_menu_border();
    echo();
    mvprintw(LINES / 2 - 2, COLS / 2 - 15, "Enter your name: ");
    getnstr(g->name, 50);
    noecho();
    while(check_username(g->name)){
        clear();
        draw_menu_border();
        echo();
        mvprintw(LINES / 2 - 1, COLS / 2 - 15, "This username has been used.");
        mvprintw(LINES / 2 - 2, COLS / 2 - 15, "Enter your name: ");
        getnstr(g->name, 50);
        noecho();
    }
    mkdir(g->name , 0755);
    char filepath[512];
    snprintf(filepath, sizeof(filepath), "%s/%s", g->name, g->name);
    fptr = fopen(filepath ,"w");
    fclose(fptr);
    snprintf(filepath, sizeof(filepath), "%s/%s", g->name, "scores");
    fptr = fopen(filepath ,"w");
    fprintf(fptr , "%d" , 0);
    fclose(fptr);
    snprintf(filepath, sizeof(filepath), "%s/%s", g->name, "started");
    fptr = fopen(filepath ,"w");
    fprintf(fptr , "%d" , 0);
    fclose(fptr);
    snprintf(filepath, sizeof(filepath), "%s/%s", g->name, "wons");
    fptr = fopen(filepath ,"w");
    fprintf(fptr , "%d" , 0);
    fclose(fptr);
    noecho();
}
bool check_username(char * name){
    FILE *ifp;
    ifp = fopen("username.txt","r");
    char * usernames;
    while (fscanf(ifp, "%s", usernames) == 1)    
        if (strcmp (usernames , name) == 0)                       
            return true; 
    ifp = fopen("username.txt","a");
    fprintf(ifp,"%s", name);
    fprintf(ifp, "\n");
    fclose(ifp); 
    return false;     
}
void get_user_password(Game *g,FILE *fptr){
    clear();
    draw_menu_border();
    echo();
    mvprintw(LINES / 2, COLS / 2 - 15, "Enter R for random Generation ");
    mvprintw(LINES / 2 - 2, COLS / 2 - 15, "Enter your password: ");
    getnstr(g->password, 50);
    noecho();
    while(check_user_password(g->password)){
        clear();
        draw_menu_border();
        echo();
        mvprintw(LINES / 2 - 1, COLS / 2 - 15, "Password is too weak.");
        mvprintw(LINES / 2, COLS / 2 - 15, "Enter R for random Generation ");
        mvprintw(LINES / 2 - 2, COLS / 2 - 15, "Enter your password: ");
        getnstr(g->password, 50);
        noecho();
    }
    char filepath[512];
    snprintf(filepath, sizeof(filepath), "%s/%s", g->name, g->name);
    fptr = fopen(filepath ,"w");
    fprintf(fptr,"%s", g->password);
    fprintf(fptr, "\n");
    fclose(fptr);
    noecho();
}
bool check_user_password(char * password){
    if (strcmp(password , "R") == 0){
        random_password_generator(password);
    }
    if(strlen(password) >= 7){
    for(int i = 0 ; i < strlen(password) ; i++){
    if (password[i] >= 'A' && password[i] <= 'Z'){
    for(int i = 0 ; i < strlen(password) ; i++){
    if (password[i] >= '0' && password[i] <= '9'){
    for(int i = 0 ; i < strlen(password) ; i++){
    if (password[i] >= 'a' && password[i] <= 'z'){
    return false;
    }}}}}}}
    return true;
}
void random_password_generator(char * password){
    char pass[30];
    pass[0] = (char)(rand()%24) + 65;
    pass[1] = (char)(rand()%22) + 34;
    pass[2] = (char)(rand()%22) + 34;
    pass[3] = (char)(rand()%11) + 47;
    pass[4] = (char)(rand()%24) + 97;
    pass[5] = (char)(rand()%24) + 65;
    pass[6] = (char)(rand()%24) + 97;
    pass[7] = (char)(rand()%11) + 34;
    pass[8] = (char)(rand()%24) + 97;
    pass[9] = (char)(rand()%24) + 65;
    pass[10] = (char)(rand()%24) + 97;
    pass[11] = (char)(rand()%22) + 33;
    pass[12] = (char)(rand()%11) + 46;
    strcat(password ,pass);
    mvprintw(LINES / 2 + 1, COLS / 2 - 15, "Your password is: %s" , password);
    getch();    
}
void get_user_email(Game *g,FILE *fptr){
    clear();
    draw_menu_border();
    echo();
    mvprintw(LINES / 2 - 2, COLS / 2 - 15, "Enter your email: ");
    getnstr(g->email, 50);
    noecho();
    while(check_user_email(g->email)){
        clear();
        draw_menu_border();
        echo();
        mvprintw(LINES / 2 - 1, COLS / 2 - 15, "Email is not correct.");
        mvprintw(LINES / 2 - 2, COLS / 2 - 15, "Enter your email: ");
        getnstr(g->email, 50);
        noecho();
    }
    char filepath[512];
    snprintf(filepath, sizeof(filepath), "%s/%s", g->name, g->name);
    fptr = fopen(filepath ,"a");
    fprintf(fptr,"%s", g->email);
    fprintf(fptr, "\n");
    fclose(fptr);
    noecho();
}
bool check_user_email(char * email){
    for(int i = 0 ; i < strlen(email) ; i++){
        if ((email[i] >= 'A' && email[i] <= 'Z')||
        (email[i] >= 'a' && email[i] <= 'z')||
        (email[i] >= '0' && email[i] <= '9')){
        if (email [i + 1] == '@'){
        for(int j = i + 1 ; j < strlen(email) ; j++){
        if ((email[j] >= 'A' && email[j] <= 'Z')||
        (email[j] >= 'a' && email[j] <= 'z')||
        (email[j] >= '0' && email[j] <= '9')){
        if (email [j + 1] == '.'){
        int counter = j + 1;
        for(int k = j + 1 ; k < strlen(email) ; k++){
        if ((email[k] >= 'A' && email[k] <= 'Z')||
        (email[k] >= 'a' && email[k] <= 'z')){
        counter++;
        if (counter == strlen(email) - 1){
        return false;
        }}}}}}}}
    }
    return true;   
}
void log_into_account(Game *g){
    FILE *fptr;
    get_username_from_loggin(g , fptr);
    get_user_password_from_loggin(g , fptr);
}
void get_username_from_loggin(Game *g,FILE *fptr){
    clear();
    draw_menu_border();
    echo();
    mvprintw(LINES / 2 - 2, COLS / 2 - 15, "Enter your name: ");
    getnstr(g->name, 50);
    noecho();
    while(check_username_from_loggin(g->name)){
        clear();
        draw_menu_border();
        echo();
        mvprintw(LINES / 2 - 1, COLS / 2 - 15, "Username not found.");
        mvprintw(LINES / 2 - 2, COLS / 2 - 15, "Enter your name: ");
        getnstr(g->name, 50);
        noecho();
    }
    noecho();
}
bool check_username_from_loggin(char * name){
    FILE *ifp;
    ifp = fopen("username.txt","r");
    char usernames[50];
    while (fscanf(ifp, "%s", usernames) == 1)    
        if (strcmp (usernames , name) == 0){                       
            return false; 
        }
    fclose(ifp); 
    return true;     
}
void get_user_password_from_loggin(Game *g,FILE *fptr){
    clear();
    draw_menu_border();
    echo();
    mvprintw(LINES / 2 - 2, COLS / 2 - 15, "Enter your Password: ");
    getnstr(g->password, 50);
    noecho();
    while(check_user_password_from_loggin(g ,g->password)){
        clear();
        draw_menu_border();
        echo();
        mvprintw(LINES / 2 - 1, COLS / 2 - 15, "Password is not correct.");
        mvprintw(LINES / 2 , COLS / 2 - 15, "Enter F if you Forgot the Password");        
        mvprintw(LINES / 2 - 2, COLS / 2 - 15, "Enter your Password: ");

        getnstr(g->password, 50);
        noecho();
    }
    noecho();
}
bool check_user_password_from_loggin(Game *g, char * password){
    if (strcmp(password , "F")==0){
        echo();
    mvprintw(LINES / 2  , COLS / 2 - 15, "                                           ");
    mvprintw(LINES / 2 - 1, COLS / 2 - 15, "                                        ");
    mvprintw(LINES / 2 - 2, COLS / 2 + 2, "                                        ");
    mvprintw(LINES / 2 - 2, COLS / 2 - 15, "Enter your Email: ");  
    getnstr(password, 50); 
    noecho();     
    }
    FILE *ifp;
    char filepath[1024];
    snprintf(filepath, sizeof(filepath), "%s/%s", g->name, g->name);
    ifp = fopen(filepath,"r");
    char passwords[50];
    while (fscanf(ifp, "%s", passwords) == 1)    
        if (strcmp (passwords , password) == 0){
            FILE *v;
            v = fopen(filepath,"r");
            fscanf(v, "%s", g->password);
            fscanf(v, "%s", g->email);
            return false;
        }
    return true;
}



int menu_page(Game *g){
    pthread_t menumusicThread;
    menu = 1;
    if(menu_playing == 0){
        pthread_create(&menumusicThread, NULL, menuplayMusic, NULL);    
        menu_playing = 1; 
    }
    draw_menu_border();

    const char *levels[] = {"Start a new game", "Resume", "Profile" , "Score board" , "Options" , "Exit"};
    int choice = 0;
    back_ground_sky();
    for (int j = 3 ; j <= 14 ; j++){
        for (int i = 12 ; i <= 48 ; i++){
            mvprintw( j , i , " ");
        }
    }
    while (1){
        draw_menu_border();
        mvprintw(1, 1, " ");
        for (int i = 0; i <= 5; ++i)
        {
            if (i == choice)
                attron(A_REVERSE);
            mvprintw(7 + i, 18, "%s", levels[i]);
            if (i == choice)
                attroff(A_REVERSE);
        }

        int ch = getch();
        if (ch == KEY_UP)
            choice = (choice == 0) ? 5 : choice - 1;
        else if (ch == KEY_DOWN)
            choice = (choice == 5) ? 0 : choice + 1;
        else if (ch == 10)
            break;
    }

    switch (choice){
    case 0:
        g->games_started += 1;
        in_game = 0;
        menu = 0;
        menu_playing = 0;
         pthread_join(menumusicThread, NULL);        
        clear();
        break;
    case 1:
        clear();
        // Resume_old_game(g);
        break;
    case 2:
        clear();
        show_user_profile(g);
        break;
    case 3:
        clear();
        show_score_board();
        break;
    case 4:
        clear();
        show_options_menu(g);
        break;
    case 5:
        add_to_score_board();
        set_final_values();
        menu = 0;
        pthread_join(menumusicThread, NULL); 
        Mix_CloseAudio();
        SDL_Quit();
        mvprintw(LINES / 2 - 8, COLS / 2 - 20, "                                   ");
        mvprintw(LINES / 2 - 7, COLS / 2 - 20, "            Are you Sure?          ");
        mvprintw(LINES / 2 - 6, COLS / 2 - 20, "                                   ");
        int ch = getch();
        endwin();
        exit(0);
        break;
    }

    return choice;
    clear();
}


void draw_rooms(){

    for (int j = 2 ; j < 32 ; j++){
        for (int i = 2 ; i < 184 ; i++){
            if(map[l][j][i].flags){
                if(map[l][j][i].signs == 'a'){
                    attron(COLOR_PAIR(1));
                    mvprintw( j , i , "%s" , apple);
                    attroff(COLOR_PAIR(1));
                    refresh();
                }
                else if(map[l][j][i].signs == 'G'){
                    attron(COLOR_PAIR(3));
                    mvprintw( j , i , "%s" , coin);
                    attroff(COLOR_PAIR(3));
                    refresh();
                }
                else if(map[l][j][i].signs == 'D'){
                    attron(COLOR_PAIR(8));
                    mvprintw( j , i , "%s" , blackcoin);
                    attroff(COLOR_PAIR(8));
                    refresh();
                }
                else if(map[l][j][i].layer == 30 && map[l][j][i].signs == 'U'){
                    attron(COLOR_PAIR(1));
                    mvprintw( j , i , "%s" , "☌");
                    attroff(COLOR_PAIR(1));  
                } 
                else if(map[l][j][i].layer == 31 && map[l][j][i].signs == 'U'){
                    attron(COLOR_PAIR(5));
                    mvprintw( j , i , "%s" , potionn);
                    attroff(COLOR_PAIR(5));  
                } 
                else if(map[l][j][i].layer == 32 && map[l][j][i].signs == 'U'){
                    attron(COLOR_PAIR(2));
                    mvprintw( j , i , "%s" , "⛦");
                    attroff(COLOR_PAIR(2));  
                }  
                else if(map[l][j][i].signs == 'm'){
                    attron(COLOR_PAIR(1));
                    mvprintw( j , i , "%s" , "♢");
                    attroff(COLOR_PAIR(1)); 
                    refresh(); 
                } 
                else if(map[l][j][i].signs == 'J'){
                    attron(COLOR_PAIR(8));
                    mvprintw( j , i , "%s" , daggerr);
                    attroff(COLOR_PAIR(8)); 
                    refresh(); 
                }
                else if(map[l][j][i].signs == 'B'){
                    attron(COLOR_PAIR(8));
                    mvprintw( j , i , "%s" , swordd);
                    attroff(COLOR_PAIR(8)); 
                    refresh(); 
                }  
                else if(map[l][j][i].signs == 'A'){
                    attron(COLOR_PAIR(8));
                    mvprintw( j , i , "%s" , arroww);
                    attroff(COLOR_PAIR(8)); 
                    refresh(); 
                } 
                else if(map[l][j][i].signs == 'L'){
                    attron(COLOR_PAIR(8));
                    mvprintw( j , i , "%s" , wandd);
                    attroff(COLOR_PAIR(8)); 
                    refresh(); 
                }   
                else if(l == 3 && j>=y_1[l][3] && j <= y2[l][3] && i>=x1[l][3]
                 && i<=x2[l][3] && map[l][j][i].signs != '+'){
                    attron(COLOR_PAIR(3));
                    mvprintw( j , i , "%c" , map[l][j][i].signs);
                    attroff(COLOR_PAIR(3)); 
                    refresh();                     
                }   
                else{
                    mvprintw( j , i , "%c" , map[l][j][i].signs);
                }             
            }
        }
    }
    for (int i = 1 ; i < 183 ; i ++){
        mvprintw(34 , i , "_");
    }
    for (int j = 35 ; j < 44 ; j ++){
        mvprintw( j, 92 , "|");
    }
    mvprintw(34 , 2 , "Massages");

}


void show_user_profile(Game *g){
    draw_menu_border();
    back_ground_sky();
    for (int j = 3 ; j <= 14 ; j++){
        for (int i = 12 ; i <= 48 ; i++){
            mvprintw( j , i , " ");
        }
    }
    mvprintw(7 , 18, "Name : %s", g->name);
    mvprintw(8 , 18, "Password : %s", g->password);
    mvprintw(9 , 18, "Eamil : %s", g->email);
    mvprintw(10 , 18, "High score : %d", g->highest);
    mvprintw(11 , 18, "Games played : %d", g->games_started);
    mvprintw(12 , 18, "Games won : %d", g->games_finished);
    getch();
}

void show_score_board(){
    draw_menu_border();
    int page = 0;
    int ceiling;
    float temp;
    int show_leader_board = 1;
    Leaderboard lead[100];
    FILE *ifp;
    int counter = 0;
    ifp = fopen("leaderboard.txt","r");
    while (fscanf(ifp, "%s", lead[counter].name) == 1){
        fscanf(ifp , "%d %d %d" , &lead[counter].score , &lead[counter].games_started , &lead[counter].games_finished);
        counter++;
    }
    for (int o = 0 ; o < counter ; o++){
        for (int oo = 0 ; oo < counter ; oo++){
            if (lead[oo].score < lead[oo + 1].score){
                Leaderboard temp = lead[oo];
                lead[oo] = lead[oo + 1];
                lead[oo + 1] = temp;  
            }
        }
    }
    ifp = fopen("leaderboard.txt","w");
    for (int o = 0 ; o < counter ; o++){
        fprintf(ifp ,"%s %d %d %d\n",lead[o].name ,lead[o].score , lead[o].games_started , lead[o].games_finished);
    }    
    back_ground_sky();
    for (int j = 3 ; j <= 10 + 5 ; j++){
        for (int i = 7 ; i <= 70 ; i++){
            mvprintw( j , i , " ");
        }
    }
    fclose(ifp);

    temp = counter / 7;
    ceiling = counter / 7;
    if (temp > ceiling) ceiling += 1;

    while(show_leader_board){
        draw_menu_border();
        mvprintw(6 , 12 ,"Rank    |    Name  Score   Gold   Started   Won");
        if (page == 0 ){
            for (int o = 0 ; o < 7 ; o++){
                if (strcmp(g.name , lead[o].name)==0){
                    attron(A_REVERSE);
                }
                if(o == 0){
                    attron(COLOR_PAIR(3)| A_BOLD);
                    mvprintw(7+o , 18 ,"%2d|%8s  %3d    %4d      %2d      %2d" ,o + 1,lead[o].name ,lead[o].score 
                    , 6 * lead[o].score , lead[o].games_started , lead[o].games_finished);
                    attroff(A_BOLD);
                    attroff(A_REVERSE);
                }
                if(o == 1){
                    attron(COLOR_PAIR(5));
                    mvprintw(7+o , 18 ,"%2d|%8s  %3d    %4d      %2d      %2d" ,o + 1,lead[o].name ,lead[o].score 
                    , 6 * lead[o].score , lead[o].games_started , lead[o].games_finished);
                    attroff(A_REVERSE);
                }
                if(o == 2){
                    attron(COLOR_PAIR(2));
                    mvprintw(7+o , 18 ,"%2d|%8s  %3d    %4d      %2d      %2d" ,o + 1,lead[o].name ,lead[o].score 
                    , 6 * lead[o].score , lead[o].games_started , lead[o].games_finished);
                    attroff(COLOR_PAIR(2));
                    attroff(COLOR_PAIR(3));
                    attroff(COLOR_PAIR(5));
                attroff(A_REVERSE);
                }
                if(o != 0 && o != 1 && o != 2) { mvprintw(7+o , 18 ,"%2d|%8s  %3d    %4d      %2d      %2d" ,o + 1,lead[o].name ,lead[o].score 
                , 6 * lead[o].score , lead[o].games_started , lead[o].games_finished); attroff(A_REVERSE);}
            }
            mvprintw(7 , 10 , "Chmp");
            mvprintw(8 , 10 , "Goat");
            mvprintw(9 , 10 , "Best");
            move(7,15);refresh();
            wprintf(L"%ls\n", L"\x01F3C6");
            refresh();move(8,13);refresh();
            wprintf(L"%ls\n", L"\x01F451");
            refresh();move(9,11);refresh();
            wprintf(L"%ls\n", L"\x01F3C5");
            refresh();
            attroff(COLOR_PAIR(2));
            attroff(COLOR_PAIR(3));
            attroff(COLOR_PAIR(5));
        }
        else{
            refresh();
            for (int j = 7 ; j <= 15; j++){
                for (int i = 7 ; i <= 70 ; i++){
                    mvprintw( j , i , " ");
                }
            }
            if(page * 7 + 6 >= counter){
                for (int o = page* 7 ; o <= counter ; o++){
                    if (strcmp(g.name , lead[o].name)==0){
                        attron(A_REVERSE);
                    }
                    mvprintw(7+o-(page*7) , 18 ,"%2d|%8s  %3d    %4d      %2d      %2d" ,o + 1,lead[o].name ,lead[o].score 
                            , 6 * lead[o].score , lead[o].games_started , lead[o].games_finished);  
                            attroff(A_REVERSE);          
                }
            }
            else{
                for (int o = page* 7 ; o <= page * 7 + 6 ; o++){
                    if (strcmp(g.name , lead[o].name)==0){
                        attron(A_REVERSE);
                    }
                    mvprintw(7+o-(page*7) , 18 ,"%2d|%8s  %3d    %4d      %2d      %2d" ,o + 1,lead[o].name ,lead[o].score 
                            , 6 * lead[o].score , lead[o].games_started , lead[o].games_finished);  
                            attroff(A_REVERSE);          
                }                
            }
        }
        int ch = getch();
        if (ch == KEY_UP){
            if(page != 0)
                page -= 1;
        }
        else if (ch == KEY_DOWN){
            if(page != ceiling)
                page += 1;
        }
        else if (ch == 10){
            break;
        }
    }
    clear();
}
void add_to_score_board(){
    if(strcmp(g.name , "Guest") == 0){
        return;
    }
    else{
        Leaderboard lead[100];
        FILE *ifp;
        int flag = 0;
        int counter = 0;
        ifp = fopen("leaderboard.txt","r");
        while (fscanf(ifp, "%s", lead[counter].name) == 1){
            fscanf(ifp , "%d %d %d" , &lead[counter].score , &lead[counter].games_started , &lead[counter].games_finished);
            counter++;
        }  
        for(int o = 0 ; o <= counter ; o++){
            if(o == counter){
                flag+=1;
                strcpy(lead[o].name , g.name);
                lead[o].score = g.highest;
                lead[o].games_started = g.games_started;
                lead[o].games_finished = g.games_finished;
                break;  
            }
            if(strcmp(lead[o].name , g.name)==0){
                lead[o].score = g.highest;
                lead[o].games_started = g.games_started;
                lead[o].games_finished = g.games_finished;
                break;
            }
        }
        ifp = fopen("leaderboard.txt","w");
        for (int o = 0 ; o < counter + flag; o++){
            fprintf(ifp ,"%s %d %d %d\n",lead[o].name ,lead[o].score , lead[o].games_started , lead[o].games_finished);
        }   
    }
}
void show_options_menu(Game* g){
    draw_menu_border();
    const char *levels[] = {"Difficulty", "Color", "Music"};
    int choice = 0;
    back_ground_sky();
    for (int j = 3 ; j <= 14 ; j++){
        for (int i = 14 ; i <= 48 ; i++){
            mvprintw( j , i , " ");
        }
    }
    while (1){
        draw_menu_border();
        mvprintw(1, 1, " ");
        for (int i = 0; i < 3; ++i){
            if (i == choice)
                attron(A_REVERSE);
            mvprintw(7 + i, 18, "%s", levels[i]);
            if (i == choice)
                attroff(A_REVERSE);
        }

        int ch = getch();
        if (ch == KEY_UP)
            choice = (choice == 0) ? 2 : choice - 1;
        else if (ch == KEY_DOWN)
            choice = (choice == 2) ? 0 : choice + 1;
        else if (ch == 10)
            break;
    }

    switch (choice){
    case 0:
        clear();
        Set_game_difficulty();
        break;
    case 1:
        clear();
        g->color = Set_player_color() + 1;
        break;
    case 2:
        clear();
        Set_game_music();
        break;
    clear();
    }
}
void Set_game_music(){
    draw_menu_border();
    const char *levels[] = {"Master Valume","Water Fall", "Snowy","Another Medium", "Core" };
    int choice = 0;
    back_ground_sky();
    for (int j = 3 ; j <= 14 ; j++){
        for (int i = 14 ; i <= 48 ; i++){
            mvprintw( j , i , " ");
        }
    }

    while (1){
        draw_menu_border();
        mvprintw(1, 1, " ");
        for (int i = 0; i < 5; ++i){
            if (i == choice)
                attron(A_REVERSE);
              if(i == 0&&dimmaster==1) {attron(A_DIM);mvprintw(7 + i, 18, "%s", levels[i]);attroff(A_DIM);}
            else if(i == 1&&dim1 == 1) {attron(A_DIM);mvprintw(7 + i, 20, "%s", levels[i]);attroff(A_DIM);}
            else if(i == 2&&dim2 == 1) {attron(A_DIM);mvprintw(7 + i, 20, "%s", levels[i]);attroff(A_DIM);}
            else if(i == 3&&dim3 == 1) {attron(A_DIM);mvprintw(7 + i, 20, "%s", levels[i]);attroff(A_DIM);}
            else if(i == 4&&dim4 == 1) {attron(A_DIM);mvprintw(7 + i, 20, "%s", levels[i]);attroff(A_DIM);}
            else if(i==0) mvprintw(7 + i, 18, "%s", levels[i]);
            else mvprintw(7 + i, 20, "%s", levels[i]);
            if (i == choice)
                attroff(A_REVERSE);
        }

        int ch = getch();
        if (ch == KEY_UP)
            choice = (choice == 0) ? 4 : choice - 1;
        else if (ch == KEY_DOWN)
            choice = (choice == 4) ? 0 : choice + 1;
        else if (ch == 'q'){
            clear();
            break;
        }
        else if (ch == 10){

            switch (choice){
            case 0:
                if (dimmaster == 0){First_floor = 0;
                dim1=1;dim2=1;dim3=1;dim4=1;dimmaster=1;
                Second_floor=0;Third_floor=0;Forth_floor=0;}
                else {First_floor = 1; dim1= 0;dim2=0;dim3=0;dim4=0;
                Second_floor=1;Third_floor=1;Forth_floor=1;dimmaster=0;}
                break;               
            case 1:
                if (dim1 == 0){First_floor = 0; dim1 = 1;}
                else {First_floor = 1; dim1 = 0;}
                break;
            case 2:
                if (dim2 == 0) {Second_floor = 0 ;dim2 = 1;}
                else {Second_floor = 0 ;dim2 = 0;}
                break;
            case 3:
                if (dim3 == 0){Third_floor = 0; dim3 = 1;}
                else {Third_floor = 0;dim3 = 0;}
                break;
            case 4:
                if (dim4 == 0){Forth_floor = 0 ;dim4 = 1;}
                else {Forth_floor = 0 ;dim4 = 0;}
                break;
            }        
        }   
    }   
}
void Set_game_difficulty(){
    draw_menu_border();
    const char *levels[] = {"Expert", "Hard" ,"Medium", "Easy" , "Peaceful"};
    int choice = 0;
    back_ground_sky();
    for (int j = 3 ; j <= 14 ; j++){
        for (int i = 14 ; i <= 48 ; i++){
            mvprintw( j , i , " ");
        }
    }
    while (1){
        draw_menu_border();
        mvprintw(1, 1, " ");
        for (int i = 0; i < 5; ++i){
            if (i == choice)
                attron(A_REVERSE);
            mvprintw(7 + i, 18, "%s", levels[i]);
            if (i == choice)
                attroff(A_REVERSE);
        }

        int ch = getch();
        if (ch == KEY_UP)
            choice = (choice == 0) ? 4 : choice - 1;
        else if (ch == KEY_DOWN)
            choice = (choice == 4) ? 0 : choice + 1;
        else if (ch == 10)
            break;
    }

    switch (choice){
    case 0:
        difficulty = 6;
        break;
    case 1:
        difficulty = 4;
        break;
    case 2:
        difficulty = 2;
        break;
    case 3:
        difficulty = 0;
        break;
    case 4:
        difficulty = -2000000000;
        break;
    default:
        clear();
    }
}

void Set_colors(){
    init_pair(0, COLOR_WHITE, COLOR_BLACK);
        init_color(1, 0, 1000, 0);/*Green*/
        init_color(2, 1000, 0, 0);/*Red*/
        init_color(3, 1000, 800, 0);/*Gold*/
        init_color(4, 1000, 412, 705);/*Pink*/
        init_color(5, 341, 420, 940);/*Light Blue*/
        init_color(6, 624, 1000, 643);/*Light Green*/
        init_color(8, 811, 616, 805);/*light purple*/
        // init_color(6, 1000, 412, 705);
    init_pair(1, 1, COLOR_BLACK);
    init_pair(2, 2, COLOR_BLACK);
    init_pair(3, 3, COLOR_BLACK);
    init_pair(4, 4, COLOR_BLACK);
    init_pair(5, 5, COLOR_BLACK);
    init_pair(6, 6, COLOR_BLACK);
    init_pair(7, 7, COLOR_BLACK);
    init_pair(8, 8, COLOR_BLACK);

    init_color(7, 1000, 1000, 1000);/*Text Color*/
}
int Set_player_color(){
    draw_menu_border();

    const char *levels[] = {"Green - @", "Red - @", "Gold - @" , "Pink - @", "Cyan - @" };
    int choice = 0;

    while (1){
        draw_menu_border();
        back_ground_sky();
        for (int j = 3 ; j <= 14 ; j++){
            for (int i = 14 ; i <= 48 ; i++){
                mvprintw( j , i , " ");
            }
        }
        mvprintw(1, 1, " ");
        for (int i = 0; i < 5; ++i){
            if (i == choice) attron(A_REVERSE);
            attron(COLOR_PAIR(i + 1));
            mvprintw(7 + i, 18, "%s", levels[i]);
            attroff(COLOR_PAIR(i + 1));
            if (i == choice) attroff(A_REVERSE);
        }

        int ch = getch();
        if (ch == KEY_UP)
            choice = (choice == 0) ? 4 : choice - 1;
        else if (ch == KEY_DOWN)
            choice = (choice == 4) ? 0 : choice + 1;
        else if (ch == 10)
            break;
    }

    return choice;
}


void draw_player(Game g){
    attron(COLOR_PAIR(g.color));
    mvprintw(g.player.y, g.player.x, "@");
    attroff(COLOR_PAIR(g.color));
}

void draw_hungerbar(){
    if (healing_speed == 2){
        if (healing%4 == 0){
            damage -= 1;
            if (damage < 0) damage = 0;
            output_massages(20);
        }
        healing += 1;
        if(healing == 10){
            elapsed += 1;
            hunger = 0;
            healing = 0;
            healing_speed = 0;
        }
    }
    if (elapsed == 0){
        if (healing%4 == 0){
            damage -= 1;
            if (damage < 0) damage = 0;
            output_massages(8);
        }
    } 
    hunger++;
    if (elapsed == 0){
        healing += 1;
        if(healing == 15){
            elapsed += 1;
            hunger = 0;
            healing = 0;
        }
    }
    else if (elapsed == 100){
        if (hunger == 1){
            hunger = 0;
            damage++;
        }
    }
    else{
        if(g.speed == 1){
            if (hunger == 12 - difficulty){
                hunger = 0;
                elapsed++;
            }
        }
        else{
            if (hunger == 9 - difficulty/2){
                hunger = 0;
                elapsed++;
            }        
        }
    }
    int remaining = 100 - elapsed;
    if(remaining <= 0){
        elapsed = 100;
    }
    float fraction = (float)remaining / 100;
    int width = 50 * fraction;

    if(remaining < 90){
        if(hunger == 5){
            damage++;
        }
    }
    attron(COLOR_PAIR(1));
    for (int i = 1; i <= width; ++i)
        mvprintw(35, i + 104, "#");
    attroff(COLOR_PAIR(1));

    attron(COLOR_PAIR(2));
    for (int i = width + 1; i <= 50; ++i)
        mvprintw(35, i + 104, ".");
    attroff(COLOR_PAIR(2));

    mvprintw(35, 93, "Hunger: %d ", remaining);
    mvprintw(35, 104, "[");
    mvprintw(35, 155, "]");
}
void draw_heathbar(){
    int remaining = 100 - damage;
    float fraction = (float)remaining / 100;
    int width = 50 * fraction;

    attron(COLOR_PAIR(1));
    for (int i = 1; i <= width; ++i)
        mvprintw(36, i + 104, "#");
    attroff(COLOR_PAIR(1));

    attron(COLOR_PAIR(2));
    for (int i = width + 1; i <= 50; ++i)
        mvprintw(36, i + 104, ".");
    attroff(COLOR_PAIR(2));

    mvprintw(36, 93, "Health: %d ", remaining);
    mvprintw(36, 104, "[");
    mvprintw(36, 155, "]");
}

void draw_food_count(){
    mvprintw(37, 93, "Gold: %d ", g.gold);
    mvprintw(38, 93, "Magical Food: %d            ", g.magic_food); 
    mvprintw(39, 93, "Quality Food: %d            ", g.quality_food);
    mvprintw(40, 93, "Normal Food: %d             ", g.normal_food);
    mvprintw(41, 93, "Roten Flesh: %d             ", g.rotenflesh);
    mvprintw(42, 93, "Melee Weapon: %s            ", g.melee_weapon_name);  
    mvprintw(43, 93, "Range Weapon: %s            ", g.range_weapon_name);
}
void weapon_menu(){
    const char *levels[] = {"Magic Wand","Arrows","Dagger","Mace","sword" , "Resume"};
    int choice = 0;
    while (1){
        draw_menu_border();
        mvprintw(1, 1, " ");
            mvprintw(35, 158, "Range");
            mvprintw(39, 158, "Melee");
        for (int i = 0; i < 6; ++i){
            if (i == choice)
                attron(A_REVERSE);
            if(i == 0) mvprintw(36 + i, 160, "%d %s", g.magic_wand , levels[i] );
            if(i == 1) mvprintw(36 + i, 160, "%d %s", g.arrows , levels[i] );
            if(i == 2) mvprintw(36 + i, 160, "%d %s", g.dagger , levels[i] );
            if(i == 3) mvprintw(37 + i, 160, "%d %s", g.mace,levels[i] );    
            if(i == 4) mvprintw(37 + i, 160, "%d %s" , g.sword,levels[i] );     
            if(i == 5) mvprintw(38 + i, 158, "%s" , levels[i] );      
            if (i == choice)
                attroff(A_REVERSE);
        }
        int ch = getch();
        if (ch == KEY_UP)
            choice = (choice == 0) ? 5 : choice - 1;
        else if (ch == KEY_DOWN)
            choice = (choice == 5) ? 0 : choice + 1;
        else if (ch == 10)
            break;
    }
    for (int j = 35 ; j <= 45 ; j++){
        for (int i = 158 ; i <= 182 ; i++){
            mvprintw( j , i , " ");
        }
    }

    switch (choice){
    case 0:
        g.range_weapon = 1;
        strcpy(g.range_weapon_name , levels[choice]);
        break;
    case 1:
        g.range_weapon = 2;
        strcpy(g.range_weapon_name , levels[choice]);
        break;
    case 2:
        g.range_weapon = 3;
        strcpy(g.range_weapon_name , levels[choice]);

//! ************anva potion ha be nesbat faz 2 kamel shavad*************************************** */
        break;
    case 3:
        g.melee_weapon = 1;
        strcpy(g.melee_weapon_name , levels[choice]);

        break;
    case 4:
        g.melee_weapon = 2;
        strcpy(g.melee_weapon_name , levels[choice]);

        break;
    default:
        clear();
    }    
    refresh();
}

void magic_menu(){

    const char *levels[] = {"Heath", "Speed" ,"Damage", "Roten Flesh" , "Resume"};
    int choice = 0;
    while (1){
        draw_menu_border();
        mvprintw(1, 1, " ");
        for (int i = 0; i < 5; ++i){
            if (i == choice)
                attron(A_REVERSE);
            if(i == 0) mvprintw(35 + i, 158, "%d %s", g.Health_potin , levels[i] );
            if(i == 1) mvprintw(35 + i, 158, "%d %s", g.Speed_potion , levels[i] );
            if(i == 2) mvprintw(35 + i, 158, "%d %s", g.Damage_potion , levels[i] );
            if(i == 3) mvprintw(35 + i, 158, "%d %s", g.rotenflesh , levels[i] );  
            if(i == 4) mvprintw(35 + i, 158, "%s",levels[i] );          
            if (i == choice)
                attroff(A_REVERSE);
        }

        int ch = getch();
        if (ch == KEY_UP)
            choice = (choice == 0) ? 4 : choice - 1;
        else if (ch == KEY_DOWN)
            choice = (choice == 4) ? 0 : choice + 1;
        else if (ch == 10)
            break;
    }
    for (int j = 35 ; j <= 45 ; j++){
        for (int i = 158 ; i <= 182 ; i++){
            mvprintw( j , i , " ");
        }
    }

    switch (choice){
    case 0:
        if(g.Health_potin > 0){
            damage -= 1;
            if (damage < 0)damage = 0;
            on_potions = 1;
            healing_speed = 2;
            output_massages(17);
            g.Health_potin -= 1;
        }
        break;
    case 1:
        if(g.Speed_potion > 0){
            g.speed = 2;
            on_potions = 1;
            output_massages(18);
            g.Speed_potion -= 1;
        }
        break;
    case 2:
        if(g.Damage_potion > 0){
//! ************anva potion ha be nesbat faz 2 kamel shavad*************************************** */

        }
        break;
    case 3:
        difficulty = 0;
        break;
    default:
        clear();
    }    

    refresh();
}
void food_menu(){
    const char *levels[] = {"Normal", "Magical" ,"Quality", "Roten Flesh" , "Resume"};
    int choice = 0;
    while (1){
        draw_menu_border();
        mvprintw(1, 1, " ");
        for (int i = 0; i < 5; ++i){
            if (i == choice)
                attron(A_REVERSE);
            if(i == 0) mvprintw(35 + i, 158, "%d %s", g.normal_food , levels[i] );
            if(i == 1) mvprintw(35 + i, 158, "%d %s", g.magic_food , levels[i] );
            if(i == 2) mvprintw(35 + i, 158, "%d %s", g.quality_food , levels[i] );
            if(i == 3) mvprintw(35 + i, 158, "%d %s", g.rotenflesh , levels[i] ); 
            if(i == 4) mvprintw(35 + i, 158, "%s",levels[i] );           
            if (i == choice)
                attroff(A_REVERSE);
        }

        int ch = getch();
        if (ch == KEY_UP)
            choice = (choice == 0) ? 4 : choice - 1;
        else if (ch == KEY_DOWN)
            choice = (choice == 4) ? 0 : choice + 1;
        else if (ch == 10)
            break;
    }
    for (int j = 35 ; j <= 45 ; j++){
        for (int i = 158 ; i <= 182 ; i++){
            mvprintw( j , i , " ");
        }
    }

    switch (choice){
    case 0:
        if(g.normal_food > 0){
            elapsed -=10;
            damage -= 3;
            if (elapsed < 0) elapsed = 0;
            if (damage < 0)damage = 0;
            output_massages(7);
            g.normal_food -= 1;
        }
        break;
    case 1:
        if(g.magic_food > 0){
            elapsed -=12;
            damage -= 5;
            if (elapsed < 0) elapsed = 0;
            if (elapsed == 0) damage -= 10;
            if (damage < 0) damage = 0;
            g.magic_food -= 1;
        }
        break;
    case 2:
        if(g.quality_food > 0){
            elapsed -=22;
            damage -= 3;
            if (elapsed < 0) elapsed = 0;
            if (elapsed == 0) damage -= 10;
            if (damage < 0) damage = 0;
            g.quality_food -= 1;
        }
        break;
    case 3:



        break;
    default:
        clear();
    }    
//! ************anva ghaza ha be nesbat faz 2 kamel shavad*************************************** */
    refresh();
}

void handle_movement(int ch, Pos *p ){

    switch (ch) {
    case KEY_UP:
        if (map[l][p->y - g.speed][p->x].signs == '<' ){
            
            l++;
            check_what_to_play(l);
            g.player.y = y_1[l][0] + 5;
            g.player.x = x1[l][0] + 5;
            clear();
            output_massages(12);
        }
        if (map[l][p->y - g.speed][p->x].flags == 3 ){
            map[l][p->y - g.speed][p->x].signs = '?'; 
            clear();
            back_ground_sky();
            draw_menu_border();
            for (int i = COLS/2 - 10 ; i <= COLS/2 + 10 ; i++){
                for (int j = LINES/2 - 5 ; j <= LINES/2 + 5 ; j++){
                    if ((i == COLS/2 - 10 || i == COLS/2 + 10)&& (j != LINES/2 - 5)){
                    mvprintw( j , i , "|");
                    }
                    else if (j == LINES/2 - 5 || j == LINES/2 + 5){
                    mvprintw( j , i , "_");
                    }
                    else{
                    mvprintw( j , i , ".");
                    }            
                }
            }
            mvprintw( LINES/2 , COLS/2 , "@"); ;
            damage = 0;
            getch();
            clear();         
        }
        if (map[l][p->y - g.speed][p->x].signs == '>' ){
            l--;
            g.player.y = y_1[l][3] + 3;
            g.player.x = x1[l][3] + 3;
            clear();
            output_massages(11);
                        check_what_to_play(l);
        }
        else if (map[l][p->y - g.speed][p->x].signs == 'm' ||
        map[l][p->y - g.speed][p->x].signs == 'a' ||
        map[l][p->y - g.speed][p->x].signs == '.' ||
         map[l][p->y - g.speed][p->x].signs == '#' ||
          map[l][p->y - g.speed][p->x].signs == '+' ||
          map[l][p->y - g.speed][p->x].signs == 'G'||
          map[l][p->y - g.speed][p->x].signs == 'U'||
          map[l][p->y - g.speed][p->x].signs == 'W'|| 
          map[l][p->y - g.speed][p->x].signs == 'L'|| 
          map[l][p->y - g.speed][p->x].signs == 'A'|| 
          map[l][p->y - g.speed][p->x].signs == 'B'|| 
          map[l][p->y - g.speed][p->x].signs == 'J'|| 
          map[l][p->y - g.speed][p->x].signs == 'D')
            p->y -= g.speed;
        break;
    case KEY_DOWN:
        if (map[l][p->y + g.speed][p->x].signs == '<' ){
            l++;
            g.player.y = y_1[l][0] + 5;
            g.player.x = x1[l][0] + 5;
            clear();
            output_massages(12);
                        check_what_to_play(l);
        }
        if (map[l][p->y + g.speed][p->x].signs == '>' ){
            l--;
            g.player.y = y_1[l][3] + 3;
            g.player.x = x1[l][3] + 3;
            clear();
            output_massages(11);
                        check_what_to_play(l);
        }
        else if (map[l][p->y + g.speed][p->x].signs == '.' ||
        map[l][p->y + g.speed][p->x].signs == 'a' ||
        map[l][p->y + g.speed][p->x].signs == 'm' ||
         map[l][p->y + g.speed][p->x].signs == '#' ||
          map[l][p->y + g.speed][p->x].signs == '+' ||
          map[l][p->y + g.speed][p->x].signs == 'G'||
          map[l][p->y + g.speed][p->x].signs == 'W'||
          map[l][p->y + g.speed][p->x].signs == 'U'||
          map[l][p->y + g.speed][p->x].signs == 'J'||
          map[l][p->y + g.speed][p->x].signs == 'B'||
          map[l][p->y + g.speed][p->x].signs == 'A'||
          map[l][p->y + g.speed][p->x].signs == 'L'||
          map[l][p->y + g.speed][p->x].signs == 'D')
            p->y += g.speed;      
        break;
    case KEY_LEFT:
        if (map[l][p->y][p->x - g.speed].signs == '<' ){
            l++;
            g.player.y = y_1[l][0] + 5;
            g.player.x = x1[l][0] + 5;
            clear();
            output_massages(12);
                        check_what_to_play(l);
        }
        if (map[l][p->y][p->x - g.speed].signs == '>' ){
            l--;
            g.player.y = y_1[l][3] + 3;
            g.player.x = x1[l][3] + 3;
            clear();
            output_massages(11);
                        check_what_to_play(l);
        }
        else if (map[l][p->y][p->x - g.speed].signs == 'm' ||
        map[l][p->y][p->x - g.speed].signs == 'a' ||
        map[l][p->y][p->x - g.speed].signs == '.' ||
         map[l][p->y][p->x - g.speed].signs == '#' ||
        map[l][p->y][p->x - g.speed].signs == '+' ||
        map[l][p->y][p->x - g.speed].signs == 'G' ||
        map[l][p->y][p->x - g.speed].signs == 'U' ||
        map[l][p->y][p->x - g.speed].signs == '?' ||
        map[l][p->y][p->x - g.speed].signs == 'W' ||
        map[l][p->y][p->x - g.speed].signs == 'J' ||
        map[l][p->y][p->x - g.speed].signs == 'B' ||
        map[l][p->y][p->x - g.speed].signs == 'L' ||
        map[l][p->y][p->x - g.speed].signs == 'A' ||
        map[l][p->y][p->x - g.speed].signs == 'D' )
            p->x -= g.speed;
        break;
    case KEY_RIGHT:
        if (map[l][p->y][p->x + g.speed].signs == '<' ){
            l++;
            if(l==1)First_floor = 0;
            if(l==2)Second_floor = 0;           
            if(l==3)Third_floor = 0;
            g.player.y = y_1[l][0] + 5;
            g.player.x = x1[l][0] + 5;
            clear();
            output_massages(12);
                        check_what_to_play(l);
        }
        if (map[l][p->y][p->x + g.speed].signs == '>' ){
            l--;
            g.player.y = y_1[l][3] + 3;
            g.player.x = x1[l][3] + 3;
            clear();
            output_massages(11);
                        check_what_to_play(l);
        }
        else if (map[l][p->y][p->x + g.speed].signs == 'a'||
        map[l][p->y][p->x + g.speed].signs == 'm' ||
        map[l][p->y][p->x + g.speed].signs == '.' ||
         map[l][p->y][p->x + g.speed].signs == '#' ||
          map[l][p->y][p->x + g.speed].signs == '+' ||
          map[l][p->y][p->x + g.speed].signs == 'G' ||
          map[l][p->y][p->x + g.speed].signs == 'D' ||
          map[l][p->y][p->x + g.speed].signs == 'U' ||  
          map[l][p->y][p->x + g.speed].signs == 'W' ||     
          map[l][p->y][p->x + g.speed].signs == 'J' ||
          map[l][p->y][p->x + g.speed].signs == 'B' ||
          map[l][p->y][p->x + g.speed].signs == 'A' ||  
          map[l][p->y][p->x + g.speed].signs == 'L' ||    
          map[l][p->y][p->x + g.speed].layer == 80||
        map[l][p->y][p->x + g.speed].signs == '?')
            p->x += g.speed;
        break;
    case '9':
        if (map[l][p->y- g.speed][p->x + g.speed].signs == '.' ||
         map[l][p->y- g.speed][p->x + g.speed].signs == '#' ||
          map[l][p->y- g.speed][p->x + g.speed].signs == '+'){
            p->x += g.speed;
            p->y -= g.speed;
        }
        break;
    case '3':
        if (map[l][p->y+ g.speed][p->x + g.speed].signs == '.' ||
         map[l][p->y+ g.speed][p->x + g.speed].signs == '#' ||
          map[l][p->y+ g.speed][p->x + g.speed].signs == '+'){
            p->x += g.speed;
            p->y += g.speed;
        }
        break;
    case '1':
        if (map[l][p->y+ g.speed][p->x - g.speed].signs == '.' ||
         map[l][p->y+ g.speed][p->x - g.speed].signs == '#' ||
          map[l][p->y+ g.speed][p->x - g.speed].signs == '+'){
            p->x -= g.speed;
            p->y += g.speed;
        }
        break;
    case '7':
        if (map[l][p->y- g.speed][p->x - g.speed].signs == '.' ||
         map[l][p->y- g.speed][p->x - g.speed].signs == '#' ||
          map[l][p->y- g.speed][p->x - g.speed].signs == '+'){
            p->x -= g.speed;
            p->y -= g.speed;
        }
        break;
    case 'v':
        if (g.speed == 1){
            g.speed = 2;
        }
        else{
            g.speed = 1;
        }
        break;
    case 'h':
        l--;
        break;
    case 'H':
        l++;
        break;
    case 'm':
        for (int j = 2 ; j < 32 ; j++){
                for (int i = 2 ; i < 184 ; i++){
                    if(map[l][j][i].signs == 'a'){
                        attron(COLOR_PAIR(1));
                        mvprintw( j , i , "%s" , apple);
                        attroff(COLOR_PAIR(1));
                        refresh();
                    }
                    else if(map[l][j][i].signs == 'G'){
                        attron(COLOR_PAIR(3));
                        mvprintw( j , i , "%s" , coin);
                        attroff(COLOR_PAIR(3));
                        refresh();
                    }
                    else if(map[l][j][i].signs == 'D'){
                        attron(COLOR_PAIR(8));
                        mvprintw( j , i , "%s" , blackcoin);
                        attroff(COLOR_PAIR(8));
                        refresh();
                    }
                    else if(map[l][j][i].layer == 30 && map[l][j][i].signs == 'U'){
                        attron(COLOR_PAIR(1));
                        mvprintw( j , i , "%s" , "☌");
                        attroff(COLOR_PAIR(1));  
                    } 
                    else if(map[l][j][i].layer == 31 && map[l][j][i].signs == 'U'){
                        attron(COLOR_PAIR(5));
                        mvprintw( j , i , "%s" , potionn);
                        attroff(COLOR_PAIR(5));  
                    } 
                    else if(map[l][j][i].layer == 32 && map[l][j][i].signs == 'U'){
                        attron(COLOR_PAIR(2));
                        mvprintw( j , i , "%s" , "⛦");
                        attroff(COLOR_PAIR(2));  
                    }  
                    else if(map[l][j][i].signs == 'm'){
                        attron(COLOR_PAIR(1));
                        mvprintw( j , i , "%s" , "♢");
                        attroff(COLOR_PAIR(1)); 
                        refresh(); 
                    } 
                    else if(map[l][j][i].signs == 'J'){
                        attron(COLOR_PAIR(8));
                        mvprintw( j , i , "%s" , daggerr);
                        attroff(COLOR_PAIR(8)); 
                        refresh(); 
                    }
                    else if(map[l][j][i].signs == 'B'){
                        attron(COLOR_PAIR(8));
                        mvprintw( j , i , "%s" , swordd);
                        attroff(COLOR_PAIR(8)); 
                        refresh(); 
                    }  
                    else if(map[l][j][i].signs == 'A'){
                        attron(COLOR_PAIR(8));
                        mvprintw( j , i , "%s" , arroww);
                        attroff(COLOR_PAIR(8)); 
                        refresh(); 
                    } 
                    else if(map[l][j][i].signs == 'L'){
                        attron(COLOR_PAIR(8));
                        mvprintw( j , i , "%s" , wandd);
                        attroff(COLOR_PAIR(8)); 
                        refresh(); 
                    }   
                    else if(l == 3 && j>=y_1[l][3] && j <= y2[l][3] && i>=x1[l][3]
                    && i<=x2[l][3] && map[l][j][i].signs != '+'){
                        attron(COLOR_PAIR(3));
                        mvprintw( j , i , "%c" , map[l][j][i].signs);
                        attroff(COLOR_PAIR(3)); 
                        refresh();                     
                    }   
                    else{
                        mvprintw( j , i , "%c" , map[l][j][i].signs);
                    }             
                }
            }
        getch();
        clear();
        break;
    case '0':
        set_final_values();
        add_to_score_board();
        mvprintw(LINES / 2 - 8, COLS / 2 - 20, "                                   ");
        mvprintw(LINES / 2 - 7, COLS / 2 - 20, "            Are you Sure?          ");
        mvprintw(LINES / 2 - 6, COLS / 2 - 20, "                                   ");
        int ch = getch();
        in_game = 1;
        break;
    case 'p':
        switch (map[l][g.player.y][g.player.x].signs){
            case '.': '#';
                break;
            case 'G':
                map[l][g.player.y][g.player.x].signs = '.';
                g.gold += 1;
                output_massages(2);
                break;
            case 'D':
                map[l][g.player.y][g.player.x].signs = '.';
                g.gold += 2;
                output_massages(3);
                break;
            case 'a':
                if(g.normal_food < 5){
                    map[l][g.player.y][g.player.x].signs = '.';
                    g.normal_food += 1;
                    output_massages(4);
                }
                else output_massages(9);
                break;
            case 'm':
                if(g.magic_food < 5){
                    map[l][g.player.y][g.player.x].signs = '.';
                    g.magic_food += 1;
                    output_massages(4);
                }
                else output_massages(10);
                break;
            case 'J':
                if(map[l][g.player.y][g.player.x].count){
                    map[l][g.player.y][g.player.x].signs = '.';
                    g.dagger += map[l][g.player.y][g.player.x].count;
                    map[l][g.player.y][g.player.x].count = 0;
                    output_massages(20);
                }
                break;
            case 'L':
                if(map[l][g.player.y][g.player.x].count){
                    map[l][g.player.y][g.player.x].signs = '.';
                    g.magic_wand += map[l][g.player.y][g.player.x].count;
                    map[l][g.player.y][g.player.x].count = 0;
                    output_massages(20);
                }
                break;
            case 'A':
                if(map[l][g.player.y][g.player.x].count){
                    map[l][g.player.y][g.player.x].signs = '.';
                    g.arrows += map[l][g.player.y][g.player.x].count;
                    map[l][g.player.y][g.player.x].count = 0;
                    output_massages(20);
                }
                break;
            case 'B':
                if(g.sword == 0){
                    if(map[l][g.player.y][g.player.x].count){
                        map[l][g.player.y][g.player.x].signs = '.';
                        g.sword += map[l][g.player.y][g.player.x].count;
                        map[l][g.player.y][g.player.x].count = 0;
                        output_massages(20);
                    }
                }
                break;
            default:
                break;
        }
        switch (map[l][g.player.y][g.player.x].layer){ 
            case 30:
                map[l][g.player.y][g.player.x].signs = '.';
                g.Health_potin += 1;  
                output_massages(14);       
                break;
            case 31:
                map[l][g.player.y][g.player.x].signs = '.';
                g.Speed_potion += 1;            
                output_massages(15);
                break;
            case 32:
                map[l][g.player.y][g.player.x].signs = '.';
                g.Damage_potion += 1;            
                output_massages(16);
                break;            
            
        }      
        break;
    case 'e':
        food_menu();
        break;
    case 'r':
        magic_menu();
        break;
    case 'q':
        weapon_menu();
        break; 
    case 'c':
/***********************************************************************************************/
        break;
    case ' ':
        int cha = getch();
        switch(cha){
            case 't':
            int range = 1;
                for(range; range <= 4 ; range++){
                    if(map[l][g.player.y-range][g.player.x].signs == '_' ||
                    map[l][g.player.y-range][g.player.x].flags == 0){
                        map[l][g.player.y-range+1][g.player.x].signs = 'J';  
                        map[l][g.player.y-range + 1][g.player.x].count += 1;     
                        break;
                    }
                    else if(map[l][g.player.y-range][g.player.x].signs == 'V'){
                        map[l][g.player.y-range][g.player.x].health -= 12;
                        if(map[l][g.player.y-range][g.player.x].health <=0){
                            map[l][g.player.y-range][g.player.x].signs = '.';
                            map[l][g.player.y-range][g.player.x].enemy = 0;
                        }
                        break;
                    }
                }
                if(range == 5) {
                    map[l][g.player.y-range + 1][g.player.x].signs = 'J';
                    map[l][g.player.y-range + 1][g.player.x].count += 1;               
                }
                // map[l][g.player.y-range][g.player.x].flags = 2;
                break;
            case KEY_DOWN:
            case KEY_LEFT:
            case KEY_RIGHT:    
        }
        break;      
    default:
        break;
    }

    if(map[l][g.player.y][g.player.x].signs == '+' && map[l][g.player.y][g.player.x].flags == 1){
        output_massages(1);
    }

    if(map[l][g.player.y][g.player.x].layer == 1){
        output_massages(6);
        damage += 7;
        map[l][g.player.y][g.player.x].signs ='^'; 
        map[l][g.player.y][g.player.x].layer = 0; 
    }

    if(map[l][g.player.y][g.player.x].layer == 80){
        output_massages(13);
        map[l][g.player.y][g.player.x].signs ='?'; 
        map[l][g.player.y][g.player.x].layer = 0; 
    }
}
void clear_massages(){
        timer_1++;
    if (timer_1 == 17){
        timer_1 = 0;
        output_massages(0);
    }
}

void output_massages(int massage){
    switch(massage){
        case 0:
            mvprintw(36 , 2 , "                                        ");
            mvprintw(37 , 2 , "                                        ");
            mvprintw(38 , 2 , "                                        ");
            break;
        case 1:
            output_massages(0);
            mvprintw(36 , 2 , "You've Entered a new Room               ");
            timer_1 = 0;
            break;
        case 2:
            output_massages(0);
            mvprintw(36 , 2 , "You've Picked up Gold                   ");
            mvprintw(37 , 2 , "Total Score: %d                        " , g.gold * 6);
            timer_1 = 0;
            break;
        case 3:
            output_massages(0);
            mvprintw(36 , 2 , "You've Picked up Dark Gold              ");
            mvprintw(37 , 2 , "Total Score: %d                        " , g.gold * 6);
            timer_1 = 0;
            break;
        case 4:
            output_massages(0);
            mvprintw(36 , 2 , "You've Picked up Normal Food            ");
            mvprintw(37 , 2 , "Total Normal Food: %d                  " , g.normal_food);
            timer_1 = 0;
            break;
        case 5:
            output_massages(0);
            mvprintw(36 , 2 , "You've Picked up Magical Food           ");
            mvprintw(37 , 2 , "Total Magical Food: %d                 " , g.magic_food * 6);
            timer_1 = 0;
            break;
        case 6:
            output_massages(0);
            mvprintw(36 , 2 , "You've Stepped on a trap                ");
            mvprintw(37 , 2 , "Health: %d                              " , 100 - damage);
            timer_1 = 0;
            break;  
        case 7:
            output_massages(0);
            mvprintw(36 , 2 , "You've eathen a Normal Food             ");
            mvprintw(37 , 2 , "And regained 10 Hunger                  ");
            mvprintw(38 , 2 , "Hunger: %d                              " , 100 - elapsed);
            timer_1 = 0;
            break;  
        case 8:
            if(damage > 0){
                output_massages(0);
                mvprintw(36 , 2 , "Your Hunger is at max                   ");
                mvprintw(37 , 2 , "You're being healed                     ");            
                mvprintw(38 , 2 , "Health: %d                              " , 100 - damage);
                timer_1 = 0;
            }
            break;  
        case 9:
            output_massages(0);
            mvprintw(36 , 2 , "Your Normal Food inventory is full      ");
            timer_1 = 0;
            break; 
        case 10:
            output_massages(0);
            mvprintw(36 , 2 , "Your Magical Food inventory is full     ");
            timer_1 = 0;
            break; 
        case 11:
            output_massages(0);
            mvprintw(36 , 2 , "You are Back to the Last Floor          ");
            mvprintw(37 , 2 , "Floor Number: %d                        " , l + 1);
            timer_1 = 0;
            break;
        case 12:
            output_massages(0);
            mvprintw(36 , 2 , "You are in the Next Floor               ");
            mvprintw(37 , 2 , "Floor Number: %d                        " , l + 1);
            timer_1 = 0;
            break;
        case 13:
            output_massages(0);
            mvprintw(36 , 2 , "You Found a Secret Room                 ");
            timer_1 = 0;
            break;
        case 14:
            output_massages(0);
            mvprintw(36 , 2 , "You've Picked up Health Potion              ");
            mvprintw(37 , 2 , "Total Health Potions: %d                            " , g.Health_potin);
            timer_1 = 0;
            break;
        case 15:
            output_massages(0);
            mvprintw(36 , 2 , "You've Picked up Speed Potion           ");
            mvprintw(37 , 2 , "Total Speed Potions: %d                  " , g.Speed_potion);
            timer_1 = 0;
            break;
        case 16:
            output_massages(0);
            mvprintw(36 , 2 , "You've Picked up Damge Potion           ");
            mvprintw(37 , 2 , "Total Damage Potions: %d                 " , g.Damage_potion);
            timer_1 = 0;
            break;
        case 20:
            output_massages(0);
            mvprintw(36 , 2 , "NEed to be filled bothing is there           ");
            mvprintw(37 , 2 , "NA NAN ANNAN AN NAN NAN NN NANN NAN A            ");
            timer_1 = 0;
            break;
        default:
            break;
    }

}

/****************************************************************/
void* welcomeplayMusic(void* arg){
    SDL_Init(SDL_INIT_AUDIO);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    
    // Load the music file
    Mix_Music* welcome_music = Mix_LoadMUS("Welcome.mp3");
    Mix_PlayMusic(welcome_music,1);
    while(welcome) {}
    Mix_FreeMusic(welcome_music);
    return NULL;
}
void* signinplayMusic(void* arg){
    SDL_Init(SDL_INIT_AUDIO);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    
    // Load the music file
    Mix_Music* menu_music = Mix_LoadMUS("Signin.mp3");
    Mix_PlayMusic(menu_music,-1);
    while(signin) {}
    Mix_FreeMusic(menu_music);
    return NULL;
}
void* menuplayMusic(void* arg){
    SDL_Init(SDL_INIT_AUDIO);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    
    // Load the music file
    Mix_Music* menu_music = Mix_LoadMUS("Menu.mp3");
    Mix_PlayMusic(menu_music,-1);
    while(menu) {}
    Mix_FreeMusic(menu_music);
    return NULL;
}
void* firstfloorplayMusic(void* arg){
    SDL_Init(SDL_INIT_AUDIO);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    
    // Load the music file
    Mix_Music* First_floor_music = Mix_LoadMUS("1_floor.mp3");
    Mix_PlayMusic(First_floor_music,-1);
    while(First_floor) {}
    Mix_FreeMusic(First_floor_music);
    return NULL;
}
void* secondfloorplayMusic(void* arg){
    SDL_Init(SDL_INIT_AUDIO);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    
    // Load the music file
    Mix_Music* Second_floor_music = Mix_LoadMUS("2_floor.mp3");
    Mix_PlayMusic(Second_floor_music,-1);
    while(Second_floor) {}
    Mix_FreeMusic(Second_floor_music);
    return NULL;
}
void* thirdfloorplayMusic(void* arg){
    SDL_Init(SDL_INIT_AUDIO);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    
    // Load the music file
    Mix_Music* Second_floor_music = Mix_LoadMUS("2_floor.mp3");
    Mix_PlayMusic(Second_floor_music,-1);
    while(Second_floor) {}
    Mix_FreeMusic(Second_floor_music);
    return NULL;
}
void* forthfloorplayMusic(void* arg){
    SDL_Init(SDL_INIT_AUDIO);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    
    // Load the music file
    Mix_Music* Second_floor_music = Mix_LoadMUS("2_floor.mp3");
    Mix_PlayMusic(Second_floor_music,-1);
    while(Second_floor) {}
    Mix_FreeMusic(Second_floor_music);
    return NULL;
}
void check_what_to_play(int l){
    First_floor_playing=0;
    Second_floor_playing=0;
    Third_floor_playing=0;
    Forth_floor_playing=0;
    pthread_t floormusicThread;
    switch(l){
        case 0:
            // First_floor = 1;
            // if(menu_playing == 0){
                pthread_create(&floormusicThread, NULL, firstfloorplayMusic, NULL);    
            //     First_floor_playing = 1; 
            // }
            break;
        case 1:
            // Second_floor = 1;
            // if(menu_playing == 0){
                pthread_create(&floormusicThread, NULL, secondfloorplayMusic, NULL);    
               // Second_floor_playing = 1; 
            //}
                        break;
        case 2:
            //Third_floor = 1;
            //if(menu_playing == 0){
                pthread_create(&floormusicThread, NULL, thirdfloorplayMusic, NULL);    
                //Third_floor_playing = 1; 
            //}
                        break;
        case 3:
            //Forth_floor = 1;
            //if(menu_playing == 0){
                pthread_create(&floormusicThread, NULL, forthfloorplayMusic, NULL);    
                //Forth_floor_playing = 1; 
            //}
                        break;
        default:
            break;
    }
}
                         
void back_ground_sky(){
char background[10][200][200]=
{{{"  *                                                                          .                                                                      .          |           '       o  "},
{"    *              o           _|_                                                                                                                         . - o -                 o  "},
{"                       \\        |                      .                                           .                                         o              +  |   '                  "},
{"    .                   \\        .                              +                           *                                                           '                             "},   
{"                     o   *                      .                                  '                                     +   '                             *                          "},
{"   .                                                             .    |         +          +         .        o                .                       o  +                           "},
{"                +                    +                              --o--                         +      '     .                            .                         o        '      "},
{"             .      +                                     o.          |                                                 .-'""'-.                                                    . "},
{"                     |                .     .           o  +                               '                +         .' () .   '.      *                              .              "},
{"           .        -+-                       *      '   '           +~~                            + .              / .      o   \\                                                 ' "},
{"                     |  .    +                                o                                     o          '    ; o    _   ()  ;                 '                                "},
{"                                                              *                      '                              ;     (_)      ;    .                  '    '      +             o"},
{"                 .  o                      ..                          .            .                ' '             \\ .        o /                .                            o     "},
{"                                                                                     o                                '.  O  .  .'    '  *                                  *         "},
{"          *        '                                    .                                o      .  '                    '-....-'                           +      o                   "},
{"                                     .               o               +              .              + .          '     o                                                        '      "},
{"                              .  '                      +                         '       +                               - o -       .                                               "},
{"             .                                    '                                  +       '           o                  | .               +             .       '                 "},
{"                                      '                                                                                                                                         .     "},
{"'             .        o                             .         |         '  o    .                                          '                                    _|_                  "},
{"                     |                                       - o -            .                        '            '.               '                            |                   "},
{"                   --o--                                 .     |                                         .                 +            '      .                           .          "},
{"                     | '         |  .                  +       .      .                .+                                                                                             "},
{"          '   *   +             -+-             .                                                                                                  .    +                             "},
{"                                 |                     *             *                          .    .                 '                                    .  ' '          *    '    "},
{"                               |                                               .                 *            .  *          .    +                                                .   "},
{"                           * --o-- '          o                                    '   +                          *             .               |                         o           "},
{"                +              |                    +           '      . _|_        o              +             .                '            -+-              o          .          "},
{"                                              '                           |                                      '            .    o            |   +                                 "},
{"                                         .                          .'                          '                    .        .                   .                               o   "},
{"               '      +                        .                                .     *                    *                                    +             |                       "},
{"                          '                            ..                    '           +                                              o      '     .       -+-       .:'            "},
{"                                          .                                                                                      '                            |    _.::' .            "},
{"    +   *                                                           ..                                          +  o             +                .               (_.'                "},
{"             '            ,                                                                                 .                                                     o    +             "},
{"                                                         |                            *              '                 .     ' *                              . *        *            "},
{"      +                                            '   - o -                    '             '           +                                                      .             *      "},
{"           .        *            +             \\         |                                                           '          .         +         '                                 "},
{"   +                                            \\     .                    +                       +               .                                             +                    "},
{"  +   ' .                           +            *                                                                +                          ' +    o                                 "},
{"                                                                                                              .         o                                                      .'     "},
{"                .                                                *                         +                       .                       .                                          "},
{"                             +              .                    '     o           ..                                                                                   .             "},
{" _|_                                                  +  +                                            .                          .                 ..                                 "},
{"o |              .            .                                        '  |                                      '                 .                                          .       "},
{"  .      '         . o     .          /   *                          '  - o -                o          |       .                                                          .          "},
{"                                     /                               '    | +  *      .               - o -         +              *                    +             .               "}}};
    for (int j = 2 ; j < 49 ; j++){
        for (int i = 2 ; i < 200 ; i++){
            mvprintw( j , i , "%c" ,background[0][j][i]);
        }
    }
}    

void get_four_levels_rooms(){

    for (int l = 0 ; l < 4 ; l++){
        for (int j = 0 ; j < 34 ; j++){
            for (int i = 0 ; i < 132 ; i++){
                map[l][j][i].signs = ' ';
                map[l][j][i].flags = 0;
                map[l][j][i].layer = 0;
                map[l][j][i].enemy = 0;
                map[l][j][i].count = 0;
                map[l][j][i].health = 0;                
            }
        }
    }
    for(int l = 0 ; l < 4 ; l++){
    int y = 0 , x = 0;
        int h = 0;
        while( h != 7){
            if (h == 0){
                x1[l][h] = (rand()% 20) + 2;
                x2[l][h] = (rand()% 20) + 24;
                y_1[l][h] = (rand()% 6) + 3;
                y2[l][h] = (rand()% 6) + 7;  

                if((x2[l][h] - x1[l][h]) > 9 &&( y2[l][h] - y_1[l][h]) > 8 ){
                    y++;
                    if (y == 3){
                        y = 0;
                        x += 1;
                    }
                    h++;
                }
            }
            if(h != 0){
                x1[l][h] = ((rand()% 40)+2) + (y * 43);
                x2[l][h] = ((rand()% 40)+2) + (y * 43);
                y_1[l][h] = ((rand()% 13)+2) + (x * 16);
                y2[l][h] = ((rand()% 13)+2) + (x * 16);
                if((x2[l][h] - x1[l][h]) > 8 &&( y2[l][h] - y_1[l][h]) > 6){
                    y++;
                    if (y == 3){
                        y = 0;
                        x += 1;
                    }
                    h++;
                }
            }
        }
    }
    g.player.y = y_1[0][0] + 5;
    g.player.x = x1[0][0] + 5;
    get_hidden_rooms();
}
void generage_weapons_int_map(int x1 , int x2 , int y_1 , int y2 ,int l){
    int arrowss =rand()%4;
    int magicwand = rand()%4;
    int daggerr = rand()%4;
    int pocketsword = rand()%4;
    Pos arrowsss;
    Pos magicwandd;
    Pos daggerrr;
    Pos pocketswordd;
    switch(arrowss){
    case 0 ... 2:
        break;
    case 3:
        arrowsss.x = rand()%(x2 - x1 - 1) + x1 + 1;
        arrowsss.y = rand()%(y2 - y_1 - 1) + y_1 + 1;
        map[l][arrowsss.y][arrowsss.x].layer = 43;
        map[l][arrowsss.y][arrowsss.x].count = 20;        
        map[l][arrowsss.y][arrowsss.x].signs = 'A';
        break;
    default:
        break;
    }
    switch(magicwand){
        case 0 ... 2:
            break;
        case 3:
            magicwandd.x = rand()%(x2 - x1 - 1) + x1 + 1;
            magicwandd.y = rand()%(y2 - y_1 - 1) + y_1 + 1;
            map[l][magicwandd.y][magicwandd.x].layer = 42;
            map[l][magicwandd.y][magicwandd.x].count = 8; 
            map[l][magicwandd.y][magicwandd.x].signs = 'L';
            break;
        default :
            break;
    }   
    switch(daggerr){
        case 0 ... 2:
            break;
        case 3:
            daggerrr.x = rand()%(x2 - x1 - 1) + x1 + 1;
            daggerrr.y = rand()%(y2 - y_1 - 1) + y_1 + 1;
            map[l][daggerrr.y][daggerrr.x].layer = 41;
            map[l][daggerrr.y][daggerrr.x].count = 10; 
            map[l][daggerrr.y][daggerrr.x].signs = 'J';
            break;
        default :
            break;
    }  
    switch(pocketsword){
        case 0 ... 2:
            break;
        case 3:
            pocketswordd.x = rand()%(x2 - x1 - 1) + x1 + 1;
            pocketswordd.y = rand()%(y2 - y_1 - 1) + y_1 + 1;
            map[l][pocketswordd.y][pocketswordd.x].layer = 40;
            map[l][pocketswordd.y][pocketswordd.x].count = 1;
            map[l][pocketswordd.y][pocketswordd.x].signs = 'B';
            break;
        default :
            break;
    }  
}
void generage_potions_int_map(int x1 , int x2 , int y_1 , int y2 ,int l){
    int healthpotion =rand()%4;
    int speedpotion = rand()%4;
    int damagepotion = rand()%4;
    Pos health_potion;
    Pos speed_potion;
    Pos damage_potion;
    switch(healthpotion){
    case 0 ... 2:
        break;
    case 3:
        health_potion.x = rand()%(x2 - x1 - 1) + x1 + 1;
        health_potion.y = rand()%(y2 - y_1 - 1) + y_1 + 1;
        map[l][health_potion.y][health_potion.x].layer = 30;
        map[l][health_potion.y][health_potion.x].signs = 'U';
        break;
    default:
        break;
    }
    switch(speedpotion){
        case 0 ... 2:
            break;
        case 3:
            speed_potion.x = rand()%(x2 - x1 - 1) + x1 + 1;
            speed_potion.y = rand()%(y2 - y_1 - 1) + y_1 + 1;
            map[l][speed_potion.y][speed_potion.x].layer = 31;
            map[l][speed_potion.y][speed_potion.x].signs = 'U';
            break;
        default :
            break;
    }   
    switch(damagepotion){
        case 0 ... 2:
            break;
        case 3:
            damage_potion.x = rand()%(x2 - x1 - 1) + x1 + 1;
            damage_potion.y = rand()%(y2 - y_1 - 1) + y_1 + 1;
            map[l][damage_potion.y][damage_potion.x].layer = 32;
            map[l][damage_potion.y][damage_potion.x].signs = 'U';
            break;
        default :
            break;
    }  
}
void generage_traps_int_map(int x1 , int x2 , int y_1 , int y2 ,int l){
    int chance =rand()%4;
    Pos gold_1 , gold_2 , gold_3;
    switch(chance){
    case 0 :
        break;
    case 1:
        gold_1.x = rand()%(x2 - x1 - 3) + x1 + 2;
        gold_1.y = rand()%(y2 - y_1 - 3) + y_1 + 2;
        map[l][gold_1.y][gold_1.x].layer = 1;
        break;
    case 2:
        gold_1.x = rand()%(x2 - x1 - 3) + x1 + 2;
        gold_1.y = rand()%(y2 - y_1 - 3) + y_1 + 2;
        gold_2.x = rand()%(x2 - x1 - 3) + x1 +2;
        gold_2.y = rand()%(y2 - y_1 -3) + y_1 + 2;
        map[l][gold_1.y][gold_1.x].layer = 1;
        map[l][gold_2.y][gold_2.x].layer = 1;
        break;
    case 3:
        gold_1.x = rand()%(x2 - x1 - 3) + x1 + 2;
        gold_1.y = rand()%(y2 - y_1 - 3) + y_1 + 2;
        gold_2.x = rand()%(x2 - x1 - 3) + x1 + 2;
        gold_2.y = rand()%(y2 - y_1 -3) + y_1 + 2;
        gold_3.x = rand()%(x2 - x1 - 3) + x1 + 2;
        gold_3.y = rand()%(y2 - y_1 - 3) + y_1 + 2;
        map[l][gold_1.y][gold_1.x].layer = 1;
        map[l][gold_2.y][gold_2.x].layer = 1;
        map[l][gold_3.y][gold_3.x].layer = 1;
        break;
    default:
        break;
    }
}

void generage_pillars_int_map(int x1 , int x2 , int y_1 , int y2 ,int l){
    int chance =rand()%5;
    Pos gold_1 , gold_2 , gold_3;
    switch(chance){
    case 0 ... 1:
        break;
    case 2:
        gold_1.x = rand()%(x2 - x1 - 3) + x1 + 2;
        gold_1.y = rand()%(y2 - y_1 - 3) + y_1 + 2;
        map[l][gold_1.y][gold_1.x].signs = 'O';
        break;
    case 3:
        gold_1.x = rand()%(x2 - x1 - 3) + x1 + 2;
        gold_1.y = rand()%(y2 - y_1 - 3) + y_1 + 2;
        gold_2.x = rand()%(x2 - x1 - 3) + x1 + 2;
        gold_2.y = rand()%(y2 - y_1 - 3) + y_1 + 2;
        map[l][gold_1.y][gold_1.x].signs = 'O';
        map[l][gold_2.y][gold_2.x].signs = 'O';
        break;
    case 4:
        gold_1.x = rand()%(x2 - x1 - 3) + x1 + 2;
        gold_1.y = rand()%(y2 - y_1 - 3) + y_1 + 2;
        gold_2.x = rand()%(x2 - x1 - 3) + x1 + 2;
        gold_2.y = rand()%(y2 - y_1 - 3) + y_1 + 2;
        gold_3.x = rand()%(x2 - x1 - 3) + x1 + 2;
        gold_3.y = rand()%(y2 - y_1 - 3) + y_1 + 2;
        map[l][gold_1.y][gold_1.x].signs = 'O';
        map[l][gold_2.y][gold_2.x].signs = 'O';
        map[l][gold_3.y][gold_3.x].signs = 'O';
        break;
    default:
        break;
    }
}
void generage_gold_int_map(int x1 , int x2 , int y_1 , int y2 ,int l){
    int chance =rand()%5;
    int darkgold = rand()%5;
    Pos gold_1 , gold_2 , gold_3;
    Pos darkgold_1;
    switch(chance){
    case 0 ... 1:

        break;
    case 2:
        gold_1.x = rand()%(x2 - x1 - 3) + x1 + 2;
        gold_1.y = rand()%(y2 - y_1 - 3) + y_1 + 2;
        map[l][gold_1.y][gold_1.x].signs = 'G';
        break;
    case 3:
        gold_1.x = rand()%(x2 - x1 - 3) + x1 + 2;
        gold_1.y = rand()%(y2 - y_1 - 3) + y_1 + 2;
        gold_2.x = rand()%(x2 - x1 - 3) + x1 + 2;
        gold_2.y = rand()%(y2 - y_1 - 3) + y_1 + 2;
        map[l][gold_1.y][gold_1.x].signs = 'G';
        map[l][gold_2.y][gold_2.x].signs = 'G';
        break;
    case 4:
        gold_1.x = rand()%(x2 - x1 - 3) + x1 + 2;
        gold_1.y = rand()%(y2 - y_1 - 3) + y_1 + 2;
        gold_2.x = rand()%(x2 - x1 - 3) + x1 + 2;
        gold_2.y = rand()%(y2 - y_1 - 3) + y_1 + 2;
        gold_3.x = rand()%(x2 - x1 - 3) + x1 + 2;
        gold_3.y = rand()%(y2 - y_1 - 3) + y_1 + 2;
        map[l][gold_1.y][gold_1.x].signs = 'G';
        map[l][gold_2.y][gold_2.x].signs = 'G';
        map[l][gold_3.y][gold_3.x].signs = 'G';
        break;
    default:
        break;
    }
    switch(darkgold){
        case 1:
        darkgold_1.x = rand()%(x2 - x1 - 1) + x1 + 1;
        darkgold_1.y = rand()%(y2 - y_1 - 1) + y_1 + 1;
        map[l][darkgold_1.y][darkgold_1.x].signs = 'D';
        break;
            break;
        default :
            break;
    }
}
void generage_food_int_map(int x1 , int x2 , int y_1 , int y2 ,int l){
    int chance =rand()%5;
    int magicfood = rand()%5;
    Pos food_1 , food_2 , food_3;
    Pos magicfood_1;
    Pos qualityfood_1;
    Pos rotenflesh_1;
    switch(chance){
    case 0 ... 1:
        break;
    case 2:
        food_1.x = rand()%(x2 - x1 - 3) + x1 + 2;
        food_1.y = rand()%(y2 - y_1 - 3) + y_1 + 2;
        map[l][food_1.y][food_1.x].signs = 'a';
        break;
    case 3:
        food_1.x = rand()%(x2 - x1 - 3) + x1 + 2;
        food_1.y = rand()%(y2 - y_1 - 3) + y_1 + 2;
        food_2.x = rand()%(x2 - x1 - 3) + x1 + 2;
        food_2.y = rand()%(y2 - y_1 - 3) + y_1 + 2;
        map[l][food_1.y][food_1.x].signs = 'a';
        map[l][food_2.y][food_2.x].signs = 'a';
        break;
    case 4:
        food_1.x = rand()%(x2 - x1 - 3) + x1 + 2;
        food_1.y = rand()%(y2 - y_1 - 3) + y_1 + 2;
        food_2.x = rand()%(x2 - x1 - 3) + x1 + 2;
        food_2.y = rand()%(y2 - y_1 - 3) + y_1 + 2;
        food_3.x = rand()%(x2 - x1 - 3) + x1 + 2;
        food_3.y = rand()%(y2 - y_1 - 3) + y_1 + 2;
        map[l][food_1.y][food_1.x].signs = 'a';
        map[l][food_2.y][food_2.x].signs = 'a';
        map[l][food_3.y][food_3.x].signs = 'a';
        break;
    default:
        break;
    }
    switch(magicfood){
        case 1:
        magicfood_1.x = rand()%(x2 - x1 - 1) + x1 + 1;
        magicfood_1.y = rand()%(y2 - y_1 - 1) + y_1 + 1;
        map[l][magicfood_1.y][magicfood_1.x].signs = 'm';
            break;
        default :
            break;
    }   
}
void fill_the_map(){  
    for(int l = 0 ; l < 4 ; l++){
        for (int r = 0 ; r < 8 ; r++){  
            for (int i = x1[l][r] ; i <= x2[l][r] ; i++){
                for (int j = y_1[l][r] ; j <= y2[l][r] ; j++){
                    if ((i == x1[l][r] || i == x2[l][r])&& (j != y_1[l][r])){
                        map[l][j][i].signs = '|';
                    }
                    else if (j == y_1[l][r] || j == y2[l][r]){
                        map[l][j][i].signs = '_';
                    }
                    else{
                        map[l][j][i].signs = '.';
                    }
                    if(r==3 && l!= 3){
                        map[l][y2[l][r] - 2][x1[l][r] + 4].signs = '<';
                    }
                    if(r==0 && l!= 0){
                        map[l][y_1[l][r] + 2][x1[l][r] + 3].signs = '>';
                    }
                    if(r == 3 && l==3){
                        map[l][y2[l][r] - 1][x1[l][r] + 2].signs = 'W';
                         map[l][y2[l][r] - 2][x1[l][r] + 3].signs = 'G';
                         map[l][y2[l][r] - 4][x1[l][r] + 4].signs = 'G';
                         map[l][y2[l][r] - 5][x1[l][r] + 3].signs = 'G';
                         map[l][y_1[l][r] + 2][x1[l][r] + 4].signs = 'G';
                         map[l][y_1[l][r] + 1][x2[l][r] - 2].signs = 'G';
                         map[l][y_1[l][r] + 2][x2[l][r] - 3].signs = 'G';
                         map[l][y_1[l][r] + 1][x2[l][r] - 4].signs = 'G';
                         map[l][y2[l][r] - 1][x1[l][r] + 4].signs = 'G';
                         map[l][y2[l][r] - 1][x1[l][r] + 2].signs = 'G';                            
                    }
                }
            }
        }
    }
        int door_x1[6][8] ;
        int door_x2[6][8] ;
        int door_y1[6][8] ;
        int door_y2[6][8] ;
        int h ;
        int jj = 0;
    for(int l = 0 ; l < 4 ; l++){
        for (int d = 0 ; d <= 4;){
            if (d != 2){
                door_x1[l][d] = x2[l][d];
                door_x2[l][d] = x1[l][d + 1];
                door_y1[l][d] = (rand()%(y2[l][d]-y_1[l][d] - 3)) + y_1[l][d] + 2;
                door_y2[l][d] = (rand()%(y2[l][d+1]-y_1[l][d+1] - 3)) + y_1[l][d+1] + 2;
                if (jj > 200 || door_y1[l][d] - door_y2[l][d] > 3 || door_y2[l][d] - door_y1[l][d] > 3 ){jj = 0 ; d++;}
                else jj++;
            }
            if (d == 2){
                door_x1[l][d] = (rand()%(x2[l][d]-x1[l][d] - 3)) + x1[l][d] + 2;
                door_x2[l][d] = (rand()%(x2[l][d+3]-x1[l][d+3] - 3)) + x1[l][d+3] + 2;
                door_y1[l][d] = y2[l][d];
                door_y2[l][d] = y_1[l][d + 3];
                if (jj > 200000 || door_x1[l][d]-door_x2[l][d] > 3 || door_x2[l][d]-door_x1[l][d]>3){jj = 0 ; d++;}
                else jj++;
            }
        }
    }
    for(int l = 0 ; l < 4 ; l++){
        int gg = 1;
        while(gg){
            door_x1[l][5] = x2[l][2];
            door_x2[l][5] = x1[l][6];
            door_y1[l][5] = (rand()%(y2[l][2]-y_1[l][2] - 3)) + y_1[l][2] + 2;
            door_y2[l][5] = (rand()%(y2[l][6]-y_1[l][6] - 3)) + y_1[l][6] + 2;
            if (jj > 200000 || door_y1[l][5]-door_y2[l][5] > 4 || door_y2[l][5]-door_y1[l][5]>4) gg = 0;
            else jj++;
        }
        gg = 1;
        jj = 1;
        while(gg){
            door_x1[l][6] = x2[l][5];
            door_x2[l][6] = x1[l][7];
            door_y1[l][6] = (rand()%(y2[l][5]-y_1[l][5] - 3)) + y_1[l][5] + 2;
            door_y2[l][6] = (rand()%(y2[l][7]-y_1[l][7] - 3)) + y_1[l][7] + 2;
            if (jj > 200000 || door_y1[l][6]-door_y2[l][6] > 4 || door_y2[l][6]-door_y1[l][6]>4){
                 gg = 0;
                map[l][door_y1[l][6]][door_x1[l][6]].layer = 80;
            }
            else jj++;
        }
    }
    for(int l = 0 ; l < 4 ; l++){
        for (int d = 0 ; d <= 6; d++){
            if (d != 2){                
                int midpoint = door_x1[l][d]+ door_x2[l][d];
                midpoint /= 2;
                if (door_x1[l][d] > door_x2[l][d]){
                    if (door_y1[l][d] > door_y2[l][d]){
                        for (int rj = door_y2[l][d] ; rj <= door_y1[l][d] ; rj++){
                            for (int ri = door_x2[l][d] ; ri <= door_x1[l][d]; ri++){
                                if ((ri >=  midpoint-1) && (ri <= midpoint + 1)) {
                                    map[l][rj][ri].signs = '#';
                                }
                                if (map[l][rj][ri].signs != '_' && map[l][rj][ri].signs != '|' && map[l][rj][ri].signs != '.'
                                && (ri >= midpoint) && rj>=door_y2[l][d]-1 && rj<=door_y2[l][d]+1){
                                    map[l][rj][ri].signs = '#';
                                }
                                if (map[l][rj][ri].signs != '_' && map[l][rj][ri].signs != '|' && map[l][rj][ri].signs != '.'
                                && (ri <= midpoint) && rj>=door_y1[l][d]-1 && rj<=door_y1[l][d]+1){
                                    map[l][rj][ri].signs = '#';
                                }        
                            }
                        }
                    }
                    if (door_y1[l][d] <= door_y2[l][d]){
                        for (int rj = door_y1[l][d] ; rj <= door_y2[l][d] ; rj++){
                            for (int ri = door_x2[l][d]; ri <= door_x1[l][d]; ri++){
                                if ((ri >=  midpoint-1) && (ri <= midpoint + 1)) {
                                    map[l][rj][ri].signs = '#';
                                }
                                if (map[l][rj][ri].signs != '_' && map[l][rj][ri].signs != '|' && map[l][rj][ri].signs != '.'
                                && (ri >= midpoint) && rj>=door_y2[l][d]-1 && rj<=door_y2[l][d]+1){
                                    map[l][rj][ri].signs = '#';
                                }
                                if (map[l][rj][ri].signs != '_' && map[l][rj][ri].signs != '|' && map[l][rj][ri].signs != '.'
                                && (ri <= midpoint) && rj>=door_y1[l][d]-1 && rj<=door_y1[l][d]+1){
                                    map[l][rj][ri].signs = '#';
                                }     
                            }
                        }
                    }                   
                }
                if (door_x1[l][d] <= door_x2[l][d]){
                    if (door_y1[l][d] > door_y2[l][d]){
                        for (int rj = door_y2[l][d] ; rj <= door_y1[l][d] ; rj++){
                            for (int ri = door_x1[l][d] ; ri <= door_x2[l][d]; ri++){
                                if ((ri >=  midpoint-1) && (ri <= midpoint + 1)) {
                                    map[l][rj][ri].signs = '#';
                                }
                                if (map[l][rj][ri].signs != '_' && map[l][rj][ri].signs != '|' && map[l][rj][ri].signs != '.'
                                && (ri >= midpoint) && rj>=door_y2[l][d]-1 && rj<=door_y2[l][d]+1){
                                    map[l][rj][ri].signs = '#';
                                }
                                if (map[l][rj][ri].signs != '_' && map[l][rj][ri].signs != '|' && map[l][rj][ri].signs != '.'
                                && (ri <= midpoint) && rj>=door_y1[l][d]-1 && rj<=door_y1[l][d]+1){
                                    map[l][rj][ri].signs = '#';
                                }        
                            }
                        }
                    }
                    if (door_y1[l][d] <= door_y2[l][d]){
                        for (int rj = door_y1[l][d] ; rj <= door_y2[l][d] ; rj++){
                            for (int ri = door_x1[l][d]; ri <= door_x2[l][d]; ri++){
                                if ((ri >=  midpoint-1) && (ri <= midpoint + 1)) {
                                    map[l][rj][ri].signs = '#';
                                }
                                if (map[l][rj][ri].signs != '_' && map[l][rj][ri].signs != '|' && map[l][rj][ri].signs != '.'
                                && (ri >= midpoint) && rj>=door_y2[l][d]-1 && rj<=door_y2[l][d]+1){
                                    map[l][rj][ri].signs = '#';
                                }
                                if (map[l][rj][ri].signs != '_' && map[l][rj][ri].signs != '|' && map[l][rj][ri].signs != '.'
                                && (ri <= midpoint) && rj>=door_y1[l][d]-1 && rj<=door_y1[l][d]+1){
                                    map[l][rj][ri].signs = '#';
                                }        
                            }
                        }
                    } 
                }
            }
            if (d == 2){
                int midpoint = door_y1[l][d]+ door_y2[l][d];
                midpoint /= 2;
                if (door_x1[l][d] > door_x2[l][d]){
                    for (int rj = door_y1[l][d] ; rj <= door_y2[l][d] ; rj++){
                        for (int ri = door_x2[l][d] ; ri <= door_x1[l][d]; ri++){
                            if ((rj > midpoint-1) && (rj <= midpoint + 1)) {
                                map[l][rj][ri].signs = '#';
                            }
                            if (map[l][rj][ri].signs != '|' && map[l][rj][ri].signs != '_' && map[l][rj][ri].signs != '.'
                            && (rj >= midpoint) && ri>=door_x2[l][d]-2 && ri<=door_x2[l][d]+2){
                                map[l][rj][ri].signs = '#';
                            }
                            if (map[l][rj][ri].signs != '|' && map[l][rj][ri].signs != '_' && map[l][rj][ri].signs != '.'
                            && (rj <= midpoint) && ri>=door_x1[l][d]-2 && ri<=door_x1[l][d]+2){
                                map[l][rj][ri].signs = '#';
                            }              
                        }
                    }
                }
                if (door_x1[l][d] <= door_x2[l][d]){
                    for (int rj = door_y1[l][d] ; rj <= door_y2[l][d]; rj++){
                        for (int ri = door_x1[l][d]; ri <= door_x2[l][d]; ri++){
                            if ((rj > midpoint-1) && (rj <= midpoint + 1)) {
                                map[l][rj][ri].signs = '#';
                            }
                            if (map[l][rj][ri].signs != '|' && map[l][rj][ri].signs != '_' && map[l][rj][ri].signs != '.'
                            && (rj >= midpoint) && ri>=door_x2[l][d]-2 && ri<=door_x2[l][d]+2){
                                map[l][rj][ri].signs = '#';
                            }
                            if (map[l][rj][ri].signs != '|' && map[l][rj][ri].signs != '_' && map[l][rj][ri].signs != '.'
                            && (rj <= midpoint) && ri>=door_x1[l][d]-2 && ri<=door_x1[l][d]+2){
                                map[l][rj][ri].signs = '#';
                            }              
                        }
                    }
                }                
            }
            map[l][door_y1[l][d]][door_x1[l][d]].signs = '+';
            map[l][door_y2[l][d]][door_x2[l][d]].signs = '+';
        }
    }
    for(int l = 0 ; l < 4 ; l++){
        for (int ri = 0 ; ri < 184 ; ri++){ 
            for (int rj = 0 ; rj < 45 ; rj++){      
                if(map[l][rj][ri].layer == 80) map[l][rj][ri].signs = '|';
            }
        }
    } 
    for(int l = 0 ; l < 4 ; l++){
        for (int r = 0 ; r < 8 ; r++){     
        generage_gold_int_map(x1[l][r],x2[l][r],y_1[l][r],y2[l][r],l); 
        generage_food_int_map(x1[l][r],x2[l][r],y_1[l][r],y2[l][r],l); 
        generage_pillars_int_map(x1[l][r],x2[l][r],y_1[l][r],y2[l][r],l); 
        generage_potions_int_map(x1[l][r],x2[l][r],y_1[l][r],y2[l][r],l);
        generage_weapons_int_map(x1[l][r],x2[l][r],y_1[l][r],y2[l][r],l);
        generage_enemies_int_map(x1[l][r],x2[l][r],y_1[l][r],y2[l][r],l);
            if(r==3 && l!= 3){
                map[l][y2[l][r] - 2][x1[l][r] + 4].signs = '<';
            }
            if(r==0 && l!= 0){
                map[l][y_1[l][r] + 2][x1[l][r] + 3].signs = '>';
            }
        }
    }   
} 
void get_hidden_rooms(){
    int hah = 0;
    int mmd = 0;
    while(hah != 4){
        x1[hah][6] = ((rand()% 40)+2) + (3 * 43);
        x2[hah][6] = ((rand()% 40)+2) + (3 * 43);
        y_1[hah][6] = ((rand()% 13)+2) + (0 * 16);
        y2[hah][6] = ((rand()% 13)+2) + (0 * 16);
        if((x2[hah][6] - x1[hah][6]) > 8 &&( y2[hah][6] - y_1[hah][6]) > 6){
            hah++;
        }
    }
    while(mmd != 4){
        x1[mmd][7] = ((rand()% 40)+2) + (3 * 43);
        x2[mmd][7] = ((rand()% 40)+2) + (3 * 43);
        y_1[mmd][7] = ((rand()% 13)+2) + (1 * 16);
        y2[mmd][7] = ((rand()% 13)+2) + (1 * 16);
        if((x2[mmd][7] - x1[mmd][7]) > 8 &&( y2[mmd][7] - y_1[mmd][7]) > 6){
            mmd++;
        }
    }
}

void check_room_index(){
    if (map[l][g.player.y][g.player.x].signs == '#'){
        for (int i = g.player.x - 4 ; i <= g.player.x + 4 ; i++){
            for (int j = g.player.y - 2; j <= g.player.y + 2 ; j++){
                if (map[l][j][i].signs == '#' || map[l][j][i].signs == '+')
                if(map[l][j][i].flags != 2)
                map[l][j][i].flags = 1;
            }
        }
    }
    if (map[l][g.player.y][g.player.x].signs == '.'||
    map[l][g.player.y][g.player.x].signs == 'a'||
    map[l][g.player.y][g.player.x].signs == 'G'||
    map[l][g.player.y][g.player.x].signs == 'U'||
    map[l][g.player.y][g.player.x].signs == 'D'||
    map[l][g.player.y][g.player.x].signs == 'm'||
    map[l][g.player.y][g.player.x].signs == 'O'){
        if (g.player.y > 0 && g.player.y <= 15 && g.player.x > 0 && g.player.x <= 43){
            for (int i = x1[l][0] ; i <= x2[l][0] ; i++){
                for (int j = y_1[l][0] ; j <= y2[l][0] ; j++){
                    map[l][j][i].flags = 2;
                }
            }
        }
    }

    if (map[l][g.player.y][g.player.x].signs == '+' || map[l][g.player.y][g.player.x].signs == '.'){
        if (g.player.y > 0 && g.player.y <= 15 && g.player.x > 43 && g.player.x <= 86){
            for (int i = x1[l][1] ; i <= x2[l][1] ; i++){
                for (int j = y_1[l][1] ; j <= y2[l][1] ; j++){
                    map[l][j][i].flags = 2;
                }
            }
        }
        if (g.player.y > 0 && g.player.y <= 15 && g.player.x > 86 && g.player.x <= 130){
            for (int i = x1[l][2] ; i <= x2[l][2] ; i++){
                for (int j = y_1[l][2] ; j <= y2[l][2] ; j++){
                    map[l][j][i].flags = 2;
                    map[l][y_1[l][2]][x1[l][2]+ 4].flags = 3;  
                }
            }
        }
        if (g.player.y > 15 && g.player.y <= 30 && g.player.x > 86 && g.player.x <= 130){
            for (int i = x1[l][5] ; i <= x2[l][5] ; i++){
                for (int j = y_1[l][5] ; j <= y2[l][5] ; j++){
                    map[l][j][i].flags = 2;
                }
            }
        }
        if (g.player.y > 15 && g.player.y <= 30 && g.player.x > 43 && g.player.x <= 86){
            for (int i = x1[l][4] ; i <= x2[l][4] ; i++){
                for (int j = y_1[l][4] ; j <= y2[l][4] ; j++){
                    map[l][j][i].flags = 2;
                }
            }
        }
        if (g.player.y > 15 && g.player.y <= 30 && g.player.x > 0 && g.player.x <= 43){
            for (int i = x1[l][3] ; i <= x2[l][3] ; i++){
                for (int j = y_1[l][3] ; j <= y2[l][3] ; j++){
                    map[l][j][i].flags = 2;
                }
            }
        }
        if (g.player.y > 0 && g.player.y <= 15 && g.player.x > 130 && g.player.x <= 184){
            for (int i = x1[l][6] ; i <= x2[l][6] ; i++){
                for (int j = y_1[l][6] ; j <= y2[l][6] ; j++){
                    map[l][j][i].flags = 2;
                }
            }
        }
        if (g.player.y > 15 && g.player.y <= 30 && g.player.x > 130 && g.player.x <= 184){
            for (int i = x1[l][7] ; i <= x2[l][7] ; i++){
                for (int j = y_1[l][7] ; j <= y2[l][7] ; j++){
                    map[l][j][i].flags = 2;
                }
            }
        }
    }
}

void Check_winning_or_losing(){
    if(damage >= 100){
        timeout(-1);
        clear();
        set_final_values();
        add_to_score_board();
        back_ground_sky();
        draw_menu_border();
        Mix_CloseAudio();
        SDL_Quit();
        mvprintw(LINES / 2 - 8, COLS / 2 - 20, "                                   ");
        mvprintw(LINES / 2 - 7, COLS / 2 - 20, "              Game Over            ");
        mvprintw(LINES / 2 - 6, COLS / 2 - 20, "                                   ");
        in_game = 1;
        int ch = getch();
    }
    if (map[l][g.player.y][g.player.x].signs == 'W'){
        timeout(-1);
        g.games_finished += 1;
        set_final_values();
        add_to_score_board();
        back_ground_sky();
        draw_menu_border();
        Mix_CloseAudio();
        SDL_Quit();
        mvprintw(LINES / 2 - 8, COLS / 2 - 20, "                                   ");
        mvprintw(LINES / 2 - 7, COLS / 2 - 20, "               You Won             ");
        mvprintw(LINES / 2 - 6, COLS / 2 - 20, "                                   ");
        int ch = getch();
        in_game = 1;
    }
}


void generage_enemies_int_map(int x1 , int x2 , int y_1 , int y2 ,int l){
    
    //  deamon

    // int healthpotion =rand()%10;
    Pos deamon;
    int healthpotion = 3;
    switch(healthpotion){
    case 0 ... 2:
        break;
    case 3:
        deamon.x = rand()%(x2 - x1 - 1) + x1 + 1;
        deamon.y = rand()%(y2 - y_1 - 1) + y_1 + 1;
        // map[l][deamon.y][deamon.x].layer = 30;
        map[l][deamon.y][deamon.x].signs = 'V';
        map[l][deamon.y][deamon.x].health = 30;
        break;
    default:
        break;
    }




}