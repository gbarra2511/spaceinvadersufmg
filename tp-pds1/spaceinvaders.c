#include <allegro5/allegro.h> 
#include <stdio.h> 
#include <stdlib.h>  
#include <time.h> 
#include <allegro5/allegro_primitives.h> 
#include <allegro5/allegro_image.h> 
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>


//comando para compilar abaixo 
//gcc -o spaceinvaders.exe spaceinvaders.c -lallegro -lallegro_primitives -lallegro_font -lallegro_ttf -lallegro_image -lallegro_audio -lallegro_acodec 

//constantes
const int SCREEN_W = 960; 
const int SCREEN_H = 540; 
const float FPS = 100; 
const int GRASS_H = 60; 
const int ALIEN_W = 50; 
const int ALIEN_H = 25; 
const int NAVE_W = 100; 
const int NAVE_H = 50; 
float pontos; 

//definicao das structs
typedef struct Nave { 
    float x; 
    float vel; 
    int dir, esq; 
    ALLEGRO_BITMAP *nave; 
    ALLEGRO_COLOR cor; 
} Nave; 

typedef struct Alien { 
    float x, y; 
    float x_vel; 
    float y_vel; 
    ALLEGRO_BITMAP *sprite; 
    int vivo; 
    ALLEGRO_COLOR cor; 
} Alien; 

typedef struct Tiro { 
    float x, y; 
    float y_vel; 
    int ativo; 
    ALLEGRO_COLOR cor; 
} Tiro; 
//inicializando as structs
void initTiro(Tiro *tiro) { 
    tiro->y_vel = 5.0; 
    tiro->y = SCREEN_H - GRASS_H - NAVE_H - 10;
    tiro->ativo = 0; 
    tiro->cor = al_map_rgb(255,0,0); 
} 

void initNave(Nave *nave) { 
    nave->x = SCREEN_W/2; 
    nave->vel = 6; 
    nave->dir = 0; 
    nave->esq = 0; 
    nave->cor = al_map_rgb(250, 250, 250); 
} 

void initAlien(Alien *alien) { 
    alien->x = 0; 
    alien->y = 0; 
    alien->x_vel = 3; 
    alien->y_vel = ALIEN_H; 
    alien->vivo = 1; 
    alien->cor = al_map_rgb(rand()%256, rand()%256, rand()%256); 
} 
//desenhando as structs
void draw_alien(Alien alien) { 
    al_draw_bitmap(alien.sprite, alien.x, alien.y, 0); 
} 

void draw_aliens(Alien *alien[5][4]) { 
    for (int i = 0; i < 5; i++) { 
        for (int j = 0; j < 4; j++) { 
            if(alien[i][j]->vivo == 1) 
                draw_alien(*alien[i][j]); 
        } 
    } 
} 
//dando update na posicao da matriz de aliens
void update_aliens(Alien *alien[5][4]) {
    int tocou_parede = 0;

    // Verifica se algum alien vivo vai tocar a parede
    for (int i = 0; i < 5 && !tocou_parede; i++) {
        for (int j = 0; j < 4; j++) {
            if (alien[i][j]->vivo == 1) {
                float proxima_x = alien[i][j]->x + alien[i][j]->x_vel;
                if (proxima_x + ALIEN_W > SCREEN_W || proxima_x < 0) {
                    tocou_parede = 1;
                    break;
                }
            }
        }
    }

    //Atualiza os aliens vivos como um todo 
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 4; j++) {
            if (alien[i][j]->vivo == 1) {
                if (tocou_parede) {
                    alien[i][j]->y += alien[i][j]->y_vel;
                    alien[i][j]->x_vel *= -1;
                }
                alien[i][j]->x += alien[i][j]->x_vel;
            }
        }
    }
}

//condicao de termino se 1
int colisao_alien_solo(Alien alien) { 
    if(alien.y + ALIEN_H > SCREEN_H - GRASS_H) { 
        return 1; 
    } 
    return 0; 
} 
//desenhando o display
void draw_scenario() { 
    al_clear_to_color(al_map_rgb(0, 0, 0)); 
    al_draw_filled_rectangle(0, SCREEN_H - GRASS_H, SCREEN_W, SCREEN_H, al_map_rgb(0, 250, 0)); 
} 
//desenhando a nave
void draw_nave(Nave nave) { 
    float y_base = SCREEN_H - GRASS_H/2; 
    float nave_x = nave.x - NAVE_W / 2;
    float nave_y = y_base - NAVE_H;

    if (nave.nave) {
        al_draw_bitmap(nave.nave, nave_x, nave_y, 0);
    } 
}


//desenhando o tiro  
void draw_tiro(Tiro tiro) { 
    al_draw_filled_circle(tiro.x, tiro.y, 6, tiro.cor); 
} 

void update_nave(Nave *nave) { 
    if(nave->dir && nave->x + nave->vel + NAVE_W/2 <= SCREEN_W) { 
        nave->x += nave->vel; 
    } 
    if(nave->esq && nave->x - nave->vel - NAVE_W/2 >= 0) { 
        nave->x -= nave->vel; 
    } 
} 

void alien_matriz(Alien *alien[5][4], ALLEGRO_BITMAP *sprite_alien) { 
    for (int i = 0; i < 5; i++) { 
        for (int j = 0; j < 4; j++) { 
            alien[i][j] = malloc(sizeof(Alien));  
            initAlien(alien[i][j]);   
            alien[i][j]->sprite = sprite_alien;          
            alien[i][j]->x = i * (ALIEN_W + 30); 
            alien[i][j]->y = j * (ALIEN_H + 30); 
        } 
    } 
} 

//libera as memorias dos aliens alocadas dinamicamente  
void libera_aliens(Alien *alien[5][4]) { 
    for (int i = 0; i < 5; i++) { 
        for (int j = 0; j < 4; j++) { 
            free(alien[i][j]); 
        } 
    } 
} 
//condicao de termino 
int colisao_aliens_solo(Alien *alien[5][4]) { 
    for (int i = 0; i < 5; i++) { 
        for (int j = 0; j < 4; j++) { 
            if (colisao_alien_solo(*alien[i][j]) && alien[i][j]->vivo) { 
                return 1; 
            } 
        } 
    } 
    return 0; 
} 
//verifica e atualiza aliens mortos 
void aliendead (Tiro *tiro, Alien *alien[5][4]) { 
    //se o tiro nao estiver ativo  
    if (!tiro->ativo) return; 
    //vai verificar para cada alien; 
    for (int i = 0; i < 5; i++) { 
        for (int j = 0; j < 4; j++) { 
            Alien *a = alien[i][j]; 
            if (a->vivo) { 
                if (tiro->x >= a->x && tiro->x <= a->x + ALIEN_W && 
                    tiro->y >= a->y && tiro->y <= a->y + ALIEN_H) { 
                    a->vivo = 0; 
                    tiro->ativo = 0; // Desativa o tiro após o acerto 
                    pontos += 150.0;
                    printf("\nPontos: %.2f", pontos); 
                    return; 
                } 
            } 
        } 
    } 
} 
//isso serve pra condicao de termino 
int allaliendead(Alien *alien[5][4]){
    int counter = 0;
    for (int i = 0; i < 5; i++) { 
        for (int j = 0; j < 4; j++) {
            if(alien[i][j]->vivo == 1){
                counter++;
            }


        }
    }
    return counter;

}


// Lê o recorde atual do arquivo
float ler_recorde() {
    float recorde = 0.0;
    FILE *file = fopen("recorde.txt", "r");
    // Se o arquivo não existir
    if (file == NULL) {
        return 0.0;
    }
   
    if (fscanf(file, "%f", &recorde) != 1) {
        recorde = 0.0;
    }
    fclose(file);
    return recorde;
}

// Compara o novo placar com o recorde anterior e atualiza o arquivo se necessário.
void salvar_novo_recorde(float pontos) {
    float recorde_anterior = ler_recorde();
    if (pontos > recorde_anterior) {
        FILE *file = fopen("recorde.txt", "w");
        if (file == NULL) {
            fprintf(stderr, "Nao foi possivel abrir o arquivo recorde\n");
            return;
        }
        fprintf(file, "%.2f", pontos);
        fclose(file);
        printf("NOVO RECORDE, PARABENS: %.2f\n", pontos);
    }
}
int main() { 
    // Objeto tela 
    ALLEGRO_DISPLAY *display = NULL; 
    // Fila de eventos event queue 
    ALLEGRO_EVENT_QUEUE *event_queue = NULL; 
    ALLEGRO_TIMER *timer = NULL; 
    //ponteiro image p um bitmap 
    ALLEGRO_BITMAP *image = NULL; 

    ALLEGRO_BITMAP *al = NULL; 
    
    //variaveis locais 
    Alien *aliens[5][4]; 
    //inicializa seed pra geracao de numeros aleatorios 
    srand(time(NULL)); 
    // Inicializa o Allegro 
    if (!al_init()) { 
        fprintf(stderr, "failed to initialize allegro!\n"); 
        return -1; 
    } 
    al_init_font_addon();       
    al_init_ttf_addon();   
    
    //pra quem for mexer lembrar que esse audio tem formato wav ou mp3 no allegro5
    if (!al_install_audio()) {
        fprintf(stderr, "Falha ao inicializar o áudio.\n");
        return -1;
    }
    if (!al_init_acodec_addon()) {
        fprintf(stderr, "Falha ao inicializar os codecs de áudio.\n");
        return -1;
    }
    if (!al_reserve_samples(1)) {
        fprintf(stderr, "Falha ao reservar canais de áudio.\n");
        return -1;
    }

    timer = al_create_timer(1.0 / FPS); 
    if (!timer) { 
        fprintf(stderr, "failed to create timer!\n"); 
        return -1; 
    } 

    display = al_create_display(SCREEN_W, SCREEN_H); 
    if (!display) { 
        fprintf(stderr, "failed to create display!\n"); 
        al_destroy_timer(timer); 
        return -1; 
    } 

    //inicializa os tipos primitivos do allegro   
    if(!al_init_primitives_addon()) { 
        fprintf(stderr, "failed to initialize primitives!\n"); 
        return -1; 
    }   

    //inicializa o modulo que permite carregar imagens no jogo 
    if(!al_init_image_addon()) { 
        fprintf(stderr, "failed to initialize image module!\n"); 
        return -1; 
    } 

    event_queue = al_create_event_queue(); 
    if (!event_queue) { 
        fprintf(stderr, "failed to create event_queue!\n"); 
        al_destroy_display(display); 
        al_destroy_timer(timer); 
        return -1; 
    } 

    if (!al_install_keyboard()) { 
        fprintf(stderr, "failed to install keyboard!\n"); 
        al_destroy_event_queue(event_queue); 
        al_destroy_display(display); 
        al_destroy_timer(timer); 
        return -1; 
    } 

    if (!al_install_mouse()) { 
        fprintf(stderr, "failed to initialize mouse!\n"); 
        al_destroy_event_queue(event_queue); 
        al_destroy_display(display); 
        al_destroy_timer(timer); 
        return -1; 
    } 

    //carregando as imagens   
    image = al_load_bitmap("assets/background.png");      
    if (!image) { 
        fprintf(stderr, "Falha ao carregar a imagem de fundo 'background.png'.\n"); 
        fprintf(stderr, "Verifique se o arquivo está na mesma pasta do executável.\n"); 
        al_destroy_display(display); 
        return -1; 
    } 
    //sprite da nave
    ALLEGRO_BITMAP *sprite_nave = al_load_bitmap("assets/spritenave.png"); 
    al_convert_mask_to_alpha(sprite_nave, al_map_rgb(255, 255, 255));
    if (!sprite_nave) { 
        fprintf(stderr, "Falha ao carregar o sprite da nave.\n"); 
        al_destroy_bitmap(image); 
        al_destroy_display(display); 
        return -1; 
    }
    //carrega a fonte
    ALLEGRO_FONT *fonte = al_load_ttf_font("assets/arial.ttf", 36, 0);
    if (!fonte) {
        fprintf(stderr, "Falha ao carregar a fonte.\n");
        return -1;
    }
    //carrega o sprite dos aliens
    ALLEGRO_BITMAP *sprite_org_al = al_load_bitmap("assets/spritealien.png"); 
    //serve pra converter o fundo branco em transparente 
    al_convert_mask_to_alpha(sprite_org_al, al_map_rgb(255, 255, 255));
    if (!sprite_org_al) { 
        fprintf(stderr, "Falha ao carregar a imagem 'spritealien.png'.\n"); 
        al_destroy_bitmap(image); 
        al_destroy_display(display); 
        return -1; 
    } 
    //musica
    ALLEGRO_SAMPLE *musica_fundo = al_load_sample("assets/funkynblink.wav"); 
    if (!musica_fundo) {
        fprintf(stderr, "Erro ao carregar música de fundo.\n");
        return -1;
    }
    // Toca a música em loop infinito
    al_play_sample(musica_fundo, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, NULL);

    al = al_create_bitmap(50, 25); 

    //variavel   
    ALLEGRO_BITMAP *target = al_get_target_bitmap(); 
    al_set_target_bitmap(al); 

    //vai copiar e redimensionar o original 
    al_draw_scaled_bitmap(sprite_org_al, 
                          0, 0, al_get_bitmap_width(sprite_org_al), al_get_bitmap_height(sprite_org_al), 
                          0, 0, 50, 25, 0); 
    al_set_target_bitmap(target); 
    //libera o original que nao era redimencionado   
    al_destroy_bitmap(sprite_org_al); 

    //fila de eventos   
    al_register_event_source(event_queue, al_get_display_event_source(display)); 
    al_register_event_source(event_queue, al_get_timer_event_source(timer)); 
    al_register_event_source(event_queue, al_get_keyboard_event_source()); 
    al_register_event_source(event_queue, al_get_mouse_event_source()); 

    //declarando variaveis principais 
    Nave nave; 
    initNave(&nave); 
    nave.nave = sprite_nave;

    Tiro tiro; 
    initTiro(&tiro); 
    alien_matriz(aliens, al); 

    int playing = 1; 
    double tempo_final = 0; 
    double tempo_inicial = al_get_time();

    al_start_timer(timer); 

    while (playing) { 
        ALLEGRO_EVENT ev; 
        al_wait_for_event(event_queue, &ev); 

        if (ev.type == ALLEGRO_EVENT_TIMER) { 
            draw_scenario(); 
            al_draw_bitmap(image, 0, 0, 0); 
            update_nave(&nave); 
            update_aliens(aliens); 
            draw_nave(nave); 
            char texto[50];
            sprintf(texto, "Pontos: %.2f", pontos);
            al_draw_text(fonte, al_map_rgb(255, 255, 255), 10, 10, 0, texto);
            draw_aliens(aliens); 

            //condicao de termino de alien tocando o solo
            if (colisao_aliens_solo(aliens)) {
                playing = 0;
                printf("VOCE PERDEU\n");
            } 
            if(allaliendead(aliens) == 0){
                playing = 0;
                printf("VOCE GANHOU\n");
            }
            if (tiro.ativo) { 
                tiro.y -= tiro.y_vel; 
                aliendead(&tiro, aliens); 
                if (tiro.y <= 0) { 
                    tiro.ativo = 0; 
                } 
                draw_tiro(tiro); 
            } 

            al_flip_display(); 
            
            if (al_get_timer_count(timer) % (int)FPS == 0) {
                printf("\n%d segundos se passaram...", (int)(al_get_timer_count(timer) / FPS));
            }
        } 
        else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) { 
            playing = 0; 
        } 
        else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) { 
            printf("\nmouse clicado em: %d, %d", ev.mouse.x, ev.mouse.y); 
        } 
        else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) { 
            printf("\ncodigo tecla: %d", ev.keyboard.keycode); 
            switch(ev.keyboard.keycode) { 
                case ALLEGRO_KEY_A: 
                    nave.esq = 1; 
                    break; 
                case ALLEGRO_KEY_D: 
                    nave.dir = 1; 
                    break; 
                case ALLEGRO_KEY_SPACE: 
                    if (tiro.ativo == 0) {  
                        tiro.x = nave.x; 
                        tiro.y = SCREEN_H - GRASS_H - NAVE_H; 
                        tiro.ativo = 1; 
                    } 
                    break; 
            } 
        } 
        else if (ev.type == ALLEGRO_EVENT_KEY_UP) { 
            printf("\ncodigo tecla: %d", ev.keyboard.keycode); 
            switch(ev.keyboard.keycode) { 
                case ALLEGRO_KEY_A: 
                    nave.esq = 0; 
                    break; 
                case ALLEGRO_KEY_D: 
                    nave.dir = 0; 
                    break; 
            } 
        } 
    } 

    tempo_final = al_get_time() - tempo_inicial;

    //essa lógica é pra pontuação ser maior para quem completou em menor tempo
    if (tempo_final > 0) {
        pontos *= (1 + (5.0 / tempo_final));
    }

    salvar_novo_recorde(pontos);
    
    char texto_pontos[50];
    char texto_recorde[50];
    float recorde_atual = ler_recorde();

    sprintf(texto_pontos, "Pontuacao Final: %.2f", pontos);
    sprintf(texto_recorde, "Recorde: %.2f", recorde_atual);

    al_clear_to_color(al_map_rgb(0, 0, 0));
    al_draw_text(fonte, al_map_rgb(255, 0, 0), SCREEN_W / 2, (SCREEN_H / 2) - 60, ALLEGRO_ALIGN_CENTER, "FIM DE JOGO");
    al_draw_text(fonte, al_map_rgb(255, 255, 255), SCREEN_W / 2, (SCREEN_H / 2), ALLEGRO_ALIGN_CENTER, texto_pontos);
    al_draw_text(fonte, al_map_rgb(255, 215, 0), SCREEN_W / 2, (SCREEN_H / 2) + 40, ALLEGRO_ALIGN_CENTER, texto_recorde);
    
    al_flip_display();

    printf("\nSua pontuacao final final foi: %.2f", pontos);
    printf("\nTempo total: %.2f segundos\n", tempo_final);

    al_rest(5.0);

    libera_aliens(aliens); 
    al_destroy_timer(timer); 
    al_destroy_bitmap(image); 
    al_destroy_bitmap(al); 
    al_destroy_display(display); 
    al_destroy_event_queue(event_queue); 
    al_destroy_font(fonte);
    al_destroy_bitmap(sprite_nave);
    al_destroy_sample(musica_fundo);

    return 0; 
}