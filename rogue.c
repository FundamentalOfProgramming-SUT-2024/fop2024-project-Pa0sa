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
    char weapon[50];
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
} Map;

typedef struct{
    char name[51];
    int score;
    int games_started;
    int games_finished;
} Leaderboard;

int l = 0;
int menu = 1;
int welcome = 1;
int First_floor = 1;
int GAME_STATUS = 1;
int timer_1 = 0;
int timer_2 = 0;
int elapsed = 0;
int difficulty = 0;
int hunger = 0;
int damage = 0;
int x1[6][10];
int y_1[6][10];
int x2[6][10];
int y2[6][10];
Map map[6][50][184];
Game g;


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
void Set_game_music(Game *g);

void create_a_new_game();

void draw_hungerbar();
void draw_heathbar();
void draw_gold_count();
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

void Check_winning_or_losing();


//*********************************************************************************
int main(){

    pthread_t musicThread;
    pthread_create(&musicThread, NULL, playMusic, NULL);    
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



    welcome_panel();

    get_player_info(&g);
    get_innitial_values();

    while(menu_page(&g)){}

    menu = 0;
    g.start_time = time(NULL);
    

    timeout(40);
    while(GAME_STATUS){

        draw_menu_border();
        check_room_index();
        draw_rooms();
        draw_player(g);
        draw_hungerbar();
        draw_heathbar();
        draw_gold_count();
        draw_food_count();
        int ch = getch();
        handle_movement(ch, &g.player);
        // mvprintw(40 , 110 , "%d , %d" , COLS , LINES);
        refresh();
        clear_massages();
        Check_winning_or_losing();
    }
    pthread_join(musicThread, NULL);

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
    g.hunger = 100;
    g.speed = 1;
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
    mvprintw(LINES / 2 - 2, COLS / 2 - 15, "Enter your password: ");
    getnstr(g->password, 50);
    noecho();
    while(check_user_password(g->password)){
        clear();
        draw_menu_border();
        echo();
        mvprintw(LINES / 2 - 1, COLS / 2 - 15, "Password is too weak.");
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
    mvprintw(LINES / 2 - 2, COLS / 2 - 15, "Enter your password or email: ");
    getnstr(g->password, 50);
    noecho();
    while(check_user_password_from_loggin(g ,g->password)){
        clear();
        draw_menu_border();
        echo();
        mvprintw(LINES / 2 - 1, COLS / 2 - 15, "Password or email is not correct.");
        mvprintw(LINES / 2 - 2, COLS / 2 - 15, "Enter your password or email: ");
        getnstr(g->password, 50);
        noecho();
    }
    noecho();
}
bool check_user_password_from_loggin(Game *g, char * password){
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
        clear();
        g->games_started += 1;
        //create_a_new_game(g);
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
                if(map[l][j][i].signs == 'G'){
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
    for (int j = 3 ; j <= 10 + counter ; j++){
        for (int i = 12 ; i <= 70 ; i++){
            mvprintw( j , i , " ");
        }
    }
    mvprintw(6 , 12 ,"Rank    |    Name  Score   Gold   Started   Won");
    for (int o = 0 ; o < counter ; o++){
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
    mvprintw(7 , 10 , "Camp");
    mvprintw(8 , 10 , "Goat");
    mvprintw(9 , 10 , "Best");
    fclose(ifp);
    move(7,15);refresh();
    wprintf(L"%ls\n", L"\x01F3C6");
    refresh();move(8,13);refresh();
    wprintf(L"%ls\n", L"\x01F451");
    refresh();move(9,11);refresh();
    wprintf(L"%ls\n", L"\x01F3C5");
    refresh();getch(); clear();
    attroff(COLOR_PAIR(2));
    attroff(COLOR_PAIR(3));
    attroff(COLOR_PAIR(5));
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
        // Set_game_music(g);
        break;
    clear();
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
        difficulty = 20;
        break;
    case 1:
        difficulty = 15;
        break;
    case 2:
        difficulty = 10;
        break;
    case 3:
        difficulty = 0;
        break;
    case 4:
        difficulty = 2000000000;
        break;
    default:
        clear();
    }
}

void Set_colors(){
    init_pair(0, COLOR_WHITE, COLOR_BLACK);
        init_color(1, 0, 1000, 0);
        init_color(2, 1000, 0, 0);
        init_color(3, 1000, 800, 0);
        init_color(4, 1000, 412, 705);
        // init_color(6, 1000, 412, 705);
        // init_color(6, 1000, 412, 705);
        // init_color(6, 1000, 412, 705);
        // init_color(6, 1000, 412, 705);
    init_pair(1, 1, COLOR_BLACK);
    init_pair(2, 2, COLOR_BLACK);
    init_pair(3, 3, COLOR_BLACK);
    init_pair(4, 4, COLOR_BLACK);
    // init_pair(5, 5, COLOR_BLACK);
    // init_pair(6, 6, COLOR_BLACK);
    // init_pair(7, COLOR_BLUE, COLOR_BLACK);
    init_pair(5, COLOR_CYAN, COLOR_BLACK);

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
    hunger++;
    if(g.speed == 1){
        if (hunger == 60 - difficulty){
            hunger = 0;
            elapsed++;
        }
    }
    else{
        if (hunger == 30 - difficulty/2){
            hunger = 0;
            elapsed++;
        }        
    }
    int remaining = 100 - elapsed;
    float fraction = (float)remaining / 100;
    int width = 60 * fraction;

    if(remaining < 90){
        if(hunger == 50){
            damage++;
        }
    }
    attron(COLOR_PAIR(1));
    for (int i = 1; i <= width; ++i)
        mvprintw(35, i + 94, "#");
    attroff(COLOR_PAIR(1));

    attron(COLOR_PAIR(2));
    for (int i = width + 1; i <= 60; ++i)
        mvprintw(35, i + 94, ".");
    attroff(COLOR_PAIR(2));

    mvprintw(35, 93, "Hunger: %d ", remaining);
    mvprintw(35, 104, "[");
    mvprintw(35, 155, "]");
}
void draw_heathbar(){
    int remaining = 100 - damage;
    float fraction = (float)remaining / 100;
    int width = 60 * fraction;

    attron(COLOR_PAIR(1));
    for (int i = 1; i <= width; ++i)
        mvprintw(36, i + 94, "#");
    attroff(COLOR_PAIR(1));

    attron(COLOR_PAIR(2));
    for (int i = width + 1; i <= 60; ++i)
        mvprintw(36, i + 94, ".");
    attroff(COLOR_PAIR(2));

    mvprintw(36, 93, "Health: %d ", remaining);
    mvprintw(36, 104, "[");
    mvprintw(36, 155, "]");
}

void draw_gold_count(){
    mvprintw(37, 93, "Gold: %d ", g.gold);
}
void draw_food_count(){
    mvprintw(38, 93, "Magical Food: %d ", g.magic_food); 
    mvprintw(39, 93, "Quality Food: %d ", g.quality_food);
    mvprintw(40, 93, "Normal Food: %d ", g.normal_food);
    mvprintw(41, 93, "Roten Flesh: %d ", g.rotenflesh);
}

void food_menu(){
    timeout(-1);
    const char *levels[] = {"Normal", "Magical" ,"Quality", "Roten Flesh"};
    int choice = 0;
    while (1){
        draw_menu_border();
        mvprintw(1, 1, " ");
        for (int i = 0; i < 4; ++i){
            if (i == choice)
                attron(A_REVERSE);
            if(i == 0) mvprintw(35 + i, 158, "%d %s", g.normal_food , levels[i] );
            if(i == 1) mvprintw(35 + i, 158, "%d %s", g.magic_food , levels[i] );
            if(i == 2) mvprintw(35 + i, 158, "%d %s", g.quality_food , levels[i] );
            if(i == 3) mvprintw(35 + i, 158, "%d %s", g.rotenflesh , levels[i] );           
            if (i == choice)
                attroff(A_REVERSE);
        }

        int ch = getch();
        if (ch == KEY_UP)
            choice = (choice == 0) ? 3 : choice - 1;
        else if (ch == KEY_DOWN)
            choice = (choice == 3) ? 0 : choice + 1;
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
            output_massages(7);
            if (elapsed < 0) elapsed = 0;
            if (elapsed == 0){damage -= 10;
            if (damage < 0)damage = 0;output_massages(8);} 
            
            g.normal_food -= 1;
        }
        break;
    case 1:
        if(g.magic_food > 0){
            elapsed -=12;
            damage -= 7;
            if (elapsed < 0) elapsed = 0;
            if (elapsed == 0) damage -= 10;
            if (damage < 0) damage = 0;
            g.magic_food -= 1;
        }
        break;
    case 2:
        if(g.quality_food > 0){
            elapsed -=22;
            if (elapsed < 0) elapsed = 0;
            if (elapsed == 0) damage -= 10;
            if (damage < 0) damage = 0;
            g.quality_food -= 1;
        }
        break;
    case 3:
        difficulty = 0;
        break;
    case 4:
        difficulty = 2000000000;
        break;
    default:
        clear();
    }    
//! ************anva ghaza ha be nesbat faz 2 kamel shavad*************************************** */
    timeout(40);
}

void handle_movement(int ch, Pos *p ){

    switch (ch) {
    case KEY_UP:
        if (map[l][p->y - g.speed][p->x].signs == '<' ){
            l++;
            g.player.y = y_1[l][0] + 5;
            g.player.x = x1[l][0] + 5;
            clear();
        }
        if (map[l][p->y - g.speed][p->x].flags == 3 ){
            map[l][p->y - g.speed][p->x].signs = '?'; 
            clear();
            timeout(-1);
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
            timeout(40);      
        }
        if (map[l][p->y - g.speed][p->x].signs == '>' ){
            l--;
            g.player.y = y_1[l][3] + 3;
            g.player.x = x1[l][3] + 3;
            clear();
        }
        else if (map[l][p->y - g.speed][p->x].signs == 'm' ||
        map[l][p->y - g.speed][p->x].signs == 'a' ||
        map[l][p->y - g.speed][p->x].signs == '.' ||
         map[l][p->y - g.speed][p->x].signs == '#' ||
          map[l][p->y - g.speed][p->x].signs == '+' ||
          map[l][p->y - g.speed][p->x].signs == 'G'||
          map[l][p->y - g.speed][p->x].signs == 'D')
            p->y -= g.speed;
        break;
    case KEY_DOWN:
        if (map[l][p->y + g.speed][p->x].signs == '<' ){
            l++;
            g.player.y = y_1[l][0] + 5;
            g.player.x = x1[l][0] + 5;
            clear();
        }
        if (map[l][p->y + g.speed][p->x].signs == '>' ){
            l--;
            g.player.y = y_1[l][3] + 3;
            g.player.x = x1[l][3] + 3;
            clear();
        }
        else if (map[l][p->y + g.speed][p->x].signs == '.' ||
        map[l][p->y + g.speed][p->x].signs == 'a' ||
        map[l][p->y + g.speed][p->x].signs == 'm' ||
         map[l][p->y + g.speed][p->x].signs == '#' ||
          map[l][p->y + g.speed][p->x].signs == '+' ||
          map[l][p->y + g.speed][p->x].signs == 'G'||
          map[l][p->y + g.speed][p->x].signs == 'D')
            p->y += g.speed;      
        break;
    case KEY_LEFT:
        if (map[l][p->y][p->x - g.speed].signs == '<' ){
            l++;
            g.player.y = y_1[l][0] + 5;
            g.player.x = x1[l][0] + 5;
            clear();
        }
        if (map[l][p->y][p->x - g.speed].signs == '>' ){
            l--;
            g.player.y = y_1[l][3] + 3;
            g.player.x = x1[l][3] + 3;
            clear();
        }
        else if (map[l][p->y][p->x - g.speed].signs == 'm' ||
        map[l][p->y][p->x - g.speed].signs == 'a' ||
        map[l][p->y][p->x - g.speed].signs == '.' ||
         map[l][p->y][p->x - g.speed].signs == '#' ||
        map[l][p->y][p->x - g.speed].signs == '+' ||
        map[l][p->y][p->x - g.speed].signs == 'G' ||
        map[l][p->y][p->x - g.speed].signs == 'D' )
            p->x -= g.speed;
        break;
    case KEY_RIGHT:
        if (map[l][p->y][p->x + g.speed].signs == '<' ){
            l++;
            g.player.y = y_1[l][0] + 5;
            g.player.x = x1[l][0] + 5;
            clear();
        }
        if (map[l][p->y][p->x + g.speed].signs == '>' ){
            l--;
            g.player.y = y_1[l][3] + 3;
            g.player.x = x1[l][3] + 3;
            clear();
        }
        else if (map[l][p->y][p->x + g.speed].signs == 'a' ||
        map[l][p->y][p->x + g.speed].signs == 'm' ||
        map[l][p->y][p->x + g.speed].signs == '.' ||
         map[l][p->y][p->x + g.speed].signs == '#' ||
          map[l][p->y][p->x + g.speed].signs == '+' ||
          map[l][p->y][p->x + g.speed].signs == 'G' ||
          map[l][p->y][p->x + g.speed].signs == 'D' )
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
            g.speed = 3;
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
        timeout(-1);
        for (int j = 1 ; j < 32 ; j++){
            for (int i = 1 ; i < 183 ; i++){
                if(map[l][j][i].signs == 'G'){
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
        timeout(40);  
        break;
    case 'q':
        set_final_values();
        add_to_score_board();
        mvprintw(LINES / 2 - 8, COLS / 2 - 20, "                                   ");
        mvprintw(LINES / 2 - 7, COLS / 2 - 20, "            Are you Sure?          ");
        mvprintw(LINES / 2 - 6, COLS / 2 - 20, "                                   ");
        int ch = getch();
        endwin();
        exit(0);
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
        }
        break;
    case 'e':
        timeout(-1);
        food_menu();
        timeout(40);
    default:
        break;
    }
    if(map[l][g.player.y][g.player.x].signs == '+' && map[l][g.player.y][g.player.x].flags == 1){
        output_massages(1);
    }
    if(map[l][g.player.y][g.player.x].layer){
        output_massages(6);
        damage += 7;
        map[l][g.player.y][g.player.x].signs ='^'; 
        map[l][g.player.y][g.player.x].layer = 0; 
    }

}
void clear_massages(){
        timer_1++;
    if (timer_1 == 100){
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
            mvprintw(37 , 2 , "Total Score : %d                        " , g.gold * 6);
            timer_1 = 0;
            break;
        case 3:
            output_massages(0);
            mvprintw(36 , 2 , "You've Picked up Dark Gold              ");
            mvprintw(37 , 2 , "Total Score : %d                        " , g.gold * 6);
            timer_1 = 0;
            break;
        case 4:
            output_massages(0);
            mvprintw(36 , 2 , "You've Picked up Normal Food            ");
            mvprintw(37 , 2 , "Total Normal Food : %d                  " , g.normal_food);
            timer_1 = 0;
            break;
        case 5:
            output_massages(0);
            mvprintw(36 , 2 , "You've Picked up Dark Gold              ");
            mvprintw(37 , 2 , "Total Magical Food : %d                 " , g.magic_food * 6);
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
            output_massages(0);
            mvprintw(36 , 2 , "Your Hunger is at max                   ");
            mvprintw(37 , 2 , "You've gained 10 Heath                  ");            
            mvprintw(38 , 2 , "Health: %d                              " , 100 - damage);
            timer_1 = 0;
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
        default:
            break;
    }

}

/****************************************************************/
void* playMusic(void* arg) {
    // Initialize SDL and SDL_Mixer
    SDL_Init(SDL_INIT_AUDIO);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    
    // Load the music file
    Mix_Music* welcome_music = Mix_LoadMUS("Welcome.mp3");
    Mix_PlayMusic(welcome_music,-1);
    while(welcome) {}
    Mix_FreeMusic(welcome_music);

    Mix_Music* menu_music = Mix_LoadMUS("Menu.mp3");
    Mix_PlayMusic(menu_music,-1);
    while(menu) {}
    Mix_FreeMusic(menu_music);

    Mix_Music* First_floor_music = Mix_LoadMUS("First_floor.mp3");
    Mix_PlayMusic(First_floor_music,-1);
    while(First_floor) {}
    Mix_FreeMusic(First_floor_music);

    Mix_CloseAudio();
    SDL_Quit();
    return NULL;
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
void generage_potions_int_map(int x1 , int x2 , int y_1 , int y2 ,int l){




    
}
void generage_traps_int_map(int x1 , int x2 , int y_1 , int y2 ,int l){
    int chance =rand()%4;
    Pos gold_1 , gold_2 , gold_3;
    switch(chance){
    case 0 :
        break;
    case 1:
        gold_1.x = rand()%(x2 - x1 - 1) + x1 + 1;
        gold_1.y = rand()%(y2 - y_1 - 1) + y_1 + 1;
        map[l][gold_1.y][gold_1.x].layer = 1;
        break;
    case 2:
        gold_1.x = rand()%(x2 - x1 - 1) + x1 + 1;
        gold_1.y = rand()%(y2 - y_1 - 1) + y_1 + 1;
        gold_2.x = rand()%(x2 - x1 - 1) + x1 + 1;
        gold_2.y = rand()%(y2 - y_1 - 1) + y_1 + 1;
        map[l][gold_1.y][gold_1.x].layer = 1;
        map[l][gold_2.y][gold_2.x].layer = 1;
        break;
    case 3:
        gold_1.x = rand()%(x2 - x1 - 1) + x1 + 1;
        gold_1.y = rand()%(y2 - y_1 - 1) + y_1 + 1;
        gold_2.x = rand()%(x2 - x1 - 1) + x1 + 1;
        gold_2.y = rand()%(y2 - y_1 - 1) + y_1 + 1;
        gold_3.x = rand()%(x2 - x1 - 1) + x1 + 1;
        gold_3.y = rand()%(y2 - y_1 - 1) + y_1 + 1;
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
        gold_1.x = rand()%(x2 - x1 - 1) + x1 + 1;
        gold_1.y = rand()%(y2 - y_1 - 1) + y_1 + 1;
        map[l][gold_1.y][gold_1.x].signs = 'G';
        break;
    case 3:
        gold_1.x = rand()%(x2 - x1 - 1) + x1 + 1;
        gold_1.y = rand()%(y2 - y_1 - 1) + y_1 + 1;
        gold_2.x = rand()%(x2 - x1 - 1) + x1 + 1;
        gold_2.y = rand()%(y2 - y_1 - 1) + y_1 + 1;
        map[l][gold_1.y][gold_1.x].signs = 'G';
        map[l][gold_2.y][gold_2.x].signs = 'G';
        break;
    case 4:
        gold_1.x = rand()%(x2 - x1 - 1) + x1 + 1;
        gold_1.y = rand()%(y2 - y_1 - 1) + y_1 + 1;
        gold_2.x = rand()%(x2 - x1 - 1) + x1 + 1;
        gold_2.y = rand()%(y2 - y_1 - 1) + y_1 + 1;
        gold_3.x = rand()%(x2 - x1 - 1) + x1 + 1;
        gold_3.y = rand()%(y2 - y_1 - 1) + y_1 + 1;
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
        food_1.x = rand()%(x2 - x1 - 1) + x1 + 1;
        food_1.y = rand()%(y2 - y_1 - 1) + y_1 + 1;
        map[l][food_1.y][food_1.x].signs = 'a';
        break;
    case 3:
        food_1.x = rand()%(x2 - x1 - 1) + x1 + 1;
        food_1.y = rand()%(y2 - y_1 - 1) + y_1 + 1;
        food_2.x = rand()%(x2 - x1 - 1) + x1 + 1;
        food_2.y = rand()%(y2 - y_1 - 1) + y_1 + 1;
        map[l][food_1.y][food_1.x].signs = 'a';
        map[l][food_2.y][food_2.x].signs = 'a';
        break;
    case 4:
        food_1.x = rand()%(x2 - x1 - 1) + x1 + 1;
        food_1.y = rand()%(y2 - y_1 - 1) + y_1 + 1;
        food_2.x = rand()%(x2 - x1 - 1) + x1 + 1;
        food_2.y = rand()%(y2 - y_1 - 1) + y_1 + 1;
        food_3.x = rand()%(x2 - x1 - 1) + x1 + 1;
        food_3.y = rand()%(y2 - y_1 - 1) + y_1 + 1;
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
            if (jj > 200000 || door_y1[l][6]-door_y2[l][6] > 4 || door_y2[l][6]-door_y1[l][6]>4) gg = 0;
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
        for (int r = 0 ; r < 8 ; r++){     
        generage_gold_int_map(x1[l][r],x2[l][r],y_1[l][r],y2[l][r],l); 
        generage_food_int_map(x1[l][r],x2[l][r],y_1[l][r],y2[l][r],l); 
        generage_pillars_int_map(x1[l][r],x2[l][r],y_1[l][r],y2[l][r],l); 
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
    if (map[l][g.player.y][g.player.x].signs == '.'){
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
        mvprintw(LINES / 2 - 8, COLS / 2 - 20, "                                   ");
        mvprintw(LINES / 2 - 7, COLS / 2 - 20, "              Game Over            ");
        mvprintw(LINES / 2 - 6, COLS / 2 - 20, "                                   ");
        int ch = getch();
        endwin();
        exit(0);
    }
    if (map[l][g.player.y][g.player.x].signs == 'W'){
        timeout(-1);
        set_final_values();
        add_to_score_board();
        back_ground_sky();
        draw_menu_border();
        mvprintw(LINES / 2 - 8, COLS / 2 - 20, "                                   ");
        mvprintw(LINES / 2 - 7, COLS / 2 - 20, "               You Won             ");
        mvprintw(LINES / 2 - 6, COLS / 2 - 20, "                                   ");
        int ch = getch();
        endwin();
        exit(0);    
    }
}