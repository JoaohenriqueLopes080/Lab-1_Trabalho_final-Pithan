#include <stdio.h>
#include <stdlib.h> // Para system()
#include <stdbool.h> // Para usar bool
#include <string.h> // Para strcpy, strcmp
#include <time.h> // Para time() e clock()
#include <math.h> // Para sqrt() e fmin()

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_image.h>

// --- Estruturas de Dados ---
typedef enum {
    CASA_VAZIA,
    PECA_JOGADOR1, // Vermelho
    PECA_JOGADOR2  // Azul
} CasaEstado;

typedef struct {
    float x;
    float y;
} PontoTabuleiro;

typedef enum {
    ESTADO_MENU,
    ESTADO_JOGANDO_PX_P,
    ESTADO_JOGANDO_PX_COMP,
    ESTADO_AJUDA,
    ESTADO_HISTORICO,
    ESTADO_PAUSADO,
    ESTADO_SAIR,
    ESTADO_CARREGAR_JOGO_SELECAO, // Estado para escolher qual jogo carregar
    ESTADO_FIM_DE_JOGO // Novo estado para exibir tela de vitória/derrota/empate
} GameState;

typedef enum {
    ETAPA_POSICIONAMENTO,
    ETAPA_MOVIMENTACAO
} EtapaJogo;

typedef enum {
    JOGADOR_HUMANO,
    JOGADOR_COMPUTADOR
} TipoJogador;

typedef enum {
    JOGADOR_VS_JOGADOR,
    JOGADOR_VS_COMPUTADOR
} ModoJogo;

typedef struct {
    int vitorias_pxp;
    int derrotas_pxp;
    int empates_pxp;
    double menor_tempo_pxp;
    double maior_tempo_pxp;

    int vitorias_pxcomp;
    int derrotas_pxcomp;
    int empates_pxcomp;
    double menor_tempo_pxcomp;
    double maior_tempo_pxcomp;
} HistoricoTotal;

typedef struct {
    ModoJogo modo;
    CasaEstado tabuleiro[8]; // 8 casas conforme a imagem
    int pecas_fora_j1; // Quantas peças J1 ainda não foram colocadas
    int pecas_fora_j2; // Quantas peças J2 ainda não foram colocadas
    int jogador_atual; // 1 ou 2
    EtapaJogo etapa_atual;
    double tempo_decorrido; // Tempo decorrido na partida salva
} JogoSalvo;

// --- Estrutura para o Contexto do Jogo (substitui variáveis globais e defines) ---
typedef struct {
    // Recursos Allegro
    ALLEGRO_DISPLAY *display;
    ALLEGRO_EVENT_QUEUE *event_queue;
    ALLEGRO_FONT *font;
    ALLEGRO_FONT *font2;
    ALLEGRO_BITMAP *background_image;
    ALLEGRO_BITMAP *help_image;
    ALLEGRO_BITMAP *play_modes_image;
    ALLEGRO_BITMAP *history_image; // Pode ser necessário para a tela de histórico

    // Constantes do Jogo (anteriormente defines)
    int largura_tela;
    int altura_tela;
    ALLEGRO_COLOR cor_fundo;
    ALLEGRO_COLOR cor_pontos;
    ALLEGRO_COLOR cor_linhas;
    ALLEGRO_COLOR cor_peca_j1;
    ALLEGRO_COLOR cor_peca_j2;
    float raio_ponto;
    float raio_peca;
    int max_historico;
    char arquivo_historico[50];
    char arquivo_salvo_pxp[50];
    char arquivo_salvo_pxcomp[50];
    char clear_command[10]; // "cls" ou "clear"

    // Estado do Jogo
    GameState current_game_state;
    CasaEstado tabuleiro_logico[8];
    PontoTabuleiro pontos_tabuleiro[8];
    int jogador_atual; // 1 para Jogador 1, 2 para Jogador 2
    EtapaJogo etapa_atual;
    int pecas_fora_jogador1;
    int pecas_fora_jogador2;
    int casa_selecionada; // -1 se nenhuma casa selecionada, ou índice da casa
    ModoJogo modo_jogo_atual;
    char mensagem_fim_de_jogo[100]; // Para exibir mensagem de vitória/derrota/empate

    // Timers
    clock_t inicio_partida_timer;
    double tempo_pausado_total;
    clock_t inicio_pausa_timer;

    // Histórico de partidas
    HistoricoTotal historico_global;
} GameContext;

// --- Protótipos de Funções ---
void limpaTela(GameContext *ctx);
void exibir_mensagem(GameContext *ctx, const char *msg);
void calcular_coordenadas_tabuleiro(GameContext *ctx, PontoTabuleiro pontos[8]);
void desenhar_tabuleiro_allegro(GameContext *ctx);
int obter_casa_por_coordenadas(GameContext *ctx, float mouse_x, float mouse_y);
void inicializar_tabuleiro_logico(CasaEstado tabuleiro[8]);
bool validar_movimento(const CasaEstado tabuleiro[8], int origem, int destino);
bool verificar_vitoria(const CasaEstado tabuleiro[8], int jogador);
void trocar_jogador(GameContext *ctx);
void avancar_etapa_se_necessario(GameContext *ctx);
void jogar_ia(GameContext *ctx);
void iniciar_novo_jogo(GameContext *ctx, ModoJogo modo);
void carregar_historico(GameContext *ctx);
void salvar_historico(GameContext *ctx);
void registrar_partida(GameContext *ctx, ModoJogo modo, int vencedor, double tempo_partida);
void salvar_jogo(GameContext *ctx);
bool carregar_jogo(GameContext *ctx, ModoJogo modo_carregar);

// Funções de UI
void desenhar_menu(GameContext *ctx);
void desenhar_menu_jogar_modos(GameContext *ctx);
void desenhar_ajuda(GameContext *ctx);
void desenhar_historico_tela(GameContext *ctx);
void desenhar_pausa(GameContext *ctx);
void desenhar_fim_de_jogo(GameContext *ctx);
void desenhar_menu_carregar_jogo_selecao(GameContext *ctx);

// Funções de processamento de eventos
void processar_eventos_menu(GameContext *ctx, ALLEGRO_EVENT *event);
void processar_eventos_jogar(GameContext *ctx, ALLEGRO_EVENT *event);
void processar_eventos_ajuda(GameContext *ctx, ALLEGRO_EVENT *event);
void processar_eventos_historico(GameContext *ctx, ALLEGRO_EVENT *event);
void processar_eventos_pausa(GameContext *ctx, ALLEGRO_EVENT *event);
void processar_eventos_fim_de_jogo(GameContext *ctx, ALLEGRO_EVENT *event);
void processar_eventos_carregar_jogo_selecao(GameContext *ctx, ALLEGRO_EVENT *event);

// --- Implementações de Funções ---

// Limpa a tela do console
void limpaTela(GameContext *ctx) {
    system(ctx->clear_command);
}

// Exibe mensagens no console e no display Allegro
void exibir_mensagem(GameContext *ctx, const char *msg) {
    limpaTela(ctx);
    printf("%s\n", msg);
    // Para Allegro, pode exibir uma mensagem na tela gráfica também
    al_draw_bitmap(ctx->background_image, 0, 0, 0); // Desenha o fundo antes da mensagem
    al_draw_text(ctx->font, al_map_rgb(0, 0, 0), ctx->largura_tela / 2, ctx->altura_tela / 2 - 20, ALLEGRO_ALIGN_CENTER, msg);
    al_draw_text(ctx->font, al_map_rgb(0, 0, 0), ctx->largura_tela / 2, ctx->altura_tela / 2 + 20, ALLEGRO_ALIGN_CENTER, "Pressione qualquer tecla para continuar...");
    al_flip_display();
    ALLEGRO_EVENT ev;
    al_wait_for_event(ctx->event_queue, &ev); // Espera por qualquer evento para continuar
    while(ev.type != ALLEGRO_EVENT_KEY_DOWN && ev.type != ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && ev.type != ALLEGRO_EVENT_DISPLAY_CLOSE) {
        al_wait_for_event(ctx->event_queue, &ev);
    }
    if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
        ctx->current_game_state = ESTADO_SAIR; // Permite sair do jogo se a janela for fechada durante uma mensagem
    }
}

// Função para calcular as coordenadas dos 8 pontos do tabuleiro triangular
void calcular_coordenadas_tabuleiro(GameContext *ctx, PontoTabuleiro pontos[8]) {
    float lado_triangulo_maior = fmin(ctx->largura_tela * 0.7, ctx->altura_tela * 0.7 / (sqrt(3.0f) / 2.0f));
    float altura_triangulo_maior = lado_triangulo_maior * (sqrt(3.0f) / 2.0f);

    float x_centro_area = ctx->largura_tela * 0.75f; // Ajuste para posicionar o tabuleiro mais à direita
    float y_centro_area = ctx->altura_tela / 2.0f;

    // P0: Vértice Superior
    pontos[0].x = x_centro_area;
    pontos[0].y = y_centro_area - (altura_triangulo_maior / 2.0f);

    // P3: Vértice Inferior Esquerdo
    pontos[3].x = x_centro_area - (lado_triangulo_maior / 2.0f);
    pontos[3].y = y_centro_area + (altura_triangulo_maior / 2.0f);

    // P5: Vértice Inferior Direito
    pontos[5].x = x_centro_area + (lado_triangulo_maior / 2.0f);
    pontos[5].y = y_centro_area + (altura_triangulo_maior / 2.0f);

    // Linha horizontal do meio: 50% da altura do triângulo, 50% da largura da base
    float y_linha_meio = pontos[0].y + (altura_triangulo_maior / 2.0f);
    float lado_triangulo_interno = lado_triangulo_maior / 2.0f;

    // P1: Ponto médio esquerdo superior (meio do lado esquerdo do triângulo grande)
    pontos[1].x = x_centro_area - (lado_triangulo_interno / 2.0f);
    pontos[1].y = y_linha_meio;

    // P2: Ponto médio direito superior (meio do lado direito do triângulo grande)
    pontos[2].x = x_centro_area + (lado_triangulo_interno / 2.0f);
    pontos[2].y = y_linha_meio;

    // P6: Ponto central da linha horizontal do meio (entre P1 e P2)
    pontos[6].x = x_centro_area;
    pontos[6].y = y_linha_meio;

    // P4: Ponto central da base
    pontos[4].x = x_centro_area;
    pontos[4].y = pontos[3].y; // Mesma altura dos vértices da base

    // P7: Ponto central do triângulo (interseção das medianas do triângulo maior)
    pontos[7].x = (pontos[0].x + pontos[3].x + pontos[5].x) / 3.0f;
    pontos[7].y = (pontos[0].y + pontos[3].y + pontos[5].y) / 3.0f;
}

// Desenha o tabuleiro e as peças
void desenhar_tabuleiro_allegro(GameContext *ctx) {
    al_draw_bitmap(ctx->background_image, 0, 0, 0); // Desenha o fundo

    // Desenhar linhas do triângulo maior
    al_draw_line(ctx->pontos_tabuleiro[0].x, ctx->pontos_tabuleiro[0].y, ctx->pontos_tabuleiro[3].x, ctx->pontos_tabuleiro[3].y, ctx->cor_linhas, 2.0f);
    al_draw_line(ctx->pontos_tabuleiro[3].x, ctx->pontos_tabuleiro[3].y, ctx->pontos_tabuleiro[5].x, ctx->pontos_tabuleiro[5].y, ctx->cor_linhas, 2.0f);
    al_draw_line(ctx->pontos_tabuleiro[5].x, ctx->pontos_tabuleiro[5].y, ctx->pontos_tabuleiro[0].x, ctx->pontos_tabuleiro[0].y, ctx->cor_linhas, 2.0f);

    // Desenhar linha horizontal do meio
    al_draw_line(ctx->pontos_tabuleiro[1].x, ctx->pontos_tabuleiro[1].y, ctx->pontos_tabuleiro[2].x, ctx->pontos_tabuleiro[2].y, ctx->cor_linhas, 2.0f);

    // Desenhar linhas verticais/diagonais
    al_draw_line(ctx->pontos_tabuleiro[0].x, ctx->pontos_tabuleiro[0].y, ctx->pontos_tabuleiro[7].x, ctx->pontos_tabuleiro[7].y, ctx->cor_linhas, 2.0f); // Topo ao centro
    al_draw_line(ctx->pontos_tabuleiro[6].x, ctx->pontos_tabuleiro[6].y, ctx->pontos_tabuleiro[4].x, ctx->pontos_tabuleiro[4].y, ctx->cor_linhas, 2.0f); // Centro linha meio à base

    // Desenhar linhas do centro aos vértices e pontos médios da base para completar a estrutura da imagem
    al_draw_line(ctx->pontos_tabuleiro[7].x, ctx->pontos_tabuleiro[7].y, ctx->pontos_tabuleiro[1].x, ctx->pontos_tabuleiro[1].y, ctx->cor_linhas, 2.0f);
    al_draw_line(ctx->pontos_tabuleiro[7].x, ctx->pontos_tabuleiro[7].y, ctx->pontos_tabuleiro[2].x, ctx->pontos_tabuleiro[2].y, ctx->cor_linhas, 2.0f);

    // Desenhar pontos e peças
    for (int i = 0; i < 8; i++) {
        al_draw_filled_circle(ctx->pontos_tabuleiro[i].x, ctx->pontos_tabuleiro[i].y, ctx->raio_ponto, ctx->cor_pontos);

        // Desenhar peças se houver
        if (ctx->tabuleiro_logico[i] == PECA_JOGADOR1) {
            al_draw_filled_circle(ctx->pontos_tabuleiro[i].x, ctx->pontos_tabuleiro[i].y, ctx->raio_peca, ctx->cor_peca_j1);
        } else if (ctx->tabuleiro_logico[i] == PECA_JOGADOR2) {
            al_draw_filled_circle(ctx->pontos_tabuleiro[i].x, ctx->pontos_tabuleiro[i].y, ctx->raio_peca, ctx->cor_peca_j2);
        }

        // Marcar casa selecionada (se houver)
        if (i == ctx->casa_selecionada) {
            al_draw_circle(ctx->pontos_tabuleiro[i].x, ctx->pontos_tabuleiro[i].y, ctx->raio_peca + 3, al_map_rgb(255, 255, 0), 2.0f); // Amarelo para seleção
        }
    }

    // Exibir informações do jogo (turnos, peças restantes, tempo)
    al_draw_textf(ctx->font, al_map_rgb(0, 0, 0), ctx->largura_tela * 0.25, 50, ALLEGRO_ALIGN_CENTER, "Jogador Atual: %d", ctx->jogador_atual);
    al_draw_textf(ctx->font, al_map_rgb(0, 0, 0), ctx->largura_tela * 0.25, 80, ALLEGRO_ALIGN_CENTER, "Etapa: %s", ctx->etapa_atual == ETAPA_POSICIONAMENTO ? "Posicionamento" : "Movimentacao");
    al_draw_textf(ctx->font, al_map_rgb(0, 0, 0), ctx->largura_tela * 0.25, 110, ALLEGRO_ALIGN_CENTER, "Pecas J1 fora: %d", ctx->pecas_fora_jogador1);
    al_draw_textf(ctx->font, al_map_rgb(0, 0, 0), ctx->largura_tela * 0.25, 140, ALLEGRO_ALIGN_CENTER, "Pecas J2 fora: %d", ctx->pecas_fora_jogador2);

    double tempo_atual_partida = (double)(clock() - ctx->inicio_partida_timer - ctx->tempo_pausado_total) / CLOCKS_PER_SEC;
    al_draw_textf(ctx->font, al_map_rgb(0, 0, 0), ctx->largura_tela * 0.25, 170, ALLEGRO_ALIGN_CENTER, "Tempo: %.1f s", tempo_atual_partida);

    al_draw_text(ctx->font, al_map_rgb(0, 0, 0), ctx->largura_tela * 0.25, ctx->altura_tela - 30, ALLEGRO_ALIGN_CENTER, "Pressione ESC para Pausar");
    al_flip_display();
}

// Mapeia coordenadas do mouse para um índice de casa do tabuleiro
int obter_casa_por_coordenadas(GameContext *ctx, float mouse_x, float mouse_y) {
    for (int i = 0; i < 8; i++) {
        float dist_x = ctx->pontos_tabuleiro[i].x - mouse_x;
        float dist_y = ctx->pontos_tabuleiro[i].y - mouse_y;
        if (sqrt(dist_x * dist_x + dist_y * dist_y) <= ctx->raio_peca) {
            return i;
        }
    }
    return -1; // Nenhuma casa encontrada
}

// Inicializa o tabuleiro lógico com casas vazias
void inicializar_tabuleiro_logico(CasaEstado tabuleiro[8]) {
    for (int i = 0; i < 8; i++) {
        tabuleiro[i] = CASA_VAZIA;
    }
}

// Verifica se um movimento é válido (adjacências)
bool validar_movimento(const CasaEstado tabuleiro[8], int origem, int destino) {
    // Definição das adjacências do tabuleiro (baseado na imagem do PDF)
    // Cada índice do array corresponde a um ponto, e a lista de ints são os pontos adjacentes.
    // Esta é uma representação estática da topologia do tabuleiro.
    int adjacencias[8][8] = {
    {1, 3, 7, -1, -1, -1, -1, -1}, // P0
    {0, 2, 6, 7, -1, -1, -1, -1},  // P1
    {1, 5, 6, 7, -1, -1, -1, -1},  // P2
    {0, 4, 7, -1, -1, -1, -1, -1}, // P3
    {3, 5, 6, 7, -1, -1, -1, -1},  // P4
    {2, 4, 7, -1, -1, -1, -1, -1}, // P5
    {1, 2, 4, 7, -1, -1, -1, -1},  // P6
    {0, 1, 2, 3, 4, 5, 6, -1}      // P7
};


    // Verifica se a origem tem uma peça do jogador atual
    if (tabuleiro[origem] == CASA_VAZIA) return false;

    // Verifica se o destino está vazio
    if (tabuleiro[destino] != CASA_VAZIA) return false;

    // Verifica se o destino é adjacente à origem
    for (int i = 0; i < 8; i++) { // Percorre as adjacências da origem
        if (adjacencias[origem][i] == -1) break; // Fim da lista de adjacências
        if (adjacencias[origem][i] == destino) {
            return true; // Movimento válido
        }
    }
    return false; // Movimento inválido
}


// Verifica se um jogador venceu (3 peças alinhadas)
bool verificar_vitoria(const CasaEstado tabuleiro[8], int jogador) {
    CasaEstado peca_jogador = (jogador == 1) ? PECA_JOGADOR1 : PECA_JOGADOR2;

    // Linhas possíveis no tabuleiro (índices dos pontos)
    // Horizontal
    if (tabuleiro[0] == peca_jogador && tabuleiro[1] == peca_jogador && tabuleiro[2] == peca_jogador) return true;
    if (tabuleiro[3] == peca_jogador && tabuleiro[4] == peca_jogador && tabuleiro[5] == peca_jogador) return true;
    // Verticais/Diagonais
    if (tabuleiro[0] == peca_jogador && tabuleiro[7] == peca_jogador && tabuleiro[4] == peca_jogador) return true;
    if (tabuleiro[3] == peca_jogador && tabuleiro[7] == peca_jogador && tabuleiro[2] == peca_jogador) return true; // P0-P7-P4
    if (tabuleiro[5] == peca_jogador && tabuleiro[7] == peca_jogador && tabuleiro[1] == peca_jogador) return true; // P3-P7-P2
    if (tabuleiro[1] == peca_jogador && tabuleiro[6] == peca_jogador && tabuleiro[4] == peca_jogador) return true; // P5-P7-P1
    if (tabuleiro[0] == peca_jogador && tabuleiro[6] == peca_jogador && tabuleiro[5] == peca_jogador) return true; // P0-P6-P5
    if (tabuleiro[3] == peca_jogador && tabuleiro[6] == peca_jogador && tabuleiro[2] == peca_jogador) return true; // P3-P6-P2
    
    return false;
}

// Troca o jogador atual
void trocar_jogador(GameContext *ctx) {
    ctx->jogador_atual = (ctx->jogador_atual == 1) ? 2 : 1;
}

// Avança a etapa do jogo se todas as peças estiverem no tabuleiro
void avancar_etapa_se_necessario(GameContext *ctx) {
    if (ctx->pecas_fora_jogador1 == 0 && ctx->pecas_fora_jogador2 == 0 && ctx->etapa_atual == ETAPA_POSICIONAMENTO) {
        ctx->etapa_atual = ETAPA_MOVIMENTACAO;
        // Inicia a contagem do tempo para a fase de movimentação
        // O tempo total da partida já está sendo contado desde o início em inicio_partida_timer
        printf("Todas as peças foram posicionadas. Iniciando fase de movimentação!\n");
        //exibir_mensagem(ctx, "Todas as pecas foram posicionadas.\nIniciando fase de movimentacao!");
    }
}

// Lógica de jogada da IA
void jogar_ia(GameContext *ctx) {
    printf("Vez do computador...\n");
    //exibir_mensagem(ctx, "Vez do computador...");

    if (ctx->etapa_atual == ETAPA_POSICIONAMENTO) {
        // IA na etapa de posicionamento
        int casa_vazia = -1;
        do {
            casa_vazia = rand() % 8; // Tenta uma casa aleatória
        } while (ctx->tabuleiro_logico[casa_vazia] != CASA_VAZIA);

        ctx->tabuleiro_logico[casa_vazia] = PECA_JOGADOR2;
        ctx->pecas_fora_jogador2--;
        printf("Computador posicionou peca em %d.\n", casa_vazia);

    } else { // ETAPA_MOVIMENTACAO
        // IA na etapa de movimentação
        int origem = -1, destino = -1;
        bool movimento_valido_encontrado = false;

        // Tenta encontrar um movimento válido aleatoriamente
        for (int tries = 0; tries < 100 && !movimento_valido_encontrado; tries++) {
            origem = rand() % 8;
            destino = rand() % 8;

            if (ctx->tabuleiro_logico[origem] == PECA_JOGADOR2 && validar_movimento(ctx->tabuleiro_logico, origem, destino)) {
                movimento_valido_encontrado = true;
            }
        }

        if (movimento_valido_encontrado) {
            ctx->tabuleiro_logico[destino] = ctx->tabuleiro_logico[origem];
            ctx->tabuleiro_logico[origem] = CASA_VAZIA;
            printf("Computador moveu peca de %d para %d.\n", origem, destino);
        } else {
            // Se não encontrou um movimento aleatório em 100 tentativas, faz uma busca mais exaustiva
            // Isso pode acontecer se o tabuleiro estiver muito cheio ou a IA estiver "presa"
            printf("IA buscando movimento exaustivamente...\n");
            for (origem = 0; origem < 8; origem++) {
                if (ctx->tabuleiro_logico[origem] == PECA_JOGADOR2) {
                    for (destino = 0; destino < 8; destino++) {
                        if (validar_movimento(ctx->tabuleiro_logico, origem, destino)) {
                            ctx->tabuleiro_logico[destino] = ctx->tabuleiro_logico[origem];
                            ctx->tabuleiro_logico[origem] = CASA_VAZIA;
                            printf("Computador moveu peca de %d para %d (exaustivo).\n", origem, destino);
                            movimento_valido_encontrado = true;
                            break;
                        }
                    }
                }
                if (movimento_valido_encontrado) break;
            }
        }

        if (!movimento_valido_encontrado) {
            // Caso extremo: IA não consegue fazer nenhum movimento válido
            // Isso indicaria que o jogo está travado para a IA ou um bug.
            printf("ERRO: IA nao conseguiu fazer um movimento valido.\n");
            strcpy(ctx->mensagem_fim_de_jogo, "Jogo travado para o Computador!");
            ctx->current_game_state = ESTADO_FIM_DE_JOGO; // Termina o jogo
            return;
        }
    }

    if (verificar_vitoria(ctx->tabuleiro_logico, 2)) {
        strcpy(ctx->mensagem_fim_de_jogo, "Computador Venceu!");
        ctx->current_game_state = ESTADO_FIM_DE_JOGO;
    } else {
        avancar_etapa_se_necessario(ctx);
        if (ctx->current_game_state != ESTADO_FIM_DE_JOGO) { // Só troca se o jogo não terminou
            trocar_jogador(ctx);
        }
    }
}

// Inicializa um novo jogo
void iniciar_novo_jogo(GameContext *ctx, ModoJogo modo) {
    inicializar_tabuleiro_logico(ctx->tabuleiro_logico);
    ctx->jogador_atual = 1;
    ctx->etapa_atual = ETAPA_POSICIONAMENTO;
    ctx->pecas_fora_jogador1 = 3;
    ctx->pecas_fora_jogador2 = 3;
    ctx->casa_selecionada = -1;
    ctx->modo_jogo_atual = modo;
    ctx->inicio_partida_timer = clock();
    ctx->tempo_pausado_total = 0.0;
    strcpy(ctx->mensagem_fim_de_jogo, ""); // Limpa mensagem
    ctx->current_game_state = (modo == JOGADOR_VS_JOGADOR) ? ESTADO_JOGANDO_PX_P : ESTADO_JOGANDO_PX_COMP;
    srand(time(NULL)); // Inicializa o gerador de números aleatórios para a IA
    printf("Novo jogo iniciado: %s\n", (modo == JOGADOR_VS_JOGADOR) ? "Jogador vs Jogador" : "Jogador vs Computador");
}

// Carrega o histórico de partidas
void carregar_historico(GameContext *ctx) {
    FILE *f = fopen(ctx->arquivo_historico, "rb");
    if (f) {
        fread(&ctx->historico_global, sizeof(HistoricoTotal), 1, f);
        fclose(f);
    } else {
        // Inicializa o histórico se o arquivo não existir
        ctx->historico_global.vitorias_pxp = 0;
        ctx->historico_global.derrotas_pxp = 0;
        ctx->historico_global.empates_pxp = 0;
        ctx->historico_global.menor_tempo_pxp = 999999.0;
        ctx->historico_global.maior_tempo_pxp = 0.0;
        ctx->historico_global.vitorias_pxcomp = 0;
        ctx->historico_global.derrotas_pxcomp = 0;
        ctx->historico_global.empates_pxcomp = 0;
        ctx->historico_global.menor_tempo_pxcomp = 999999.0;
        ctx->historico_global.maior_tempo_pxcomp = 0.0;
    }
}

// Salva o histórico de partidas
void salvar_historico(GameContext *ctx) {
    FILE *f = fopen(ctx->arquivo_historico, "wb");
    if (f) {
        fwrite(&ctx->historico_global, sizeof(HistoricoTotal), 1, f);
        fclose(f);
    } else {
        fprintf(stderr, "Erro ao salvar historico!\n");
    }
}

// Registra uma partida no histórico
void registrar_partida(GameContext *ctx, ModoJogo modo, int vencedor, double tempo_partida) {
    if (modo == JOGADOR_VS_JOGADOR) {
        if (vencedor == 1) ctx->historico_global.vitorias_pxp++;
        else if (vencedor == 2) ctx->historico_global.derrotas_pxp++;
        else ctx->historico_global.empates_pxp++; // 0 ou outro valor para empate

        if (tempo_partida < ctx->historico_global.menor_tempo_pxp) ctx->historico_global.menor_tempo_pxp = tempo_partida;
        if (tempo_partida > ctx->historico_global.maior_tempo_pxp) ctx->historico_global.maior_tempo_pxp = tempo_partida;
    } else { // JOGADOR_VS_COMPUTADOR
        if (vencedor == 1) ctx->historico_global.vitorias_pxcomp++;
        else if (vencedor == 2) ctx->historico_global.derrotas_pxcomp++; // IA venceu
        else ctx->historico_global.empates_pxcomp++; // 0 ou outro valor para empate

        if (tempo_partida < ctx->historico_global.menor_tempo_pxcomp) ctx->historico_global.menor_tempo_pxcomp = tempo_partida;
        if (tempo_partida > ctx->historico_global.maior_tempo_pxcomp) ctx->historico_global.maior_tempo_pxcomp = tempo_partida;
    }
    salvar_historico(ctx);
}

// Salva o estado atual do jogo
void salvar_jogo(GameContext *ctx) {
    JogoSalvo jogo_atual;
    jogo_atual.modo = ctx->modo_jogo_atual;
    memcpy(jogo_atual.tabuleiro, ctx->tabuleiro_logico, sizeof(CasaEstado) * 8);
    jogo_atual.pecas_fora_j1 = ctx->pecas_fora_jogador1;
    jogo_atual.pecas_fora_j2 = ctx->pecas_fora_jogador2;
    jogo_atual.jogador_atual = ctx->jogador_atual;
    jogo_atual.etapa_atual = ctx->etapa_atual;
    jogo_atual.tempo_decorrido = (double)(clock() - ctx->inicio_partida_timer - ctx->tempo_pausado_total) / CLOCKS_PER_SEC;

    FILE *f = NULL;
    if (ctx->modo_jogo_atual == JOGADOR_VS_JOGADOR) {
        f = fopen(ctx->arquivo_salvo_pxp, "wb");
    } else {
        f = fopen(ctx->arquivo_salvo_pxcomp, "wb");
    }

    if (f) {
        fwrite(&jogo_atual, sizeof(JogoSalvo), 1, f);
        fclose(f);
        exibir_mensagem(ctx, "Jogo salvo com sucesso!");
    } else {
        exibir_mensagem(ctx, "Erro ao salvar o jogo!");
    }
}

// Carrega um jogo salvo
bool carregar_jogo(GameContext *ctx, ModoJogo modo_carregar) {
    JogoSalvo jogo_carregado;
    FILE *f = NULL;
    if (modo_carregar == JOGADOR_VS_JOGADOR) {
        f = fopen(ctx->arquivo_salvo_pxp, "rb");
    } else {
        f = fopen(ctx->arquivo_salvo_pxcomp, "rb");
    }

    if (f) {
        fread(&jogo_carregado, sizeof(JogoSalvo), 1, f);
        fclose(f);

        ctx->modo_jogo_atual = jogo_carregado.modo;
        memcpy(ctx->tabuleiro_logico, jogo_carregado.tabuleiro, sizeof(CasaEstado) * 8);
        ctx->pecas_fora_jogador1 = jogo_carregado.pecas_fora_j1;
        ctx->pecas_fora_jogador2 = jogo_carregado.pecas_fora_j2;
        ctx->jogador_atual = jogo_carregado.jogador_atual;
        ctx->etapa_atual = jogo_carregado.etapa_atual;
        ctx->tempo_pausado_total = 0.0; // Zera o tempo pausado para o novo jogo
        ctx->inicio_partida_timer = clock() - (clock_t)(jogo_carregado.tempo_decorrido * CLOCKS_PER_SEC); // Ajusta o timer
        ctx->casa_selecionada = -1;
        strcpy(ctx->mensagem_fim_de_jogo, ""); // Limpa mensagem

        ctx->current_game_state = (ctx->modo_jogo_atual == JOGADOR_VS_JOGADOR) ? ESTADO_JOGANDO_PX_P : ESTADO_JOGANDO_PX_COMP;
        exibir_mensagem(ctx, "Jogo carregado com sucesso!");
        return true;
    } else {
        exibir_mensagem(ctx, "Nenhum jogo salvo encontrado para este modo.");
        return false;
    }
}

// --- Funções de UI ---

// Desenha a tela do menu principal
void desenhar_menu(GameContext *ctx) {
    al_draw_bitmap(ctx->background_image, 0, 0, 0);

    al_draw_text(ctx->font, al_map_rgb(0, 0, 0), 160, 75, ALLEGRO_ALIGN_CENTER, "Tri Angle");
    al_draw_text(ctx->font, al_map_rgb(0, 0, 0), 160, 125, ALLEGRO_ALIGN_CENTER, "Jogar");
    al_draw_text(ctx->font, al_map_rgb(0, 0, 0), 160, 150, ALLEGRO_ALIGN_CENTER, "Carregar Jogo"); // Nova opção
    al_draw_text(ctx->font, al_map_rgb(0, 0, 0), 160, 175, ALLEGRO_ALIGN_CENTER, "Ajuda");
    al_draw_text(ctx->font, al_map_rgb(0, 0, 0), 160, 200, ALLEGRO_ALIGN_CENTER, "Historico");
    al_draw_text(ctx->font, al_map_rgb(0, 0, 0), 160, 225, ALLEGRO_ALIGN_CENTER, "Sair"); // Ajuste de posição
    al_flip_display();
}

// Desenha a tela de seleção de modo de jogo (Jogar)
void desenhar_menu_jogar_modos(GameContext *ctx) {
    al_draw_bitmap(ctx->background_image, 0, 0, 0);
    if (ctx->play_modes_image) {
        al_draw_scaled_bitmap(ctx->play_modes_image,
                             0, 0, al_get_bitmap_width(ctx->play_modes_image), al_get_bitmap_height(ctx->play_modes_image),
                             0, 0, ctx->largura_tela, ctx->altura_tela,
                             0);
    }
    al_draw_text(ctx->font2, al_map_rgb(0, 0 , 0 ), 150, 310, ALLEGRO_ALIGN_CENTER, "Jogador vs Jogador");
    al_draw_text(ctx->font2, al_map_rgb(0, 0 , 0 ), 585, 310, ALLEGRO_ALIGN_CENTER, "Jogador vs Computador");
    al_draw_text(ctx->font, al_map_rgb(255, 0, 0), ctx->largura_tela / 2, ctx->altura_tela - 50, ALLEGRO_ALIGN_CENTER, "Retornar ao Menu");
    al_flip_display();
}

// Desenha a tela de ajuda
void desenhar_ajuda(GameContext *ctx) {
    al_draw_bitmap(ctx->background_image, 0, 0, 0);
    if (ctx->help_image) {
        al_draw_scaled_bitmap(ctx->help_image,
                             0, 0, al_get_bitmap_width(ctx->help_image), al_get_bitmap_height(ctx->help_image),
                             0, 0, ctx->largura_tela, ctx->altura_tela,
                             0);
    }
    al_draw_text(ctx->font, al_map_rgb(255, 0, 0), ctx->largura_tela / 2, ctx->altura_tela - 50, ALLEGRO_ALIGN_CENTER, "Retornar ao Menu");
    al_flip_display();
}

// Desenha a tela de histórico
void desenhar_historico_tela(GameContext *ctx) {
    al_draw_bitmap(ctx->background_image, 0, 0, 0);
    al_draw_text(ctx->font, al_map_rgb(0, 0, 0), ctx->largura_tela / 2, 50, ALLEGRO_ALIGN_CENTER, "Historico de Partidas");

    char buffer[100];

    // Histórico PxP
    al_draw_text(ctx->font, al_map_rgb(0, 0, 0), ctx->largura_tela / 4, 100, ALLEGRO_ALIGN_LEFT, "Jogador vs Jogador:");
    sprintf(buffer, "Vitorias: %d", ctx->historico_global.vitorias_pxp);
    al_draw_text(ctx->font, al_map_rgb(0, 0, 0), ctx->largura_tela / 4, 130, ALLEGRO_ALIGN_LEFT, buffer);
    sprintf(buffer, "Derrotas: %d", ctx->historico_global.derrotas_pxp);
    al_draw_text(ctx->font, al_map_rgb(0, 0, 0), ctx->largura_tela / 4, 150, ALLEGRO_ALIGN_LEFT, buffer);
    sprintf(buffer, "Empates: %d", ctx->historico_global.empates_pxp);
    al_draw_text(ctx->font, al_map_rgb(0, 0, 0), ctx->largura_tela / 4, 170, ALLEGRO_ALIGN_LEFT, buffer);
    if (ctx->historico_global.menor_tempo_pxp < 999999.0) {
        sprintf(buffer, "Menor Tempo: %.1f s", ctx->historico_global.menor_tempo_pxp);
        al_draw_text(ctx->font, al_map_rgb(0, 0, 0), ctx->largura_tela / 4, 190, ALLEGRO_ALIGN_LEFT, buffer);
        sprintf(buffer, "Maior Tempo: %.1f s", ctx->historico_global.maior_tempo_pxp);
        al_draw_text(ctx->font, al_map_rgb(0, 0, 0), ctx->largura_tela / 4, 210, ALLEGRO_ALIGN_LEFT, buffer);
    } else {
        al_draw_text(ctx->font, al_map_rgb(0, 0, 0), ctx->largura_tela / 4, 190, ALLEGRO_ALIGN_LEFT, "Menor Tempo: N/A");
        al_draw_text(ctx->font, al_map_rgb(0, 0, 0), ctx->largura_tela / 4, 210, ALLEGRO_ALIGN_LEFT, "Maior Tempo: N/A");
    }

    // Histórico PxComp
    al_draw_text(ctx->font, al_map_rgb(0, 0, 0), ctx->largura_tela * 3 / 4, 100, ALLEGRO_ALIGN_RIGHT, "Jogador vs Computador:");
    sprintf(buffer, "Vitorias: %d", ctx->historico_global.vitorias_pxcomp);
    al_draw_text(ctx->font, al_map_rgb(0, 0, 0), ctx->largura_tela * 3 / 4, 130, ALLEGRO_ALIGN_RIGHT, buffer);
    sprintf(buffer, "Derrotas: %d", ctx->historico_global.derrotas_pxcomp);
    al_draw_text(ctx->font, al_map_rgb(0, 0, 0), ctx->largura_tela * 3 / 4, 150, ALLEGRO_ALIGN_RIGHT, buffer);
    sprintf(buffer, "Empates: %d", ctx->historico_global.empates_pxcomp);
    al_draw_text(ctx->font, al_map_rgb(0, 0, 0), ctx->largura_tela * 3 / 4, 170, ALLEGRO_ALIGN_RIGHT, buffer);
     if (ctx->historico_global.menor_tempo_pxcomp < 999999.0) {
        sprintf(buffer, "Menor Tempo: %.1f s", ctx->historico_global.menor_tempo_pxcomp);
        al_draw_text(ctx->font, al_map_rgb(0, 0, 0), ctx->largura_tela * 3 / 4, 190, ALLEGRO_ALIGN_RIGHT, buffer);
        sprintf(buffer, "Maior Tempo: %.1f s", ctx->historico_global.maior_tempo_pxcomp);
        al_draw_text(ctx->font, al_map_rgb(0, 0, 0), ctx->largura_tela * 3 / 4, 210, ALLEGRO_ALIGN_RIGHT, buffer);
    } else {
        al_draw_text(ctx->font, al_map_rgb(0, 0, 0), ctx->largura_tela * 3 / 4, 190, ALLEGRO_ALIGN_RIGHT, "Menor Tempo: N/A");
        al_draw_text(ctx->font, al_map_rgb(0, 0, 0), ctx->largura_tela * 3 / 4, 210, ALLEGRO_ALIGN_RIGHT, "Maior Tempo: N/A");
    }

    al_draw_text(ctx->font, al_map_rgb(255, 0, 0), ctx->largura_tela / 2, ctx->altura_tela - 50, ALLEGRO_ALIGN_CENTER, "Retornar ao Menu");
    al_flip_display();
}

// Desenha a tela de pausa
void desenhar_pausa(GameContext *ctx) {
    al_draw_bitmap(ctx->background_image, 0, 0, 0);
    al_draw_text(ctx->font, al_map_rgb(0, 0, 0), ctx->largura_tela / 2, 100, ALLEGRO_ALIGN_CENTER, "Jogo Pausado");
    al_draw_text(ctx->font, al_map_rgb(0, 0, 0), ctx->largura_tela / 2, 150, ALLEGRO_ALIGN_CENTER, "Continuar");
    al_draw_text(ctx->font, al_map_rgb(0, 0, 0), ctx->largura_tela / 2, 180, ALLEGRO_ALIGN_CENTER, "Salvar Jogo");
    al_draw_text(ctx->font, al_map_rgb(0, 0, 0), ctx->largura_tela / 2, 210, ALLEGRO_ALIGN_CENTER, "Menu Principal");
    al_flip_display();
}

// Desenha a tela de fim de jogo
void desenhar_fim_de_jogo(GameContext *ctx) {
    al_draw_bitmap(ctx->background_image, 0, 0, 0);
    al_draw_text(ctx->font, al_map_rgb(0, 0, 0), ctx->largura_tela / 2, 150, ALLEGRO_ALIGN_CENTER, ctx->mensagem_fim_de_jogo);
    al_draw_text(ctx->font, al_map_rgb(255, 0, 0), ctx->largura_tela / 2, ctx->altura_tela - 50, ALLEGRO_ALIGN_CENTER, "Pressione ESC ou Clique para Retornar ao Menu");
    al_flip_display();
}

// Desenha a tela de seleção de jogo para carregar
void desenhar_menu_carregar_jogo_selecao(GameContext *ctx) {
    al_draw_bitmap(ctx->background_image, 0, 0, 0);
    al_draw_text(ctx->font, al_map_rgb(0, 0, 0), ctx->largura_tela / 2, 100, ALLEGRO_ALIGN_CENTER, "Carregar Jogo Salvo:");
    al_draw_text(ctx->font, al_map_rgb(0, 0, 0), ctx->largura_tela / 2, 150, ALLEGRO_ALIGN_CENTER, "Jogador vs Jogador");
    al_draw_text(ctx->font, al_map_rgb(0, 0, 0), ctx->largura_tela / 2, 180, ALLEGRO_ALIGN_CENTER, "Jogador vs Computador");
    al_draw_text(ctx->font, al_map_rgb(255, 0, 0), ctx->largura_tela / 2, ctx->altura_tela - 50, ALLEGRO_ALIGN_CENTER, "Retornar ao Menu");
    al_flip_display();
}

// --- Funções de Processamento de Eventos ---

void processar_eventos_menu(GameContext *ctx, ALLEGRO_EVENT *event) {
    int mouse_x = event->mouse.x;
    int mouse_y = event->mouse.y;

    // Coordenadas e tamanhos para os botões do menu (ajustados para a nova estrutura)
    int btn_x_center = 160; // Posição X de ALIGN_CENTER
    int btn_w = 200; // Largura aproximada dos botões
    int btn_h = 30; // Altura aproximada dos botões

    // Jogar
    int jogar_y = 125;
    if (mouse_x >= btn_x_center - btn_w / 2 && mouse_x <= btn_x_center + btn_w / 2 &&
        mouse_y >= jogar_y && mouse_y <= jogar_y + btn_h) {
        ctx->current_game_state = ESTADO_MENU; // Temporário para desenhar modos
        desenhar_menu_jogar_modos(ctx);
        ctx->current_game_state = ESTADO_JOGANDO_PX_P; // Será alterado novamente ao selecionar modo
    }
    // Carregar Jogo
    int carregar_y = 150;
    if (mouse_x >= btn_x_center - btn_w / 2 && mouse_x <= btn_x_center + btn_w / 2 &&
        mouse_y >= carregar_y && mouse_y <= carregar_y + btn_h) {
        ctx->current_game_state = ESTADO_CARREGAR_JOGO_SELECAO;
    }
    // Ajuda
    int ajuda_y = 175;
    if (mouse_x >= btn_x_center - btn_w / 2 && mouse_x <= btn_x_center + btn_w / 2 &&
        mouse_y >= ajuda_y && mouse_y <= ajuda_y + btn_h) {
        ctx->current_game_state = ESTADO_AJUDA;
    }
    // Historico
    int historico_y = 200;
    if (mouse_x >= btn_x_center - btn_w / 2 && mouse_x <= btn_x_center + btn_w / 2 &&
        mouse_y >= historico_y && mouse_y <= historico_y + btn_h) {
        carregar_historico(ctx);
        ctx->current_game_state = ESTADO_HISTORICO;
    }
    // Sair
    int sair_y = 225;
    if (mouse_x >= btn_x_center - btn_w / 2 && mouse_x <= btn_x_center + btn_w / 2 &&
        mouse_y >= sair_y && mouse_y <= sair_y + btn_h) {
        ctx->current_game_state = ESTADO_SAIR;
    }
}

void processar_eventos_jogar(GameContext *ctx, ALLEGRO_EVENT *event) {
    if (event->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
        int mouse_x = event->mouse.x;
        int mouse_y = event->mouse.y;

        // Retornar ao Menu (botão na parte inferior central)
        int rmenu_x = ctx->largura_tela / 2;
        int rmenu_y = ctx->altura_tela - 50;
        int rmenu_w = 200;
        int rmenu_h = 40;
        if (mouse_x >= rmenu_x - rmenu_w / 2 && mouse_x <= rmenu_x + rmenu_w / 2 &&
            mouse_y >= rmenu_y && mouse_y <= rmenu_y + rmenu_h) {
            ctx->current_game_state = ESTADO_MENU;
            return;
        }
        
        // Clicks nos modos de jogo
        // "Jogador vs Jogador" - X = 150, Y = 310
        int pvp_x_center = 150;
        int pvp_y_center = 310;
        int pvp_w = 250; // Largura aproximada do texto + margem
        int pvp_h = al_get_font_line_height(ctx->font2);

        if (mouse_x >= pvp_x_center - pvp_w / 2 && mouse_x <= pvp_x_center + pvp_w / 2 &&
            mouse_y >= pvp_y_center && mouse_y <= pvp_y_center + pvp_h) {
            iniciar_novo_jogo(ctx, JOGADOR_VS_JOGADOR);
            return;
        }

        // "Jogador vs Computador" - X = 585, Y = 310
        int pvcomp_x_center = 585;
        int pvcomp_y_center = 310;
        int pvcomp_w = 250; // Largura aproximada do texto + margem
        int pvcomp_h = al_get_font_line_height(ctx->font2);

        if (mouse_x >= pvcomp_x_center - pvcomp_w / 2 && mouse_x <= pvcomp_x_center + pvcomp_w / 2 &&
            mouse_y >= pvcomp_y_center && mouse_y <= pvcomp_y_center + pvcomp_h) {
            iniciar_novo_jogo(ctx, JOGADOR_VS_COMPUTADOR);
            return;
        }
    }
}

void processar_eventos_ajuda(GameContext *ctx, ALLEGRO_EVENT *event) {
    if (event->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
        int mouse_x = event->mouse.x;
        int mouse_y = event->mouse.y;

        int rmenu_x = ctx->largura_tela / 2;
        int rmenu_y = ctx->altura_tela - 50;
        int rmenu_w = 200;
        int rmenu_h = 40;
        if (mouse_x >= rmenu_x - rmenu_w / 2 && mouse_x <= rmenu_x + rmenu_w / 2 &&
            mouse_y >= rmenu_y && mouse_y <= rmenu_y + rmenu_h) {
            ctx->current_game_state = ESTADO_MENU;
            al_destroy_bitmap(ctx->help_image); // Limpa a imagem quando sai da tela
            ctx->help_image = NULL;
        }
    }
}

void processar_eventos_historico(GameContext *ctx, ALLEGRO_EVENT *event) {
    if (event->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN || event->type == ALLEGRO_EVENT_KEY_DOWN) {
        int mouse_x = event->mouse.x;
        int mouse_y = event->mouse.y;

        int rmenu_x = ctx->largura_tela / 2;
        int rmenu_y = ctx->altura_tela - 50;
        int rmenu_w = 200;
        int rmenu_h = 40;
        if (mouse_x >= rmenu_x - rmenu_w / 2 && mouse_x <= rmenu_x + rmenu_w / 2 &&
            mouse_y >= rmenu_y && mouse_y <= rmenu_y + rmenu_h) {
            ctx->current_game_state = ESTADO_MENU;
        }
    }
}

void processar_eventos_pausa(GameContext *ctx, ALLEGRO_EVENT *event) {
    int mouse_x = event->mouse.x;
    int mouse_y = event->mouse.y;

    int btn_x_center = ctx->largura_tela / 2;
    int btn_w = 200;
    int btn_h = 30;

    // Continuar
    int cont_y = 150;
    if (mouse_x >= btn_x_center - btn_w / 2 && mouse_x <= btn_x_center + btn_w / 2 &&
        mouse_y >= cont_y && mouse_y <= cont_y + btn_h) {
        ctx->tempo_pausado_total += (double)(clock() - ctx->inicio_pausa_timer) / CLOCKS_PER_SEC;
        ctx->current_game_state = (ctx->modo_jogo_atual == JOGADOR_VS_JOGADOR) ? ESTADO_JOGANDO_PX_P : ESTADO_JOGANDO_PX_COMP;
        return;
    }
    // Salvar Jogo
    int salvar_y = 180;
    if (mouse_x >= btn_x_center - btn_w / 2 && mouse_x <= btn_x_center + btn_w / 2 &&
        mouse_y >= salvar_y && mouse_y <= salvar_y + btn_h) {
        salvar_jogo(ctx);
        // Permanece na tela de pausa após salvar
        return;
    }
    // Menu Principal
    int menu_y = 210;
    if (mouse_x >= btn_x_center - btn_w / 2 && mouse_x <= btn_x_center + btn_w / 2 &&
        mouse_y >= menu_y && mouse_y <= menu_y + btn_h) {
        ctx->current_game_state = ESTADO_MENU;
        return;
    }
}

void processar_eventos_fim_de_jogo(GameContext *ctx, ALLEGRO_EVENT *event) {
    if (event->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN || event->type == ALLEGRO_EVENT_KEY_DOWN) {
        ctx->current_game_state = ESTADO_MENU; // Volta para o menu
    }
}

void processar_eventos_carregar_jogo_selecao(GameContext *ctx, ALLEGRO_EVENT *event) {
    int mouse_x = event->mouse.x;
    int mouse_y = event->mouse.y;

    int btn_x_center = ctx->largura_tela / 2;
    int btn_w = 300;
    int btn_h = 30;

    // Carregar JXJ
    int jxj_y = 150;
    if (mouse_x >= btn_x_center - btn_w / 2 && mouse_x <= btn_x_center + btn_w / 2 &&
        mouse_y >= jxj_y && mouse_y <= jxj_y + btn_h) {
        if (carregar_jogo(ctx, JOGADOR_VS_JOGADOR)) {
            // O estado já foi alterado dentro de carregar_jogo
        } else {
            // Permanece na tela de seleção de carregamento se não carregou
            desenhar_menu_carregar_jogo_selecao(ctx);
        }
        return;
    }
    // Carregar JxCOMP
    int jxcomp_y = 180;
    if (mouse_x >= btn_x_center - btn_w / 2 && mouse_x <= btn_x_center + btn_w / 2 &&
        mouse_y >= jxcomp_y && mouse_y <= jxcomp_y + btn_h) {
        if (carregar_jogo(ctx, JOGADOR_VS_COMPUTADOR)) {
            // O estado já foi alterado dentro de carregar_jogo
        } else {
            // Permanece na tela de seleção de carregamento se não carregou
             desenhar_menu_carregar_jogo_selecao(ctx);
        }
        return;
    }
    // Retornar ao Menu
    int rmenu_x = ctx->largura_tela / 2;
    int rmenu_y = ctx->altura_tela - 50;
    int rmenu_w = 200;
    int rmenu_h = 40;
    if (mouse_x >= rmenu_x - rmenu_w / 2 && mouse_x <= rmenu_x + rmenu_w / 2 &&
        mouse_y >= rmenu_y && mouse_y <= rmenu_y + rmenu_h) {
        ctx->current_game_state = ESTADO_MENU;
        return;
    }
}

void processar_eventos_jogar_tabuleiro(GameContext *ctx, ALLEGRO_EVENT *event) {
    if (event->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
        int casa_clicada = obter_casa_por_coordenadas(ctx, (float)event->mouse.x, (float)event->mouse.y);

        if (ctx->etapa_atual == ETAPA_POSICIONAMENTO) {
            if (casa_clicada != -1 && ctx->tabuleiro_logico[casa_clicada] == CASA_VAZIA) {
                if (ctx->jogador_atual == 1 && ctx->pecas_fora_jogador1 > 0) {
                    ctx->tabuleiro_logico[casa_clicada] = PECA_JOGADOR1;
                    ctx->pecas_fora_jogador1--;
                    if (verificar_vitoria(ctx->tabuleiro_logico, 1)) {
                        strcpy(ctx->mensagem_fim_de_jogo, "Jogador 1 Venceu!");
                        ctx->current_game_state = ESTADO_FIM_DE_JOGO;
                    } else {
                        avancar_etapa_se_necessario(ctx);
                        if (ctx->current_game_state != ESTADO_FIM_DE_JOGO) {
                            trocar_jogador(ctx);
                        }
                    }
                } else if (ctx->jogador_atual == 2 && ctx->pecas_fora_jogador2 > 0) {
                    ctx->tabuleiro_logico[casa_clicada] = PECA_JOGADOR2;
                    ctx->pecas_fora_jogador2--;
                    if (verificar_vitoria(ctx->tabuleiro_logico, 2)) {
                        strcpy(ctx->mensagem_fim_de_jogo, "Jogador 2 Venceu!");
                        ctx->current_game_state = ESTADO_FIM_DE_JOGO;
                    } else {
                        avancar_etapa_se_necessario(ctx);
                        if (ctx->current_game_state != ESTADO_FIM_DE_JOGO) {
                            trocar_jogador(ctx);
                        }
                    }
                }
            } else {
                printf("Casa invalida ou ocupada para posicionamento.\n");
            }
        } else { // ETAPA_MOVIMENTACAO
            if (ctx->casa_selecionada == -1) { // Nenhuma peça selecionada
                if ((ctx->jogador_atual == 1 && ctx->tabuleiro_logico[casa_clicada] == PECA_JOGADOR1) ||
                    (ctx->jogador_atual == 2 && ctx->tabuleiro_logico[casa_clicada] == PECA_JOGADOR2)) {
                    ctx->casa_selecionada = casa_clicada;
                } else {
                    printf("Selecione uma de suas pecas.\n");
                }
            } else { // Peça já selecionada, tentando mover
                if (validar_movimento(ctx->tabuleiro_logico, ctx->casa_selecionada, casa_clicada)) {
                    ctx->tabuleiro_logico[casa_clicada] = ctx->tabuleiro_logico[ctx->casa_selecionada];
                    ctx->tabuleiro_logico[ctx->casa_selecionada] = CASA_VAZIA;
                    ctx->casa_selecionada = -1; // Limpa seleção

                    if (verificar_vitoria(ctx->tabuleiro_logico, ctx->jogador_atual)) {
                        strcpy(ctx->mensagem_fim_de_jogo, (ctx->jogador_atual == 1) ? "Jogador 1 Venceu!" : "Jogador 2 Venceu!");
                        ctx->current_game_state = ESTADO_FIM_DE_JOGO;
                    } else {
                        trocar_jogador(ctx);
                    }
                } else {
                    printf("Movimento invalido. Tente novamente ou selecione outra peca.\n");
                    ctx->casa_selecionada = -1; // Desseleciona para nova tentativa
                }
            }
        }
    } else if (event->type == ALLEGRO_EVENT_KEY_DOWN) {
        if (event->keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
            ctx->inicio_pausa_timer = clock(); // Marca o início da pausa
            ctx->current_game_state = ESTADO_PAUSADO;
        }
    }
}


// --- Main ---
int main(void) {
    GameContext ctx;

    // Inicialização das constantes
    ctx.largura_tela = 728;
    ctx.altura_tela = 410;
    ctx.cor_fundo = al_map_rgb(255, 255, 255);
    ctx.cor_pontos = al_map_rgb(0, 0, 0);
    ctx.cor_linhas = al_map_rgb(100, 100, 100);
    ctx.cor_peca_j1 = al_map_rgb(255, 0, 0);
    ctx.cor_peca_j2 = al_map_rgb(0, 0, 255);
    ctx.raio_ponto = 8.0f;
    ctx.raio_peca = 12.0f;
    ctx.max_historico = 100;
    strcpy(ctx.arquivo_historico, "historico.dat");
    strcpy(ctx.arquivo_salvo_pxp, "jogo_salvo_pxp.dat");
    strcpy(ctx.arquivo_salvo_pxcomp, "jogo_salvo_pxcomp.dat");

    #ifdef _WIN32
     strcpy(ctx.clear_command, "cls");
    #else
     strcpy(ctx.clear_command, "clear");
    #endif

    // Inicializa a Allegro e seus addons
    if (!al_init()) {
        fprintf(stderr, "Falha ao inicializar Allegro!\n");
        return -1;
    }
    al_init_font_addon();
    al_init_ttf_addon();
    al_init_primitives_addon();
    al_init_image_addon(); // Inicializa o addon de imagem

    if (!al_install_mouse()) {
        fprintf(stderr, "Falha ao inicializar o mouse!\n");
        return -1;
    }
    if (!al_install_keyboard()) {
        fprintf(stderr, "Falha ao inicializar o teclado!\n");
        al_uninstall_mouse();
        return -1;
    }

    ctx.font = al_load_ttf_font("Content/alice.ttf", 24, 0);
    if (!ctx.font) {
        fprintf(stderr, "Falha ao carregar fonte alice.ttf!\n");
        al_uninstall_mouse();
        al_uninstall_keyboard();
        return -1;
    }

    ctx.font2 = al_load_ttf_font("Content/mine.otf", 24, 0);
    if (!ctx.font2) {
        fprintf(stderr, "Falha ao carregar fonte mine.otf!\n");
        al_destroy_font(ctx.font);
        al_uninstall_mouse();
        al_uninstall_keyboard();
        return -1;
    }

    ctx.display = al_create_display(ctx.largura_tela, ctx.altura_tela);
    if (!ctx.display) {
        fprintf(stderr, "Falha ao criar janela!\n");
        al_destroy_font(ctx.font);
        al_destroy_font(ctx.font2);
        al_uninstall_mouse();
        al_uninstall_keyboard();
        return -1;
    }
    al_set_window_title(ctx.display, "Tri Angle");

    ctx.background_image = al_load_bitmap("Content/ele.jpg");
    if (!ctx.background_image) {
        fprintf(stderr, "Falha ao carregar imagem de fundo Content/ele.jpg!\n");
        al_destroy_display(ctx.display);
        al_destroy_font(ctx.font);
        al_destroy_font(ctx.font2);
        al_uninstall_mouse();
        al_uninstall_keyboard();
        return -1;
    }

    // Carregar imagens para os outros estados, ou deixá-las como NULL e carregar quando necessário
    ctx.help_image = NULL; // Carregar apenas quando o estado for ESTADO_AJUDA
    ctx.play_modes_image = NULL; // Carregar apenas quando o estado for ESTADO_JOGAR_MODOS
    ctx.history_image = NULL; // Carregar apenas quando o estado for ESTADO_HISTORICO

    // Cria uma fila de eventos
    ctx.event_queue = al_create_event_queue();
    if (!ctx.event_queue) {
        fprintf(stderr, "Falha ao criar fila de eventos!\n");
        al_destroy_display(ctx.display);
        al_destroy_font(ctx.font);
        al_destroy_font(ctx.font2);
        al_destroy_bitmap(ctx.background_image);
        al_uninstall_mouse();
        al_uninstall_keyboard();
        return -1;
    }

    al_register_event_source(ctx.event_queue, al_get_display_event_source(ctx.display));
    al_register_event_source(ctx.event_queue, al_get_mouse_event_source());
    al_register_event_source(ctx.event_queue, al_get_keyboard_event_source());

    // Inicialização do estado do jogo
    ctx.current_game_state = ESTADO_MENU;
    calcular_coordenadas_tabuleiro(&ctx, ctx.pontos_tabuleiro); // Calcula as coordenadas do tabuleiro
    carregar_historico(&ctx); // Carrega o histórico ao iniciar o jogo

    bool running = true;
    while (running) {
        ALLEGRO_EVENT event;
        al_wait_for_event(ctx.event_queue, &event);

        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            running = false;
        }

        switch (ctx.current_game_state) {
            case ESTADO_MENU:
                if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
                    processar_eventos_menu(&ctx, &event);
                }
                desenhar_menu(&ctx);
                break;
            case ESTADO_JOGANDO_PX_P:
            case ESTADO_JOGANDO_PX_COMP:
                if (ctx.current_game_state == ESTADO_JOGANDO_PX_COMP && ctx.jogador_atual == 2) {
                    // Turno da IA
                    jogar_ia(&ctx);
                } else {
                    processar_eventos_jogar_tabuleiro(&ctx, &event);
                }
                desenhar_tabuleiro_allegro(&ctx);
                break;
            case ESTADO_AJUDA:
                if (!ctx.help_image) { // Carrega a imagem da ajuda apenas uma vez
                    ctx.help_image = al_load_bitmap("Content/help.png");
                    if (!ctx.help_image) {
                        fprintf(stderr, "Falha ao carregar imagem help.png!\n");
                        // Tratar erro, talvez voltar para o menu ou sair
                        ctx.current_game_state = ESTADO_MENU;
                        break;
                    }
                }
                if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
                    processar_eventos_ajuda(&ctx, &event);
                }
                desenhar_ajuda(&ctx);
                break;
            case ESTADO_HISTORICO:
                if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN || event.type == ALLEGRO_EVENT_KEY_DOWN) {
                    processar_eventos_historico(&ctx, &event);
                }
                desenhar_historico_tela(&ctx);
                break;
            case ESTADO_PAUSADO:
                if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
                    processar_eventos_pausa(&ctx, &event);
                }
                desenhar_pausa(&ctx);
                break;
            case ESTADO_CARREGAR_JOGO_SELECAO:
                if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
                    processar_eventos_carregar_jogo_selecao(&ctx, &event);
                }
                desenhar_menu_carregar_jogo_selecao(&ctx);
                break;
            case ESTADO_FIM_DE_JOGO: {
                double tempo_total = (double)(clock() - ctx.inicio_partida_timer) / CLOCKS_PER_SEC;
                if (!strcmp(ctx.mensagem_fim_de_jogo, "Jogador 1 Venceu!")) {
                    registrar_partida(&ctx, ctx.modo_jogo_atual, 1, tempo_total);
                } else if (!strcmp(ctx.mensagem_fim_de_jogo, "Jogador 2 Venceu!") || !strcmp(ctx.mensagem_fim_de_jogo, "Computador Venceu!")) {
                    registrar_partida(&ctx, ctx.modo_jogo_atual, 2, tempo_total);
                } else { // Empate ou jogo travado
                    registrar_partida(&ctx, ctx.modo_jogo_atual, 0, tempo_total);
                }
                processar_eventos_fim_de_jogo(&ctx, &event);
                desenhar_fim_de_jogo(&ctx);
                break;
            }
            case ESTADO_SAIR:
                running = false;
                break;
        }
        
        // Lógica para carregar play_modes_image na primeira vez que o menu jogar modos é acessado
        if (ctx.current_game_state == ESTADO_JOGANDO_PX_P && !ctx.play_modes_image && event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
             // Este if/else if é um hack para o comportamento do seu código
             // onde `showing_play` é definido ANTES de carregar a imagem
             // e `current_game_state` é alterado ANTES de carregar
             // O ideal seria um estado `ESTADO_SELECIONAR_MODO_JOGAR`
             // Mas para seguir a base do seu código, preciso manter essa lógica.
             // Se o clique for em 'Jogar' e ainda não carregou a imagem, carrega.
            int jogar_x = 160, jogar_y = 125, jogar_w = 100, jogar_h = 30; // Do seu código
            int mouse_x = event.mouse.x;
            int mouse_y = event.mouse.y;
            if (mouse_x >= jogar_x - jogar_w / 2 && mouse_x <= jogar_x + jogar_w / 2 &&
                mouse_y >= jogar_y && mouse_y <= jogar_y + jogar_h) {
                ctx.play_modes_image = al_load_bitmap("Content/Modos.png");
                if (!ctx.play_modes_image) {
                    fprintf(stderr, "Falha ao carregar imagem Modos.png!\n");
                    ctx.current_game_state = ESTADO_MENU; // Volta para o menu
                } else {
                    desenhar_menu_jogar_modos(&ctx); // Desenha a tela de modos de jogo
                }
            }
        }
         // Se estamos na tela de seleção de modos, e o evento é de mouse
        if ((ctx.current_game_state == ESTADO_JOGANDO_PX_P || ctx.current_game_state == ESTADO_JOGANDO_PX_COMP) && event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            // Verifica se o clique foi nos botões de modo de jogo ou "retornar ao menu"
            // Esta chamada vai tentar iniciar um jogo ou voltar ao menu
            processar_eventos_jogar(&ctx, &event);
        }
    }

    // Limpeza
    if (ctx.help_image) al_destroy_bitmap(ctx.help_image);
    if (ctx.play_modes_image) al_destroy_bitmap(ctx.play_modes_image);
    if (ctx.history_image) al_destroy_bitmap(ctx.history_image); // Se foi carregada
    if (ctx.background_image) al_destroy_bitmap(ctx.background_image);
    if (ctx.font) al_destroy_font(ctx.font);
    if (ctx.font2) al_destroy_font(ctx.font2);
    if (ctx.event_queue) al_destroy_event_queue(ctx.event_queue);
    if (ctx.display) al_destroy_display(ctx.display);
    al_uninstall_mouse();
    al_uninstall_keyboard();

    return 0;
}

// Compilar com: gcc t4.c -o trab -lallegro -lallegro_font -lallegro_ttf -lallegro_primitives -lallegro_image -lallegro_main