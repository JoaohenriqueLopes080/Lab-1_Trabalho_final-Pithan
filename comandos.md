# Comandos Allegro: Lista Detalhada e Instruções

## 1. Inicialização

- **al_init()**
    - Inicializa a biblioteca Allegro.
    - **Uso:** Deve ser chamado antes de qualquer outro comando Allegro.
    - ```c
        if (!al_init()) {
                // erro de inicialização
        }
        ```

- **al_install_keyboard()**
    - Habilita o uso do teclado.
    - **Uso:** Necessário para capturar eventos do teclado.
    - ```c
        al_install_keyboard();
        ```

- **al_install_mouse()**
    - Habilita o uso do mouse.
    - **Uso:** Necessário para capturar eventos do mouse.
    - ```c
        al_install_mouse();
        ```

## 2. Display (Janela)

- **al_create_display(largura, altura)**
    - Cria uma janela para renderização.
    - **Uso:** 
    - ```c
        ALLEGRO_DISPLAY *display = al_create_display(800, 600);
        ```

- **al_destroy_display(display)**
    - Destroi a janela criada.
    - **Uso:** 
    - ```c
        al_destroy_display(display);
        ```

## 3. Eventos

- **al_create_event_queue()**
    - Cria uma fila de eventos.
    - **Uso:** 
    - ```c
        ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();
        ```

- **al_register_event_source(queue, fonte)**
    - Registra uma fonte de eventos (teclado, mouse, display, etc).
    - **Uso:** 
    - ```c
        al_register_event_source(queue, al_get_keyboard_event_source());
        ```

- **al_wait_for_event(queue, &evento)**
    - Espera por um evento na fila.
    - **Uso:** 
    - ```c
        ALLEGRO_EVENT evento;
        al_wait_for_event(queue, &evento);
        ```

## 4. Desenho

- **al_clear_to_color(cor)**
    - Limpa a tela com uma cor.
    - **Uso:** 
    - ```c
        al_clear_to_color(al_map_rgb(0,0,0));
        ```

- **al_draw_bitmap(bitmap, x, y, flags)**
    - Desenha uma imagem na tela.
    - **Uso:** 
    - ```c
        al_draw_bitmap(imagem, 100, 100, 0);
        ```

- **al_flip_display()**
    - Atualiza a tela com o que foi desenhado.
    - **Uso:** 
    - ```c
        al_flip_display();
        ```

## 5. Bitmaps (Imagens)

- **al_load_bitmap("arquivo.png")**
    - Carrega uma imagem.
    - **Uso:** 
    - ```c
        ALLEGRO_BITMAP *imagem = al_load_bitmap("imagem.png");
        ```

- **al_destroy_bitmap(bitmap)**
    - Libera a memória da imagem.
    - **Uso:** 
    - ```c
        al_destroy_bitmap(imagem);
        ```

## 6. Temporizadores

- **al_create_timer(segundos)**
    - Cria um temporizador.
    - **Uso:** 
    - ```c
        ALLEGRO_TIMER *timer = al_create_timer(1.0 / 60);
        ```

- **al_start_timer(timer)**
    - Inicia o temporizador.
    - **Uso:** 
    - ```c
        al_start_timer(timer);
        ```

- **al_destroy_timer(timer)**
    - Destroi o temporizador.
    - **Uso:** 
    - ```c
        al_destroy_timer(timer);
        ```

## 7. Finalização

- **al_uninstall_system()**
    - Finaliza o Allegro e libera recursos.
    - **Uso:** 
    - ```c
        al_uninstall_system();
        ```

---

> Consulte a [documentação oficial do Allegro](https://liballeg.org/a5docs/) para mais detalhes e comandos avançados.