#ifndef _SNAKE_
#define _SNAKE_

#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

typedef struct _queue_element {
    int16_t pos_to_change_x;
    int16_t pos_to_change_y;
    int16_t last_changed_cell;
    int16_t direction;
    struct _queue_element *next;
} queue_element;

typedef struct _queue {
    queue_element *head;
    queue_element *tail;
} queue;

typedef struct _snake_cell {
    int16_t cell_id;
    int16_t pos_x;
    int16_t pos_y;
    int16_t direction;
    struct _snake_cell *next;
} snake_cell;

typedef struct _snake {
    int8_t direction;
    int16_t snake_size;
    //This will be used for parsing.
    snake_cell *snake_head;
    //This will be used for adding.
    snake_cell *snake_tail;
    int16_t last_x_t;
    int16_t last_y_t;
    queue *moves;
} snake;

queue *create_queue() {
    queue *q = (queue *) malloc(sizeof(queue));
    q->head = NULL;
    q->tail = NULL;

    return q;
}

void add_queue_element(queue **q, int16_t pos_to_change_x, int16_t pos_to_change_y, int16_t direction) {
    queue_element *qe = (queue_element*) malloc(sizeof(queue_element));
    qe->pos_to_change_x = pos_to_change_x;
    qe->pos_to_change_y = pos_to_change_y;
    qe->direction = direction;
    qe->last_changed_cell = -1;
    qe->next = NULL;

    if ((*q)->head == NULL) {
        (*q)->head = qe;
        (*q)->tail = qe;
        return;
    }

    if ((*q)->head == (*q)->tail) {
        (*q)->head->next = qe;
        (*q)->tail = qe;
        return;
    }

    (*q)->tail->next = qe;
    (*q)->tail = qe;
}

snake *create_snake(int16_t pos_x, int16_t pos_y, int8_t direction) {
    snake *s = (snake *) malloc(sizeof(snake));
    //last_direction is -1 if the snake is fully going into the same direction
    //if last_direction != -1 => the snake is going in two directions => tail has not changed
    s->direction = direction;
    s->snake_size = 0;

    s->snake_head = (snake_cell *) malloc(sizeof(snake_cell));
    s->snake_head->next = NULL;
    s->snake_head->pos_x = pos_x;
    s->snake_head->pos_y = pos_y;
    s->snake_head->cell_id = s->snake_size;
    s->snake_head->direction = direction;
    s->snake_tail = s->snake_head;

    s->last_x_t = -1;
    s->last_y_t = -1;

    s->moves = create_queue();

    s->snake_size++;

    return s;
}

snake_cell *add_snake_cell(snake *s) {
    snake_cell *new_cell = (snake_cell *) malloc(sizeof(snake_cell));
    new_cell->next = NULL;
    new_cell->cell_id = s->snake_size;
    new_cell->direction = s->snake_tail->direction;
    s->snake_size++;

    int direction = s->snake_tail->direction;

    if (direction == UP) {
        new_cell->pos_x = s->snake_tail->pos_x + 1;
        new_cell->pos_y = s->snake_tail->pos_y;
    } else if (direction == DOWN) {
        new_cell->pos_x = s->snake_tail->pos_x - 1;
        new_cell->pos_y = s->snake_tail->pos_y;
    } else if (direction == LEFT) {
        new_cell->pos_x = s->snake_tail->pos_x;
        new_cell->pos_y = s->snake_tail->pos_y + 1;
    } else {
        new_cell->pos_x = s->snake_tail->pos_x;
        new_cell->pos_y = s->snake_tail->pos_y - 1;
    }

    if (s->snake_tail == s->snake_head) {
        s->snake_head->next = new_cell;
        s->snake_tail = new_cell;
    } else {
        s->snake_tail->next = new_cell;
        s->snake_tail = new_cell;
    }

    return new_cell;
}

void move_snake(snake *s) {
    snake_cell *curr_cell = s->snake_head;
    int direction = -1;

    while (curr_cell != NULL) {
        direction = curr_cell->direction;

        queue_element *curr_qe = s->moves->head;
        while (curr_qe != NULL) {
            if (curr_qe->last_changed_cell == s->snake_size - 1) {
                s->moves->head = s->moves->head->next;
                queue_element *to_delete = curr_qe;
                curr_qe = curr_qe->next;
                free(to_delete);
            } else if (curr_cell->cell_id <= curr_qe->last_changed_cell) {
                curr_qe = curr_qe->next; 
            } else if (curr_cell->pos_x == curr_qe->pos_to_change_x && curr_cell->pos_y == curr_qe->pos_to_change_y) {
                direction = curr_qe->direction;
                curr_qe->last_changed_cell = curr_cell->cell_id;
                curr_cell->direction = direction;

                break;
            } else {
                break;
            }
        }

        //Serial.println("Ajung si aici dupa ce ies de acolo.");

        if (curr_cell == s->snake_tail) {
            s->last_x_t = curr_cell->pos_x;
            s->last_y_t = curr_cell->pos_y;
        }

        if (direction == UP) {
            curr_cell->pos_x--;
        } else if (direction == DOWN) {
            curr_cell->pos_x++;
        } else if (direction == LEFT) {
            curr_cell->pos_y--;
        } else if (direction == RIGHT) {
            curr_cell->pos_y++;
        }

        if (curr_cell->pos_x < 30) {
            curr_cell->pos_x = 320;
        } else if (curr_cell->pos_x > 320) {
            curr_cell->pos_x = 30;
        }

        if (curr_cell->pos_y < 0) {
            curr_cell->pos_y = 240;
        } else if (curr_cell->pos_y > 240) {
            curr_cell->pos_y = 0;
        }

        curr_cell = curr_cell->next;
    }
}

void delete_snake(snake *s) {
    snake_cell *curr_cell = s->snake_head;
    snake_cell *last_cell = s->snake_head;

    while(curr_cell != NULL) {
        last_cell = curr_cell;
        curr_cell = curr_cell->next;

        free(last_cell);
    }

    free(s);
}

#endif