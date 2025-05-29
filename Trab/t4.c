// Inclui o arquivo de cabeçalho da biblioteca Allegro 5
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

    if (!al_init())
    {
        fprintf(stderr, "Falha ao inicializar Allegro!\n");
        return -1;
    }
    al_init_font_addon();
    al_init_ttf_addon();
    al_init_primitives_addon();

    if (!al_install_mouse())
    {
        fprintf(stderr, "Falha ao inicializar o mouse!\n");
        return -1;
    }

    font = al_create_builtin_font();
    if (!font)
    {
        fprintf(stderr, "Falha ao criar fonte!\n");
        return -1;
    }

    janela = al_create_display(640, 480);
    if (!janela)
    {
        fprintf(stderr, "Falha ao criar janela!\n");
        al_destroy_font(font);
        return -1;
    }

    // cor de fundo --- mudar para foto
    al_init_image_addon();

        ALLEGRO_BITMAP *background = al_load_bitmap("Content/Finalbackground.jpeg");    
        if (!background) {
        fprintf(stderr, "Falha ao carregar imagem de fundo!\n");
        al_destroy_display(janela);
        al_destroy_font(font);
        return -1;
    }
    al_draw_bitmap(background, 0, 0, 0);

    al_draw_text(font, al_map_rgb(0, 0, 0), 320, 85, ALLEGRO_ALIGN_CENTER, "Tri Angle");

  
    // Escreve "Jogar" centralizado no bloco
    al_draw_text(font, al_map_rgb(0, 0, 0), 320, 150, ALLEGRO_ALIGN_CENTER, "Jogar");

    // Escreve "Sair" centralizado no bloco
    al_draw_text(font, al_map_rgb(0, 0, 0), 320, 250, ALLEGRO_ALIGN_CENTER, "Sair");


     


    al_flip_display();


    

    ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();
    if (!queue)
    {
        fprintf(stderr, "Falha ao criar fila de eventos!\n");
        al_destroy_display(janela);
        al_destroy_font(font);
        return -1;
    }
    al_register_event_source(queue, al_get_display_event_source(janela));

    bool running = true;
    while (running)
    {
        ALLEGRO_EVENT event;
        al_wait_for_event(queue, &event);
        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        {
            running = false;
        }
    }

    al_destroy_event_queue(queue);
    al_destroy_display(janela);
    al_destroy_font(font);
    al_uninstall_mouse();

    return 0;
}