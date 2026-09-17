#include "tema.h"

Font fonteSkyrim = {0};
Texture2D texturaFundo = {0};

void carregarRecursosSkyrim(void) {
    int quantidade = 0;
    int *pontos = LoadCodepoints(
        " !\"#$%&'()*+,-./0123456789:;<=>?@"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`"
        "abcdefghijklmnopqrstuvwxyz{|}~"
        "áàâãéêíóôõúüçÁÀÂÃÉÊÍÓÔÕÚÜÇ",
        &quantidade);

    fonteSkyrim = LoadFontEx("interface/fontes/FuturaCondensed.otf", 64, pontos, quantidade);
    UnloadCodepoints(pontos);

    if (fonteSkyrim.texture.id == 0) {
        TraceLog(LOG_WARNING, "Fonte nao encontrada.");
        fonteSkyrim = GetFontDefault();
    } else {
        SetTextureFilter(fonteSkyrim.texture, TEXTURE_FILTER_BILINEAR);
    }

    // Carrega a imagem de fundo (certifique-se de colocar um arquivo fundo.png na pasta interface)
    texturaFundo = LoadTexture("interface/imagens/fundoLogin.png");
}

void descarregarRecursosSkyrim(void) {
    if (fonteSkyrim.texture.id != GetFontDefault().texture.id) UnloadFont(fonteSkyrim);
    if (texturaFundo.id != 0) UnloadTexture(texturaFundo);
}

void DrawTextSkyrim(const char *texto, int x, int y, int tamanho, Color cor) {
    DrawTextEx(fonteSkyrim, texto, (Vector2){ (float)x, (float)y }, (float)tamanho, 2.0f, cor);
}

int MedirTextoSkyrim(const char *texto, int tamanho) {
    return (int)MeasureTextEx(fonteSkyrim, texto, (float)tamanho, 2.0f).x;
}

// Desenha a geometria vazada com extremidades pontiagudas
bool desenharBotaoSkyrim(Rectangle bounds, const char *texto, const char *valorCampo, bool ativo) {
    Vector2 mouse = GetMousePosition();
    bool hover = CheckCollisionPointRec(mouse, bounds);
    bool clicado = hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

    float cap = bounds.height / 2.0f;
    Color corLinha = (hover || ativo) ? WHITE : GRAY;
    Color corTexto = (hover || ativo) ? WHITE : LIGHTGRAY;

    // Fundo translúcido ao passar o mouse
    if (hover) {
        DrawRectangle(bounds.x + cap, bounds.y, bounds.width - cap*2, bounds.height, Fade(WHITE, 0.1f));
        DrawTriangle((Vector2){bounds.x + cap, bounds.y}, (Vector2){bounds.x, bounds.y + cap}, (Vector2){bounds.x + cap, bounds.y + bounds.height}, Fade(WHITE, 0.1f));
        DrawTriangle((Vector2){bounds.x + bounds.width - cap, bounds.y}, (Vector2){bounds.x + bounds.width - cap, bounds.y + bounds.height}, (Vector2){bounds.x + bounds.width, bounds.y + cap}, Fade(WHITE, 0.1f));
    }

    // Bordas poligonais
    DrawLineEx((Vector2){bounds.x + cap, bounds.y}, (Vector2){bounds.x + bounds.width - cap, bounds.y}, 2, corLinha);
    DrawLineEx((Vector2){bounds.x + cap, bounds.y + bounds.height}, (Vector2){bounds.x + bounds.width - cap, bounds.y + bounds.height}, 2, corLinha);
    DrawLineEx((Vector2){bounds.x + cap, bounds.y}, (Vector2){bounds.x, bounds.y + cap}, 2, corLinha);
    DrawLineEx((Vector2){bounds.x, bounds.y + cap}, (Vector2){bounds.x + cap, bounds.y + bounds.height}, 2, corLinha);
    DrawLineEx((Vector2){bounds.x + bounds.width - cap, bounds.y}, (Vector2){bounds.x + bounds.width, bounds.y + cap}, 2, corLinha);
    DrawLineEx((Vector2){bounds.x + bounds.width, bounds.y + cap}, (Vector2){bounds.x + bounds.width - cap, bounds.y + bounds.height}, 2, corLinha);

    // Renderização de Texto
    if (valorCampo) {
        // Se for um campo de digitação, mostra o título e o valor digitado
        DrawTextSkyrim(texto, bounds.x + cap + 10, bounds.y + bounds.height/2 - 10, 20, corLinha);
        DrawTextSkyrim(valorCampo, bounds.x + cap + 100, bounds.y + bounds.height/2 - 10, 20, corTexto);
    } else {
        // Se for um botão de ação, centraliza o texto
        int tw = MedirTextoSkyrim(texto, 24);
        DrawTextSkyrim(texto, bounds.x + bounds.width/2 - tw/2, bounds.y + bounds.height/2 - 12, 24, corTexto);
    }

    return clicado;
}
