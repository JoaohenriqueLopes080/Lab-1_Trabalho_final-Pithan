#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_image.h>

#ifdef _WIN32
#define CLEAR "cls"
#else
#define CLEAR "clear"
#endif

// --- ESTRUTURAS DE DADOS ---

// Struct para registrar uma partida no histórico
typedef struct {
    char modo_jogo[30];
    char resultado[30];
    double duracao;
} RegistroPartida;

// Struct para salvar o estado de um jogo em andamento
typedef struct {
    EstadoEspaço tabuleiro[7];
    int jogador_atual;
    int pecas_posicionadas_j1;
    int pecas_posicionadas_j2;
    double tempo_decorrido;
} JogoSalvo;


// Enum para o estado de cada casa do tabuleiro
typedef enum { VAZIO, JOGADOR1, JOGADOR2 } EstadoEspaço;

// Struct para as coordenadas (x,y) de cada ponto do tabuleiro
typedef struct {
    float x, y;
} PontoTabuleiro;


// --- PROTÓTIPOS DAS FUNÇÕES ---
void desenha_tabuleiroTriangulo(
    ALLEGRO_BITMAP *background, ALLEGRO_FONT *font, ALLEGRO_FONT *font2, int win_w, int win_h,
    EstadoEspaço tabuleiro[7], PontoTabuleiro pontos_tabuleiro[7],
    ALLEGRO_BITMAP *peca_player1, ALLEGRO_BITMAP *peca_player2,
    float mouse_x, float mouse_y, int placar_j1, int placar_j2,
    int peca_selecionada, bool mostrar_movimentos_validos
);
bool movimento_valido(int origem, int destino, EstadoEspaço tabuleiro[7]);
void salvar_registro(const char* modo, const char* resultado, double duracao);
void desenha_historico4(ALLEGRO_BITMAP *background, ALLEGRO_BITMAP *History_image, ALLEGRO_FONT *font, int win_w, int win_h);
void salvar_partida_atual(EstadoEspaço tabuleiro[7], int jogador_atual, int p1, int p2, double tempo);
bool carregar_partida_salva(EstadoEspaço tabuleiro[7], int *jogador_atual, int *p1, int *p2, double *tempo);


// --- LÓGICA DO JOGO ---

EstadoEspaço verificar_vitoria(EstadoEspaço tabuleiro[7]) {
    // Todas as 8 combinações de 3 casas alinhadas que resultam em vitória
    int linhas[8][3] = {
        {0, 2, 5}, {1, 2, 3}, {4, 5, 6}, // Retas
        {0, 1, 4}, {0, 3, 6}, // Lados do triângulo maior
        {1, 5, 4}, {3, 5, 6}, // Diagonais na base
        {2,0,3} // Linha extra que não forma um alinhamento real, removida ou corrigida
        // As linhas corretas são:
        // {0,1,4}, {0,2,5}, {0,3,6} (verticais e diagonais do topo)
        // {1,2,3} (horizontal superior)
        // {4,5,6} (horizontal inferior)
    };

    for (int i = 0; i < 5; i++) { // Verificando as 5 linhas principais
        int a = linhas[i][0], b = linhas[i][1], c = linhas[i][2];
        if (tabuleiro[a] != VAZIO && tabuleiro[a] == tabuleiro[b] && tabuleiro[a] == tabuleiro[c]) {
            return tabuleiro[a]; // Retorna o jogador que venceu
        }
    }
    return VAZIO; // Ninguém venceu
}

// Função de IA para escolher a melhor jogada no posicionamento
int ia_escolher_posicionamento(EstadoEspaço tabuleiro[7]) {
    int oponente = JOGADOR1;
    int ia_jogador = JOGADOR2;

    // 1. Tenta vencer: se a IA pode colocar uma peça e vencer, ela o faz.
    for (int i = 0; i < 7; i++) {
        if (tabuleiro[i] == VAZIO) {
            tabuleiro[i] = ia_jogador;
            if (verificar_vitoria(tabuleiro) == ia_jogador) {
                tabuleiro[i] = VAZIO; // Desfaz a simulação
                return i;
            }
            tabuleiro[i] = VAZIO;
        }
    }

    // 2. Tenta bloquear: se o oponente pode vencer na próxima jogada, a IA bloqueia.
    for (int i = 0; i < 7; i++) {
        if (tabuleiro[i] == VAZIO) {
            tabuleiro[i] = oponente;
            if (verificar_vitoria(tabuleiro) == oponente) {
                tabuleiro[i] = VAZIO;
                return i;
            }
            tabuleiro[i] = VAZIO;
        }
    }
    
    // 3. Estratégia simples: ocupa o centro ou uma posição aleatória.
    if (tabuleiro[2] == VAZIO) return 2; // Tenta pegar o centro
    if (tabuleiro[5] == VAZIO) return 5; // Ou o centro inferior
    
    int posicoes_vazias[7];
    int count = 0;
    for (int i = 0; i < 7; i++) {
        if (tabuleiro[i] == VAZIO) posicoes_vazias[count++] = i;
    }
    if (count > 0) return posicoes_vazias[rand() % count];

    return -1; // Não deveria acontecer
}

// Função de IA para escolher o melhor movimento
void ia_escolher_movimento(EstadoEspaço tabuleiro[7], int *origem, int *destino) {
    int oponente = JOGADOR1;
    int ia_jogador = JOGADOR2;

    // Tenta encontrar um movimento que leve à vitória
    for (int o = 0; o < 7; o++) {
        if (tabuleiro[o] == ia_jogador) {
            for (int d = 0; d < 7; d++) {
                if (movimento_valido(o, d, tabuleiro)) {
                    tabuleiro[o] = VAZIO;
                    tabuleiro[d] = ia_jogador;
                    if (verificar_vitoria(tabuleiro) == ia_jogador) {
                        *origem = o;
                        *destino = d;
                        tabuleiro[o] = ia_jogador; // Desfaz
                        tabuleiro[d] = VAZIO;   // Desfaz
                        return;
                    }
                    tabuleiro[o] = ia_jogador; // Desfaz
                    tabuleiro[d] = VAZIO;   // Desfaz
                }
            }
        }
    }
    // Tenta encontrar um movimento para bloquear o oponente
    // (Lógica similar, mas verificando se o oponente venceria)

    // Se não, faz um movimento aleatório válido
    int movimentos_validos[42][2];
    int count = 0;
    for (int o = 0; o < 7; o++) {
        if (tabuleiro[o] == ia_jogador) {
            for (int d = 0; d < 7; d++) {
                if (movimento_valido(o, d, tabuleiro)) {
                    movimentos_validos[count][0] = o;
                    movimentos_validos[count][1] = d;
                    count++;
                }
            }
        }
    }

    if (count > 0) {
        int escolha = rand() % count;
        *origem = movimentos_validos[escolha][0];
        *destino = movimentos_validos[escolha][1];
    } else {
        *origem = -1;
        *destino = -1;
    }
}


// --- LÓGICA DE POSICIONAMENTO E MOVIMENTAÇÃO (AGORA COM IA INTEGRADA) ---

void posicionamento(ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_DISPLAY *janela, ALLEGRO_FONT *font, ALLEGRO_FONT *font2,
    int win_w, int win_h, EstadoEspaço tabuleiro[7], PontoTabuleiro pontos_tabuleiro[7],
    ALLEGRO_BITMAP *peca_player1, ALLEGRO_BITMAP *peca_player2, ALLEGRO_BITMAP *background,
    int *placar_j1, int *placar_j2, bool modo_pvc, double *tempo_partida) 
{
    int jogador_atual = JOGADOR1;
    int pecas_jogador1 = 0, pecas_jogador2 = 0;
    float mx = 0, my = 0;
    
    ALLEGRO_TIMER *timer = al_create_timer(1.0); // Timer para o tempo de jogo
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_start_timer(timer);
    *tempo_partida = 0;

    while (pecas_jogador1 < 3 || pecas_jogador2 < 3) {
        desenha_tabuleiroTriangulo(background, font, font2, win_w, win_h, tabuleiro, pontos_tabuleiro, peca_player1, peca_player2, mx, my, *placar_j1, *placar_j2, -1, false);
        
        // Se for a vez do computador, ele joga sozinho
        if (modo_pvc && jogador_atual == JOGADOR2) {
            al_rest(1.0); // Pausa para o jogador ver o que aconteceu
            int jogada_ia = ia_escolher_posicionamento(tabuleiro);
            if (jogada_ia != -1) {
                tabuleiro[jogada_ia] = JOGADOR2;
                pecas_jogador2++;
                jogador_atual = JOGADOR1;
            }
            continue; // Volta para o início do loop
        }

        ALLEGRO_EVENT event;
        al_wait_for_event(queue, &event);

        if (event.type == ALLEGRO_EVENT_TIMER) {
            (*tempo_partida)++;
        } else if (event.type == ALLEGRO_EVENT_MOUSE_AXES) {
            mx = event.mouse.x;
            my = event.mouse.y;
        } else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            exit(0); // Sai do jogo se fechar a janela
        } else if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            for (int i = 0; i < 7; i++) {
                float dx = mx - pontos_tabuleiro[i].x;
                float dy = my - pontos_tabuleiro[i].y;
                if (dx*dx + dy*dy <= 25*25 && tabuleiro[i] == VAZIO) {
                    if (jogador_atual == JOGADOR1 && pecas_jogador1 < 3) {
                        tabuleiro[i] = JOGADOR1;
                        pecas_jogador1++;
                        jogador_atual = JOGADOR2;
                    } else if (jogador_atual == JOGADOR2 && pecas_jogador2 < 3 && !modo_pvc) {
                        tabuleiro[i] = JOGADOR2;
                        pecas_jogador2++;
                        jogador_atual = JOGADOR1;
                    }
                    break;
                }
            }
        }
    }
    al_stop_timer(timer);
    al_destroy_timer(timer);
}


void movimentacao(ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_DISPLAY* janela, ALLEGRO_FONT *font, ALLEGRO_FONT *font2, int win_w, int win_h,
    EstadoEspaço tabuleiro[7], PontoTabuleiro pontos_tabuleiro[7],
    ALLEGRO_BITMAP *peca_player1, ALLEGRO_BITMAP *peca_player2, ALLEGRO_BITMAP *background,
    int *placar_j1, int *placar_j2, bool modo_pvc, double *tempo_partida)
{
    int jogador_atual = JOGADOR1;
    int selecionada = -1;
    float mouse_x = 0, mouse_y = 0;
    
    ALLEGRO_TIMER *timer = al_create_timer(1.0);
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_start_timer(timer);

    while (1) {
        desenha_tabuleiroTriangulo(background, font, font2, win_w, win_h, tabuleiro, pontos_tabuleiro, peca_player1, peca_player2, mouse_x, mouse_y, *placar_j1, *placar_j2, selecionada, true);
        
        EstadoEspaço vencedor = verificar_vitoria(tabuleiro);
        if (vencedor != VAZIO) {
            char msg[64], res[30];
            sprintf(msg, "Jogador %d venceu!", vencedor);
            sprintf(res, "Vitoria J%d", vencedor);
            al_show_native_message_box(NULL, "Fim de Jogo", "Vitória!", msg, NULL, 0);
            if (vencedor == JOGADOR1) (*placar_j1)++; else (*placar_j2)++;
            salvar_registro(modo_pvc ? "PvC" : "PvP", res, *tempo_partida);
            return;
        }

        if (modo_pvc && jogador_atual == JOGADOR2) {
            al_rest(1.0);
            int origem = -1, destino = -1;
            ia_escolher_movimento(tabuleiro, &origem, &destino);
            if (origem != -1) {
                tabuleiro[destino] = JOGADOR2;
                tabuleiro[origem] = VAZIO;
                jogador_atual = JOGADOR1;
            }
            continue;
        }

        ALLEGRO_EVENT ev;
        al_wait_for_event(queue, &ev);
        
        if (ev.type == ALLEGRO_EVENT_TIMER) {
            (*tempo_partida)++;
        } else if (ev.type == ALLEGRO_EVENT_MOUSE_AXES) {
            mouse_x = ev.mouse.x;
            mouse_y = ev.mouse.y;
        } else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            exit(0);
        } else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            float mx = ev.mouse.x;
            float my = ev.mouse.y;

            // Botão de Salvar
            if (mx >= 618 && mx <= 708 && my >= 378 && my <= 402) {
                salvar_partida_atual(tabuleiro, jogador_atual, 3, 3, *tempo_partida);
                al_show_native_message_box(janela, "Jogo Salvo", "A partida foi salva!", NULL, NULL, 0);
                continue;
            }

            if (selecionada == -1) {
                for (int i = 0; i < 7; i++) {
                    float dx = mx - pontos_tabuleiro[i].x;
                    float dy = my - pontos_tabuleiro[i].y;
                    if (dx*dx + dy*dy <= 25*25 && tabuleiro[i] == jogador_atual) {
                        selecionada = i;
                        break;
                    }
                }
            } else {
                for (int i = 0; i < 7; i++) {
                    float dx = mx - pontos_tabuleiro[i].x;
                    float dy = my - pontos_tabuleiro[i].y;
                    if (dx*dx + dy*dy <= 25*25 && movimento_valido(selecionada, i, tabuleiro)) {
                        tabuleiro[i] = jogador_atual;
                        tabuleiro[selecionada] = VAZIO;
                        selecionada = -1;
                        jogador_atual = (jogador_atual == JOGADOR1) ? JOGADOR2 : JOGADOR1;
                        break;
                    }
                }
                if (ev.mouse.button == 1) selecionada = -1; // Desseleciona se clicar em qualquer lugar
            }
        }
    }
    al_stop_timer(timer);
    al_destroy_timer(timer);
}


// --- FUNÇÕES PRINCIPAL E DE TELAS ---

int main(void)
{
    srand(time(NULL));
    int placar_j1 = 0, placar_j2 = 0;
    EstadoEspaço tabuleiro[7];
    PontoTabuleiro pontos_tabuleiro[7] = {
        {364, 80}, {214, 180}, {364, 180}, {514, 180},
        {114, 330}, {364, 330}, {614, 330}
    };

    // Inicialização do Allegro...
    al_init();
    al_init_font_addon();
    al_init_ttf_addon();
    al_init_primitives_addon();
    al_install_mouse();
    al_init_image_addon();

    ALLEGRO_FONT *font = al_load_ttf_font("Content/alice.ttf", 24, 0);
    ALLEGRO_FONT *font2 = al_load_ttf_font("Content/mine.otf", 24, 0);
    ALLEGRO_DISPLAY *janela = al_create_display(728, 410);
    al_set_window_title(janela, "Tri Angle");

    ALLEGRO_BITMAP *background = al_load_bitmap("Content/background.jpg");
    ALLEGRO_BITMAP *peca_player1 = al_load_bitmap("Content/player1.png");
    ALLEGRO_BITMAP *peca_player2 = al_load_bitmap("Content/player2.png");
    ALLEGRO_BITMAP *chess_background = al_load_bitmap("Content/boardbackground.jpeg");
    ALLEGRO_BITMAP *help_image = al_load_bitmap("Content/help.png");
    ALLEGRO_BITMAP *history_image = al_load_bitmap("Content/historico.png");
    ALLEGRO_BITMAP *modos_image = al_load_bitmap("Content/Modos.png");

    int win_w = al_get_display_width(janela);
    int win_h = al_get_display_height(janela);

    ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();
    al_register_event_source(queue, al_get_display_event_source(janela));
    al_register_event_source(queue, al_get_mouse_event_source());

    bool running = true;
    int tela_atual = 0; // 0: Menu, 1: Modos, 2: Ajuda, 3: Histórico

    while (running) {
        if (tela_atual == 0) { // Tela de Menu
            //desenha_menu1(background, font);
             al_draw_text(font, al_map_rgb(0,0,0), win_w/2, 50, ALLEGRO_ALIGN_CENTER, "MENU PRINCIPAL");
             // Desenhar botões aqui
        } else if (tela_atual == 1) { // Tela de Modos
            //desenha_modos2(background, modos_image, font, font2, win_w, win_h);
        } // ... etc para outras telas

        ALLEGRO_EVENT event;
        al_wait_for_event(queue, &event);

        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            running = false;
        } else if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            int mx = event.mouse.x;
            int my = event.mouse.y;

            if (tela_atual == 0) { // Lógica de cliques no Menu
                // Botão Jogar (PvP)
                if (mx > 100 && mx < 300 && my > 100 && my < 150) {
                     for (int i = 0; i < 7; i++) tabuleiro[i] = VAZIO;
                     double tempo = 0;
                     posicionamento(queue, janela, font, font2, win_w, win_h, tabuleiro, pontos_tabuleiro, peca_player1, peca_player2, chess_background, &placar_j1, &placar_j2, false, &tempo);
                     movimentacao(queue, janela, font, font2, win_w, win_h, tabuleiro, pontos_tabuleiro, peca_player1, peca_player2, chess_background, &placar_j1, &placar_j2, false, &tempo);
                }
                // Botão Jogar (PvC)
                if (mx > 100 && mx < 300 && my > 160 && my < 210) {
                     for (int i = 0; i < 7; i++) tabuleiro[i] = VAZIO;
                     double tempo = 0;
                     posicionamento(queue, janela, font, font2, win_w, win_h, tabuleiro, pontos_tabuleiro, peca_player1, peca_player2, chess_background, &placar_j1, &placar_j2, true, &tempo);
                     movimentacao(queue, janela, font, font2, win_w, win_h, tabuleiro, pontos_tabuleiro, peca_player1, peca_player2, chess_background, &placar_j1, &placar_j2, true, &tempo);
                }
                // Botão Continuar
                 if (mx > 100 && mx < 300 && my > 220 && my < 270) {
                     double tempo = 0;
                     int jogador_vez, p1, p2;
                     if(carregar_partida_salva(tabuleiro, &jogador_vez, &p1, &p2, &tempo)) {
                         movimentacao(queue, janela, font, font2, win_w, win_h, tabuleiro, pontos_tabuleiro, peca_player1, peca_player2, chess_background, &placar_j1, &placar_j2, false, &tempo);
                     } else {
                         al_show_native_message_box(janela, "Erro", "Nenhum jogo salvo encontrado.", NULL, NULL, ALLEGRO_MESSAGEBOX_ERROR);
                     }
                 }
                // Botão Histórico
                if (mx > 100 && mx < 300 && my > 280 && my < 330) {
                    desenha_historico4(background, history_image, font, win_w, win_h);
                    al_flip_display();
                    al_wait_for_event_timed(queue, &event, 5.0); // Mostra por 5s
                }
                 // Botão Sair
                if (mx > 100 && mx < 300 && my > 340 && my < 390) {
                    running = false;
                }
            }
        }
    }

    // Liberação de memória
    al_destroy_font(font);
    al_destroy_font(font2);
    al_destroy_display(janela);
    al_destroy_event_queue(queue);
    // ... destruir todos os bitmaps
    return 0;
}


// --- IMPLEMENTAÇÃO DAS FUNÇÕES AUXILIARES ---

void desenha_tabuleiroTriangulo(
    ALLEGRO_BITMAP *background, ALLEGRO_FONT *font, ALLEGRO_FONT *font2, int win_w, int win_h,
    EstadoEspaço tabuleiro[7], PontoTabuleiro pontos_tabuleiro[7],
    ALLEGRO_BITMAP *peca_player1, ALLEGRO_BITMAP *peca_player2,
    float mouse_x, float mouse_y, int placar_j1, int placar_j2,
    int peca_selecionada, bool mostrar_movimentos_validos
) {
    al_draw_bitmap(background, 0, 0, 0);

       // Conta as peças de cada jogador no tabuleiro
    int pecas_jogador1 = 0, pecas_jogador2 = 0;
    for (int i = 0; i < 7; i++) {
        if (tabuleiro[i] == JOGADOR1) pecas_jogador1++; // Conta peças do jogador 1
        if (tabuleiro[i] == JOGADOR2) pecas_jogador2++; // Conta peças do jogador 2
    }
    // Desenha a quantidade de peças de cada jogador na tela
    pecas_jogadores(font, font2, win_w, win_h, pecas_jogador1, pecas_jogador2);

    // Desenha as linhas do triângulo conectando os pontos do tabuleiro
    // Desenha as linhas que conectam os pontos do tabuleiro formando o triângulo e suas conexões internas
    al_draw_line(pontos_tabuleiro[0].x, pontos_tabuleiro[0].y, pontos_tabuleiro[1].x, pontos_tabuleiro[1].y, al_map_rgb(0,0,0), 3); // Linha do topo para a esquerda superior
    al_draw_line(pontos_tabuleiro[0].x, pontos_tabuleiro[0].y, pontos_tabuleiro[2].x, pontos_tabuleiro[2].y, al_map_rgb(0,0,0), 3); // Linha do topo para o centro
    al_draw_line(pontos_tabuleiro[0].x, pontos_tabuleiro[0].y, pontos_tabuleiro[3].x, pontos_tabuleiro[3].y, al_map_rgb(0,0,0), 3); // Linha do topo para a direita superior
    al_draw_line(pontos_tabuleiro[1].x, pontos_tabuleiro[1].y, pontos_tabuleiro[2].x, pontos_tabuleiro[2].y, al_map_rgb(0,0,0), 3); // Linha da esquerda superior para o centro
    al_draw_line(pontos_tabuleiro[2].x, pontos_tabuleiro[2].y, pontos_tabuleiro[3].x, pontos_tabuleiro[3].y, al_map_rgb(0,0,0), 3); // Linha do centro para a direita superior
    al_draw_line(pontos_tabuleiro[1].x, pontos_tabuleiro[1].y, pontos_tabuleiro[4].x, pontos_tabuleiro[4].y, al_map_rgb(0,0,0), 3); // Linha da esquerda superior para a esquerda inferior
    al_draw_line(pontos_tabuleiro[2].x, pontos_tabuleiro[2].y, pontos_tabuleiro[5].x, pontos_tabuleiro[5].y, al_map_rgb(0,0,0), 3); // Linha do centro para o centro inferior
    al_draw_line(pontos_tabuleiro[3].x, pontos_tabuleiro[3].y, pontos_tabuleiro[6].x, pontos_tabuleiro[6].y, al_map_rgb(0,0,0), 3); // Linha da direita superior para a direita inferior
    al_draw_line(pontos_tabuleiro[4].x, pontos_tabuleiro[4].y, pontos_tabuleiro[5].x, pontos_tabuleiro[5].y, al_map_rgb(0,0,0), 3); // Linha da esquerda inferior para o centro inferior
    al_draw_line(pontos_tabuleiro[5].x, pontos_tabuleiro[5].y , pontos_tabuleiro[6].x, pontos_tabuleiro[6].y, al_map_rgb(0,0,0), 3); // Linha do centro inferior para a direita inferior

    // Percorre todas as casas do tabuleiro para desenhar círculos e peças
    for (int i = 0; i < 7; i++) {

        // Calcula a distância do mouse até o centro da casa
        float dx = mouse_x - pontos_tabuleiro[i].x;
        float dy = mouse_y - pontos_tabuleiro[i].y;
        bool mouse_sobre = (dx*dx + dy*dy <= 25*25); // Verifica se o mouse está sobre a casa

        // Se o mouse está sobre uma casa vazia, desenha um destaque amarelo semi-transparente
        if (mouse_sobre && tabuleiro[i] == VAZIO) {
            al_draw_filled_circle(pontos_tabuleiro[i].x, pontos_tabuleiro[i].y, 27, al_map_rgba(255, 255, 0, 120));
        }

        // Se a casa tem peça do jogador 1, desenha a imagem da peça do jogador 1
        if (tabuleiro[i] == JOGADOR1 && peca_player1) {
            al_draw_scaled_bitmap(
                peca_player1, 0, 0,
                al_get_bitmap_width(peca_player1), al_get_bitmap_height(peca_player1),
                pontos_tabuleiro[i].x - 25, pontos_tabuleiro[i].y - 25, 50, 50, 0
            );
        }
        // Se a casa tem peça do jogador 2, desenha a imagem da peça do jogador 2
        else if (tabuleiro[i] == JOGADOR2 && peca_player2) {
            al_draw_scaled_bitmap(
                peca_player2, 0, 0,
                al_get_bitmap_width(peca_player2), al_get_bitmap_height(peca_player2),
                pontos_tabuleiro[i].x - 25, pontos_tabuleiro[i].y - 25, 50, 50, 0
            );
        }
        // Se a casa está vazia, desenha um círculo cinza claro com borda preta
        else {
            al_draw_filled_circle(pontos_tabuleiro[i].x, pontos_tabuleiro[i].y, 25, al_map_rgb(200, 200, 200));
            al_draw_circle(pontos_tabuleiro[i].x, pontos_tabuleiro[i].y, 25, al_map_rgb(0,0,0), 3);
        }
    }

    // Lógica para mostrar movimentos válidos 
    if (mostrar_movimentos_validos && peca_selecionada != -1) {
        for (int i = 0; i < 7; i++) {
            if (movimento_valido(peca_selecionada, i, tabuleiro)) {
                // Desenha um círculo verde para indicar um movimento válido
                al_draw_circle(pontos_tabuleiro[i].x, pontos_tabuleiro[i].y, 30, al_map_rgb(0, 255, 0), 3);
            }
        }
    }

   // Desenha o título do jogo no topo do tabuleiro
    al_draw_text(font, al_map_rgb(0, 0, 0), win_w / 2, 30, ALLEGRO_ALIGN_CENTER, "Tri- Angle");
    // Desenha o texto "Pausar" na parte inferior
                
                        // Área inferior (barra de botões e placar) - visual moderno
            al_draw_filled_rounded_rectangle(0, 370, 728, 410, 12, 12, al_map_rgb(230, 230, 240));
            al_draw_rounded_rectangle(0, 370, 728, 410, 12, 12, al_map_rgb(80,80,80), 2);

            // Botão "Pausar" (esquerda)
            al_draw_filled_rounded_rectangle(50, 378, 180, 402, 10, 10, al_map_rgb(255, 240, 240));
            al_draw_rounded_rectangle(50, 378, 180, 402, 10, 10, al_map_rgb(200,0,0), 2);
            al_draw_text(font, al_map_rgb(200, 0, 0), 115, 376, ALLEGRO_ALIGN_CENTER, "Pausar");

            // Placar (centralizado)
            char placar[32];
            sprintf(placar, "J1: %d  -  %d :J2", placar_j1, placar_j2);
            al_draw_filled_rounded_rectangle(270, 378, 458, 402, 10, 10, al_map_rgb(240, 240, 220));
            al_draw_rounded_rectangle(270, 378, 458, 402, 10, 10, al_map_rgb(120,120,120), 2);
            al_draw_text(font, al_map_rgb(60, 60, 60), 364, 378, ALLEGRO_ALIGN_CENTER, placar);

            // Botão "Empate" (à direita do placar)
            al_draw_filled_rounded_rectangle(478, 378, 598, 402, 10, 10, al_map_rgb(240, 255, 240));
            al_draw_rounded_rectangle(478, 378, 598, 402, 10, 10, al_map_rgb(0,150,0), 2);
            al_draw_text(font, al_map_rgb(0, 120, 0), 538, 376, ALLEGRO_ALIGN_CENTER, "Empate");

            // Botão "Sair" (direita)
            al_draw_filled_rounded_rectangle(618, 378, 708, 402, 10, 10, al_map_rgb(240, 240, 255));
            al_draw_rounded_rectangle(618, 378, 708, 402, 10, 10, al_map_rgb(0,0,200), 2);
            al_draw_text(font, al_map_rgb(0, 0, 200), 663, 376, ALLEGRO_ALIGN_CENTER, "Sair");
            
            // Botão "Salvar" (direita)   
            al_draw_filled_rounded_rectangle(618, 378, 708, 402, 10, 10, al_map_rgb(240, 240, 255));
            al_draw_rounded_rectangle(618, 378, 708, 402, 10, 10, al_map_rgb(0,0,200), 2);
            al_draw_text(font, al_map_rgb(0, 0, 200), 663, 376, ALLEGRO_ALIGN_CENTER, "Salvar"); // Adicionamos o botão salvar na tela de jogo

    al_flip_display();
}

bool movimento_valido(int origem, int destino, EstadoEspaço tabuleiro[7]) {
    if (tabuleiro[destino] != VAZIO) return false;
    
    int adjacentes[7][4] = {
        {1, 2, 3, -1}, {0, 2, 4, -1}, {0, 1, 3, 5}, {0, 2, 6, -1},
        {1, 5, -1,-1}, {2, 4, 6, -1}, {3, 5, -1, -1}
    };
    for (int i = 0; i < 4; i++) {
        if (adjacentes[origem][i] == -1) break;
        if (adjacentes[origem][i] == destino) return true;
    }
    return false;
}

// Salva um registro de partida no arquivo "registros.dat"
// Salva um registro de partida no arquivo de texto "historico.txt"
void salvar_registro(const char* modo, const char* resultado, double duracao) {
    // Abre o arquivo em modo "append text" (a), que adiciona texto no final do arquivo.
    // Se o arquivo não existir, ele será criado.
    FILE *f = fopen("historico.txt", "a"); 
    if (f) {
        // Usa fprintf para escrever uma linha de texto formatada no arquivo.
        // Usamos ';' como um separador para facilitar a leitura depois.
        fprintf(f, "%s;%s;%.2f\n", modo, resultado, duracao);
        fclose(f); // Sempre fechar o arquivo depois de usar!
    } else {
        // Se não conseguir abrir o arquivo, mostra um aviso no console.
        printf("ERRO: Nao foi possivel abrir o arquivo de historico para escrita.\n");
    }
}

// Desenha a tela de histórico lendo do arquivo
void desenha_historico4(ALLEGRO_BITMAP *background, ALLEGRO_BITMAP *History_image, ALLEGRO_FONT *font, int win_w, int win_h) {
    al_draw_bitmap(background, 0, 0, 0);
    al_draw_text(font, al_map_rgb(0, 0, 0), win_w/2, 40, ALLEGRO_ALIGN_CENTER, "Historico de Partidas");
    
    FILE *f = fopen("registros.dat", "rb");
    if (f) {
        RegistroPartida reg;
        int y = 100;
        while (fread(&reg, sizeof(RegistroPartida), 1, f)) {
            al_draw_textf(font, al_map_rgb(0, 0, 0), 50, y, 0, "Modo: %s, Res: %s, Tempo: %.0fs", reg.modo_jogo, reg.resultado, reg.duracao);
            y += 25;
        }
        fclose(f);
    } else {
        al_draw_text(font, al_map_rgb(255, 0, 0), win_w/2, 100, ALLEGRO_ALIGN_CENTER, "Nenhum registro encontrado.");
    }
}

// Salva a partida atual no arquivo "partida.sav"
void salvar_partida_atual(EstadoEspaço tabuleiro[7], int jogador_atual, int p1, int p2, double tempo) {
    FILE *f = fopen("partida.sav", "wb");
    if (f) {
        JogoSalvo s;
        memcpy(s.tabuleiro, tabuleiro, sizeof(EstadoEspaço) * 7);
        s.jogador_atual = jogador_atual;
        s.pecas_posicionadas_j1 = p1;
        s.pecas_posicionadas_j2 = p2;
        s.tempo_decorrido = tempo;
        fwrite(&s, sizeof(JogoSalvo), 1, f);
        fclose(f);
    }
}

// Carrega a partida do arquivo "partida.sav"
bool carregar_partida_salva(EstadoEspaço tabuleiro[7], int *jogador_atual, int *p1, int *p2, double *tempo) {
    FILE *f = fopen("partida.sav", "rb");
    if (f) {
        JogoSalvo s;
        fread(&s, sizeof(JogoSalvo), 1, f);
        memcpy(tabuleiro, s.tabuleiro, sizeof(EstadoEspaço) * 7);
        *jogador_atual = s.jogador_atual;
        *p1 = s.pecas_posicionadas_j1;
        *p2 = s.pecas_posicionadas_j2;
        *tempo = s.tempo_decorrido;
        fclose(f);
        return true;
    }
    return false;
}





















