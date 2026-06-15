# Relatório Técnico — Simulador de Ecossistema: Lobo, Ovelha e Caçador

## 1. Identificação

- **Disciplina:** Estrutura de Dados
- **Linguagem:** C
- **Tema:** Simulação de ecossistema com três facções (Ovelha, Lobo, Caçador) e obstáculos
- **Arquivo principal:** `ProjetoDeEstrutura_Automatico.c`

---

## 2. Estruturas de Dados Utilizadas

### 2.1 Enumerações (`enum`)

```c
typedef enum { TIME_OVELHA=0, TIME_LOBO=1, TIME_CACADOR=2 } TipoTIME;
typedef enum { OBST_ARVORE=0,  OBST_PEDRA=1  } TipoObstaculo;
typedef enum { CELULA_VAZIA=0, CELULA_ENTIDADE=1, CELULA_OBSTACULO=2 } EstadoCelula;
```

**Finalidade:** Definir de forma legível os tipos de entidade, tipos de obstáculo e o estado de cada célula do tabuleiro. Evita o uso de "números mágicos" e torna o código mais expressivo.

---

### 2.2 Structs

#### `Obstaculo`
```c
typedef struct {
    TipoObstaculo tipo;
    int x, y;
} Obstaculo;
```

**Finalidade:** Representa um obstáculo fixo no tabuleiro (árvore ou pedra). Armazena seu tipo e coordenadas.

**Justificativa:** Estrutura simples e direta. Como obstáculos não se movem nem interagem, não há necessidade de campos adicionais.

---

#### `Entidade`
```c
typedef struct {
    TipoTIME tipo;
    int x, y;
    int energia;
    int idade;
    int cooldownReproducao;
    int morreuPorPredacao;
    int id;
    struct Entidade* proximo;
} Entidade;
```

**Finalidade:** Representa cada ser vivo da simulação (ovelha, lobo ou caçador). Contém atributos de estado (energia, idade) e um ponteiro `proximo` que viabiliza a **lista encadeada** de entidades.

**Justificativa:** A lista encadeada foi escolhida por permitir inserção e remoção dinâmicas de entidades a cada rodada (nascimentos e mortes) sem realocação de memória. O ponteiro `proximo` permite percorrer todas as entidades de forma eficiente.

---

#### `LogRodada`
```c
typedef struct {
    char linhas[MAX_EVENTOS][TAM_EVENTO];
    int total;
} LogRodada;
```

**Finalidade:** Armazena os eventos textuais ocorridos em cada rodada (predações, proteções, nascimentos, mortes).

**Justificativa:** Matriz bidimensional de caracteres com capacidade fixa (256 eventos de até 128 caracteres). Optou-se por tamanho fixo para evitar alocação dinâmica frequente, já que o número de eventos por rodada é limitado.

---

#### `Estatisticas`
```c
typedef struct {
    int mortesPorFome[3];
    int mortesPorPredacao[3];
    int nascimentos[3];
    int predacoesLobo;
    int eliminacoesCacador;
    int protecoes;
    int movimentos;
    int rodadasComReproducao;
    int pico[3];
} Estatisticas;
```

**Finalidade:** Acumula estatísticas globais da partida para exibição ao final.

**Justificativa:** Vetores de tamanho 3 para cada facção permitem acesso direto por índice do enum `TipoTIME`. Estrutura plana e de fácil acesso.

---

#### `Tabuleiro`
```c
typedef struct {
    int largura, altura;
    EstadoCelula** estado;
    void***        conteudo;
    Entidade*      listaEntidades;
    int            contagemPorTipo[3];
    int            totalObstaculos;
    int            rodadaAtual;
    int            proximoId;
    LogRodada      log;
    Estatisticas   stats;
} Tabuleiro;
```

**Finalidade:** Estrutura central que agrega todos os dados da simulação: o grid do tabuleiro, a lista de entidades, contadores, log e estatísticas.

**Justificativa:**
- `estado[linha][coluna]` — matriz de enum que indica rapidamente se uma célula está vazia, ocupada por entidade ou por obstáculo. Permite consulta O(1) para verificação de colisões.
- `conteudo[linha][coluna]` — matriz de ponteiros genéricos (`void*`) que armazena o ponteiro para a `Entidade` ou `Obstaculo` naquela posição. O uso de `void*` permite tratar ambos os tipos na mesma matriz.
- `listaEntidades` — cabeça da lista encadeada de entidades, permitindo percorrer todas as entidades a cada rodada sem varrer o grid inteiro.

---

## 3. Estratégia da Solução

### 3.1 Fluxo Principal

```
1. Usuário informa o tamanho N do tabuleiro (mínimo 5)
2. Tabuleiro é alocado dinamicamente (matriz N×N)
3. Entidades e obstáculos são posicionados aleatoriamente
4. Loop principal executa até condição de vitória:
   a. Movimentar todas as entidades
   b. Processar interações (predação/proteção)
   c. Remover entidades mortas
   d. Processar reprodução
   e. Avançar idade e cooldowns
   f. Exibir estado atual
5. Exibir estatísticas finais e liberar memória
```

### 3.2 Posicionamento Inicial

As quantidades são calculadas com base no tamanho do tabuleiro:
- **Ovelhas:** 12% das células
- **Lobos:** 12% das células
- **Caçadores:** 5% das células
- **Obstáculos:** 10% das células (alternando entre árvore e pedra)

O algoritmo `sortearPosicaoLivre` utiliza um vetor auxiliar de células ocupadas para garantir posicionamento sem sobreposição.

### 3.3 Movimentação

Cada entidade tenta mover-se em uma direção aleatória (cima, baixo, esquerda, direita). Uma ordem aleatória das 4 direções é gerada por embaralhamento (Fisher-Yates) para evitar viés direcional. A movimentação consome 1 de energia.

### 3.4 Interações

As interações são verificadas por vizinhança (4 direções):

| Interação | Ação | Efeito |
|---|---|---|
| Lobo + Ovelha | Predação | Ovelha morre, lobo ganha +20 energia |
| Caçador + Lobo | Predação | Lobo morre, caçador ganha +10 energia |
| Caçador + Ovelha | Proteção | Caçador perde 5 energia, ovelha ganha +10 energia |

### 3.5 Reprodução

Duas entidades do mesmo tipo, vizinhas, com idade ≤ 10 e cooldown = 0 podem gerar um filho. A nova entidade é posicionada em uma célula vazia vizinha a um dos pais.

### 3.6 Senilidade e Morte

Entidades com idade > 15 perdem 2 de energia por rodada (senilidade). Entidades com energia ≤ 0 são removidas da lista encadeada e suas células são liberadas.

### 3.7 Condição de Vitória

A simulação termina quando resta apenas 1 facção viva ou quando o limite de 1000 rodadas é atingido.

---

## 4. Justificativa das Escolhas Técnicas

### 4.1 Matriz de Estado + Matriz de Conteúdo

O uso de duas matrizes separadas (uma para o tipo de célula e outra para o ponteiro) permite consultas rápidas de ocupação (O(1)) sem necessidade de percorrer a lista de entidades. A matriz de ponteiros genéricos (`void*`) evita duplicação de estruturas.

### 4.2 Lista Encadeada para Entidades

A lista encadeada simples foi escolhida porque:
- **Inserções e remoções são frequentes** (nascimentos e mortes a cada rodada)
- **Não há necessidade de acesso indexado** — todas as entidades são percorridas sequencialmente
- **Crescimento dinâmico** — não há desperdício de memória com tamanho fixo

### 4.3 Array Fixo para Log

O log usa uma matriz de tamanho fixo (256×128). Embora menos flexível que uma lista dinâmica, evita fragmentação de memória e alocações/desalocações constantes. O limite de 256 eventos por rodada é suficiente para tabuleiros de até tamanho razoável.

### 4.4 Alocação Dinâmica do Tabuleiro

O tabuleiro é alocado como um array de ponteiros para linhas, permitindo que o tamanho seja definido em tempo de execução. A função `criarTabuleiro` aloca `N` linhas de `N` colunas, resultando em uma matriz N×N contiguous em cada linha (mas não entre linhas).

---

## 5. Análise de Memória

### 5.1 Memória Alocada Estaticamente

| Variável | Tamanho |
|---|---|
| `dx`, `dy` | 4 × `sizeof(int)` cada |
| `ord[4]` | 4 × `sizeof(int)` |
| Constantes (#define) | 0 (pré-processador) |

### 5.2 Memória Alocada Dinamicamente

#### Tabuleiro (`criarTabuleiro`)
```
sizeof(Tabuleiro) ≈ 48 bytes (struct)
+ N × sizeof(EstadoCelula*)      — linhas da matriz estado
+ N × sizeof(void**)              — linhas da matriz conteúdo
+ N × N × sizeof(EstadoCelula)    — células da matriz estado
+ N × N × sizeof(void*)           — células da matriz conteúdo
```

Exemplo para N = 10:
```
48 + 10×8 + 10×8 + 100×1 + 100×8 = 48 + 80 + 80 + 100 + 800 ≈ 1108 bytes
```

#### Entidades
```
sizeof(Entidade) ≈ 48 bytes cada
Total = 48 × (QtdOvelhas + QtdLobos + QtdCacadores)
Para N = 10: 48 × (12 + 12 + 5) = 48 × 29 = 1392 bytes
```

#### Obstáculos
```
sizeof(Obstaculo) ≈ 12 bytes cada
Total = 12 × QtdObstaculos
Para N = 10: 12 × 10 = 120 bytes
```

### 5.3 Vetor Auxiliar (`ocupado`)
```
N × N × sizeof(int) bytes — alocado temporariamente no posicionamento inicial
Para N = 10: 100 × 4 = 400 bytes
```

### 5.4 Análise de Escalabilidade

#### Cenário Pequeno (N = 10, 10 rodadas)
- Tabuleiro: ~1.1 KB
- Entidades (29): ~1.4 KB
- Obstáculos (10): ~120 bytes
- **Total aproximado: ~2.6 KB**

#### Cenário Médio (N = 20, 50 rodadas)
- Tabuleiro: ~4.2 KB
- Entidades (116): ~5.6 KB
- Obstáculos (40): ~480 bytes
- **Total aproximado: ~10.3 KB**

#### Cenário Grande (N = 50, 100 rodadas)
- Tabuleiro: ~26 KB
- Entidades (725): ~34.8 KB
- Obstáculos (250): ~3 KB
- **Total aproximado: ~64 KB**

#### Cenário Extremo (N = 100, 1000 rodadas)
- Tabuleiro: ~104 KB
- Entidades (2900): ~139 KB
- Obstáculos (1000): ~12 KB
- **Total aproximado: ~255 KB**

O crescimento é **quadrático** em relação ao tamanho do tabuleiro (O(N²) para as matrizes) e **linear** em relação ao número de entidades.


## 6. Principais Dificuldades

1. **Gerenciamento de memória:** Garantir que toda memória alocada fosse liberada corretamente, especialmente ao remover entidades da lista encadeada durante a iteração.
2. **Sincronização das matrizes:** Manter a consistência entre `estado[][]`, `conteudo[][]` e a `listaEntidades` durante movimentação e remoção.
3. **Aleatoriedade sem viés:** Implementar o embaralhamento Fisher-Yates para garantir distribuição uniforme das direções de movimento.
4. **Reprodução:** Evitar que a mesma entidade se reproduza múltiplas vezes na mesma rodada (uso do cooldown) e encontrar células vizinhas livres para o filho.
5. **Proteção:** Implementar a lógica de proteção do caçador à ovelha sem que isso impedisse a predação lobo→ovelha (a proteção só ocorre se caçador e ovelha forem vizinhos, e consome energia do caçador).


## 7. Diagrama de Fluxo da Simulação

```
        ┌──────────────────────────────────────────────────┐
        │                   INÍCIO                         │
        │   scanf(tamanho) → criarTabuleiro(N)             │
        │   posicionarAleatorio()                          │
        └────────────────────┬─────────────────────────────┘
                             │
                             ▼
              ┌──────────────────────────────┐
              │    LOOP PRINCIPAL            │
              │  while (!verificarVitoria()) │
              └──────────┬───────────────────┘
                         │
              ┌──────────▼──────────┐
              │  movimentarTodos()   │
              │  (aleatório, 4 dir)  │
              └──────────┬───────────┘
                         │
              ┌──────────▼───────────┐
              │ processarInteracoes() │
              │  predação/proteção    │
              └──────────┬────────────┘
                         │
              ┌──────────▼─────────────┐
              │ removerEntidadesMortas() │
              │  (energia ≤ 0)          │
              └──────────┬──────────────┘
                         │
              ┌──────────▼──────────────┐
              │ processarReproducao()    │
              │  (vizinhos mesmo tipo)   │
              └──────────┬───────────────┘
                         │
              ┌──────────▼──────────────┐
              │  avancarIdade()          │
              │  (+1 idade, cooldown)    │
              └──────────┬───────────────┘
                         │
              ┌──────────▼──────────────┐
              │  imprimirTabuleiro()     │
              │  imprimirLogRodada()     │
              └──────────┬───────────────┘
                         │
                         ▼
              Condição de vitória?
              ┌─────┴─────┐
              │           │
             SIM         NÃO
              │           │
              ▼           └───────────────┐
     ┌─────────────────┐                  │
     │ imprimirEstatis- │                  │
     │ ticasFinais()    │                  │
     │ liberarTabuleiro │                  │
     └────────┬─────────┘                  │
              │                            │
              ▼                            │
             FIM                           │
              └────────────────────────────┘
```


## 8. Conclusão

O simulador implementa todas as mecânicas exigidas: movimentação aleatória, predação (lobo→ovelha e caçador→lobo), proteção (caçador→ovelha), reprodução entre entidades do mesmo tipo, obstáculos fixos, senilidade e estatísticas detalhadas. As estruturas de dados foram escolhidas para equilibrar simplicidade de implementação com desempenho adequado para tabuleiros de até tamanho moderado. A lista encadeada de entidades, combinada com matrizes de estado, permite operações O(1) para consulta de células e O(n) para iteração sobre entidades, onde n é o número de entidades vivas.
