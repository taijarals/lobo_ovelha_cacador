#ifndef TYPES_H
#define TYPES_H

/* tipos e constantes globais do projeto
   trabalho de estruturas de dados - simulacao de ecossistema */

/* faccoes que existem no mapa */
typedef enum {
    FACTION_NONE   = 0,
    FACTION_WOLF   = 1,   /* lobo    - predador, mata ovelha */
    FACTION_SHEEP  = 2,   /* ovelha  - presa do lobo */
    FACTION_HUNTER = 3    /* cacador - caca o lobo, protege ovelha indiretamente */
} FactionType;

/* obstaculos estaticos (nao se movem) */
typedef enum {
    OBSTACLE_NONE = 0,
    OBSTACLE_TREE = 1,
    OBSTACLE_ROCK = 2
} ObstacleType;

/* direcoes possiveis de movimento */
typedef enum {
    DIR_UP    = 0,
    DIR_DOWN  = 1,
    DIR_LEFT  = 2,
    DIR_RIGHT = 3,
    DIR_COUNT = 4
} Direction;

/* porcentagem de cada coisa no mapa */
#define PCT_WOLF        12
#define PCT_SHEEP       12
#define PCT_HUNTER       5
#define PCT_OBSTACLE    10

/* vida inicial de cada entidade */
#define WOLF_HEALTH    100
#define SHEEP_HEALTH    80
#define HUNTER_HEALTH   90

/* regras de interacao */
#define HUNT_CHANCE_PCT  70   /* % de chance do cacador matar o lobo */
#define REPRO_CHANCE_PCT 15   /* % de chance de reproducao */
#define MAX_ENTITIES   10000  /* limite pra nao explodir a memoria */

#define MIN_BOARD_SIZE   5

#endif