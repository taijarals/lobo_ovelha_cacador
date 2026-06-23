# Simulador de Ecossistema em C

Trabalho de Estruturas de Dados. Simula um ecossistema com três facções
(lobos, ovelhas e caçadores) num tabuleiro, com movimento aleatório,
combate, proteção e reprodução. A cadeia alimentar é: Caçador → Lobo → Ovelha.
A simulação acaba quando só sobra uma facção ou o limite de rodadas é atingido.

---

## Como compilar e rodar

Este é um programa em C feito para rodar no terminal Linux. Para isso, é
preciso ter `gcc` e `make` instalados.

> **Se você usa Windows:** o jeito mais simples é instalar o **Ubuntu** através
> do WSL (Windows Subsystem for Linux). Veja o passo a passo completo na seção
> [Como rodar no Windows usando Ubuntu (WSL)](#como-rodar-no-windows-usando-ubuntu-wsl)
> mais abaixo.

```bash
# compilar
make

# compilar e já executar
make run

# compilar com sanitizers (detecta vazamento de memória e undefined behavior)
make debug
./ecosistema_debug

# limpar os binários
make clean
```

---

## Como rodar no Windows usando Ubuntu (WSL)

Este programa é em C puro, compilado com `gcc` e `make` — ferramentas que
existem nativamente no **Linux**. O Windows não tem isso por padrão, então a
forma mais simples (e gratuita, e oficial) de ter um Linux funcionando dentro
do seu Windows é instalar o **Ubuntu via WSL** (Windows Subsystem for Linux).
Não precisa de máquina virtual, não precisa de pendrive, não precisa formatar
nada — o Ubuntu passa a abrir como um programa normal, só que por dentro é um
Linux de verdade.

### Resumo rápido (se você já souber o que está fazendo)

```powershell
# 1. No PowerShell como Administrador:
wsl --install
# 2. reinicie o PC, abra "Ubuntu" no menu Iniciar e crie usuário/senha
# 3. dentro do Ubuntu:
sudo apt update && sudo apt install build-essential -y
cd /mnt/c/Users/Erik/Downloads/trabalho_estr_dados
make run
```

Se preferir o passo a passo completo e explicado, continue lendo.

---

### Passo 1 — Instalar o WSL e o Ubuntu

1. Clique no menu **Iniciar**, digite `PowerShell`, clique com o botão
   direito em **Windows PowerShell** e escolha **"Executar como
   administrador"**.
2. Na janela azul que abrir, digite o comando abaixo e aperte Enter:
   ```powershell
   wsl --install
   ```
3. Esse comando faz tudo sozinho: ativa o WSL no Windows e já baixa e instala
   o **Ubuntu** (é a distribuição Linux que vem por padrão). Pode demorar
   alguns minutos, dependendo da internet.
4. Quando terminar, ele vai pedir para **reiniciar o computador**. Reinicie.

> **Deu erro no comando `wsl --install`?** Normalmente é porque o Windows
> está desatualizado (esse comando precisa do Windows 10 versão 2004+ ou
> Windows 11). Nesse caso:
> 1. Abra a **Microsoft Store**, procure por **"Ubuntu"** e instale.
> 2. Aperte `Win + R`, digite `optionalfeatures` e Enter.
> 3. Marque a caixa **"Subsistema do Windows para Linux"** e clique OK.
> 4. Reinicie o computador e abra o Ubuntu pela Microsoft Store/menu Iniciar.

### Passo 2 — Primeira abertura do Ubuntu

1. Depois de reiniciar, procure por **"Ubuntu"** no menu Iniciar e abra.
2. Na primeira vez, uma tela preta vai aparecer instalando os últimos
   arquivos (só na primeira abertura, demora um pouco).
3. Em seguida ele vai pedir para criar um usuário Linux:
   ```
   Enter new UNIX username: 
   New password: 
   Retype new password: 
   ```
   Pode digitar qualquer nome de usuário e qualquer senha — é só local,
   ninguém mais vai ver. **Atenção:** ao digitar a senha, nada aparece na
   tela (nem `*`), isso é normal no Linux, é só confirmação visual mesmo.
4. Depois disso você já está dentro de um terminal Ubuntu, algo como:
   ```
   seu_usuario@DESKTOP-XXXX:~$
   ```
   Esse `~$` é o terminal esperando você digitar comandos.

> Da próxima vez, não precisa repetir nada disso — só abrir "Ubuntu" de novo
> no menu Iniciar que ele já abre direto no terminal.

### Passo 3 — Instalar o compilador (gcc e make)

Ainda dentro do terminal Ubuntu, rode estes três comandos, um por vez
(aperte Enter depois de cada um e espere terminar):

```bash
sudo apt update
sudo apt upgrade -y
sudo apt install build-essential -y
```

- `sudo` = "rodar como administrador" (ele vai pedir a senha que você criou
  no passo 2 — pode digitar, mesmo sem aparecer nada na tela, e Enter).
- `build-essential` é um pacote que já traz o `gcc` (compilador de C) e o
  `make` (ferramenta de automação de build) de uma vez.

Para confirmar que instalou certo:

```bash
gcc --version
make --version
```

Se aparecer um número de versão em cada um, está tudo certo.

### Passo 4 — Encontrar a pasta do projeto

O Ubuntu dentro do WSL consegue ver os arquivos do Windows normalmente. Os
discos do Windows aparecem dentro de `/mnt/`. Por exemplo:

| Caminho no Windows                                  | Caminho equivalente no Ubuntu (WSL)                     |
|--------------------------------------------------------|-----------------------------------------------------------|
| `C:\Users\...\Downloads\trabalho_estr_dados`         | `/mnt/c/Users/.../Downloads/trabalho_estr_dados`         |
| `D:\Faculdade\trabalho_estr_dados`                    | `/mnt/d/Faculdade/trabalho_estr_dados`                    |

Basta trocar `C:\` por `/mnt/c/`, `D:\` por `/mnt/d/`, e trocar as barras `\`
por `/`. Dentro do terminal Ubuntu, use `cd` (change directory) para entrar
na pasta:

```bash
cd /mnt/c/Users/.../Downloads/trabalho_estr_dados
```

Dica: se não souber o caminho exato, abra a pasta no Explorador de Arquivos
do Windows, clique na barra de endereço para copiar o caminho, e adapte ele
para o formato `/mnt/...` acima.

Para confirmar que está na pasta certa, rode `ls` (lista os arquivos):

```bash
ls
```

Você deve ver algo como `Makefile  README.md  include  src` na resposta.

### Passo 5 — Compilar e jogar

Ainda dentro da pasta do projeto, no terminal Ubuntu:

```bash
make
```

Isso compila o programa. Se quiser compilar **e já executar** de uma vez:

```bash
make run
```

O programa vai perguntar o tamanho do tabuleiro e o número de rodadas, por
exemplo:

```
Numero de linhas  (minimo 5): 10
Numero de colunas (minimo 5): 10
Numero de rodadas (minimo 1) : 30
```

Depois disso a simulação roda sozinha e o tabuleiro vai aparecendo no
terminal a cada rodada — não precisa apertar mais nada.

Para rodar de novo sem recompilar:

```bash
./ecosistema
```

Para limpar os arquivos compilados (caso queira recompilar do zero):

```bash
make clean
```

### Solução de problemas comuns

| Problema                                                | Solução                                                                 |
|----------------------------------------------------------|--------------------------------------------------------------------------|
| `make: command not found`                               | O `build-essential` não foi instalado — repita o Passo 3.                |
| `Permission denied` ao rodar `./ecosistema`              | Rode `chmod +x ecosistema` e tente de novo.                              |
| `bash: cd: No such file or directory`                    | Confira o caminho — Windows usa `\`, Ubuntu usa `/`, e tudo é minúsculo importa (case-sensitive). |
| Makefile dá erro estranho de sintaxe                     | O arquivo pode ter vindo com quebra de linha do Windows. Rode `sudo apt install dos2unix -y` e depois `dos2unix Makefile`. |
| Quero voltar a editar no VS Code normalmente             | Instale a extensão **"WSL"** no VS Code, depois rode `code .` de dentro da pasta no terminal Ubuntu — ele abre o VS Code já conectado ao Linux. |

---

## Estrutura de arquivos

```
trabalho_estr_dados/
├── include/          ← cabeçalhos (.h)
│   ├── types.h       ← enums e constantes
│   ├── entity.h      ← struct Entity + lista encadeada
│   ├── board.h       ← struct Cell + tabuleiro 2D
│   ├── movement.h    ← movimentação
│   ├── interaction.h ← combate, proteção, reprodução
│   ├── simulation.h  ← loop principal
│   └── display.h     ← saída no terminal
├── src/              ← implementações (.c)
│   ├── main.c
│   ├── entity.c
│   ├── board.c
│   ├── movement.c
│   ├── interaction.c
│   ├── simulation.c
│   └── display.c
├── obj/              ← arquivos .o (gerado pelo make)
├── Makefile
└── README.md
```

---

## Regras do ecossistema

### Facções

| Símbolo | Facção   | % do mapa | Papel                         |
|---------|----------|-----------|-------------------------------|
| `W`     | Lobo     | 12%       | Predador — mata ovelha        |
| `O`     | Ovelha   | 12%       | Presa — vulnerável ao lobo    |
| `C`     | Caçador  | 5%        | Caca o lobo, protege a ovelha |

### Cadeia alimentar

```
Caçador → Lobo → Ovelha
```

### Obstáculos

| Símbolo | Tipo   | % do mapa |
|---------|--------|-----------|
| `T`     | Árvore | 5%        |
| `R`     | Pedra  | 5%        |

### Interações por rodada

1. **Predação** — lobo adjacente a ovelha → mata a ovelha diretamente
2. **Predação** — caçador adjacente a lobo → 70% de chance de matar o lobo
3. **Proteção** — caçador protege a ovelha indiretamente eliminando lobos
4. **Vulnerabilidade** — ovelha é vulnerável ao lobo; lobo é vulnerável ao caçador
5. **Reprodução** — duas entidades da mesma facção adjacentes → 15% de chance de criar filho

---

## Estruturas de dados utilizadas

### Lista duplamente encadeada (Entity)
Usei lista encadeada porque a remoção de entidades mortas no meio é O(1) com
os ponteiros `prev`/`next`. Com array fixo teria que deslocar elementos,
e com lista simples precisaria percorrer até o nó anterior.

A remoção é feita em duas fases (mark-and-sweep) para não invalidar ponteiros
durante o loop de interações.

### Matriz 2D dinâmica (Board)
O tabuleiro é um `Cell**` (array de arrays). O acesso `grid[r][c]` é O(1)
e a alocação com `calloc` já inicializa tudo com zero.

### Contadores redundantes (EntityList)
Mantenho contadores separados de lobos/ovelhas/caçadores. Isso deixa a
verificação de vitória O(1) sem precisar percorrer a lista inteira.

---

## Constantes configuráveis (include/types.h)

| Constante          | Padrão | Descrição                         |
|--------------------|--------|-----------------------------------|
| `PCT_WOLF`         | 12     | % do mapa com lobos               |
| `PCT_SHEEP`        | 12     | % do mapa com ovelhas             |
| `PCT_HUNTER`       | 5      | % do mapa com caçadores           |
| `PCT_OBSTACLE`     | 10     | % do mapa com obstáculos          |
| `HUNT_CHANCE_PCT`  | 70     | % de chance do caçador matar lobo |
| `REPRO_CHANCE_PCT` | 15     | % de reprodução por par adjacente |
| `MAX_ENTITIES`     | 10000  | limite máximo de entidades        |
| `MIN_BOARD_SIZE`   | 5      | tamanho mínimo do tabuleiro       |