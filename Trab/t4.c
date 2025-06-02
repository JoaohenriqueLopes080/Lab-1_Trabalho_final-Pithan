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

int main(void)
{
    ALLEGRO_DISPLAY *janela = NULL;
    ALLEGRO_FONT *font = NULL;

    // Inicializa a Allegro e seus addons
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

    janela = al_create_display(728, 410);
    if (!janela) {
        fprintf(stderr, "Falha ao criar janela!\n");
        al_destroy_font(font);
        return -1;
    }
    al_set_window_title(janela, "Tri Angle");

    // Background
    al_init_image_addon();
    ALLEGRO_BITMAP *background = al_load_bitmap("Content/ele.jpg");
    if (!background) {
        fprintf(stderr, "Falha ao carregar imagem de fundo!\n");
        al_destroy_display(janela);
        al_destroy_font(font);
        return -1;
    }

    int win_w = al_get_display_width(janela);
    int win_h = al_get_display_height(janela);

    al_draw_bitmap(background, 0, 0, 0);

    // Titulo
    al_draw_text(font, al_map_rgb(0, 0, 0), 160, 75, ALLEGRO_ALIGN_CENTER, "Tri Angle");

    // Textos do menu
    al_draw_text(font, al_map_rgb(0, 0, 0), 160, 125, ALLEGRO_ALIGN_CENTER, "Jogar");
    al_draw_text(font, al_map_rgb(0, 0, 0), 160, 150, ALLEGRO_ALIGN_CENTER, "Ajuda");
    al_draw_text(font, al_map_rgb(0, 0, 0), 160, 175, ALLEGRO_ALIGN_CENTER, "Historico");
    al_draw_text(font, al_map_rgb(0, 0, 0), 160, 200, ALLEGRO_ALIGN_CENTER, "Sair");

    al_flip_display();

    // Coordenadas de cada botão do menu
    int jogar_x = 160, jogar_y = 125, jogar_w = 100, jogar_h = 30;
    int ajuda_x = 160, ajuda_y = 150, ajuda_w = 100, ajuda_h = 30;
    int historico_x = 160, historico_y = 175, historico_w = 100, historico_h = 30;
    int sair_x = 160, sair_y = 200, sair_w = 100, sair_h = 30;

    // Cria uma fila de eventos para capturar eventos da janela
    ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();
    if (!queue) {
        fprintf(stderr, "Falha ao criar fila de eventos!\n");
        al_destroy_display(janela);
        al_destroy_font(font);
        return -1;
    }

    al_register_event_source(queue, al_get_display_event_source(janela));
    al_register_event_source(queue, al_get_mouse_event_source());

    bool running = true;
    bool showing_play = false;
    bool showing_help = false;
    bool showing_history = false;
    bool exit = false;

    ALLEGRO_BITMAP *help_image = NULL;
    ALLEGRO_BITMAP *play_image = NULL;
    ALLEGRO_BITMAP *History_image = NULL;
   

    int mouse_x = 0, mouse_y = 0;
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
                    // Aqui você pode adicionar a lógica para iniciar o jogo
                    play_image = al_load_bitmap("Content/modos.png");



                } else if (
                    mouse_x >= ajuda_x - ajuda_w / 2 &&
                    mouse_x <= ajuda_x + ajuda_w / 2 &&
                    mouse_y >= ajuda_y &&
                    mouse_y <= ajuda_y + ajuda_h) {
                    help_image = al_load_bitmap("Content/help.png");
                    if (help_image) {
                        al_draw_bitmap(background, 0, 0, 0);
                        int img_w = al_get_bitmap_width(help_image);
                        int img_h = al_get_bitmap_height(help_image);
                        al_draw_scaled_bitmap(help_image,
                                             0, 0, img_w, img_h,
                                             0, 0, win_w, win_h,
                                             0);
                        al_draw_text(font, al_map_rgb(255, 0, 0), 160, 300, ALLEGRO_ALIGN_CENTER, "Retornar ao Menu");
                        al_flip_display();
                        showing_help = true;
                    }
                }
                // ...outros botões do menu...
            }
            // 2. Clique na tela de ajuda
            else if (showing_help) {
                int rmenu_x = 160;
                int rmenu_y = 300;
                int rmenu_w = 200;
                int rmenu_h = 40;
                if (
                    mouse_x >= rmenu_x - rmenu_w / 2 &&
                    mouse_x <= rmenu_x + rmenu_w / 2 &&
                    mouse_y >= rmenu_y &&
                    mouse_y <= rmenu_y + rmenu_h) {
                    al_draw_bitmap(background, 0, 0, 0);
                    al_draw_text(font, al_map_rgb(0, 0, 0), 160, 75, ALLEGRO_ALIGN_CENTER, "Tri Angle");
                    al_draw_text(font, al_map_rgb(0, 0, 0), 160, 125, ALLEGRO_ALIGN_CENTER, "Jogar");
                    al_draw_text(font, al_map_rgb(0, 0, 0), 160, 150, ALLEGRO_ALIGN_CENTER, "Ajuda");
                    al_draw_text(font, al_map_rgb(0, 0, 0), 160, 175, ALLEGRO_ALIGN_CENTER, "Historico");
                    al_draw_text(font, al_map_rgb(0, 0, 0), 160, 200, ALLEGRO_ALIGN_CENTER, "Sair");
                    al_flip_display();
                    al_destroy_bitmap(help_image);
                    help_image = NULL;
                    showing_help = false;
                }
            }

            // 3. Clique em "Sair"
            if (!exit &&
                mouse_x >= sair_x - sair_h / 2 &&
                mouse_x <= sair_x + sair_w / 2 &&
                mouse_y >= sair_y &&
                mouse_y <= sair_y + sair_h) {
                al_destroy_event_queue(queue);
                al_destroy_display(janela);
                al_destroy_font(font);
                al_uninstall_mouse();
            }
        }
    }

    if (help_image)
    al_destroy_bitmap(help_image);
    al_destroy_event_queue(queue);
    al_destroy_display(janela);
    al_destroy_font(font);
    al_uninstall_mouse();

    return 0;
}

// Compilar com: gcc t4.c -o trab -lallegro -lallegro_font -lallegro_ttf -lallegro_primitives -lallegro_image -lallegro_main