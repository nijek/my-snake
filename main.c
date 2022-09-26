#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <ncurses.h>

#include "linkedlist.h"

#define HEIGHT 20
#define WIDTH 50
#define TRUE 1
#define FALSE 0



void display(char board[HEIGHT][WIDTH], int max_i, int max_j, int score, int max_score) {
    int i, j;
    move(0, 0);	
    printw("SCORE: %d MAX SCORE: %d\n", score, max_score);
    for(i = 0; i < max_i; i++) {
        for(j = 0; j < max_j; j++)
            printw("%c", board[i][j]);
        printw("\n");
    }

}

int hit_wall(struct node *head) {
    if (head->l == 0 || head->c == 0 || head->l == HEIGHT - 1 || head->c == WIDTH - 1)
        return TRUE;
    return FALSE;
}

int hit_snake(struct node *head) {
    struct node *ptr;
    ptr = head->next;
    while (ptr) {
        if(head->l == ptr->l && head->c == ptr->c)
            return TRUE;
        ptr = ptr->next;
    }
    return FALSE;
}

int hit_food(struct node *head, int food_position[2]) {
    struct node *ptr;
    ptr = head;
    while (ptr) {
        if (ptr->l == food_position[0] && ptr->c == food_position[1])
            return TRUE;
        ptr = ptr->next;
    }
    return FALSE;
    
}

void add_snake(char board[HEIGHT][WIDTH], struct node *head) {
    struct node *ptr;
    ptr = head;
    while(ptr) {
        board[ptr->l][ptr->c] = 's';
        ptr = ptr->next;
    }
}

void clear_snake(char board[HEIGHT][WIDTH], struct node *head) {
    struct node *ptr;
    ptr = head;
    while(ptr) {
        board[ptr->l][ptr->c] = ' ';
        ptr = ptr->next;
    }
}

struct node* increase_snake(struct node *head, char direction) {
    int l,c;
    l = c = 0;
    
    switch (direction)
    {
    case 'w':
        l = -1;
        break;
    case 's':
        l = +1;
        break; 
    case 'a':
        c = -1;
        break;
    case 'd':
        c = +1;
        break;
    default:
        break;
    }

    head = add_after_head(head, head->l, head->c);
    head->l += l; head->c += c;

    return head;

}

struct node *update_snake(char board[HEIGHT][WIDTH], struct node *head, char direction, char last_key) {
    int l,c;
    
    l = c = 0;
    switch (direction)
    {
    case 'w':
        l = -1;
        break;
    case 's':
        l = +1;
        break; 
    case 'a':
        c = -1;
        break;
    case 'd':
        c = +1;
        break;
    default:
        break;
    }
    
    if (head->next)
        if (head->l + l == head->next->l && head->c + c == head->next->c) {
            head = add(head, head->l - l, head->c - c);
            head = del_last(head);
            return head;
        }
    head = add(head, head->l + l, head->c + c);
    head = del_last(head);
    return head; 
}

int random_x() {
    int random;
    do {
        random = rand() % HEIGHT;
    }
    while (random == 0 || random == HEIGHT - 1);
    return random;

}

int random_y() {
    int random;
    do {
        random = rand() % WIDTH;
    }
    while (random == 0 || random == WIDTH - 1);
    return random;

}

int kbhit(void)
{
    int ch = getch();

    if (ch != ERR) {
        ungetch(ch);
        return 1;
    } else {
        return 0;
    }
}

void add_food(char board[HEIGHT][WIDTH], int food_position[2]) {
    int l = random_x();
    int c = random_y();
    board[l][c] = '*';
    food_position[0] = l;
    food_position[1] = c;
}

void add_board_walls(char board[HEIGHT][WIDTH]) {
        for (int i = 0; i < HEIGHT; i++)
            for(int j = 0; j < WIDTH; j++) {
                if (i == 0 || j == 0 || i == HEIGHT - 1 || j == WIDTH - 1) 
                    board[i][j] = '#';
                else
                    board[i][j] = ' ';
            }
}

int new_game(int max_score, int level) {
    
    int clock_tick = (int)(CLOCKS_PER_SEC * ((float)(10-level))/20);
    int food_position[2], score;
    char board[HEIGHT][WIDTH];
    char key_pressed = 'a';
    char last_key = '\n';
    
    score = 0;
    
    struct node *snake = NULL;
    
    snake = create(HEIGHT/2, WIDTH/2);
    add_board_walls(board);
    add_food(board, food_position);
    
    while(TRUE) {
        clock_t target = clock() + clock_tick;
        /*wait key hit or time*/
        while (!kbhit() && clock() < target) {}
        if (kbhit()) {
            last_key = key_pressed;    
            key_pressed = getch();
            refresh();
        }
        
        clear_snake(board, snake);
        snake = update_snake(board, snake, key_pressed, last_key);
        add_snake(board, snake);   
        display(board, HEIGHT, WIDTH, score, max_score);
        
        if (hit_food(snake, food_position)) {
            score += 1;
            board[food_position[0]][food_position[1]] = ' ';
            add_food(board, food_position);
            snake = increase_snake(snake, key_pressed);
            add_snake(board, snake);
            display(board, HEIGHT, WIDTH, score, max_score);

        }

        if (hit_wall(snake) || hit_snake(snake)) {
            free_list(snake);
            return score;
        }    
    }
}

int main() {
    time_t t;
    char key_pressed;
    int score, max_score, level;
    score = max_score = 0;
    srand((unsigned) time(&t));

    initscr();
    cbreak();
    noecho();
    nodelay(stdscr, TRUE);
    scrollok(stdscr, TRUE);

    while(TRUE) {
        clear();
        printw("Qual nível de dificuldade (1..9)?\n");
        while (!kbhit());
        if (kbhit()) {   
            key_pressed = getch();
            level = key_pressed - '0';
            refresh();
        }

        score = new_game(max_score, level);
        if(score > max_score) max_score = score;
        
        printw("\n\nvocê perdeu\n\n");
        printw("SCORE: %d MELHOR SCORE: %d\n", score, max_score);
        flushinp();
        printw("Quer tentar novamente? S/n");
        
        while (!kbhit());
        if (kbhit()) {   
            key_pressed = getch();
            refresh();
        }
        if (key_pressed == 'n') {
            endwin();
            return 0;
        }

    }
    return 0;
 
}
