#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

#define ALTO_M 25
#define ANCHO_M 40

int main(void) {
    char MAPA[ALTO_M][ANCHO_M];

    const int ANCHO = 800;
    const int ALTO = 600;

    const int TAM_CUBO = 50;
    const float VELOCIDAD = 5.0;

    float x = 375;
    float y = 275;

    int ejecutar = 1;
    int redibujar = 1;

    int tecla_arriba = 0;
    int tecla_abajo = 0;
    int tecla_izquierda = 0;
    int tecla_derecha = 0;

    if (!al_init()) {
        printf("Error: no se pudo iniciar Allegro.\n");
        return 1;
    }

    if (!al_install_keyboard()) {
        printf("Error: no se pudo iniciar el teclado.\n");
        return 1;
    }

    if (!al_init_primitives_addon()) {
        printf("Error: no se pudo iniciar allegro_primitives.\n");
        return 1;
    }

    /*
       Importante para tu computadora:
       esta línea evita el Segmentation fault al crear la ventana.

       */
    al_set_new_display_flags(ALLEGRO_WINDOWED | ALLEGRO_OPENGL);

    ALLEGRO_DISPLAY *ventana = al_create_display(ANCHO, ALTO);

    if (ventana == NULL) {
        printf("Error: no se pudo crear la ventana.\n");
        return 1;
    }

    ALLEGRO_EVENT_QUEUE *cola_eventos = al_create_event_queue();

    if (cola_eventos == NULL) {
        printf("Error: no se pudo crear la cola de eventos.\n");
        al_destroy_display(ventana);
        return 1;
    }

    ALLEGRO_TIMER *timer = al_create_timer(1.0 / 60.0);

    if (timer == NULL) {
        printf("Error: no se pudo crear el timer.\n");
        al_destroy_event_queue(cola_eventos);
        al_destroy_display(ventana);
        return 1;
    }

    al_register_event_source(
        cola_eventos,
        al_get_display_event_source(ventana)
    );

    al_register_event_source(
        cola_eventos,
        al_get_keyboard_event_source()
    );

    al_register_event_source(
        cola_eventos,
        al_get_timer_event_source(timer)
    );

    al_start_timer(timer);

    while (ejecutar) {
        ALLEGRO_EVENT evento;

        al_wait_for_event(cola_eventos, &evento);

        if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            ejecutar = 0;
        }

        if (evento.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (evento.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                ejecutar = 0;
            }

            if (evento.keyboard.keycode == ALLEGRO_KEY_UP) {
                tecla_arriba = 1;
            }

            if (evento.keyboard.keycode == ALLEGRO_KEY_DOWN) {
                tecla_abajo = 1;
            }

            if (evento.keyboard.keycode == ALLEGRO_KEY_LEFT) {
                tecla_izquierda = 1;
            }

            if (evento.keyboard.keycode == ALLEGRO_KEY_RIGHT) {
                tecla_derecha = 1;
            }
        }

        if (evento.type == ALLEGRO_EVENT_KEY_UP) {
            if (evento.keyboard.keycode == ALLEGRO_KEY_UP) {
                tecla_arriba = 0;
            }

            if (evento.keyboard.keycode == ALLEGRO_KEY_DOWN) {
                tecla_abajo = 0;
            }

            if (evento.keyboard.keycode == ALLEGRO_KEY_LEFT) {
                tecla_izquierda = 0;
            }

            if (evento.keyboard.keycode == ALLEGRO_KEY_RIGHT) {
                tecla_derecha = 0;
            }
        }

        if (evento.type == ALLEGRO_EVENT_TIMER) {
            if (tecla_arriba == 1) {
                y -= VELOCIDAD;
            }

            if (tecla_abajo == 1) {
                y += VELOCIDAD;
            }

            if (tecla_izquierda == 1) {
                x -= VELOCIDAD;
            }

            if (tecla_derecha == 1) {
                x += VELOCIDAD;
            }

            if (x < 0) {
                x = 0;
            }

            if (y < 0) {
                y = 0;
            }

            if (x > ANCHO - TAM_CUBO) {
                x = ANCHO - TAM_CUBO;
            }

            if (y > ALTO - TAM_CUBO) {
                y = ALTO - TAM_CUBO;
            }

            redibujar = 1;
        }

        if (redibujar == 1 && al_is_event_queue_empty(cola_eventos)) {
            redibujar = 0;

            al_clear_to_color(al_map_rgb(0, 0, 0));

            al_draw_filled_rectangle(
                x,
                y,
                x + TAM_CUBO,
                y + TAM_CUBO,
                al_map_rgb(255, 255, 255)
            );

            al_flip_display();
        }
    }

    al_destroy_timer(timer);
    al_destroy_event_queue(cola_eventos);
    al_destroy_display(ventana);

    return 0;
}

void mapa(char MATRIZ[ALTO_M][ANCHO_M]) {
    FILE *mapa=fopen("mapa.txt","r");
    int i,j;
    char letra;

    if(!mapa){
        printf("No se pudo abrir");
        return;
    }

    for(i=0;i<ALTO_M;i++){
        for(j=0;j<ANCHO_M;j++){
            letra=fgetc(MATRIZ);
            if (letra==EOF){
                break;
            }
            MATRIZ[i][j]=letra;
        }
        fgetc(MATRIZ);
    }

    for(i=0;i<ALTO_M;i++){
        for(j=0;j<ANCHO_M;j++){
            printf("%c",MATRIZ[i][j]);
        }
        printf("\n");
    }
    fclose(mapa);
}