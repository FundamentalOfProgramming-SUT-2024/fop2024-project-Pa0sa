#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
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
    int max_items;
    int time_limit;
    int highest;
    time_t start_time;
    char name[51];
    char password[51];
    char email[51];

} Game;

void wellcome_panel();
void draw_menu_border();

void get_player_info(Game *g);

void create_a_new_account(Game *g);
void get_username(Game *g,FILE *fptr);
void get_user_password(Game *g,FILE *fptr);
void get_user_email(Game *g,FILE *fptr);
bool check_username(char * name);
bool check_user_password(char * password);
bool check_user_email(char * email);

void log_into_account(Game *g);
void get_username_from_loggin(Game *g,FILE *fptr);
bool check_username_from_loggin(char * name); 
void get_player_info(Game *g);
void password_from_loggin(Game *g,FILE *fptr);
bool check_user_password_from_loggin(Game *g,char * password);
void menu_page(Game *g);

void show_score_board(Game *g);
void show_user_profile(Game *g);
void show_options_menu(Game *g);

void create_a_new_game();



void draw_board();
void get_four_levels_rooms(int x1[4][10], int y1[4][10], int x2[4][10], int y2[4][10]);
void handle_movement(int ch, Pos *p ,int speed);
void draw_player(Game *g);
void draw_rooms(int x1[4][10] ,int y1[4][10],int x2[4][10],int y2[4][10]);







//*********************************************************************************
int main(){
    Game g;
    g.player.y = LINES/2;
    g.player.x = COLS/2;
    
    int speed = 1;

    int x1[4][10];
    int y1[4][10];
    int x2[4][10];
    int y2[4][10];
    get_four_levels_rooms(&x1 , &y1 , &x2 , &y2);

    initscr();
    start_color();
    init_pair(2, COLOR_GREEN, COLOR_BLACK);

    srand(time(0));
    noecho();
    curs_set(FALSE);
    keypad(stdscr, TRUE);

    wellcome_panel();
    get_player_info(&g);
    menu_page(&g);

    mvprintw(140 , 25, "((()))");

    int GAME_STATUS = 1;
    while(GAME_STATUS){
        // clear();
        draw_menu_border();
        draw_rooms(x1 , y1 , x2 , y2);
        draw_player(&g);
        int ch = getch();
        handle_movement(ch, &g.player , speed);
        refresh();
        // if (0) GAME_STATUS = 0;
    }
    return 0;
}

//********************************************************************************







void wellcome_panel(){
    draw_menu_border();

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
}


void get_player_info(Game *g){
    draw_menu_border();

    const char *levels[] = {"Create a new account", "Log in", "Enter as a Guest"};
    int choice = 0;

    while (1)
    {
        draw_menu_border();
        mvprintw(1, 1, " ");
        for (int i = 0; i < 3; ++i)
        {
            if (i == choice)
                attron(A_REVERSE);
            mvprintw(4 + i, 5, "%s", levels[i]);
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

        break;
    }
    clear();
}

void draw_menu_border(){
    clear();
    for (int x = 0; x < COLS; ++x)
    {
        mvprintw(0, x, "-");
        mvprintw(LINES - 1, x, "-");
    }
    for (int y = 0; y < LINES; ++y)
    {
        mvprintw(y, 0, "|");
        mvprintw(y, COLS - 1, "|");
    }
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
    mkdir(g->name);
    char filepath[512];
    snprintf(filepath, sizeof(filepath), "%s/%s", g->name, g->name);
    fptr = fopen(filepath ,"w");
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
    fprintf(ifp, name);
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
    fprintf(fptr, g->password);
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
    fprintf(fptr, g->email);
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



void menu_page(Game *g){
    draw_menu_border();

    const char *levels[] = {"Start a new game", "Resume", "Profile" , "Score board" , "Options"};
    int choice = 0;

    while (1)
    {
        draw_menu_border();
        mvprintw(1, 1, " ");
        for (int i = 0; i <= 4; ++i)
        {
            if (i == choice)
                attron(A_REVERSE);
            mvprintw(4 + i, 5, "%s", levels[i]);
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
        // create_a_new_game(g);
        break;
    case 1:
        // Resume_old_game(g);
        break;
    case 2:
        show_user_profile(g);
        break;
    case 3:
        // show_score_board(g);
        break;
    case 4:
        show_options_menu(g);
        break;
    }
    clear();
}

// void create_a_new_game(){
//     clear();
//     // draw_board();



// }

void get_four_levels_rooms(int x1[4][10], int y1[4][10], int x2[4][10], int y2[4][10]){
    int y = 0 , x = 0;
    int h = 0;
    for(int l = 0 ; l < 4 ; l++){
        while( h != 6){
            x1[l][h] = (rand()% 31) + (y * 31);
            x2[l][h] = (rand()% 31) + (y * 31);
            y1[l][h] = (rand()% 13) + (x * 13);
            y2[l][h] = (rand()% 13) + (x * 13);
            if((x2[l][h] - x1[l][h]) > 6 &&( y2[l][h] - y1[l][h]) > 4){
                y++;
                if (y == 3){
                    y = 0;
                    x += 1;
                }
                h++;
            }
        }
        for (int r = 0 ; r < 6 ; r++){
            if (x1[l][r] > x2[l][r]){
                int temp;
                temp = x1[l][r];
                x1[l][r] = x2[l][r] ;
                x2[l][r] = temp;
            }
            if (y1[l][r] > y2[l][r]){
                int temp;
                temp = y1[l][r];
                y1[l][r] = y2[l][r] ;
                y2[l][r] = temp;
            }
        }
    }
    
}
void draw_rooms(int x1[4][10] ,int y1[4][10],int x2[4][10],int y2[4][10]){
    int l = 0;
        for (int r = 0 ; r < 6 ; r++){
            for (int i = x1[l][r] ; i <= x2[l][r] ; i++){
                for (int j = y1[l][r] ; j <= y2[l][r] ; j++){
                    if ((i == x1[l][r] || i == x2[l][r])&& (j != y1[l][r])){
                        mvprintw( j , i , "|");
                    }
                    else if (j == y1[l][r] || j == y2[l][r]){
                        mvprintw( j , i , "_");
                    }
                    else{
                        mvprintw( j , i , ".");  
                    }
                }
            }
            refresh();
        }  
}
void show_user_profile(Game *g){
    draw_menu_border();
    
    mvprintw(4 , 5, "Name : %s", g->name);
    mvprintw(5 , 5, "Password : %s", g->password);
    mvprintw(6 , 5, "Eamil : %s", g->email);
    mvprintw(7 , 5, "High score : %d", g->highest);
    getch();
}

void show_score_board(Game *g){







}

void show_options_menu(Game* g){

    draw_menu_border();

    const char *levels[] = {"Icon", "Colore", "Music"};
    int choice = 0;

    while (1){
        draw_menu_border();
        mvprintw(1, 1, " ");
        for (int i = 0; i < 3; ++i){
            if (i == choice)
                attron(A_REVERSE);
            mvprintw(4 + i, 5, "%s", levels[i]);
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
        // create_a_new_game(g);
        break;
    case 1:
        // Resume_old_game(g);
        break;
    case 2:
        //show_user_profile(g);
        break;
    clear();


    }
}

void draw_player(Game *g){

    attron(COLOR_PAIR(2));
    mvprintw(g->player.y, g->player.x, "@");

    attroff(COLOR_PAIR(2));
    refresh();
}

// void draw_roomsd(Game *g){


// }

void handle_movement(int ch, Pos *p , int speed){

    switch (ch) {
    case KEY_UP:
        if (p->y - speed > 0)
            p->y -= speed;
        break;
    case KEY_DOWN:
        if (p->y + speed < LINES - 1)
            p->y += speed;
        break;
    case KEY_LEFT:
        if (p->x - speed > 0)
            p->x -= speed;
        break;
    case KEY_RIGHT:
        if (p->x + speed < COLS - 1)
            ++p->x;
        break;
    case 'v':/******************************************************/
        if (speed == 1){
            speed = 20;
        }
        if(speed == 20){
            speed = 1;
        }
        break;

    case 'q':
        endwin();
        exit(0);
    }
    
}