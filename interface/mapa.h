#ifndef MAPA_H
#define MAPA_H

#include "raylib.h"
#include "contexto.h"

#define NUM_PROVINCIAS 46

typedef struct {
    Texture2D textura;
    Vector2 posicao;
    const char *nome;
} Provincia;

extern Provincia provincias[NUM_PROVINCIAS];

void carregarMapa(void);
void descarregarMapa(void);
void atualizarEDesenharMapa(ContextoApp *ctx, float larguraVisual, float alturaVisual);

#endif
