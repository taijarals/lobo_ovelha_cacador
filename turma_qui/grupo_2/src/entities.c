#include <stdio.h>
#include <stdlib.h>

#include "entities.h"

void entity_list_init(EntityList *list) {
    list->count = 0;
    list->capacity = 16;
    list->next_id = 1;
    list->items = malloc(sizeof(Entity) * list->capacity);

    if (list->items == NULL) {
        fprintf(stderr, "Erro: nao foi possivel alocar a lista de entidades.\n");
        exit(EXIT_FAILURE);
    }
}

void entity_list_free(EntityList *list) {
    free(list->items);
    list->items = NULL;
    list->count = 0;
    list->capacity = 0;
    list->next_id = 1;
}

Entity *entity_list_add(EntityList *list, EntityType type, Position pos) {
    if (list->count == list->capacity) {
        int new_capacity = list->capacity * 2;
        Entity *new_items = realloc(list->items, sizeof(Entity) * new_capacity);

        if (new_items == NULL) {
            fprintf(stderr, "Erro: nao foi possivel expandir a lista de entidades.\n");
            exit(EXIT_FAILURE);
        }

        list->items = new_items;
        list->capacity = new_capacity;
    }

    Entity *entity = &list->items[list->count++];
    entity->id = list->next_id++;
    entity->type = type;
    entity->pos = pos;
    entity->alive = 1;
    entity->age = 0;
    entity->energy = type == ENTITY_ZOMBIE ? 8 : 10;

    return entity;
}

void entity_list_remove_dead(EntityList *list) {
    int write = 0;

    for (int read = 0; read < list->count; read++) {
        if (list->items[read].alive) {
            list->items[write++] = list->items[read];
        }
    }

    list->count = write;
}

const char *entity_type_name(EntityType type) {
    switch (type) {
        case ENTITY_HUMAN:
            return "Humano";
        case ENTITY_ZOMBIE:
            return "Zumbi";
        case ENTITY_SOLDIER:
            return "Soldado";
        default:
            return "Desconhecido";
    }
}

char entity_symbol(EntityType type) {
    switch (type) {
        case ENTITY_HUMAN:
            return 'H';
        case ENTITY_ZOMBIE:
            return 'Z';
        case ENTITY_SOLDIER:
            return 'S';
        default:
            return '?';
    }
}
