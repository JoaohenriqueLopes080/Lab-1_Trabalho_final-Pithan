// Exemplo: Detectando eventos de mouse com Allegro

// Cria uma fila de eventos para receber eventos do sistema (como mouse, teclado, etc.)
ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();

// Registra a fonte de eventos do mouse na fila, permitindo capturar eventos do mouse
al_register_event_source(queue, al_get_mouse_event_source());a

// Loop principal que fica esperando e processando eventos indefinidamente
while (true) {
    ALLEGRO_EVENT evento;
    // Espera até que um evento ocorra e armazena esse evento na variável 'evento'
    al_wait_for_event(queue, &evento);

    // Verifica se o evento é de pressionamento de botão do mouse
    if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
        // Imprime a posição do mouse quando o botão é pressionado
        printf("Botão do mouse pressionado em (%d, %d)\n", evento.mouse.x, evento.mouse.y);
    }
    // Verifica se o evento é de liberação de botão do mouse
    if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
        // Imprime a posição do mouse quando o botão é liberado
        printf("Botão do mouse liberado em (%d, %d)\n", evento.mouse.x, evento.mouse.y);
    }
    // Verifica se o evento é de movimento do mouse
    if (evento.type == ALLEGRO_EVENT_MOUSE_AXES) {
        // Imprime a nova posição do mouse após o movimento
        printf("Mouse movido para (%d, %d)\n", evento.mouse.x, evento.mouse.y);
    }
}