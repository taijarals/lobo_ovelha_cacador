#include <stdio.h>
#include <stdlib.h>
#include "entity.h"

/* inicializa a lista zerada */
void entity_list_init(EntityList* list) {
    list->head    = NULL;
    list->tail    = NULL;
    list->total   = 0;
    list->wolves  = 0;
    list->sheep   = 0;
    list->hunters = 0;
    list->next_id = 1;
}

/*
 * Cria uma entidade e insere no final da lista
 * insercao no tail e O(1) porque guardo o ponteiro tail
 * sem isso teria que percorrer tudo pra achar o fim
 */
Entity* entity_create(EntityList* list, FactionType faction, int row, int col) {
    /* limite pra evitar que a reproducao exploda a memoria */
    if (list->total >= MAX_ENTITIES) return NULL;

    Entity* e = (Entity*)malloc(sizeof(Entity));
    if (!e) {
        fprintf(stderr, "Erro: falha ao alocar entidade.\n");
        return NULL;
    }

    e->id           = list->next_id++;
    e->faction      = faction;
    e->row          = row;
    e->col          = col;
    e->alive        = 1;
    e->rounds_alive = 0;
    e->prev         = NULL;
    e->next         = NULL;

    /* define vida e atualiza contador da faccao */
    switch (faction) {
        case FACTION_WOLF:   e->health = WOLF_HEALTH;   list->wolves++;  break;
        case FACTION_SHEEP:  e->health = SHEEP_HEALTH;  list->sheep++;   break;
        case FACTION_HUNTER: e->health = HUNTER_HEALTH; list->hunters++; break;
        default:              e->health = 0;                             break;
    }
    list->total++;

    /* insere no fim da lista encadeada */
    if (!list->head) {
        list->head = e;
        list->tail = e;
    } else {
        e->prev          = list->tail;
        list->tail->next = e;
        list->tail       = e;
    }

    return e;
}

/*
 * Marca como morta mas NAO libera a memoria ainda
 * se liberasse aqui no meio do loop de interacoes, os ponteiros
 * ->next do iterador ficariam apontando pra lixo (undefined behavior)
 * por isso separo em mark (aqui) e sweep (depois do loop)
 */
void entity_mark_dead(EntityList* list, Entity* e) {
    if (!e || !e->alive) return;

    e->alive = 0;
    list->total--;

    switch (e->faction) {
        case FACTION_WOLF:   list->wolves--;  break;
        case FACTION_SHEEP:  list->sheep--;   break;
        case FACTION_HUNTER: list->hunters--; break;
        default:                               break;
    }
}

/* passa pela lista e libera tudo que esta marcado como morto */
void entity_sweep_dead(EntityList* list) {
    Entity* e = list->head;
    while (e) {
        Entity* prox = e->next;

        if (!e->alive) {
            /* remove da lista duplamente encadeada */
            if (e->prev) e->prev->next = e->next;
            else         list->head    = e->next;

            if (e->next) e->next->prev = e->prev;
            else         list->tail    = e->prev;

            free(e);
        }

        e = prox;
    }
}

/* libera toda a lista (chamado so no encerramento) */
void entity_list_destroy(EntityList* list) {
    Entity* e = list->head;
    while (e) {
        Entity* prox = e->next;
        free(e);
        e = prox;
    }
    entity_list_init(list);
}

char entity_symbol(FactionType faction) {
    switch (faction) {
        case FACTION_WOLF:   return 'W';
        case FACTION_SHEEP:  return 'O';
        case FACTION_HUNTER: return 'C';
        default:              return '?';
    }
}

const char* faction_name(FactionType faction) {
    switch (faction) {
        case FACTION_WOLF:   return "Lobo";
        case FACTION_SHEEP:  return "Ovelha";
        case FACTION_HUNTER: return "Cacador";
        default:              return "Desconhecido";
    }
}
