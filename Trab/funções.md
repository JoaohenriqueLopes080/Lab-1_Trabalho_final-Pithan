// --- Protótipos de Funções ---
void limpaTela();
void exibir_mensagem();
void calcular_coordenadas_tabuleiro();
void desenhar_tabuleiro_allegro();
int obter_casa_por_coordenadas();
void inicializar_tabuleiro_logico();
bool validar_movimento();
bool verificar_vitoria();
void trocar_jogador();
void avancar_etapa_se_necessario();
void jogar_ia();
void iniciar_novo_jogo();
void carregar_historico();
void salvar_historico();
void registrar_partida();
void salvar_jogo();
bool carregar_jogo();

// Funções de UI
void desenhar_menu1();
void desenhar_modos2();
void desenhar_ajuda3();
void desenhar_historico4();
void desenhar_pausa5();
void desenhar_fim_de_jogo6();
void desenhar_menu_carregar_jogo_selecao7();

// Funções de processamento de eventos
void processar_eventos_menu();
void processar_eventos_jogar();
void processar_eventos_ajuda();
void processar_eventos_historico();
void processar_eventos_pausa();
void processar_eventos_fim_de_jogo();
void processar_eventos_carregar_jogo_selecao();