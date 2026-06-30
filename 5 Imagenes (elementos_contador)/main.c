//gcc main.c -o elementos_contador_2.exe $(pkg-config --libs --cflags allegro-5 allegro_main-5 allegro_primitives-5 allegro_font-5 allegro_image-5)
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>

#define ALTO_M 25
#define ANCHO_M 40

/*TAM_TILE TAM_JUGADOR.*/
#define TAM_TILE 37
#define TAM_JUGADOR 37
#define VELOCIDAD 3.0
#define MARGEN_SUPERIOR 35

#define ANCHO (ANCHO_M * TAM_TILE)
#define ALTO (ALTO_M * TAM_TILE + MARGEN_SUPERIOR)

/*Leer nombre de imagenes*/
#define IMG_JUGADOR "personaje.png"
#define IMG_PARED "pared.png"
#define IMG_ENERGIA "energia.png"
#define IMG_ATAQUE "ataque.png"

void mapa(char MATRIZ[ALTO_M][ANCHO_M], float* x, float* y);
void dibujar_mapa(char MATRIZ[ALTO_M][ANCHO_M], ALLEGRO_BITMAP* imagen_pared, ALLEGRO_BITMAP* imagen_energia, ALLEGRO_BITMAP* imagen_ataque);
void dibujar_jugador(float x, float y, ALLEGRO_BITMAP* imagen_jugador);
int puede_mover(char MATRIZ[ALTO_M][ANCHO_M], float x, float y);
void recoger_elemento(char MATRIZ[ALTO_M][ANCHO_M], float x, float y, int* energia_obtenida, int* ataques_obtenidos);
ALLEGRO_BITMAP* cargar_imagen(const char* nombre_archivo);

int main() {
    int i, j;
    char MAPA[ALTO_M][ANCHO_M];

    float x = TAM_TILE * 2;
    float y = TAM_TILE * 2;

    int ejecutar = 1;
    int energia_obtenida = 0;
    int ataques_obtenidos = 0;

    ALLEGRO_DISPLAY* ventana = NULL;
    ALLEGRO_FONT* fuente = NULL;
    ALLEGRO_EVENT_QUEUE* cola_eventos = NULL;
    ALLEGRO_TIMER* timer = NULL;

    ALLEGRO_BITMAP* imagen_jugador = NULL;
    ALLEGRO_BITMAP* imagen_pared = NULL;
    ALLEGRO_BITMAP* imagen_energia = NULL;
    ALLEGRO_BITMAP* imagen_ataque = NULL;

    mapa(MAPA, &x, &y);

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

    if (!al_init_image_addon()) {
        printf("Error: no se pudo iniciar allegro_image.\n");
        return 1;
    }

    al_init_font_addon();

    /*Pantalla completa*/
    al_set_new_display_flags(
        ALLEGRO_WINDOWED |
        ALLEGRO_OPENGL |
        ALLEGRO_RESIZABLE |
        ALLEGRO_MAXIMIZED
    );

    ventana = al_create_display(ANCHO, ALTO);

    if (ventana == NULL) {
        printf("Error: no se pudo crear la ventana.\n");
        return 1;
    }

    imagen_jugador = cargar_imagen(IMG_JUGADOR);
    imagen_pared = cargar_imagen(IMG_PARED);
    imagen_energia = cargar_imagen(IMG_ENERGIA);
    imagen_ataque = cargar_imagen(IMG_ATAQUE);

    fuente = al_create_builtin_font();

    if (fuente == NULL) {
        printf("Error: no se pudo crear la fuente.\n");
        if (imagen_jugador != NULL) al_destroy_bitmap(imagen_jugador);
        if (imagen_pared != NULL) al_destroy_bitmap(imagen_pared);
        if (imagen_energia != NULL) al_destroy_bitmap(imagen_energia);
        if (imagen_ataque != NULL) al_destroy_bitmap(imagen_ataque);
        al_destroy_display(ventana);
        return 1;
    }

    cola_eventos = al_create_event_queue();

    if (cola_eventos == NULL) {
        printf("Error: no se pudo crear la cola de eventos.\n");
        al_destroy_font(fuente);
        if (imagen_jugador != NULL) al_destroy_bitmap(imagen_jugador);
        if (imagen_pared != NULL) al_destroy_bitmap(imagen_pared);
        if (imagen_energia != NULL) al_destroy_bitmap(imagen_energia);
        if (imagen_ataque != NULL) al_destroy_bitmap(imagen_ataque);
        al_destroy_display(ventana);
        return 1;
    }

    timer = al_create_timer(1.0 / 60.0);

    if (timer == NULL) {
        printf("Error: no se pudo crear el timer.\n");
        al_destroy_event_queue(cola_eventos);
        al_destroy_font(fuente);
        if (imagen_jugador != NULL) al_destroy_bitmap(imagen_jugador);
        if (imagen_pared != NULL) al_destroy_bitmap(imagen_pared);
        if (imagen_energia != NULL) al_destroy_bitmap(imagen_energia);
        if (imagen_ataque != NULL) al_destroy_bitmap(imagen_ataque);
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

            recoger_elemento(MAPA, x, y, &energia_obtenida, &ataques_obtenidos);

            al_clear_to_color(al_map_rgb(0, 0, 0));
            dibujar_mapa(MAPA, imagen_pared, imagen_energia, imagen_ataque);
            dibujar_jugador(x, y, imagen_jugador);

            al_draw_textf(
                fuente,
                al_map_rgb(255, 255, 255),
                10,
                6,
                0,
                "Energía: %d",
                energia_obtenida
            );

            al_draw_textf(
                fuente,
                al_map_rgb(255, 255, 255),
                150,
                6,
                0,
                "Ataques: %d",
                ataques_obtenidos
            );

            al_flip_display();
        }
    }

    al_destroy_timer(timer);
    al_destroy_font(fuente);

    if (imagen_jugador != NULL) {
        al_destroy_bitmap(imagen_jugador);
    }

    if (imagen_pared != NULL) {
        al_destroy_bitmap(imagen_pared);
    }

    if (imagen_energia != NULL) {
        al_destroy_bitmap(imagen_energia);
    }

    if (imagen_ataque != NULL) {
        al_destroy_bitmap(imagen_ataque);
    }

    al_uninstall_keyboard();
    al_destroy_event_queue(cola_eventos);
    al_destroy_display(ventana);
    return 0;
}

/*Busca nombre del archivo*/
void mapa(char MATRIZ[ALTO_M][ANCHO_M], float* x, float* y) {
    FILE* archivo_mapa = fopen("mapa_3.txt", "r");
    int i, j;

    for (i = 0; i < ALTO_M; i++) {
        for (j = 0; j < ANCHO_M; j++) {
            MATRIZ[i][j] = '.';
        }
    }

    if (!archivo_mapa) {
        printf("No se pudo abrir el archivo mapa_3.txt\n");
        return;
    }

    for (i = 0; i < ALTO_M; i++) {
        for (j = 0; j < ANCHO_M; j++) {

            if (fscanf(archivo_mapa, " %c", &MATRIZ[i][j]) != 1) {
                MATRIZ[i][j] = '.';
            }

            if (MATRIZ[i][j] == '0') {
                MATRIZ[i][j] = '.';
            }

            if (MATRIZ[i][j] == 'p') {
                *x = j * TAM_TILE;
                *y = i * TAM_TILE;
                MATRIZ[i][j] = '.';
            }
        }
    }

    fclose(archivo_mapa);
    return;
}

ALLEGRO_BITMAP* cargar_imagen(const char* nombre_archivo) {
    ALLEGRO_BITMAP* imagen = al_load_bitmap(nombre_archivo);

    if (imagen == NULL) {
        printf("Aviso: no se pudo cargar %s. Se usara dibujo basico.\n", nombre_archivo);
    }

    return imagen;
}

void dibujar_mapa(char MATRIZ[ALTO_M][ANCHO_M], ALLEGRO_BITMAP* imagen_pared, ALLEGRO_BITMAP* imagen_energia, ALLEGRO_BITMAP* imagen_ataque) {
    int i, j;
    int x, y;

    for (i = 0; i < ALTO_M; i++) {
        for (j = 0; j < ANCHO_M; j++) {
            x = j * TAM_TILE;
            y = MARGEN_SUPERIOR + i * TAM_TILE;

            if (MATRIZ[i][j] == '1') {
                if (imagen_pared != NULL) {
                    al_draw_scaled_bitmap(
                        imagen_pared,
                        0,
                        0,
                        al_get_bitmap_width(imagen_pared),
                        al_get_bitmap_height(imagen_pared),
                        x,
                        y,
                        TAM_TILE,
                        TAM_TILE,
                        0
                    );
                }
            }
            else if (MATRIZ[i][j] == 'u') {
                if (imagen_energia != NULL) {
                    al_draw_scaled_bitmap(
                        imagen_energia,
                        0,
                        0,
                        al_get_bitmap_width(imagen_energia),
                        al_get_bitmap_height(imagen_energia),
                        x,
                        y,
                        TAM_TILE,
                        TAM_TILE,
                        0
                    );
                }
            }
            else if (MATRIZ[i][j] == 'a') {
                if (imagen_ataque != NULL) {
                    al_draw_scaled_bitmap(
                        imagen_ataque,
                        0,
                        0,
                        al_get_bitmap_width(imagen_ataque),
                        al_get_bitmap_height(imagen_ataque),
                        x,
                        y,
                        TAM_TILE,
                        TAM_TILE,
                        0
                    );
                }
            }
        }
    }

    return;
}

void dibujar_jugador(float x, float y, ALLEGRO_BITMAP* imagen_jugador) {
    if (imagen_jugador != NULL) {
        al_draw_scaled_bitmap(
            imagen_jugador,
            0,
            0,
            al_get_bitmap_width(imagen_jugador),
            al_get_bitmap_height(imagen_jugador),
            x,
            y + MARGEN_SUPERIOR,
            TAM_JUGADOR,
            TAM_JUGADOR,
            0
        );
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

void recoger_elemento(char MATRIZ[ALTO_M][ANCHO_M], float x, float y, int* energia_obtenida, int* ataques_obtenidos) {
    int fila_inicio, fila_fin;
    int columna_inicio, columna_fin;
    int i, j;

    columna_inicio = (int)(x / TAM_TILE);
    columna_fin = (int)((x + TAM_JUGADOR - 1) / TAM_TILE);
    fila_inicio = (int)(y / TAM_TILE);
    fila_fin = (int)((y + TAM_JUGADOR - 1) / TAM_TILE);

    for (i = fila_inicio; i <= fila_fin; i++) {
        for (j = columna_inicio; j <= columna_fin; j++) {
            if (MATRIZ[i][j] == 'u') {
                MATRIZ[i][j] = '.';
                *energia_obtenida = *energia_obtenida + 1;
            }
            else if (MATRIZ[i][j] == 'a') {
                MATRIZ[i][j] = '.';
                *ataques_obtenidos = *ataques_obtenidos + 1;
            }
        }
    }

    return;
}