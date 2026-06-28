#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

#define ALTO_M 25
#define ANCHO_M 40

#define ANCHO 800
#define ALTO 600

#define TAM_TILE 20
#define TAM_JUGADOR 18
#define VELOCIDAD 3.0

void mapa(char MATRIZ[ALTO_M][ANCHO_M]);
void dibujar_mapa(char MATRIZ[ALTO_M][ANCHO_M]);
int puede_mover(char MATRIZ[ALTO_M][ANCHO_M], float x, float y);

int main() {
    int i, j;
    char MAPA[ALTO_M][ANCHO_M];

    float x = TAM_TILE * 2;
    float y = TAM_TILE * 2;

    int ejecutar = 1;

    mapa(MAPA); //mapa(MAPA,&x,&y);

    for (i = 0; i < ALTO_M; i++) {
        for (j = 0; j < ANCHO_M; j++) {
            printf("%c", MAPA[i][j]);
        }
        printf("\n");
    }

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

    /*Pantalla completa*/
    al_set_new_display_flags(
    ALLEGRO_WINDOWED |
    ALLEGRO_OPENGL |
    ALLEGRO_RESIZABLE |
    ALLEGRO_MAXIMIZED
);

    ALLEGRO_DISPLAY* ventana = al_create_display(ANCHO, ALTO);

    if (ventana == NULL) {
        printf("Error: no se pudo crear la ventana.\n");
        return 1;
    }

    ALLEGRO_EVENT_QUEUE* cola_eventos = al_create_event_queue();

    if (cola_eventos == NULL) {
        printf("Error: no se pudo crear la cola de eventos.\n");
        al_destroy_display(ventana);
        return 1;
    }

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0);

    if (timer == NULL) {
        printf("Error: no se pudo crear el timer.\n");
        al_destroy_event_queue(cola_eventos);
        al_destroy_display(ventana);
        return 1;
    }

    al_register_event_source(cola_eventos, al_get_display_event_source(ventana));
    al_register_event_source(cola_eventos, al_get_keyboard_event_source());
    al_register_event_source(cola_eventos, al_get_timer_event_source(timer));
    al_start_timer(timer);

    while (ejecutar) {
        ALLEGRO_EVENT evento;
        al_wait_for_event(cola_eventos, &evento);

        if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            ejecutar = 0;
        }

        if (evento.type == ALLEGRO_EVENT_TIMER) {
            ALLEGRO_KEYBOARD_STATE estado_teclado;
            float nuevo_x;
            float nuevo_y;

            al_get_keyboard_state(&estado_teclado);

            if (al_key_down(&estado_teclado, ALLEGRO_KEY_ESCAPE)) {
                ejecutar = 0;
            }

            nuevo_x = x;

            if (al_key_down(&estado_teclado, ALLEGRO_KEY_A)) {
                nuevo_x -= VELOCIDAD;
            }

            if (al_key_down(&estado_teclado, ALLEGRO_KEY_D)) {
                nuevo_x += VELOCIDAD;
            }

            if (puede_mover(MAPA, nuevo_x, y)) {
                x = nuevo_x;
            }

            nuevo_y = y;

            if (al_key_down(&estado_teclado, ALLEGRO_KEY_W)) {
                nuevo_y -= VELOCIDAD;
            }

            if (al_key_down(&estado_teclado, ALLEGRO_KEY_S)) {
                nuevo_y += VELOCIDAD;
            }

            if (puede_mover(MAPA, x, nuevo_y)) {
                y = nuevo_y;
            }

            al_clear_to_color(al_map_rgb(0, 0, 0));
            dibujar_mapa(MAPA);
            /*crear funcion wue dibuja jugador*/
            al_draw_filled_rectangle(
                x,
                y,
                x + TAM_JUGADOR,
                y + TAM_JUGADOR,
                al_map_rgb(0, 255, 255)
            );

            al_flip_display();
        }
    }

    al_destroy_timer(timer);
    al_uninstall_keyboard();
    al_destroy_event_queue(cola_eventos);
    al_destroy_display(ventana);
    return 0;
}

/*Busca nombre del archivo*/
void mapa(char MATRIZ[ALTO_M][ANCHO_M]) {
    FILE* archivo_mapa = fopen("mapa_1.txt", "r");
    int i, j;

    for (i = 0; i < ALTO_M; i++) {
        for (j = 0; j < ANCHO_M; j++) {
            MATRIZ[i][j] = '.';
        }
    }

    if (!archivo_mapa) {
        printf("No se pudo abrir el archivo mapa_1.txt\n");
        return;
    }

    for (i = 0; i < ALTO_M; i++) {
        for (j = 0; j < ANCHO_M; j++) {

            if (fscanf(archivo_mapa, " %c", &MATRIZ[i][j]) != 1) {
                MATRIZ[i][j] = '.';
            }
        }
    }

    fclose(archivo_mapa);
    return;
}

void dibujar_mapa(char MATRIZ[ALTO_M][ANCHO_M]) {
    int i, j;
    int x, y;

    for (i = 0; i < ALTO_M; i++) {
        for (j = 0; j < ANCHO_M; j++) {
            x = j * TAM_TILE;
            y = i * TAM_TILE;

            if (MATRIZ[i][j] == '1') {
                /*crear funcion que dibuja muros*/
                al_draw_filled_rectangle(
                    x,
                    y,
                    x + TAM_TILE,
                    y + TAM_TILE,
                    al_map_rgb(80, 80, 80)
                );

                al_draw_rectangle(
                    x,
                    y,
                    x + TAM_TILE,
                    y + TAM_TILE,
                    al_map_rgb(180, 180, 180),
                    1
                );
            }
        }
    }

    return;
}

int puede_mover(char MATRIZ[ALTO_M][ANCHO_M], float x, float y) {
    int fila_inicio, fila_fin;
    int columna_inicio, columna_fin;
    int i, j;

    if (x < 0 || y < 0) {
        return 0;
    }

    if (x + TAM_JUGADOR > ANCHO_M * TAM_TILE) {
        return 0;
    }

    if (y + TAM_JUGADOR > ALTO_M * TAM_TILE) {
        return 0;
    }

    columna_inicio = (int)(x / TAM_TILE);
    columna_fin = (int)((x + TAM_JUGADOR - 1) / TAM_TILE);
    fila_inicio = (int)(y / TAM_TILE);
    fila_fin = (int)((y + TAM_JUGADOR - 1) / TAM_TILE);

    for (i = fila_inicio; i <= fila_fin; i++) {
        for (j = columna_inicio; j <= columna_fin; j++) {
            if (MATRIZ[i][j] == '1') {
                return 0;
            }
        }
    }

    return 1;
}