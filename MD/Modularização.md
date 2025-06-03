# Estrutura de Modularização

## 1.  Principal (`main.c`)
- **Responsável pelo fluxo geral do programa**
- Gerencia o menu principal e direciona para os s apropriados
- Contém a função `main()` que inicia o programa

---

## 2.   Interface
- **Responsável por toda a interação com o usuário**

### Funções:
- `exibir_menu_principal()` — Mostra as opções do menu
- `exibir_tutorial()` — Mostra as regras do jogo
- `exibir_historico()` — Mostra o histórico de partidas
- `exibir_tabuleiro()` — Mostra o estado atual do tabuleiro
- `limpar_tela()` — Limpa a tela do console
- `exibir_mensagem()` — Mostra mensagens para o usuário

---

## 3.   Jogo 
- **Contém a lógica principal do jogo**

### Funções:
- `iniciar_jogo_pvp()` — Inicia jogo player vs player
- `iniciar_jogo_pvc()` — Inicia jogo player vs computador
- `gerenciar_turno()` — Controla a alternância de turnos
- `verificar_vitoria()` — Verifica se há um vencedor
- `verificar_empate()` — Verifica condições de empate

---

## 4.Tabuleiro 
- **Gerencia o estado do tabuleiro e movimentações**

### Funções:
- `inicializar_tabuleiro()` — Prepara o tabuleiro para nova partida
- `posicionar_peca()` — Coloca uma peça no tabuleiro (fase 1)
- `mover_peca()` — Move uma peça no tabuleiro (fase 2)
- `validar_movimento()` — Verifica se um movimento é válido
- `obter_movimentos_validos()` — Retorna movimentos possíveis
- `verificar_alinhamento()` — Verifica se há 3 peças alinhadas

---

## 5.IA 
- **Implementa a lógica do computador**

### Funções:
- `escolher_posicionamento()` — Decide onde posicionar peça (fase 1)
- `escolher_movimento()` — Decide como mover peça (fase 2)
- `avaliar_jogadas()` — Avalia as jogadas possíveis

---

## 6.Persistência
- **Gerencia salvamento e carregamento de dados**

### Funções:
- `salvar_jogo()` — Salva o estado atual do jogo
- `carregar_jogo()` — Carrega um jogo salvo
- `salvar_historico()` — Atualiza o histórico de partidas
- `carregar_historico()` — Lê o histórico de partidas

---

## 7.   Utilidades 
- **Funções auxiliares gerais**

### Funções:
- `gerar_numero_aleatorio()` — Para sorteio inicial
- `cronometro_iniciar()` — Inicia contagem de tempo
- `cronometro_pausar()` — Pausa contagem
- `cronometro_continuar()` — Continua contagem
- `cronometro_parar()` — Finaliza contagem

---

## 8.  Gráfico
- **Implementa a interface gráfica com Allegro 5**

### Funções:
- `inicializar_allegro()` — Configura o ambiente gráfico
- `desenhar_tabuleiro()` — Renderiza o tabuleiro
- `desenhar_pecas()` — Renderiza as peças
- `processar_eventos()` — Trata eventos do mouse/teclado
- `finalizar_allegro()` — Libera recursos gráficos

---

## Fluxo Principal

```mermaid
graph TD
    A[Inicialização (main.c)]
    B[Menu Principal (interface.c)]
    C{Seleção de modo de jogo}
    D[PvP (jogo.c + tabuleiro.c)]
    E[PvC (jogo.c + tabuleiro.c + ia.c)]
    F[Fase de posicionamento (tabuleiro.c)]
    G[Fase de movimentação (tabuleiro.c)]
    H[Verificação de fim de jogo (jogo.c)]
    I[Atualização de histórico (persistencia.c)]
    J[Retorno ao menu ou saída]

    A --> B --> C
    C --> D
    C --> E
    D --> F --> G --> H --> I --> J
    E --> F
```

---

