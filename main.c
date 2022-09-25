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



void display(char board[HEIGHT][WIDTH], int max_i, int max_j) {
    int i, j;
    move(0, 0);	
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



int main() {
    time_t t;
    int clock_tick = 500000;
    int food_position[2], i, j, c;
    char board[HEIGHT][WIDTH];
    char key_pressed = 'a';
    char last_key = '\n';
    
    struct node *snake = NULL;

    snake = create(HEIGHT/2, WIDTH/2);
      
    srand((unsigned) time(&t));

        initscr();
        cbreak();
        noecho();
        nodelay(stdscr, TRUE);
        scrollok(stdscr, TRUE);


    for (i = c = 0; i < HEIGHT; i++)
        for(j = 0; j < WIDTH; j++, c++) {
            if (i == 0 || j == 0 || i == HEIGHT - 1 || j == WIDTH - 1) 
                board[i][j] = '#';
            else
                board[i][j] = ' ';
        }
     
    
    food_position[0] = random_x();
    food_position[1] = random_y();
    board[food_position[0]][food_position[1]] = '*';
   
    while(TRUE) {
        clock_t target = clock() + 0.5*CLOCKS_PER_SEC;
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
        display(board, HEIGHT, WIDTH);
        
        if (hit_food(snake, food_position)) {
            board[food_position[0]][food_position[1]] = ' ';
            food_position[0] = random_x();
            food_position[1] = random_y();
            board[food_position[0]][food_position[1]] = '*';
            snake = increase_snake(snake, key_pressed);
            add_snake(board, snake);
            display(board, HEIGHT, WIDTH);

        }

        if (hit_wall(snake) || hit_snake(snake)) {
            endwin();
            puts("\n\n\nvocê perdeu\n\n\n");
            return 0;
        }

    }

    return 0;
}