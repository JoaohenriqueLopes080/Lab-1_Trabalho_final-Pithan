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

// Tipos e variáveis globais
typedef enum { VAZIO, JOGADOR1, JOGADOR2 } EstadoEspaço;

typedef struct {
    float x, y;
} PontoTabuleiro;

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

// Funções utilitárias
void limpaTela() {
    system(CLEAR);
}

// Funções de desenho de telas
void desenha_menu1(ALLEGRO_BITMAP *background, ALLEGRO_FONT *font) {
    al_draw_bitmap(background, 0, 0, 0);
    al_draw_text(font, al_map_rgb(0, 0, 0), 160, 75, ALLEGRO_ALIGN_CENTER, "Tri Angle");
    al_draw_text(font, al_map_rgb(0, 0, 0), 160, 125, ALLEGRO_ALIGN_CENTER, "Jogar");
    al_draw_text(font, al_map_rgb(0, 0, 0), 160, 150, ALLEGRO_ALIGN_CENTER, "Ajuda");
    al_draw_text(font, al_map_rgb(0, 0, 0), 160, 175, ALLEGRO_ALIGN_CENTER, "Historico");
    al_draw_text(font, al_map_rgb(0, 0, 0), 160, 200, ALLEGRO_ALIGN_CENTER, "Sair");
    al_flip_display();
}

void desenha_modos2(ALLEGRO_BITMAP *background, ALLEGRO_BITMAP *play_image, ALLEGRO_FONT *font, ALLEGRO_FONT *font2, int win_w, int win_h) {
    al_draw_bitmap(background, 0, 0, 0);
    int play_w = al_get_bitmap_width(play_image);
    int play_h = al_get_bitmap_height(play_image);
    al_draw_scaled_bitmap(play_image, 0, 0, play_w, play_h, 0, 0, win_w, win_h, 0);
    al_draw_text(font2, al_map_rgb(0, 0 , 0 ), 150, 310, ALLEGRO_ALIGN_CENTER, "Jogar");
    al_draw_text(font2, al_map_rgb(0, 0 , 0 ), 585, 310, ALLEGRO_ALIGN_CENTER, "Jogar");
    al_draw_text(font, al_map_rgb(255, 0, 0), 370, 300, ALLEGRO_ALIGN_CENTER, "Retornar ao Menu");
    al_flip_display();
}

void pecas_jogadores(ALLEGRO_FONT *font, int win_w, int win_h, int pecas_jogador1, int pecas_jogador2) {
    // Desenha o texto dos jogadores
    al_draw_text(font, al_map_rgb(255, 0, 0), 30, 10, ALLEGRO_ALIGN_LEFT, "Jogador 1");
    al_draw_text(font, al_map_rgb(0, 0, 255), win_w - 30, 10, ALLEGRO_ALIGN_RIGHT, "Jogador 2");

    // Desenha as peças do Jogador 1 
    for (int i = 0; i < 3; i++) {
        ALLEGRO_COLOR cor = (i < pecas_jogador1) ? al_map_rgb(255, 0, 0) : al_map_rgb(200, 200, 200);
        al_draw_filled_circle(40 + i * 30, 40, 12, cor);
        al_draw_circle(40 + i * 30, 40, 12, al_map_rgb(0,0,0), 2);
    }

    // Desenha as peças do Jogador 2
    for (int i = 0; i < 3; i++) {
        ALLEGRO_COLOR cor = (i < pecas_jogador2) ? al_map_rgb(0, 0, 255) : al_map_rgb(200, 200, 200);
        al_draw_filled_circle(win_w - 40 - i * 30, 40, 12, cor);
        al_draw_circle(win_w - 40 - i * 30, 40, 12, al_map_rgb(0,0,0), 2);
    }
}

void desenha_tabuleiroTriangulo( ALLEGRO_BITMAP *background, ALLEGRO_FONT *font, int win_w, int win_h,
                                EstadoEspaço tabuleiro[7], ALLEGRO_BITMAP *peca_player1, ALLEGRO_BITMAP *peca_player2 ) 
    {
    al_draw_bitmap(background, 0, 0, 0);

    // Conte as peças de cada jogador no tabuleiro
    int pecas_jogador1 = 0, pecas_jogador2 = 0;
    for (int i = 0; i < 7; i++) {
        if (tabuleiro[i] == JOGADOR1) pecas_jogador1++;
        if (tabuleiro[i] == JOGADOR2) pecas_jogador2++;
    }
    pecas_jogadores(font, win_w, win_h, pecas_jogador1, pecas_jogador2);

    // Desenha as linhas do triângulo conectando os pontos
    al_draw_line(pontos_tabuleiro[0].x, pontos_tabuleiro[0].y, pontos_tabuleiro[1].x, pontos_tabuleiro[1].y, al_map_rgb(0,0,0), 3);
    al_draw_line(pontos_tabuleiro[0].x, pontos_tabuleiro[0].y, pontos_tabuleiro[2].x, pontos_tabuleiro[2].y, al_map_rgb(0,0,0), 3);
    al_draw_line(pontos_tabuleiro[0].x, pontos_tabuleiro[0].y, pontos_tabuleiro[3].x, pontos_tabuleiro[3].y, al_map_rgb(0,0,0), 3);
    al_draw_line(pontos_tabuleiro[1].x, pontos_tabuleiro[1].y, pontos_tabuleiro[2].x, pontos_tabuleiro[2].y, al_map_rgb(0,0,0), 3);
    al_draw_line(pontos_tabuleiro[2].x, pontos_tabuleiro[2].y, pontos_tabuleiro[3].x, pontos_tabuleiro[3].y, al_map_rgb(0,0,0), 3);
    al_draw_line(pontos_tabuleiro[1].x, pontos_tabuleiro[1].y, pontos_tabuleiro[4].x, pontos_tabuleiro[4].y, al_map_rgb(0,0,0), 3);
    al_draw_line(pontos_tabuleiro[2].x, pontos_tabuleiro[2].y, pontos_tabuleiro[5].x, pontos_tabuleiro[5].y, al_map_rgb(0,0,0), 3);
    al_draw_line(pontos_tabuleiro[3].x, pontos_tabuleiro[3].y, pontos_tabuleiro[6].x, pontos_tabuleiro[6].y, al_map_rgb(0,0,0), 3);
    al_draw_line(pontos_tabuleiro[4].x, pontos_tabuleiro[4].y, pontos_tabuleiro[5].x, pontos_tabuleiro[5].y, al_map_rgb(0,0,0), 3);
    al_draw_line(pontos_tabuleiro[5].x, pontos_tabuleiro[5].y , pontos_tabuleiro[6].x, pontos_tabuleiro[6].y, al_map_rgb(0,0,0), 3);

    // Desenha os círculos das casas e as peças dos jogadores
    for (int i = 0; i < 7; i++) {
        if (tabuleiro[i] == JOGADOR1 && peca_player1) {
            al_draw_scaled_bitmap(
                peca_player1, 0, 0,
                al_get_bitmap_width(peca_player1), al_get_bitmap_height(peca_player1),
                pontos_tabuleiro[i].x - 25, pontos_tabuleiro[i].y - 25, 50, 50, 0
            );
        } else if (tabuleiro[i] == JOGADOR2 && peca_player2) {
            al_draw_scaled_bitmap(
                peca_player2, 0, 0,
                al_get_bitmap_width(peca_player2), al_get_bitmap_height(peca_player2),
                pontos_tabuleiro[i].x - 25, pontos_tabuleiro[i].y - 25, 50, 50, 0
            );
        } else {
            al_draw_filled_circle(pontos_tabuleiro[i].x, pontos_tabuleiro[i].y, 25, al_map_rgb(200, 200, 200));
            al_draw_circle(pontos_tabuleiro[i].x, pontos_tabuleiro[i].y, 25, al_map_rgb(0,0,0), 3);
        }
    }

    // Desenha textos na tela do tabuleiro
    al_draw_text(font, al_map_rgb(0, 0, 0), win_w / 2, 30, ALLEGRO_ALIGN_CENTER, "Tri- Angle");
    al_draw_text(font, al_map_rgb(255, 0, 0), 160, 380, ALLEGRO_ALIGN_CENTER, "Pausar");
    al_flip_display();
}


void desenha_ajuda3(ALLEGRO_BITMAP *background, ALLEGRO_BITMAP *help_image, ALLEGRO_FONT *font, int win_w, int win_h) {
    al_draw_bitmap(background, 0, 0, 0);
    int img_w = al_get_bitmap_width(help_image);
    int img_h = al_get_bitmap_height(help_image);
    al_draw_scaled_bitmap(help_image, 0, 0, img_w, img_h, 0, 0, win_w, win_h, 0);
    al_draw_text(font, al_map_rgb(255, 0, 0), 160, 300, ALLEGRO_ALIGN_CENTER, "Retornar ao Menu");
    al_flip_display();
}

void desenha_historico4(ALLEGRO_BITMAP *background, ALLEGRO_BITMAP *History_image ,ALLEGRO_FONT *font, int win_w, int win_h) {
    al_draw_bitmap(background, 0, 0, 0);
    int history_w = al_get_bitmap_width(History_image);
    int history_h = al_get_bitmap_height(History_image);
    al_draw_scaled_bitmap(History_image, 0, 0, history_w, history_h, 0, 0, win_w, win_h, 0);

    al_draw_text(font, al_map_rgb(0, 0, 0), win_w / 2, win_h / 2 - 20, ALLEGRO_ALIGN_CENTER, "Historico");
    al_draw_text(font, al_map_rgb(255, 0, 0), 160, 300, ALLEGRO_ALIGN_CENTER, "Retornar ao Menu");
    al_flip_display();
}

// Funções de lógica do jogo
void etapa_posicionamento(ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_BITMAP *background, ALLEGRO_FONT *font, int win_w, int win_h, EstadoEspaço tabuleiro[7], 
    ALLEGRO_BITMAP *peca_player1, ALLEGRO_BITMAP *peca_player2)  { 
    int jogador_atual = JOGADOR1;
    int pecas_jogador1 = 0, pecas_jogador2 = 0;
    int total_pecas = 0;
    bool posicionando = true;

    while (posicionando) {
        desenha_tabuleiroTriangulo(background, font, win_w, win_h, tabuleiro, peca_player1, peca_player2);
        ALLEGRO_EVENT event;
        al_wait_for_event(queue, &event);

        if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            float mx = event.mouse.x;
            float my = event.mouse.y;

            // Verifica em qual casa o jogador clicou e se está vazia
            for (int i = 0; i < 7; i++) {
                float dx = mx - pontos_tabuleiro[i].x;
                float dy = my - pontos_tabuleiro[i].y;
                if (dx*dx + dy*dy <= 25*25 && tabuleiro[i] == VAZIO) {
                    // Só permite se o jogador ainda não colocou as 3 peças
                    if ((jogador_atual == JOGADOR1 && pecas_jogador1 < 3) ||
                    (jogador_atual == JOGADOR2 && pecas_jogador2 < 3)) {

                    tabuleiro[i] = jogador_atual;
                    if (jogador_atual == JOGADOR1) pecas_jogador1++;
                    else pecas_jogador2++;

                    // Alterna jogador
                    jogador_atual = (jogador_atual == JOGADOR1) ? JOGADOR2 : JOGADOR1;
                }
                break;
            }
        }
    }
        // Sai da etapa quando todas as peças foram posicionadas
        if (pecas_jogador1 == 3 && pecas_jogador2 == 3) {
            posicionando = false;
        }
    }
}

void pausar_jogo(ALLEGRO_FONT *font, int win_w, int win_h) {
    al_clear_to_color(al_map_rgb(0, 0, 0));
    al_draw_text(font, al_map_rgb(255, 255, 255), win_w / 2, win_h / 2 - 20, ALLEGRO_ALIGN_CENTER, "Jogo Pausado");
    al_draw_text(font, al_map_rgb(255, 0, 0), win_w / 2, win_h / 2 + 20, ALLEGRO_ALIGN_CENTER, "Pressione qualquer tecla para continuar");
    al_flip_display();
    al_rest(2.0); // Pausa por 2 segundos
}

// Função principal
int main(void)
{
    ALLEGRO_DISPLAY *janela = NULL;
    ALLEGRO_FONT *font = NULL;
    ALLEGRO_FONT *font2 = NULL;

    // Inicializações Allegro
    if (!al_init()) {
        fprintf(stderr, "Falha ao inicializar Allegro!\n");
        return -1;
    }
    al_init_font_addon();
    al_init_ttf_addon();
    al_init_primitives_addon();

    if (!al_install_mouse()) {
        fprintf(stderr, "Falha ao inicializar o mouse!\n");
        return -1;
    }

    font = al_load_ttf_font("Content/alice.ttf", 24, 0);
    if (!font) {
        fprintf(stderr, "Falha ao carregar fonte alice.ttf!\n");
        return -1;
    }

    font2 = al_load_ttf_font("Content/mine.otf", 24, 0);
    if (!font2) {
        fprintf(stderr, "Falha ao carregar fonte mine.otf!\n");
        al_destroy_font(font);
        return -1;
    }

    janela = al_create_display(728, 410);
    if (!janela) {
        fprintf(stderr, "Falha ao criar janela!\n");
        al_destroy_font(font);
        al_destroy_font(font2);
        return -1;
    }

    al_set_window_title(janela, "Tri Angle");

    al_init_image_addon();
    ALLEGRO_BITMAP *background = al_load_bitmap("Content/background.jpg");
    if (!background) {
        fprintf(stderr, "Falha ao carregar imagem de fundo!\n");
        al_destroy_display(janela);
        al_destroy_font(font);
        al_destroy_font(font2);
        return -1;
    }

    int win_w = al_get_display_width(janela);
    int win_h = al_get_display_height(janela);

    desenha_menu1(background, font);

    // Coordenadas dos botões do menu principal
    int jogar_x = 160, jogar_y = 125, jogar_w = 100, jogar_h = 30;
    int ajuda_x = 160, ajuda_y = 150, ajuda_w = 100, ajuda_h = 30;
    int historico_x = 160, historico_y = 175, historico_w = 100, historico_h = 30;
    int sair_x = 160, sair_y = 200, sair_w = 100, sair_h = 30;

    ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();
    if (!queue) {
        fprintf(stderr, "Falha ao criar fila de eventos!\n");
        al_destroy_display(janela);
        al_destroy_font(font);
        al_destroy_font(font2);
        al_destroy_bitmap(background);
        return -1;
    }
    
    ALLEGRO_BITMAP *peca_player1 = al_load_bitmap("Content/player1.png");
    ALLEGRO_BITMAP *peca_player2 = al_load_bitmap("Content/player2.png");
    if (!peca_player1 || !peca_player2) {
    fprintf(stderr, "Falha ao carregar imagens das peças!\n");
    al_destroy_event_queue(queue);
    al_destroy_display(janela);
}


    al_register_event_source(queue, al_get_display_event_source(janela));
    al_register_event_source(queue, al_get_mouse_event_source());

    bool running = true;
    bool showing_play = false;
    bool showing_help = false;
    bool showing_history = false;
 
    ALLEGRO_BITMAP *help_image = NULL;
    ALLEGRO_BITMAP *play_image = NULL;
    ALLEGRO_BITMAP *History_image = NULL;
    ALLEGRO_BITMAP *Chess_background_image = NULL;

    int mouse_x = 0, mouse_y = 0;

    // Loop principal
    while (running) {
        ALLEGRO_EVENT event;
        al_wait_for_event(queue, &event);

        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            running = false;
        } else if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            mouse_x = event.mouse.x;
            mouse_y = event.mouse.y;

            // 1. Clique no menu principal
            if (!showing_help) {
                if (!showing_play &&
                    mouse_x >= jogar_x - jogar_w / 2 &&
                    mouse_x <= jogar_x + jogar_w / 2 &&
                    mouse_y >= jogar_y &&
                    mouse_y <= jogar_y + jogar_h) {
                    printf("Jogar selecionado - Mostrando Modos.\n");
                    play_image = al_load_bitmap("Content/Modos.png");
                    if (play_image) {
                        desenha_modos2(background, play_image, font, font2, win_w, win_h);
                        showing_play = true;
                    }
                } else if (showing_play) {
                    // Modos de jogo
                    int pvp_x = 150, pvp_y = 310, pvp_w = 250, pvp_h = al_get_font_line_height(font2);
                    if (mouse_x >= pvp_x - pvp_w / 2 && mouse_x <= pvp_x + pvp_w / 2 && mouse_y >= pvp_y && mouse_y <= pvp_y + pvp_h) {
                        limpaTela();

                         printf("Modo PVP selecionado.\n");

                         for (int i = 0; i < 7; i++) {
                            tabuleiro[i] = VAZIO;
                        }

                        ALLEGRO_BITMAP *Chess_background_image = al_load_bitmap("Content/boardbackground.jpeg");
                        if (!Chess_background_image) {
                            fprintf(stderr, "Falha ao carregar boardbackground.jpeg!\n");
                            Chess_background_image = background; 
                        }

                        if (play_image) {
                            al_destroy_bitmap(play_image);
                            play_image = NULL;
                        }

                        etapa_posicionamento(queue, Chess_background_image, font, win_w, win_h, tabuleiro, peca_player1, peca_player2);

                         if (Chess_background_image != background) {
                            al_destroy_bitmap(Chess_background_image);
                            Chess_background_image = NULL;
                        }


                                        
                    }
                    int pvm_x = 585, pvm_y = 310, pvm_w = 250, pvm_h = al_get_font_line_height(font2);
                    if (mouse_x >= pvm_x - pvm_w / 2 && mouse_x <= pvm_x + pvm_w / 2 && mouse_y >= pvm_y && mouse_y <= pvm_y + pvm_h) {
                        limpaTela();
                        
                        printf("Modo PVM selecionado.\n");

                        for (int i = 0; i < 7; i++) {
                            tabuleiro[i] = VAZIO;
                        }

                        ALLEGRO_BITMAP *Chess_background_image = al_load_bitmap("Content/boardbackground.jpeg");
                        if (!Chess_background_image) {
                            fprintf(stderr, "Falha ao carregar boardbackground.jpeg!\n");
                            Chess_background_image = background; 
                        }

                        if (play_image) {
                            al_destroy_bitmap(play_image);
                            play_image = NULL;
                        }


                        etapa_posicionamento(queue, Chess_background_image, font, win_w, win_h, tabuleiro, peca_player1, peca_player2);
                       
                         if (Chess_background_image != background) {
                         al_destroy_bitmap(Chess_background_image);
                         Chess_background_image = NULL;


                      }

                    }
                    int rmenu_x = 370, rmenu_y = 300, rmenu_w = 200, rmenu_h = 40;
                    if (mouse_x >= rmenu_x - rmenu_w / 2 && mouse_x <= rmenu_x + rmenu_w / 2 && mouse_y >= rmenu_y && mouse_y <= rmenu_y + rmenu_h) {
                        printf("Retornando ao menu principaln");
                        desenha_menu1(background, font);
                        if (play_image) {
                            al_destroy_bitmap(play_image);
                            play_image = NULL;
                        }
                        showing_play = false;
                    }
                } else if (
                    mouse_x >= ajuda_x - ajuda_w / 2 &&
                    mouse_x <= ajuda_x + ajuda_w / 2 &&
                    mouse_y >= ajuda_y &&
                    mouse_y <= ajuda_y + ajuda_h) {
                    help_image = al_load_bitmap("Content/help.png");
                    if (help_image) {
                        desenha_ajuda3(background, help_image, font, win_w, win_h);
                        showing_help = true;
                    }
                }
            }
            // 2. Clique na tela de ajuda
            else if (showing_help) {
                int rmenu_x = 160, rmenu_y = 300, rmenu_w = 200, rmenu_h = 40;
                if (mouse_x >= rmenu_x - rmenu_w / 2 && mouse_x <= rmenu_x + rmenu_w / 2 && mouse_y >= rmenu_y && mouse_y <= rmenu_y + rmenu_h) {
                    printf("Retornando ao menu principal.\n");
                    desenha_menu1(background, font);
                    al_destroy_bitmap(help_image);
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
                History_image = al_load_bitmap("Content/historico.png");
                if (History_image)  {
                    printf("Historico selecionado.\n");
                    desenha_historico4(background, History_image, font, win_w, win_h);
                    showing_history = true;
                }
            } else if (showing_history) {
                int rmenu_x = 160, rmenu_y = 300, rmenu_w = 200, rmenu_h = 40;
                if (mouse_x >= rmenu_x - rmenu_w / 2 && mouse_x <= rmenu_x + rmenu_w / 2 && mouse_y >= rmenu_y && mouse_y <= rmenu_y + rmenu_h) {
                    printf("Retornando ao menu principal.\n");
                    desenha_menu1(background, font);
                    al_destroy_bitmap(History_image);
                    History_image = NULL;
                    showing_history = false;
                }
            }
            // 4. Clique em "Sair"
            if (mouse_x >= sair_x - sair_h / 2 
                && mouse_x <= sair_x + sair_w / 2 
                && mouse_y >= sair_y 
                && mouse_y <= sair_y + sair_h) {
                running = false;
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

    return 0;
}
// Compilar com: gcc t4.c -o trab -lallegro -lallegro_font -lallegro_ttf -lallegro_primitives -lallegro_image -lallegro_main
