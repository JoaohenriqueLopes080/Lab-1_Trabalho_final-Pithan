#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h> // Para desenhar formas

#include <stdio.h> // Para printf em caso de erros
#include <math.h>  // Para funções matemáticas como sqrt

// Definições de cores (pode ajustar)
#define COR_PONTOS al_map_rgb(0, 0, 0)       // Preto
#define COR_LINHAS al_map_rgb(100, 100, 100) // Cinza escuro

// Dimensões da tela
const int LARGURA = 800;
const int ALTURA = 600;

// Raio dos pontos
const float RAIO_PONTO = 5.0f;

int main(int argc, char **argv) {
    ALLEGRO_DISPLAY *display = NULL;

    // 1. Inicializar Allegro
    if (!al_init()) {
        fprintf(stderr, "Falha ao inicializar Allegro.\n");
        return -1;
    }

    // 2. Inicializar add-on de primitivas (para desenhar linhas e círculos)
    if (!al_init_primitives_addon()) {
        fprintf(stderr, "Falha ao inicializar add-on de primitivas.\n");
        return -1;
    }

    // 3. Criar a janela
    display = al_create_display(LARGURA, ALTURA);
    if (!display) {
        fprintf(stderr, "Falha ao criar display.\n");
        return -1;
    }

    al_set_window_title(display, "Triângulo Geométrico - Allegro 5");

    // Coordenadas para o triângulo equilátero maior
    // Vamos centralizar o triângulo na tela e definir um tamanho
    float lado_triangulo_maior = 400.0f;
    float altura_triangulo_maior = lado_triangulo_maior * (sqrt(3.0f) / 2.0f);

    // Vértices do triângulo maior
    float x_centro = LARGURA / 2.0f;
    float y_centro = ALTURA / 2.0f;

    // Vértice superior (top)
    float p1_x = x_centro;
    float p1_y = y_centro - (altura_triangulo_maior / 2.0f);

    // Vértice inferior esquerdo (left)
    float p2_x = x_centro - (lado_triangulo_maior / 2.0f);
    float p2_y = y_centro + (altura_triangulo_maior / 2.0f);

    // Vértice inferior direito (right)
    float p3_x = x_centro + (lado_triangulo_maior / 2.0f);
    float p3_y = y_centro + (altura_triangulo_maior / 2.0f);

    // Pontos médios dos lados do triângulo maior
    float pm12_x = (p1_x + p2_x) / 2.0f; // Ponto médio entre p1 e p2
    float pm12_y = (p1_y + p2_y) / 2.0f;

    float pm23_x = (p2_x + p3_x) / 2.0f; // Ponto médio entre p2 e p3
    float pm23_y = (p2_y + p3_y) / 2.0f;

    float pm31_x = (p3_x + p1_x) / 2.0f; // Ponto médio entre p3 e p1
    float pm31_y = (p3_y + p1_y) / 2.0f;

    // Ponto central do triângulo (interseção das medianas)
    float centro_x = (p1_x + p2_x + p3_x) / 3.0f;
    float centro_y = (p1_y + p2_y + p3_y) / 3.0f;

    // Início do loop principal
    while (true) {
        // Limpar a tela com uma cor de fundo (branco)
        al_clear_to_color(al_map_rgb(255, 255, 255));

        // ------------------ Desenhar linhas ------------------

        // Linhas do triângulo maior
        al_draw_line(p1_x, p1_y, p2_x, p2_y, COR_LINHAS, 1.0f);
        al_draw_line(p2_x, p2_y, p3_x, p3_y, COR_LINHAS, 1.0f);
        al_draw_line(p3_x, p3_y, p1_x, p1_y, COR_LINHAS, 1.0f);

        // Linhas do triângulo invertido no centro (formado pelos pontos médios)
        al_draw_line(pm12_x, pm12_y, pm23_x, pm23_y, COR_LINHAS, 1.0f);
        al_draw_line(pm23_x, pm23_y, pm31_x, pm31_y, COR_LINHAS, 1.0f);
        al_draw_line(pm31_x, pm31_y, pm12_x, pm12_y, COR_LINHAS, 1.0f);

        // Linhas do ponto central para os pontos médios
        al_draw_line(centro_x, centro_y, pm12_x, pm12_y, COR_LINHAS, 1.0f);
        al_draw_line(centro_x, centro_y, pm23_x, pm23_y, COR_LINHAS, 1.0f);
        al_draw_line(centro_x, centro_y, pm31_x, pm31_y, COR_LINHAS, 1.0f);

        // Linha do ponto central para o vértice superior (p1)
        al_draw_line(centro_x, centro_y, p1_x, p1_y, COR_LINHAS, 1.0f);


        // ------------------ Desenhar pontos ------------------

        // Vértices do triângulo maior
        al_draw_filled_circle(p1_x, p1_y, RAIO_PONTO, COR_PONTOS);
        al_draw_filled_circle(p2_x, p2_y, RAIO_PONTO, COR_PONTOS);
        al_draw_filled_circle(p3_x, p3_y, RAIO_PONTO, COR_PONTOS);

        // Pontos médios dos lados
        al_draw_filled_circle(pm12_x, pm12_y, RAIO_PONTO, COR_PONTOS);
        al_draw_filled_circle(pm23_x, pm23_y, RAIO_PONTO, COR_PONTOS);
        al_draw_filled_circle(pm31_x, pm31_y, RAIO_PONTO, COR_PONTOS);

        // Ponto central
        al_draw_filled_circle(centro_x, centro_y, RAIO_PONTO, COR_PONTOS);

        // Atualizar a tela
        al_flip_display();

        // Para sair do loop, você precisaria de um sistema de eventos.
        // Para este exemplo simples, a janela ficará aberta até ser fechada manualmente.
        // Em um jogo real, você processaria eventos de teclado/mouse.
        ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();
        al_register_event_source(event_queue, al_get_display_event_source(display));
        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev);
        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            break; // Sair do loop se a janela for fechada
        }
        al_destroy_event_queue(event_queue);
    }

    // 4. Destruir display e finalizar Allegro
    al_destroy_display(display);
    al_shutdown_primitives_addon();
    al_uninstall_system(); // Não há al_uninstall_system, apenas al_shutdown_system se você instalou

    return 0;
}