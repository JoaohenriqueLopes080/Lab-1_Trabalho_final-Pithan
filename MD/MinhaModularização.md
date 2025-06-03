# Guia Básico de Markdown

Markdown é uma linguagem de marcação simples para formatar textos. Veja abaixo os comandos mais usados:

## Títulos

Use `#` para criar títulos:

```markdown
# Título 1
## Título 2
### Título 3
```

## Ênfase

- **Negrito:** `**texto**` → **texto**
- *Itálico:* `*texto*` → *texto*
- ~~Riscado:~~ `~~texto~~` → ~~texto~~

## Listas

**Lista não ordenada:**
```markdown
- Item 1
- Item 2
    - Subitem
```

**Lista ordenada:**
```markdown
1. Primeiro
2. Segundo
```

## Links e Imagens

- [Link](https://www.markdownguide.org): `[texto](url)`
- ![Imagem](https://via.placeholder.com/100): `![alt](url)`

## Códigos

- `Código em linha`: `` `código` ``
- Bloco de código:
        ```markdown
        ```
        seu código aqui
        ```
        ```

## Citações

> Use `>` para criar citações.

## Tabelas

```markdown
| Coluna 1 | Coluna 2 |
|----------|----------|
| Valor 1  | Valor 2  |
```

---

Explore e combine esses comandos para criar documentos incríveis em Markdown!



# Minha Modularização Do Trabalho

## Tri-Angle

### Menu Principal
**Lista ordenada:**
```markdown
1. Jogar
2. ajuda
3. Historico
4. Sair


## Modos

## Ao clicar em jogar a tela atualiza e aparece esses dois modos.

### Jogador Vs Jogador

### Jogador vs Computador

### Fim De Jogo.

## O jogador que conseguir alinhar três peças é o vencedor. Vale lembrar que o jogo não permite captura
## de peças. O jogo pode durar muito tempo sem que haja um vencedor. Neste caso, os jogadores devem
## decidir pelo empate

## Fim de Modos


# AJUDA

## Ajuda é um tutorial.
### como é muito texto e não ha interação com essa parte, colocar todo o tutorial em uma foto png e colocar ela para aparecer
## deixar um retangulo escrito retornar ao menu principal no canto inferior esquerdo.

## fim ajuda


## Historico

## deve exibir um histórico sobre as partidas anteriores
## daquele jogador, identificando, para cada opção de jogo (player x player e player x computador) o
## número de partidas jogadas e em que situação cada uma terminou. Além disso, o histórico deve exibir
## o maior e o menor tempo decorrido entre as partidas de cada opção de jogo. Para essa informação, o
## programa deve contar com um timer para medir a duração de cada partida. O histórico deve ser salvo
## em um arquivo, a fim de ser exibido a qualquer momento

## fim Historico

## Sair

## ao clicar fecha o jogo 

## fim sair


# Recursos


## 2 Recursos


**Lista ordenada:**
```markdown
1. Pausar
2. Salvar
```
## 1 - Pausar

### implica em parar o jogo e depois reiniciar de onde parou.
### É importante observar que, neste caso, o timer que contabiliza a duração da partida, deve ser pausado
### e depois reiniciado de onde parou.

## 2 - Salvar

### deve permitir armazenar o jogo em andamento em um arquivo (diferente do
### arquivo do Histórico), a fim de retomar em um momento posterior. Somente é possível salvar a última
### partida por opção de jogo (no caso, a última partida para a qual foi solicitado o salvamento). Sendo
### assim, o usuário pode ter duas partidas salvas: uma para a opção player x player e outra para a opção
### player x computador. Para o salvamento de uma partida em andamento, deve-se armazenar, pelo
### menos: a opção de jogo (player x player e player x computador), o estado do tabuleiro (contendo as
### posições de todas as peças), o número e cores de peças fora do tabuleiro (para o caso de pausa na
### primeira etapa de jogo), indicação do próximo jogador e o valor do timer. Estas informações devem
### estar disponíveis para o programa quando o jogador quiser retomar o jogo interrompido. A retomada
### do jogo pode consistir em mais uma opção do menu descrito anteriormente

# OUTPUT

### deve se preocupar com a exibição do andamento do jogo ( output ), de modo a
### permitir seu acompanhamento de forma organizada e adequada. Neste sentido, o programa deve exibir
### todo o fluxo de andamento da partida, indicando cada movimentação.
### Por exemplo, supondo uma implementação com matriz, em que as linhas são identificadas com letras
### e as colunas com números. Uma possível exibição de jogada seria o jogador com peças amarelas
### movimentou a peça B1 para a posição C1, ou jogador com peças vermelhas movimentou a peça C1
### para a posição C2. 


### A avaliação irá levar em conta o modo como a exibição do jogo foi implementada. 


## feitos - 1 de junho
## AJUDA, SAIDA E conhecimento de allegro na base.
