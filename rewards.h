#ifndef _REWARDS_
#define _REWARDS_

typedef struct _list {
    int pos_x;
    int pos_y;
    int r;
    int g;
    int b;
    struct _list *next;
} list;

list *alloc_reward() {
    list *head = (list*)malloc(sizeof(list));

    head->next = NULL;
    head->pos_x = -1;
    head->pos_y = -1;

    return head;
}

void generate_reward(list **head) {
    if (*head == NULL) {
        *head = alloc_reward();
        (*head)->pos_x = rand() % 320;
        (*head)->pos_y = rand() % 240;
        (*head)->r = rand() % 230 + 25;
        (*head)->g = rand() % 230 + 25;
        (*head)->b = rand() % 230 + 25;

        return;
    }

    list *new_element = alloc_reward();
    new_element->pos_x = rand() % 320;
    new_element->pos_y = rand() % 240;
    new_element->r = rand() % 230 + 25;
    new_element->g = rand() % 230 + 25;
    new_element->b = rand() % 230 + 25;
    
    new_element->next = (*head);
    (*head) = new_element;

    return;
}

void remove_reward(list **head, list *element) {
    list *current_element = *head;
    list *old_element = *head;

    while (current_element != NULL) {
        if (current_element == element) {
            if (current_element == *head) {
                *head = (*head)->next;
            } else if (current_element->next == NULL) {
                if (old_element == *head) {
                    (*head)->next = NULL;
                } else {
                    old_element->next = NULL;
                }
            } else {
                if (old_element == *head) {
                    (*head)->next = (*head)->next->next;
                } else {
                    old_element->next = old_element->next->next;
                }
            }

            free(element);
            break;
        }
        old_element = current_element;
        current_element = current_element->next;
    }
}

void print_rewards(list *rewards) {
    list *curr = rewards;

    while (curr != NULL) {
        Serial.print("(");
        Serial.print(curr->pos_x);
        Serial.print(", ");
        Serial.print(curr->pos_y);
        Serial.print(") ");

        curr = curr->next;
    }

    Serial.println();
}

#endif