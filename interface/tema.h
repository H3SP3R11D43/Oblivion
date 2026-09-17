#ifndef TEMA_H
#define TEMA_H

#include <stdbool.h>
#include "raylib.h"

extern Font fonteSkyrim;
extern Texture2D texturaFundo;

void carregarRecursosSkyrim(void);
void descarregarRecursosSkyrim(void);
void DrawTextSkyrim(const char *texto, int x, int y, int tamanho, Color cor);
int MedirTextoSkyrim(const char *texto, int tamanho);
bool desenharBotaoSkyrim(Rectangle bounds, const char *texto, const char *valorCampo, bool ativo);

#endif
