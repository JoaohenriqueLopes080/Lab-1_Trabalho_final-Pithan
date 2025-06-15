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


typedef struct {
    char modo_jogo[30];  // Modo de jogo (PvP ou PvC)
    char resultado[30];  // Resultado da partida (Vitória, Empate, etc.)
    double duracao;      // Duração da partida em segundos
} RegistroPartida;


// Define um tipo enum para representar o estado de cada espaço do tabuleiro: vazio, jogador 1 ou jogador 2
typedef enum { VAZIO, JOGADOR1, JOGADOR2 } EstadoEspaço;

// Estrutura para armazenar as coordenadas (x, y) de cada ponto do tabuleiro
typedef struct {
    float x, y;
} PontoTabuleiro;


// Funções utilitárias

// Função para limpar o terminal, usando o comando apropriado para o sistema operacional
void limpaTela() {
    system(CLEAR);
}

// Função para desenhar a quantidade de peças de cada jogador na tela
void pecas_jogadores(ALLEGRO_FONT *font, ALLEGRO_FONT *font2, int win_w, int win_h, int pecas_jogador1, int pecas_jogador2) {
    // Desenha o texto "Jogador 1" no canto superior esquerdo
    al_draw_text(font, al_map_rgb(255, 0, 0), 30, 10, ALLEGRO_ALIGN_LEFT, "Jogador 1");
    // Desenha o texto "Jogador 2" no canto superior direito
    al_draw_text(font, al_map_rgb(0, 0, 255), win_w - 30, 10, ALLEGRO_ALIGN_RIGHT, "Jogador 2");

    // Carrega as imagens das peças dos jogadores
    ALLEGRO_BITMAP *pecapng_player1 = al_load_bitmap("Content/player1.png");
    ALLEGRO_BITMAP *pecapng_player2 = al_load_bitmap("Content/player2.png");

    // Desenha as peças do Jogador 1 (PNG se já posicionada, círculo cinza se não)
    for (int i = 0; i < 3; i++) {
        if (i < pecas_jogador1 && pecapng_player1) {
            al_draw_scaled_bitmap(
                pecapng_player1, 0, 0,
                al_get_bitmap_width(pecapng_player1), al_get_bitmap_height(pecapng_player1),
                28 + i * 30, 28, 24, 24, 0
            );
        } else {
            al_draw_filled_circle(40 + i * 30, 40, 12, al_map_rgb(200, 200, 200));
            al_draw_circle(40 + i * 30, 40, 12, al_map_rgb(0,0,0), 2);
        }
    }

    // Desenha as peças do Jogador 2 (PNG se já posicionada, círculo cinza se não)
    for (int i = 0; i < 3; i++) {
        if (i < pecas_jogador2 && pecapng_player2) {
            al_draw_scaled_bitmap(
                pecapng_player2, 0, 0,
                al_get_bitmap_width(pecapng_player2), al_get_bitmap_height(pecapng_player2),
                win_w - 52 - i * 30, 28, 24, 24, 0
            );
        } else {
            al_draw_filled_circle(win_w - 40 - i * 30, 40, 12, al_map_rgb(200, 200, 200));
            al_draw_circle(win_w - 40 - i * 30, 40, 12, al_map_rgb(0,0,0), 2);
        }
    }

    if (pecapng_player1) al_destroy_bitmap(pecapng_player1);
    if (pecapng_player2) al_destroy_bitmap(pecapng_player2);
}

void desenha_tabuleiroTriangulo(
    ALLEGRO_BITMAP *background, ALLEGRO_FONT *font, ALLEGRO_FONT *font2, int win_w, int win_h,
    EstadoEspaço tabuleiro[7], PontoTabuleiro pontos_tabuleiro[7],
    ALLEGRO_BITMAP *peca_player1, ALLEGRO_BITMAP *peca_player2,
    float mouse_x, float mouse_y, int placar_j1, int placar_j2
    ) {
    // Desenha a imagem de fundo na tela
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

              
           
 // Atualiza a tela para mostrar tudo desenhado
    al_flip_display();
}

// Funções de desenho de telas
void desenha_menu1(ALLEGRO_BITMAP *background, ALLEGRO_FONT *font) {
    // Desenha a imagem de fundo na posição (0, 0)
    al_draw_bitmap(background, 0, 0, 0);

    // Desenha o título do jogo "Tri Angle" na posição (160, 75)
    al_draw_text(font, al_map_rgb(0, 0, 0), 160, 75, ALLEGRO_ALIGN_CENTER, "Tri Angle");

    // Desenha o texto "Jogar" na posição (160, 125)
    al_draw_text(font, al_map_rgb(0, 0, 0), 160, 125, ALLEGRO_ALIGN_CENTER, "Jogar");

    // Desenha o texto "Ajuda" na posição (160, 150)
    al_draw_text(font, al_map_rgb(0, 0, 0), 160, 150, ALLEGRO_ALIGN_CENTER, "Ajuda");

    // Desenha o texto "Historico" na posição (160, 175)
    al_draw_text(font, al_map_rgb(0, 0, 0), 160, 175, ALLEGRO_ALIGN_CENTER, "Historico");

    // Desenha o texto "Sair" na posição (160, 200)
    al_draw_text(font, al_map_rgb(0, 0, 0), 160, 200, ALLEGRO_ALIGN_CENTER, "Sair");

    // Atualiza a tela para mostrar as alterações
    al_flip_display();
}

// Função para desenhar a tela de seleção de modos de jogo
void desenha_modos2(ALLEGRO_BITMAP *background, ALLEGRO_BITMAP *play_image, ALLEGRO_FONT *font, ALLEGRO_FONT *font2, int win_w, int win_h) {
    al_draw_bitmap(background, 0, 0, 0); // Desenha o fundo na tela
    int play_w = al_get_bitmap_width(play_image); // Obtém a largura da imagem de modos
    int play_h = al_get_bitmap_height(play_image); // Obtém a altura da imagem de modos
    al_draw_scaled_bitmap(play_image, 0, 0, play_w, play_h, 0, 0, win_w, win_h, 0); // Desenha a imagem de modos redimensionada para a janela
    al_draw_text(font2, al_map_rgb(0, 0 , 0 ), 150, 310, ALLEGRO_ALIGN_CENTER, "Jogar"); // Escreve "Jogar" (modo PVP) na posição esquerda
    al_draw_text(font2, al_map_rgb(0, 0 , 0 ), 585, 310, ALLEGRO_ALIGN_CENTER, "Jogar"); // Escreve "Jogar" (modo PVM) na posição direita
    al_draw_text(font, al_map_rgb(255, 0, 0), 370, 300, ALLEGRO_ALIGN_CENTER, "Retornar ao Menu"); // Escreve "Retornar ao Menu" no centro
    al_flip_display(); // Atualiza a tela
}


void desenha_ajuda3(ALLEGRO_BITMAP *background, ALLEGRO_BITMAP *help_image, ALLEGRO_FONT *font, int win_w, int win_h) {
    // Desenha a imagem de fundo na tela, começando do canto superior esquerdo (0,0)
    al_draw_bitmap(background, 0, 0, 0);

    // Obtém a largura da imagem de ajuda
    int img_w = al_get_bitmap_width(help_image);
    // Obtém a altura da imagem de ajuda
    int img_h = al_get_bitmap_height(help_image);

    // Desenha a imagem de ajuda redimensionada para ocupar toda a janela
    al_draw_scaled_bitmap(help_image, 0, 0, img_w, img_h, 0, 0, win_w, win_h, 0);

    // Desenha o texto "Retornar ao Menu" na posição (160, 300) com cor vermelha

    al_draw_text(font, al_map_rgb(255, 0, 0), 160, 300, ALLEGRO_ALIGN_CENTER, "Retornar ao Menu");

    // Atualiza a tela para exibir as alterações
    al_flip_display();
}

void desenha_historico4(ALLEGRO_BITMAP *background, ALLEGRO_BITMAP *History_image ,ALLEGRO_FONT *font, int win_w, int win_h) {
    // Desenha a imagem de fundo na tela, começando do canto superior esquerdo (0,0)
    al_draw_bitmap(background, 0, 0, 0);

    // Obtém a largura da imagem de histórico
    int history_w = al_get_bitmap_width(History_image);
    // Obtém a altura da imagem de histórico
    int history_h = al_get_bitmap_height(History_image);

    // Desenha a imagem de histórico redimensionada para ocupar toda a janela
    al_draw_scaled_bitmap(History_image, 0, 0, history_w, history_h, 0, 0, win_w, win_h, 0);

    // Desenha o texto "Historico" centralizado um pouco acima do centro da tela
    al_draw_text(font, al_map_rgb(0, 0, 0), 364, 40, ALLEGRO_ALIGN_CENTER, "Historico");

    // Desenha o texto "Retornar ao Menu" na posição (160, 300) com cor vermelha
    al_draw_text(font, al_map_rgb(255, 0, 0), 160, 300, ALLEGRO_ALIGN_CENTER, "Retornar ao Menu");

    // Atualiza a tela para exibir as alterações
    al_flip_display();
}


// Função para verificar se um movimento entre duas casas é válido (adjacente ou salto)
bool movimento_valido(int origem, int destino, EstadoEspaço tabuleiro[7]) {
    // Matriz de adjacências diretas: cada linha indica as casas adjacentes à casa de índice correspondente
    int adjacentes[7][4] = {
        {1, 2, 3, -1},    // 0: adjacente a 1, 2, 3
        {0, 2, 4, -1},    // 1: adjacente a 0, 2, 4
        {0, 1, 3, 5},     // 2: adjacente a 0, 1, 3, 5
        {0, 2, 6, -1},    // 3: adjacente a 0, 2, 6
        {1, 5, -1, -1},   // 4: adjacente a 1, 5
        {2, 4, 6, -1},    // 5: adjacente a 2, 4, 6
        {3, 5, -1, -1}    // 6: adjacente a 3, 5
    };

    // Matriz de saltos possíveis: cada linha representa {origem, sobre, destino}
    int saltos[][3] = {
        {0, 2, 5}, {0, 1, 4}, {0, 3, 6},      // Saltos a partir da casa 0
        {1, 2, 3}, {1, 4, 5},                 // Saltos a partir da casa 1
        {2, 0, 3}, {2, 5, 6}, {2, 1, 4},      // Saltos a partir da casa 2
        {3, 2, 1}, {3, 6, 5}, {3, 0, 1},      // Saltos a partir da casa 3
        {4, 1, 0}, {4, 5, 6},                 // Saltos a partir da casa 4
        {5, 2, 0}, {5, 4, 1}, {5, 6, 3},      // Saltos a partir da casa 5
        {6, 3, 0}, {6, 5, 2}, {6, 4, 1}       // Saltos a partir da casa 6
    };
    int num_saltos = sizeof(saltos) / sizeof(saltos[0]); // Calcula o número de saltos possíveis

    // Verifica se destino é adjacente direto da origem
    for (int i = 0; i < 4; i++) {
        if (adjacentes[origem][i] == -1) break;           // Fim da lista de adjacentes
        if (adjacentes[origem][i] == destino) return true; // Movimento válido se for adjacente
    }
    // Verifica se é possível realizar um salto sobre uma peça adversária
    for (int i = 0; i < num_saltos; i++) {
        if (saltos[i][0] == origem && saltos[i][2] == destino) { // Se origem e destino correspondem ao salto
            int sobre = saltos[i][1];                            // Índice da casa "sobre" a ser pulada
            // Só permite salto se a casa "sobre" não está vazia, é adversária e destino está vazio
            if (tabuleiro[sobre] != VAZIO && tabuleiro[sobre] != tabuleiro[origem] && tabuleiro[destino] == VAZIO)
                return true; // Movimento de salto válido
        }
    }
    return false; // Movimento inválido
}



EstadoEspaço verificar_vitoria(EstadoEspaço tabuleiro[7]) {
    // Todas as combinações de 3 casas alinhadas possíveis
    int linhas[8][3] = {
        {0, 1, 4}, {0, 2, 5}, {0, 3, 6},
        {1, 2, 3}, {4, 5, 6}, {1, 5, 6},
        {3, 5, 4}, {2, 5, 3}
    };

    for (int i = 0; i < 8; i++) {
        int a = linhas[i][0], b = linhas[i][1], c = linhas[i][2];
        if (tabuleiro[a] != VAZIO &&
            tabuleiro[a] == tabuleiro[b] &&
            tabuleiro[a] == tabuleiro[c]) {
            return tabuleiro[a]; // Retorna o jogador que venceu
        }
    }
    return VAZIO; // Retorna VAZIO se não houver vencedor
}


void empate_final(ALLEGRO_FONT *font, int win_w, int win_h) {
    // Define a posição do popup
    // Fundo semi-transparente
    al_draw_filled_rectangle(0, 0, win_w, win_h, al_map_rgba(0, 0, 0, 180));
    // Retângulo do popup (apenas números literais, sem variáveis)
    al_draw_filled_rounded_rectangle(164, 145, 564, 265, 20, 20, al_map_rgb(240, 240, 255));
    al_draw_rounded_rectangle(164, 145, 564, 265, 20, 20, al_map_rgb(0,0,0), 3);

    // Mensagem centralizada
    al_draw_text(font, al_map_rgb(0, 120, 0), 364, 175, ALLEGRO_ALIGN_CENTER, "Empate! Ambos os jogadores concordaram em empatar.");
    al_draw_text(font, al_map_rgb(80, 80, 80), 364, 215, ALLEGRO_ALIGN_CENTER, "Clique para fechar");
    al_flip_display();

    // Espera um clique para fechar o popup
    ALLEGRO_EVENT_QUEUE *popup_queue = al_create_event_queue();
    al_register_event_source(popup_queue, al_get_mouse_event_source());
    ALLEGRO_EVENT ev;
    while (1) {
        al_wait_for_event(popup_queue, &ev);
        if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
            break;
    }
    al_destroy_event_queue(popup_queue);
}

void empate_espera(ALLEGRO_FONT *font, int win_w, int win_h) {
    al_draw_filled_rectangle(0, 0, win_w, win_h, al_map_rgba(0, 0, 0, 180));
    al_draw_filled_rounded_rectangle(164, 145, 564, 265, 20, 20, al_map_rgb(240, 240, 255));
    al_draw_rounded_rectangle(164, 145, 564, 265, 20, 20, al_map_rgb(0,0,0), 3);

    al_draw_text(font, al_map_rgb(200, 120, 0), 364, 175, ALLEGRO_ALIGN_CENTER, "Aguardando... O outro jogador também deve pedir empate.");
    al_draw_text(font, al_map_rgb(80, 80, 80), 364, 215, ALLEGRO_ALIGN_CENTER, "Clique para fechar");
    al_flip_display();

    ALLEGRO_EVENT_QUEUE *popup_queue = al_create_event_queue();
    al_register_event_source(popup_queue, al_get_mouse_event_source());
    ALLEGRO_EVENT ev;
    while (1) {
        al_wait_for_event(popup_queue, &ev);
        if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            break; // Adicione o fechamento do loop
        }
    }
    al_destroy_event_queue(popup_queue);
}

// Função para exibir a tela de pausa do jogo
void pausar_jogo(ALLEGRO_FONT *font2, int win_w, int win_h) {
    // Limpa a tela e define a cor de fundo como preto


    al_draw_filled_rectangle(0, 0, 728, 410, al_map_rgba(0, 0, 0, 180));
    al_draw_filled_rounded_rectangle(164, 145, 564, 265, 20, 20, al_map_rgb(240, 240, 255));
    al_draw_rounded_rectangle(164, 145, 564, 265, 20, 20, al_map_rgb(0,0,0), 3);

    // "Jogo Pausado" no topo do retângulo, centralizado
    al_draw_text(font2, al_map_rgb(255, 255, 255), 364, 155, ALLEGRO_ALIGN_CENTER, "Jogo Pausado");
    // Desenha o texto de instrução em vermelho, logo abaixo do texto principal
    al_draw_text(font2, al_map_rgb(255, 0, 0), win_w / 2, win_h / 2 + 20, ALLEGRO_ALIGN_CENTER, "Pressione qualquer tecla para continuar");
    // Atualiza a tela para mostrar os textos desenhados
    al_flip_display();
    // Aguarda 2 segundos (pausa temporária)
    al_rest(2.0); // Pausa por 2 segundos
}


// Função para implementar IA simples
int ia_escolher_jogada(EstadoEspaço tabuleiro[7], PontoTabuleiro pontos_tabuleiro[7], int jogador_ia) {
    // Estratégia simples: primeiro tenta vencer, depois bloqueia o oponente, senão joga aleatoriamente
    
    // 1. Verifica se pode vencer em uma jogada
    for (int i = 0; i < 7; i++) {
        if (tabuleiro[i] == VAZIO) {
            tabuleiro[i] = jogador_ia; // Simula a jogada
            if (verificar_vitoria(tabuleiro) == jogador_ia) {
                tabuleiro[i] = VAZIO; // Desfaz a simulação
                return i; // Retorna a posição vencedora
            }
            tabuleiro[i] = VAZIO; // Desfaz a simulação
        }
    }
    
    // 2. Verifica se precisa bloquear o oponente
    int oponente = (jogador_ia == JOGADOR1) ? JOGADOR2 : JOGADOR1;
    for (int i = 0; i < 7; i++) {
        if (tabuleiro[i] == VAZIO) {
            tabuleiro[i] = oponente; // Simula jogada do oponente
            if (verificar_vitoria(tabuleiro) == oponente) {
                tabuleiro[i] = VAZIO; // Desfaz a simulação
                return i; // Bloqueia o oponente
            }
            tabuleiro[i] = VAZIO; // Desfaz a simulação
        }
    }
    
    // 3. Joga em uma posição aleatória válida
    int posicoes_validas[7];
    int num_validas = 0;
    
    for (int i = 0; i < 7; i++) {
        if (tabuleiro[i] == VAZIO) {
            posicoes_validas[num_validas++] = i;
        }
    }
    
    if (num_validas > 0) {
        return posicoes_validas[rand() % num_validas];
    }
    
    return -1; // Nenhuma jogada válida
}

// Função para IA escolher movimento na fase de movimentação
bool ia_escolher_movimento(EstadoEspaço tabuleiro[7], int jogador_ia, int *origem, int *destino) {
    // Primeiro tenta vencer
    for (int i = 0; i < 7; i++) {
        if (tabuleiro[i] == jogador_ia) {
            for (int j = 0; j < 7; j++) {
                if (movimento_valido(i, j, tabuleiro)) {
                    // Simula o movimento
                    EstadoEspaço temp = tabuleiro[j];
                    tabuleiro[j] = tabuleiro[i];
                    tabuleiro[i] = VAZIO;
                    
                    if (verificar_vitoria(tabuleiro) == jogador_ia) {
                        // Desfaz a simulação
                        tabuleiro[i] = tabuleiro[j];
                        tabuleiro[j] = temp;
                        *origem = i;
                        *destino = j;
                        return true;
                    }
                    
                    // Desfaz a simulação
                    tabuleiro[i] = tabuleiro[j];
                    tabuleiro[j] = temp;
                }
            }
        }
    }
    
    // Depois tenta bloquear
    int oponente = (jogador_ia == JOGADOR1) ? JOGADOR2 : JOGADOR1;
    for (int i = 0; i < 7; i++) {
        if (tabuleiro[i] == jogador_ia) {
            for (int j = 0; j < 7; j++) {
                if (movimento_valido(i, j, tabuleiro)) {
                    // Simula o movimento
                    EstadoEspaço temp = tabuleiro[j];
                    tabuleiro[j] = tabuleiro[i];
                    tabuleiro[i] = VAZIO;
                    
                    // Verifica se bloqueia uma vitória do oponente
                    bool bloqueia = false;
                    for (int k = 0; k < 7; k++) {
                        if (tabuleiro[k] == VAZIO) {
                            tabuleiro[k] = oponente;
                            if (verificar_vitoria(tabuleiro) == oponente) {
                                bloqueia = true;
                            }
                            tabuleiro[k] = VAZIO;
                            if (bloqueia) break;
                        }
                    }
                    
                    // Desfaz a simulação
                    tabuleiro[i] = tabuleiro[j];
                    tabuleiro[j] = temp;
                    
                    if (bloqueia) {
                        *origem = i;
                        *destino = j;
                        return true;
                    }
                }
            }
        }
    }
    
    // Movimento aleatório
    for (int i = 0; i < 7; i++) {
        if (tabuleiro[i] == jogador_ia) {
            for (int j = 0; j < 7; j++) {
                if (movimento_valido(i, j, tabuleiro)) {
                    *origem = i;
                    *destino = j;
                    return true;
                }
            }
        }
    }
    
    return false;
}


void posicionamento(ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_BITMAP *background, ALLEGRO_FONT *font, ALLEGRO_FONT *font2, int win_w, int win_h,
    EstadoEspaço tabuleiro[7], PontoTabuleiro pontos_tabuleiro[7],
    ALLEGRO_BITMAP *peca_player1, ALLEGRO_BITMAP *peca_player2, int *placar_j1, int *placar_j2) {
    int jogador_atual = JOGADOR1; // Começa com o jogador 1
    int pecas_jogador1 = 0, pecas_jogador2 = 0; // Contadores de peças posicionadas por cada jogador
    int total_pecas = 0; // Não é usado, pode ser removido
    bool posicionando = true; // Flag para controlar o loop de posicionamento
    float mx = 0, my = 0; // Coordenadas do mouse

    while (posicionando) { // Loop até todas as peças serem posicionadas
        desenha_tabuleiroTriangulo(background, font, font2, win_w, win_h, tabuleiro, pontos_tabuleiro, peca_player1, peca_player2, mx, my, *placar_j1, *placar_j2);
        ALLEGRO_EVENT event; // Evento Allegro
        al_wait_for_event(queue, &event); // Espera por um evento (mouse, etc.)

        if (event.type == ALLEGRO_EVENT_MOUSE_AXES) { // Se o mouse se moveu
            mx = event.mouse.x; // Atualiza coordenada X do mouse
            my = event.mouse.y; // Atualiza coordenada Y do mouse
        }

        if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) { // Se o mouse foi clicado
            float mx = event.mouse.x; // Pega coordenada X do clique
            float my = event.mouse.y; // Pega coordenada Y do clique

              if (mx >= 50 && mx <= 180 && my >= 378 && my <= 402) {
                pausar_jogo(font2, win_w, win_h);
                continue; // Volta para o loop sem processar jogada
            }


            // Verifica em qual casa do tabuleiro o jogador clicou e se está vazia
            for (int i = 0; i < 7; i++) {
                float dx = mx - pontos_tabuleiro[i].x; // Diferença X para o centro da casa
                float dy = my - pontos_tabuleiro[i].y; // Diferença Y para o centro da casa
                if (dx*dx + dy*dy <= 25*25 && tabuleiro[i] == VAZIO) { // Se clicou dentro do círculo e está vazio
                    // Só permite se o jogador ainda não colocou as 3 peças
                    if ((jogador_atual == JOGADOR1 && pecas_jogador1 < 3) ||
                        (jogador_atual == JOGADOR2 && pecas_jogador2 < 3)) {

                        tabuleiro[i] = jogador_atual; // Coloca a peça do jogador atual na casa
                        if (jogador_atual == JOGADOR1) pecas_jogador1++; // Incrementa peças do jogador 1
                        else pecas_jogador2++; // Incrementa peças do jogador 2

                        EstadoEspaço vencedor = verificar_vitoria(tabuleiro);
                            if (vencedor != VAZIO) {
                                char msg[64];
                                sprintf(msg, "Jogador %d venceu!", vencedor);
                                al_show_native_message_box(NULL, "Fim de Jogo", "Vitória!", msg, NULL, 0);
                                if (vencedor == JOGADOR1) (*placar_j1)++;
                                else if (vencedor == JOGADOR2) (*placar_j2)++;
                                return; // Sai da função de posicionamento
}
                        

                        // Alterna para o outro jogador
                        jogador_atual = (jogador_atual == JOGADOR1) ? JOGADOR2 : JOGADOR1;
                    }
                    break; // Sai do for após posicionar
                }
            }
        }
        // Sai da etapa quando ambos os jogadores já posicionaram 3 peças
        if (pecas_jogador1 == 3 && pecas_jogador2 == 3) {
            posicionando = false; // Encerra o loop
        }
        
    }
}

void movimentacao(
    ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_BITMAP *background, ALLEGRO_FONT *font, ALLEGRO_FONT *font2, int win_w, int win_h,
    EstadoEspaço tabuleiro[7], PontoTabuleiro pontos_tabuleiro[7],
    ALLEGRO_BITMAP *peca_player1, ALLEGRO_BITMAP *peca_player2,
    int *placar_j1, int *placar_j2) {

    bool empate_pedido[2] = {false, false}; // [0]=JOGADOR1, [1]=JOGADOR2
    int jogador = JOGADOR1;      // Jogador atual (começa com o jogador 1)
    int selecionada = -1;        // Índice da peça selecionada (-1 = nenhuma selecionada)
    float mouse_x = 0, mouse_y = 0; // Coordenadas do mouse

    while (1) {
        desenha_tabuleiroTriangulo(background, font, font2, win_w, win_h, tabuleiro, pontos_tabuleiro, peca_player1, peca_player2, mouse_x, mouse_y, *placar_j1, *placar_j2);
        ALLEGRO_EVENT ev;
        al_wait_for_event(queue, &ev);

        if (ev.type == ALLEGRO_EVENT_MOUSE_AXES) {
            mouse_x = ev.mouse.x;
            mouse_y = ev.mouse.y;
        }

        if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            float mx = ev.mouse.x;
            float my = ev.mouse.y;

            if (mx >= 50 && mx <= 180 && my >= 378 && my <= 402) {
                pausar_jogo(font2, win_w, win_h);
                continue;
            }

            if (mx >= 290 && mx <= 438 && my >= 375 && my <= 405) {
                empate_pedido[jogador - 1] = true;
                if (empate_pedido[0] && empate_pedido[1]) {
                    empate_final(font, win_w, win_h);
                    return;
                } else {
                    empate_espera(font, win_w, win_h);
                }
                continue;
            }

            if (selecionada == -1) {
                for (int i = 0; i < 7; i++) {
                    float dx = mx - pontos_tabuleiro[i].x;
                    float dy = my - pontos_tabuleiro[i].y;
                    if (dx * dx + dy * dy <= 25 * 25 && tabuleiro[i] == jogador) {
                        selecionada = i;
                        break;
                    }
                }
            } else {
                for (int i = 0; i < 7; i++) {
                    float dx = mx - pontos_tabuleiro[i].x;
                    float dy = my - pontos_tabuleiro[i].y;
                    if (dx * dx + dy * dy <= 25 * 25 && tabuleiro[i] == VAZIO) {
                        if (movimento_valido(selecionada, i, tabuleiro)) {
                            tabuleiro[i] = jogador;
                            tabuleiro[selecionada] = VAZIO;

                            
                            EstadoEspaço vencedor = verificar_vitoria(tabuleiro);
                            if (vencedor != VAZIO) {
                                char msg[64];
                                sprintf(msg, "Jogador %d venceu!", vencedor);
                                al_show_native_message_box(NULL, "Fim de Jogo", "Vitória!", msg, NULL, 0);
                            // Dentro da função movimentacao
                            if (vencedor == JOGADOR1) (*placar_j1)++; // CORRETO: Incrementando o VALOR
                            else if (vencedor == JOGADOR2) (*placar_j2)++; // CORRETO: Incrementando o VALOR
                                return;
                            }

                            selecionada = -1;
                            jogador = (jogador == JOGADOR1) ? JOGADOR2 : JOGADOR1;
                        } else {
                            selecionada = -1;
                        }
                        break;
                    }
                }
                selecionada = -1;
            }
        }
    }
}



// Função principal
int main(void)
{
    
    int placar_j1 = 0, placar_j2 = 0; // Placar dos jogadores

    // Inicializa o tabuleiro com todas as casas vazias
    EstadoEspaço tabuleiro[7];
    PontoTabuleiro pontos_tabuleiro[7] = {
    {364, 80},   // 0 - topo
    {214, 180},  // 1 - esquerda superior
    {364, 180},  // 2 - centro
    {514, 180},  // 3 - direita superior
    {114, 330},  // 4 - esquerda inferior
    {364, 330},  // 5 - centro inferior
    {614, 330}   // 6 - direita inferior
};

    ALLEGRO_DISPLAY *janela = NULL; // Ponteiro para a janela principal
    ALLEGRO_FONT *font = NULL;      // Fonte principal
    ALLEGRO_FONT *font2 = NULL;     // Fonte secundária

    // Inicializações Allegro
    if (!al_init()) { // Inicializa Allegro
        fprintf(stderr, "Falha ao inicializar Allegro!\n");
        return -1;
    }
    al_init_font_addon();       // Inicializa o addon de fontes
    al_init_ttf_addon();        // Inicializa o addon de fontes TTF
    al_init_primitives_addon(); // Inicializa o addon de primitivas gráficas

    if (!al_install_mouse()) { // Inicializa o mouse
        fprintf(stderr, "Falha ao inicializar o mouse!\n");
        return -1;
    }

    font = al_load_ttf_font("Content/alice.ttf", 24, 0); // Carrega a fonte alice.ttf
    if (!font) {
        fprintf(stderr, "Falha ao carregar fonte alice.ttf!\n");
        return -1;
    }

    font2 = al_load_ttf_font("Content/mine.otf", 24, 0); // Carrega a fonte mine.otf
    if (!font2) {
        fprintf(stderr, "Falha ao carregar fonte mine.otf!\n");
        al_destroy_font(font);
        return -1;
    }

    janela = al_create_display(728, 410); // Cria a janela do jogo
    if (!janela) {
        fprintf(stderr, "Falha ao criar janela!\n");
        al_destroy_font(font);
        al_destroy_font(font2);
        return -1;
    }

    al_set_window_title(janela, "Tri Angle"); // Define o título da janela

    al_init_image_addon(); // Inicializa o addon de imagens
    ALLEGRO_BITMAP *background = al_load_bitmap("Content/background.jpg"); // Carrega a imagem de fundo
    if (!background) {
        fprintf(stderr, "Falha ao carregar imagem de fundo!\n");
        al_destroy_display(janela);
        al_destroy_font(font);
        al_destroy_font(font2);
        return -1;
    }

    int win_w = al_get_display_width(janela);  // Obtém a largura da janela
    int win_h = al_get_display_height(janela); // Obtém a altura da janela

    desenha_menu1(background, font); // Desenha o menu principal

    // Coordenadas dos botões do menu principal
    int jogar_x = 160, jogar_y = 125, jogar_w = 100, jogar_h = 30;
    int ajuda_x = 160, ajuda_y = 150, ajuda_w = 100, ajuda_h = 30;
    int historico_x = 160, historico_y = 175, historico_w = 100, historico_h = 30;
    int sair_x = 160, sair_y = 200, sair_w = 100, sair_h = 30;

    ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue(); // Cria a fila de eventos
    if (!queue) {
        fprintf(stderr, "Falha ao criar fila de eventos!\n");
        al_destroy_display(janela);
        al_destroy_font(font);
        al_destroy_font(font2);
        al_destroy_bitmap(background);
        return -1;
    }
    
    ALLEGRO_BITMAP *peca_player1 = al_load_bitmap("Content/player1.png"); // Carrega imagem da peça do jogador 1
    ALLEGRO_BITMAP *peca_player2 = al_load_bitmap("Content/player2.png"); // Carrega imagem da peça do jogador 2
    if (!peca_player1 || !peca_player2) {
         fprintf(stderr, "Falha ao carregar imagens das peças!\n");
        al_destroy_event_queue(queue);
        al_destroy_display(janela);
        al_destroy_font(font);
        al_destroy_font(font2);
        al_destroy_bitmap(background);
        return -1; // Encerra o programa corretamente
  }

    al_register_event_source(queue, al_get_display_event_source(janela)); // Registra eventos da janela
    al_register_event_source(queue, al_get_mouse_event_source());         // Registra eventos do mouse

 
    bool showing_play = false;     // Flag para tela de modos de jogo
    bool showing_help = false;     // Flag para tela de ajuda
    bool showing_history = false;  // Flag para tela de histórico
 
    ALLEGRO_BITMAP *help_image = NULL;         // Imagem da tela de ajuda
    ALLEGRO_BITMAP *play_image = NULL;         // Imagem da tela de modos de jogo
    ALLEGRO_BITMAP *History_image = NULL;      // Imagem da tela de histórico
    ALLEGRO_BITMAP *Chess_background_image = NULL; // Imagem de fundo do tabuleiro

    int mouse_x = 0, mouse_y = 0; // Coordenadas do mouse

   bool running = true;           // Flag para manter o loop principal
    // Loop principal
    while (running) {
        ALLEGRO_EVENT event; // Evento Allegro
        al_wait_for_event(queue, &event); // Espera por um evento

        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) { // Fechar janela
            running = false;
        } else if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) { // Clique do mouse
            mouse_x = event.mouse.x; // Atualiza coordenada X do mouse
            mouse_y = event.mouse.y; // Atualiza coordenada Y do mouse
        }

            // 1. Clique no menu principal
            if (!showing_help) { // Se não está na tela de ajuda
                if (!showing_play &&
                    mouse_x >= jogar_x - jogar_w / 2 &&
                    mouse_x <= jogar_x + jogar_w / 2 &&
                    mouse_y >= jogar_y &&
                    mouse_y <= jogar_y + jogar_h) {
                    // Clique em "Jogar"
                    printf("Jogar selecionado - Mostrando Modos.\n");
                    play_image = al_load_bitmap("Content/Modos.png"); // Carrega imagem dos modos
                    if (play_image) {
                        desenha_modos2(background, play_image, font, font2, win_w, win_h); // Desenha tela de modos
                        showing_play = true;
                    }
                } else if (showing_play) {
                    // Modos de jogo
                    int pvp_x = 150, pvp_y = 310, pvp_w = 250, pvp_h = al_get_font_line_height(font2);
                    if (mouse_x >= pvp_x - pvp_w / 2 && mouse_x <= pvp_x + pvp_w / 2 && mouse_y >= pvp_y && mouse_y <= pvp_y + pvp_h) {
                        // Clique em "Jogar" (PVP)
                        limpaTela();

                        printf("Modo PVP selecionado.\n");

                        for (int i = 0; i < 7; i++) {
                            tabuleiro[i] = VAZIO; // Limpa o tabuleiro
                        }

                        ALLEGRO_BITMAP *Chess_background_image = al_load_bitmap("Content/boardbackground.jpeg"); // Carrega fundo do tabuleiro
                        if (!Chess_background_image) {
                            fprintf(stderr, "Falha ao carregar boardbackground.jpeg!\n");
                            Chess_background_image = background; // Usa fundo padrão se falhar
                        }

                        if (play_image) {
                            al_destroy_bitmap(play_image); // Libera imagem dos modos
                            play_image = NULL;
                        // Chamada correta:
                        posicionamento(queue, Chess_background_image, font, font2, win_w, win_h, tabuleiro, pontos_tabuleiro, peca_player1, peca_player2, &placar_j1, &placar_j2);
                        movimentacao(queue, Chess_background_image, font, font2, win_w, win_h, tabuleiro, pontos_tabuleiro, peca_player1, peca_player2, &placar_j1, &placar_j2);
    

                        if (Chess_background_image != background) {
                            al_destroy_bitmap(Chess_background_image); // Libera fundo do tabuleiro
                            Chess_background_image = NULL;
                        }
                    }
                }
                    // Verifica se o mouse clicou no botão "PVM"
                    int pvm_x = 585, pvm_y = 310, pvm_w = 250, pvm_h = al_get_font_line_height(font2);
                    if (mouse_x >= pvm_x - pvm_w / 2 && mouse_x <= pvm_x + pvm_w / 2 && mouse_y >= pvm_y && mouse_y <= pvm_y + pvm_h) {
                        // Clique em "Jogar" (PVM)
                        limpaTela();
                        
                        printf("Modo PVM selecionado.\n");

                        for (int i = 0; i < 7; i++) {
                            tabuleiro[i] = VAZIO; // Limpa o tabuleiro
                        }

                        ALLEGRO_BITMAP *Chess_background_image = al_load_bitmap("Content/boardbackground.jpeg"); // Carrega fundo do tabuleiro
                        if (!Chess_background_image) {
                            fprintf(stderr, "Falha ao carregar boardbackground.jpeg!\n");
                            Chess_background_image = background; // Usa fundo padrão se falhar
                        }

                        if (play_image) {
                            al_destroy_bitmap(play_image); // Libera imagem dos modos
                            play_image = NULL;
                        posicionamento(queue, Chess_background_image, font, font2, win_w, win_h, tabuleiro, pontos_tabuleiro, peca_player1, peca_player2, &placar_j1, &placar_j2);
                       
                        if (Chess_background_image != background) {
                            al_destroy_bitmap(Chess_background_image); // Libera fundo do tabuleiro
                            Chess_background_image = NULL;
                        }
                    }
                    int rmenu_x = 370, rmenu_y = 300, rmenu_w = 200, rmenu_h = 40;
                    if (mouse_x >= rmenu_x - rmenu_w / 2 && mouse_x <= rmenu_x + rmenu_w / 2 && mouse_y >= rmenu_y && mouse_y <= rmenu_y + rmenu_h) {
                        // Clique em "Retornar ao Menu"
                        printf("Retornando ao menu principaln");
                        desenha_menu1(background, font); // Redesenha menu principal
                        if (play_image) {
                            al_destroy_bitmap(play_image); // Libera imagem dos modos
                            play_image = NULL;
                        }
                        showing_play = false;
                    }
                } else if (
                    mouse_x >= ajuda_x - ajuda_w / 2 &&
                    mouse_x <= ajuda_x + ajuda_w / 2 &&
                    mouse_y >= ajuda_y &&
                    mouse_y <= ajuda_y + ajuda_h) {
                    // Clique em "Ajuda"
                    help_image = al_load_bitmap("Content/help.png"); // Carrega imagem de ajuda
                    if (help_image) {
                        desenha_ajuda3(background, help_image, font, win_w, win_h); // Desenha tela de ajuda
                        showing_help = true;
                    }
                }
            }
            // 2. Clique na tela de ajuda
            else if (showing_help) {
                int rmenu_x = 160, rmenu_y = 300, rmenu_w = 200, rmenu_h = 40;
                if (mouse_x >= rmenu_x - rmenu_w / 2 && mouse_x <= rmenu_x + rmenu_w / 2 && mouse_y >= rmenu_y && mouse_y <= rmenu_y + rmenu_h) {
                    // Clique em "Retornar ao Menu" na tela de ajuda
                    printf("Retornando ao menu principal.\n");
                    desenha_menu1(background, font); // Redesenha menu principal
                    al_destroy_bitmap(help_image);    // Libera imagem de ajuda
                    help_image = NULL;
                    showing_help = false;
                }
            }
            // 3. Clique no botão "Historico"
            if (!showing_history
                && mouse_x >= historico_x - historico_w / 2 
                && mouse_x <= historico_x + historico_w / 2
                && mouse_y >= historico_y
                && mouse_y <= historico_y + historico_h) {
                // Clique em "Historico"
                History_image = al_load_bitmap("Content/historico.png"); // Carrega imagem de histórico
                if (History_image)  {
                    printf("Historico selecionado.\n");
                    desenha_historico4(background, History_image, font, win_w, win_h); // Desenha tela de histórico
                    showing_history = true;
                }
            } else if (showing_history) {
                int rmenu_x = 160, rmenu_y = 300, rmenu_w = 200, rmenu_h = 40;
                if (mouse_x >= rmenu_x - rmenu_w / 2 && mouse_x <= rmenu_x + rmenu_w / 2 && mouse_y >= rmenu_y && mouse_y <= rmenu_y + rmenu_h) {
                    // Clique em "Retornar ao Menu" na tela de histórico
                    printf("Retornando ao menu principal.\n");
                    desenha_menu1(background, font); // Redesenha menu principal
                    al_destroy_bitmap(History_image); // Libera imagem de histórico
                    History_image = NULL;
                    showing_history = false;
                }
            }
            // 4. Clique em "Sair"
            if (mouse_x >= sair_x - sair_h / 2 
                && mouse_x <= sair_x + sair_w / 2 
                && mouse_y >= sair_y 
                && mouse_y <= sair_y + sair_h) {
                // Clique em "Sair"
                running = false; // Encerra o loop principal
            }
        }
    }

      // Libera recursos
    if (help_image) al_destroy_bitmap(help_image);
    if (play_image) al_destroy_bitmap(play_image);
    if (History_image) al_destroy_bitmap(History_image);
    if (background) al_destroy_bitmap(background);
    al_destroy_event_queue(queue);
    al_destroy_display(janela);
    al_destroy_font(font);
    al_destroy_font(font2);
    al_uninstall_mouse();

    return 0; // Retorna 0 para indicar sucesso
}

