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

// Estruturas de dados
typedef struct {
    int tamanho_tabuleiro;
    int pecas_por_jogador;
    int max_jogadas;
    const char* arquivo_historico;
    const char* arquivo_pvp;
    const char* arquivo_pvc;
} ConfiguracaoJogo;

typedef struct {
    int** tabuleiro;
    int pecas_fora_jogador1;
    int pecas_fora_jogador2;
    int pecas_fora_computador;
    int jogador_atual;
    int fase;
    int movimentos_realizados;
    time_t inicio_partida;
    time_t tempo_pausado;
    bool em_pausa;
    int modo_jogo;
    ConfiguracaoJogo config;
} EstadoJogo;

typedef struct {
    int partidas_pvp;
    int partidas_pvc;
    int vitorias_jogador1_pvp;
    int vitorias_jogador2_pvp;
    int vitorias_jogador_pvc;
    int vitorias_computador_pvc;
    int empates_pvp;
    int empates_pvc;
    double maior_tempo_pvp;
    double menor_tempo_pvp;
    double maior_tempo_pvc;
    double menor_tempo_pvc;
} Historico;

// 1. Configuração e inicialização
ConfiguracaoJogo criar_configuracao() {
    ConfiguracaoJogo config = {
        .tamanho_tabuleiro = 5,
        .pecas_por_jogador = 3,
        .max_jogadas = 100,
        .arquivo_historico = "historico.dat",
        .arquivo_pvp = "save_pvp.dat",
        .arquivo_pvc = "save_pvc.dat"
    };
    return config;
}

EstadoJogo* criar_estado_jogo(ConfiguracaoJogo config) {
    EstadoJogo* estado = (EstadoJogo*)malloc(sizeof(EstadoJogo));
    estado->config = config;
    estado->tabuleiro = (int**)malloc(config.tamanho_tabuleiro * sizeof(int*));
    for (int i = 0; i < config.tamanho_tabuleiro; i++) {
        estado->tabuleiro[i] = (int*)malloc(config.tamanho_tabuleiro * sizeof(int));
        for (int j = 0; j < config.tamanho_tabuleiro; j++) {
            estado->tabuleiro[i][j] = 0;
        }
    }
    return estado;
}

void destruir_estado_jogo(EstadoJogo* estado) {
    for (int i = 0; i < estado->config.tamanho_tabuleiro; i++) {
        free(estado->tabuleiro[i]);
    }
    free(estado->tabuleiro);
    free(estado);
}

void inicializar_jogo(EstadoJogo* estado, int modo) {
    estado->modo_jogo = modo;
    estado->pecas_fora_jogador1 = estado->config.pecas_por_jogador;
    estado->pecas_fora_jogador2 = (modo == 0) ? estado->config.pecas_por_jogador : 0;
    estado->pecas_fora_computador = (modo == 1) ? estado->config.pecas_por_jogador : 0;
    estado->jogador_atual = (rand() % 2) + 1;
    estado->fase = 0;
    estado->movimentos_realizados = 0;
    estado->inicio_partida = time(NULL);
    estado->tempo_pausado = 0;
    estado->em_pausa = false;
    for (int i = 0; i < estado->config.tamanho_tabuleiro; i++)
        for (int j = 0; j < estado->config.tamanho_tabuleiro; j++)
            estado->tabuleiro[i][j] = 0;
}

// 2. Histórico
void carregar_historico(Historico* historico) {
    FILE* f = fopen("historico.dat", "rb");
    if (f) {
        fread(historico, sizeof(Historico), 1, f);
        fclose(f);
    }
}

void salvar_historico(Historico* historico) {
    FILE* f = fopen("historico.dat", "wb");
    if (f) {
        fwrite(historico, sizeof(Historico), 1, f);
        fclose(f);
    }
}

// 3. Interface gráfica Allegro (mouse)
void desenhar_texto_centralizado(ALLEGRO_FONT* fonte, const char* texto, int y, ALLEGRO_COLOR cor) {
    int largura = al_get_display_width(al_get_current_display());
    int texto_largura = al_get_text_width(fonte, texto);
    al_draw_text(fonte, cor, largura/2 - texto_largura/2, y, 0, texto);
}

void desenhar_botao(ALLEGRO_FONT* fonte, const char* texto, int x, int y, int w, int h, bool hover) {
    ALLEGRO_COLOR cor_fundo = hover ? al_map_rgb(200,200,255) : al_map_rgb(220,220,220);
    al_draw_filled_rectangle(x, y, x+w, y+h, cor_fundo);
    al_draw_rectangle(x, y, x+w, y+h, al_map_rgb(0,0,0), 2);
    int texto_largura = al_get_text_width(fonte, texto);
    int texto_altura = al_get_font_line_height(fonte);
    al_draw_text(fonte, al_map_rgb(0,0,0), x+w/2-texto_largura/2, y+h/2-texto_altura/2, 0, texto);
}

int menu_principal_grafico(ALLEGRO_FONT* fonte) {
    const int n_opcoes = 5;
    const char* opcoes[] = {
        "Jogar PvP",
        "Jogar PvC",
        "Tutorial",
        "Histórico",
        "Sair"
    };
    int largura = 400, altura = 60, espacamento = 20;
    int x = 100, y = 100;
    int hover = -1;

    ALLEGRO_EVENT_QUEUE* fila = al_create_event_queue();
    al_register_event_source(fila, al_get_mouse_event_source());
    al_register_event_source(fila, al_get_display_event_source(al_get_current_display()));

    while (1) {
        al_clear_to_color(al_map_rgb(255,255,255));
        desenhar_texto_centralizado(fonte, "=== JOGO DE TABULEIRO ===", 40, al_map_rgb(0,0,0));
        for (int i = 0; i < n_opcoes; i++) {
            desenhar_botao(fonte, opcoes[i], x, y + i*(altura+espacamento), largura, altura, hover == i);
        }
        al_flip_display();

        ALLEGRO_EVENT ev;
        al_wait_for_event(fila, &ev);
        if (ev.type == ALLEGRO_EVENT_MOUSE_AXES) {
            hover = -1;
            for (int i = 0; i < n_opcoes; i++) {
                int bx = x, by = y + i*(altura+espacamento);
                if (ev.mouse.x >= bx && ev.mouse.x <= bx+largura && ev.mouse.y >= by && ev.mouse.y <= by+altura) {
                    hover = i;
                }
            }
        }
        if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && ev.mouse.button == 1 && hover != -1) {
            al_destroy_event_queue(fila);
            return hover+1;
        }
        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            al_destroy_event_queue(fila);
            return 5;
        }
    }
}

void exibir_tutorial_grafico(ALLEGRO_FONT* fonte) {
    ALLEGRO_EVENT_QUEUE* fila = al_create_event_queue();
    al_register_event_source(fila, al_get_mouse_event_source());
    al_register_event_source(fila, al_get_display_event_source(al_get_current_display()));
    int largura = 300, altura = 60, x = 150, y = 400;
    bool hover = false;
    while (1) {
        al_clear_to_color(al_map_rgb(255,255,255));
        desenhar_texto_centralizado(fonte, "=== Tutorial ===", 50, al_map_rgb(0,0,0));
        desenhar_texto_centralizado(fonte, "O objetivo do jogo é alinhar suas peças ou capturar as do adversário.", 120, al_map_rgb(0,0,0));
        desenhar_texto_centralizado(fonte, "Durante a fase de posicionamento, coloque suas peças no tabuleiro.", 160, al_map_rgb(0,0,0));
        desenhar_texto_centralizado(fonte, "Depois, movimente-as conforme as regras.", 200, al_map_rgb(0,0,0));
        desenhar_botao(fonte, "Voltar ao menu", x, y, largura, altura, hover);
        al_flip_display();

        ALLEGRO_EVENT ev;
        al_wait_for_event(fila, &ev);
        if (ev.type == ALLEGRO_EVENT_MOUSE_AXES) {
            hover = (ev.mouse.x >= x && ev.mouse.x <= x+largura && ev.mouse.y >= y && ev.mouse.y <= y+altura);
        }
        if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && ev.mouse.button == 1 && hover) {
            break;
        }
        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) break;
    }
    al_destroy_event_queue(fila);
}

void exibir_historico_grafico(Historico* historico, ALLEGRO_FONT* fonte) {
    ALLEGRO_EVENT_QUEUE* fila = al_create_event_queue();
    al_register_event_source(fila, al_get_mouse_event_source());
    al_register_event_source(fila, al_get_display_event_source(al_get_current_display()));
    int largura = 300, altura = 60, x = 150, y = 400;
    bool hover = false;
    char buffer[128];
    while (1) {
        al_clear_to_color(al_map_rgb(255,255,255));
        desenhar_texto_centralizado(fonte, "=== Histórico ===", 40, al_map_rgb(0,0,0));
        sprintf(buffer, "Partidas PvP: %d", historico->partidas_pvp);
        desenhar_texto_centralizado(fonte, buffer, 80, al_map_rgb(0,0,0));
        sprintf(buffer, "Vitórias Jogador 1 (PvP): %d", historico->vitorias_jogador1_pvp);
        desenhar_texto_centralizado(fonte, buffer, 110, al_map_rgb(0,0,0));
        sprintf(buffer, "Vitórias Jogador 2 (PvP): %d", historico->vitorias_jogador2_pvp);
        desenhar_texto_centralizado(fonte, buffer, 140, al_map_rgb(0,0,0));
        sprintf(buffer, "Empates PvP: %d", historico->empates_pvp);
        desenhar_texto_centralizado(fonte, buffer, 170, al_map_rgb(0,0,0));
        sprintf(buffer, "Partidas PvC: %d", historico->partidas_pvc);
        desenhar_texto_centralizado(fonte, buffer, 200, al_map_rgb(0,0,0));
        sprintf(buffer, "Vitórias Jogador (PvC): %d", historico->vitorias_jogador_pvc);
        desenhar_texto_centralizado(fonte, buffer, 230, al_map_rgb(0,0,0));
        sprintf(buffer, "Vitórias Computador (PvC): %d", historico->vitorias_computador_pvc);
        desenhar_texto_centralizado(fonte, buffer, 260, al_map_rgb(0,0,0));
        sprintf(buffer, "Empates PvC: %d", historico->empates_pvc);
        desenhar_texto_centralizado(fonte, buffer, 290, al_map_rgb(0,0,0));
        desenhar_botao(fonte, "Voltar ao menu", x, y, largura, altura, hover);
        al_flip_display();

        ALLEGRO_EVENT ev;
        al_wait_for_event(fila, &ev);
        if (ev.type == ALLEGRO_EVENT_MOUSE_AXES) {
            hover = (ev.mouse.x >= x && ev.mouse.x <= x+largura && ev.mouse.y >= y && ev.mouse.y <= y+altura);
        }
        if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && ev.mouse.button == 1 && hover) {
            break;
        }
        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) break;
    }
    al_destroy_event_queue(fila);
}

// 4. Mecânica do jogo
bool validar_posicionamento(EstadoJogo* estado, int x, int y) {
    if (x < 0 || y < 0 || x >= estado->config.tamanho_tabuleiro || y >= estado->config.tamanho_tabuleiro)
        return false;
    return estado->tabuleiro[x][y] == 0;
}

bool realizar_posicionamento(EstadoJogo* estado, int x, int y) {
    if (!validar_posicionamento(estado, x, y))
        return false;
    estado->tabuleiro[x][y] = estado->jogador_atual;
    if (estado->jogador_atual == 1)
        estado->pecas_fora_jogador1--;
    else if (estado->modo_jogo == 0)
        estado->pecas_fora_jogador2--;
    else
        estado->pecas_fora_computador--;
    return true;
}

bool validar_movimento(EstadoJogo* estado, int x_origem, int y_origem, int x_destino, int y_destino) {
    if (x_origem < 0 || y_origem < 0 || x_destino < 0 || y_destino < 0)
        return false;
    if (x_origem >= estado->config.tamanho_tabuleiro || y_origem >= estado->config.tamanho_tabuleiro)
        return false;
    if (x_destino >= estado->config.tamanho_tabuleiro || y_destino >= estado->config.tamanho_tabuleiro)
        return false;
    if (estado->tabuleiro[x_origem][y_origem] != estado->jogador_atual)
        return false;
    if (estado->tabuleiro[x_destino][y_destino] != 0)
        return false;
    if ((abs(x_origem - x_destino) == 1 && y_origem == y_destino) ||
        (abs(y_origem - y_destino) == 1 && x_origem == x_destino))
        return true;
    return false;
}

bool realizar_movimento(EstadoJogo* estado, int x_origem, int y_origem, int x_destino, int y_destino) {
    if (!validar_movimento(estado, x_origem, y_origem, x_destino, y_destino))
        return false;
    estado->tabuleiro[x_destino][y_destino] = estado->tabuleiro[x_origem][y_origem];
    estado->tabuleiro[x_origem][y_origem] = 0;
    return true;
}

bool verificar_vitoria(EstadoJogo* estado, int x, int y) {
    int jogador = estado->tabuleiro[x][y];
    int n = estado->config.tamanho_tabuleiro;
    bool linha = true, coluna = true, diag1 = true, diag2 = true;
    for (int i = 0; i < n; i++) {
        if (estado->tabuleiro[x][i] != jogador) linha = false;
        if (estado->tabuleiro[i][y] != jogador) coluna = false;
        if (estado->tabuleiro[i][i] != jogador) diag1 = false;
        if (estado->tabuleiro[i][n - 1 - i] != jogador) diag2 = false;
    }
    return linha || coluna || diag1 || diag2;
}

bool verificar_empate(EstadoJogo* estado) {
    return estado->movimentos_realizados >= estado->config.max_jogadas;
}

void alternar_jogador(EstadoJogo* estado) {
    estado->jogador_atual = (estado->jogador_atual == 1) ? 2 : 1;
}

// 5. Salvamento e carregamento
void salvar_jogo(EstadoJogo* estado, const char* arquivo) {
    FILE* f = fopen(arquivo, "wb");
    if (!f) return;
    fwrite(estado, sizeof(EstadoJogo), 1, f);
    for (int i = 0; i < estado->config.tamanho_tabuleiro; i++)
        fwrite(estado->tabuleiro[i], sizeof(int), estado->config.tamanho_tabuleiro, f);
    fclose(f);
}

bool carregar_jogo(EstadoJogo* estado, const char* arquivo) {
    FILE* f = fopen(arquivo, "rb");
    if (!f) return false;
    fread(estado, sizeof(EstadoJogo), 1, f);
    for (int i = 0; i < estado->config.tamanho_tabuleiro; i++)
        fread(estado->tabuleiro[i], sizeof(int), estado->config.tamanho_tabuleiro, f);
    fclose(f);
    return true;
}

// 6. IA
void jogada_computador(EstadoJogo* estado) {
    int n = estado->config.tamanho_tabuleiro;
    if (estado->pecas_fora_computador > 0) {
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                if (validar_posicionamento(estado, i, j)) {
                    realizar_posicionamento(estado, i, j);
                    return;
                }
    } else {
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                if (estado->tabuleiro[i][j] == 2) {
                    if (i > 0 && estado->tabuleiro[i-1][j] == 0) {
                        realizar_movimento(estado, i, j, i-1, j);
                        return;
                    }
                    if (i < n-1 && estado->tabuleiro[i+1][j] == 0) {
                        realizar_movimento(estado, i, j, i+1, j);
                        return;
                    }
                    if (j > 0 && estado->tabuleiro[i][j-1] == 0) {
                        realizar_movimento(estado, i, j, i, j-1);
                        return;
                    }
                    if (j < n-1 && estado->tabuleiro[i][j+1] == 0) {
                        realizar_movimento(estado, i, j, i, j+1);
                        return;
                    }
                }
    }
}

// 7. Atualização do histórico e finalização
void atualizar_historico(Historico* historico, EstadoJogo* estado, int vencedor, double duracao) {
    if (estado->modo_jogo == 0) {
        historico->partidas_pvp++;
        if (vencedor == 1) historico->vitorias_jogador1_pvp++;
        else if (vencedor == 2) historico->vitorias_jogador2_pvp++;
        else historico->empates_pvp++;
        if (duracao > historico->maior_tempo_pvp) historico->maior_tempo_pvp = duracao;
        if (historico->menor_tempo_pvp == 0 || duracao < historico->menor_tempo_pvp) historico->menor_tempo_pvp = duracao;
    } else {
        historico->partidas_pvc++;
        if (vencedor == 1) historico->vitorias_jogador_pvc++;
        else if (vencedor == 2) historico->vitorias_computador_pvc++;
        else historico->empates_pvc++;
        if (duracao > historico->maior_tempo_pvc) historico->maior_tempo_pvc = duracao;
        if (historico->menor_tempo_pvc == 0 || duracao < historico->menor_tempo_pvc) historico->menor_tempo_pvc = duracao;
    }
}

void finalizar_jogo_grafico(EstadoJogo* estado, Historico* historico, int vencedor, ALLEGRO_FONT* fonte) {
    double duracao = difftime(time(NULL), estado->inicio_partida);
    atualizar_historico(historico, estado, vencedor, duracao);

    char msg[64];
    if (vencedor == 1)
        strcpy(msg, "Jogador 1 venceu!");
    else if (vencedor == 2)
        strcpy(msg, "Jogador 2/Computador venceu!");
    else
        strcpy(msg, "Empate!");

    ALLEGRO_EVENT_QUEUE* fila = al_create_event_queue();
    al_register_event_source(fila, al_get_mouse_event_source());
    al_register_event_source(fila, al_get_display_event_source(al_get_current_display()));
    int largura = 300, altura = 60, x = 150, y = 320;
    bool hover = false;
    while (1) {
        al_clear_to_color(al_map_rgb(255,255,255));
        desenhar_texto_centralizado(fonte, "Jogo finalizado!", 200, al_map_rgb(0,0,0));
        desenhar_texto_centralizado(fonte, msg, 250, al_map_rgb(0,0,255));
        desenhar_botao(fonte, "Voltar ao menu", x, y, largura, altura, hover);
        al_flip_display();

        ALLEGRO_EVENT ev;
        al_wait_for_event(fila, &ev);
        if (ev.type == ALLEGRO_EVENT_MOUSE_AXES) {
            hover = (ev.mouse.x >= x && ev.mouse.x <= x+largura && ev.mouse.y >= y && ev.mouse.y <= y+altura);
        }
        if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && ev.mouse.button == 1 && hover) {
            break;
        }
        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) break;
    }
    al_destroy_event_queue(fila);
}

// 8. Allegro (gráfico)
void inicializar_allegro(ALLEGRO_DISPLAY** display, ALLEGRO_EVENT_QUEUE** event_queue) {
    al_init();
    al_install_keyboard();
    al_install_mouse();
    al_init_font_addon();
    al_init_ttf_addon();
    al_init_primitives_addon();
    *display = al_create_display(600, 600);
    *event_queue = al_create_event_queue();
    al_register_event_source(*event_queue, al_get_display_event_source(*display));
}

void finalizar_allegro(ALLEGRO_DISPLAY* display, ALLEGRO_EVENT_QUEUE* event_queue) {
    al_destroy_event_queue(event_queue);
    al_destroy_display(display);
}

void desenhar_tabuleiro(EstadoJogo* estado, ALLEGRO_FONT* fonte) {
    int n = estado->config.tamanho_tabuleiro;
    int size = 500 / n;
    al_clear_to_color(al_map_rgb(255,255,255));
    for (int i = 0; i <= n; i++) {
        al_draw_line(50, 50 + i*size, 50 + n*size, 50 + i*size, al_map_rgb(0,0,0), 2);
        al_draw_line(50 + i*size, 50, 50 + i*size, 50 + n*size, al_map_rgb(0,0,0), 2);
    }
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            if (estado->tabuleiro[i][j] == 1)
                al_draw_filled_circle(50 + j*size + size/2, 50 + i*size + size/2, size/3, al_map_rgb(0,0,255));
            else if (estado->tabuleiro[i][j] == 2)
                al_draw_filled_circle(50 + j*size + size/2, 50 + i*size + size/2, size/3, al_map_rgb(255,0,0));
    char buffer[64];
    sprintf(buffer, "Jogador atual: %d", estado->jogador_atual);
    al_draw_text(fonte, al_map_rgb(0,0,0), 50, 10, 0, buffer);
    al_flip_display();
}

// 9. Loops de jogo (gráficos)
void iniciar_jogo_pvp(EstadoJogo* estado, Historico* historico, ALLEGRO_FONT* fonte) {
    int n = estado->config.tamanho_tabuleiro;
    int size = 500 / n;
    bool jogando = true;
    int fase = 0; // 0: posicionamento, 1: movimentação
    int pecas_total = estado->config.pecas_por_jogador * 2;
    int pecas_colocadas = 0;
    int vencedor = 0;

    ALLEGRO_EVENT_QUEUE* fila = al_create_event_queue();
    al_register_event_source(fila, al_get_mouse_event_source());
    al_register_event_source(fila, al_get_display_event_source(al_get_current_display()));

    int x_sel = -1, y_sel = -1;

    while (jogando) {
        desenhar_tabuleiro(estado, fonte);
        if (fase == 0) {
            desenhar_texto_centralizado(fonte, "Fase de posicionamento: clique para colocar peça", 560, al_map_rgb(0,0,0));
        } else {
            desenhar_texto_centralizado(fonte, "Fase de movimentação: clique origem e destino", 560, al_map_rgb(0,0,0));
        }
        al_flip_display();

        ALLEGRO_EVENT ev;
        al_wait_for_event(fila, &ev);

        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            jogando = false;
            break;
        }
        if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            int mx = ev.mouse.x;
            int my = ev.mouse.y;
            int x = (my - 50) / size;
            int y = (mx - 50) / size;
            if (x >= 0 && x < n && y >= 0 && y < n) {
                if (fase == 0) {
                    if (realizar_posicionamento(estado, x, y)) {
                        pecas_colocadas++;
                        if (verificar_vitoria(estado, x, y)) {
                            vencedor = estado->jogador_atual;
                            jogando = false;
                        }
                        if (pecas_colocadas >= pecas_total) {
                            fase = 1;
                        }
                        alternar_jogador(estado);
                    }
                } else {
                    if (x_sel == -1 && estado->tabuleiro[x][y] == estado->jogador_atual) {
                        x_sel = x;
                        y_sel = y;
                    } else if (x_sel != -1) {
                        if (realizar_movimento(estado, x_sel, y_sel, x, y)) {
                            estado->movimentos_realizados++;
                            if (verificar_vitoria(estado, x, y)) {
                                vencedor = estado->jogador_atual;
                                jogando = false;
                            } else if (verificar_empate(estado)) {
                                vencedor = 0;
                                jogando = false;
                            }
                            alternar_jogador(estado);
                        }
                        x_sel = y_sel = -1;
                    }
                }
            }
        }
    }
    al_destroy_event_queue(fila);
    finalizar_jogo_grafico(estado, historico, vencedor, fonte);
}

void iniciar_jogo_pvc(EstadoJogo* estado, Historico* historico, ALLEGRO_FONT* fonte) {
    int n = estado->config.tamanho_tabuleiro;
    int size = 500 / n;
    bool jogando = true;
    int fase = 0; // 0: posicionamento, 1: movimentação
    int pecas_total = estado->config.pecas_por_jogador * 2;
    int pecas_colocadas = 0;
    int vencedor = 0;

    ALLEGRO_EVENT_QUEUE* fila = al_create_event_queue();
    al_register_event_source(fila, al_get_mouse_event_source());
    al_register_event_source(fila, al_get_display_event_source(al_get_current_display()));

    int x_sel = -1, y_sel = -1;

    while (jogando) {
        desenhar_tabuleiro(estado, fonte);
        if (fase == 0) {
            desenhar_texto_centralizado(fonte, "Fase de posicionamento: clique para colocar peça", 560, al_map_rgb(0,0,0));
        } else {
            desenhar_texto_centralizado(fonte, "Fase de movimentação: clique origem e destino", 560, al_map_rgb(0,0,0));
        }
        al_flip_display();

        if (estado->jogador_atual == 2) {
            // Computador joga
            if (fase == 0) {
                jogada_computador(estado);
                pecas_colocadas++;
                // Verifica vitória
                for (int i = 0; i < n; i++)
                    for (int j = 0; j < n; j++)
                        if (estado->tabuleiro[i][j] == 2 && verificar_vitoria(estado, i, j)) {
                            vencedor = 2;
                            jogando = false;
                        }
                if (pecas_colocadas >= pecas_total) {
                    fase = 1;
                }
                alternar_jogador(estado);
            } else {
                jogada_computador(estado);
                // Verifica vitória
                for (int i = 0; i < n; i++)
                    for (int j = 0; j < n; j++)
                        if (estado->tabuleiro[i][j] == 2 && verificar_vitoria(estado, i, j)) {
                            vencedor = 2;
                            jogando = false;
                        }
                if (verificar_empate(estado)) {
                    vencedor = 0;
                    jogando = false;
                }
                alternar_jogador(estado);
            }
            al_rest(0.5);
            continue;
        }

        ALLEGRO_EVENT ev;
        al_wait_for_event(fila, &ev);

        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            jogando = false;
            break;
        }
        if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            int mx = ev.mouse.x;
            int my = ev.mouse.y;
            int x = (my - 50) / size;
            int y = (mx - 50) / size;
            if (x >= 0 && x < n && y >= 0 && y < n) {
                if (fase == 0) {
                    if (realizar_posicionamento(estado, x, y)) {
                        pecas_colocadas++;
                        if (verificar_vitoria(estado, x, y)) {
                            vencedor = estado->jogador_atual;
                            jogando = false;
                        }
                        if (pecas_colocadas >= pecas_total) {
                            fase = 1;
                        }
                        alternar_jogador(estado);
                    }
                } else {
                    if (x_sel == -1 && estado->tabuleiro[x][y] == estado->jogador_atual) {
                        x_sel = x;
                        y_sel = y;
                    } else if (x_sel != -1) {
                        if (realizar_movimento(estado, x_sel, y_sel, x, y)) {
                            estado->movimentos_realizados++;
                            if (verificar_vitoria(estado, x, y)) {
                                vencedor = estado->jogador_atual;
                                jogando = false;
                            } else if (verificar_empate(estado)) {
                                vencedor = 0;
                                jogando = false;
                            }
                            alternar_jogador(estado);
                        }
                        x_sel = y_sel = -1;
                    }
                }
            }
        }
    }
    al_destroy_event_queue(fila);
    finalizar_jogo_grafico(estado, historico, vencedor, fonte);
}

// 10. Main
int main() {
    ConfiguracaoJogo config = criar_configuracao();
    EstadoJogo* estado = criar_estado_jogo(config);
    Historico historico = {0};
    carregar_historico(&historico);
    srand(time(NULL));
    ALLEGRO_DISPLAY* display = NULL;
    ALLEGRO_EVENT_QUEUE* event_queue = NULL;
    inicializar_allegro(&display, &event_queue);

    ALLEGRO_FONT* fonte = al_create_builtin_font();

    bool executando = true;
    while (executando) {
        int opcao = menu_principal_grafico(fonte);
        switch (opcao) {
            case 1:
                inicializar_jogo(estado, 0);
                iniciar_jogo_pvp(estado, &historico, fonte);
                break;
            case 2:
                inicializar_jogo(estado, 1);
                iniciar_jogo_pvc(estado, &historico, fonte);
                break;
            case 3:
                exibir_tutorial_grafico(fonte);
                break;
            case 4:
                exibir_historico_grafico(&historico, fonte);
                break;
            case 5:
                executando = false;
                break;
        }
    }
    salvar_historico(&historico);
    destruir_estado_jogo(estado);
    al_destroy_font(fonte);
    finalizar_allegro(display, event_queue);
    return 0;
}