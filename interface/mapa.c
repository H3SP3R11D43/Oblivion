#include <stdbool.h>

#include "mapa.h"
#include "tema.h"

/* Dados das provincias (nome + posicao), migrados de rotas.c.
   A textura de cada uma e' preenchida em carregarMapa(). */
Provincia provincias[NUM_PROVINCIAS] = {
    /* 0  */ { {0}, {4018, 1115}, "Dawnstar" },
    /* 1  */ { {0}, {3038, 4088}, "Falkreath" },
    /* 2  */ { {0}, {5295, 1031}, "Winterhold" },
    /* 3  */ { {0}, {5623, 2120}, "Windhelm" },
    /* 4  */ { {0}, {2932, 1712}, "Morthal" },
    /* 5  */ { {0}, {6233, 4246}, "Riften" },
    /* 6  */ { {0}, {3908, 2800}, "Whiterun" },
    /* 7  */ { {0}, {2595, 1037}, "Solitude" },
    /* 8  */ { {0}, {784, 2667},  "Markarth" },
    /* 9  */ { {0}, {2225, 2611}, "Rorikstead" },
    /* 10 */ { {0}, {1438, 2130}, "Karthwasten" },
    /* 11 */ { {0}, {3883, 3442}, "Riverwood" },
    /* 12 */ { {0}, {3777, 3972}, "Helgen" },
    /* 13 */ { {0}, {1940, 1419}, "Dragon Bridge" },
    /* 14 */ { {0}, {4699, 3696}, "Ivarstead" },
    /* 15 */ { {0}, {6072, 3575}, "Shor's Stone" },
    /* 16 */ { {0}, {1162, 1459}, "Mor Khazgul" },
    /* 17 */ { {0}, {1204, 3002}, "Dushnikh Yal" },
    /* 18 */ { {0}, {5636, 4427}, "Largashbur" },
    /* 19 */ { {0}, {5948, 2611}, "Narzulbur" },
    /* 20 */ { {0}, {6617, 4235}, "Chale Espinho-Negro" },
    /* 21 */ { {0}, {5721, 2350}, "Fazenda Brandy-Mug" },
    /* 22 */ { {0}, {2869, 3993}, "Santuario da Dark Brotherhood" },
    /* 23 */ { {0}, {4026, 911},  "Santuario de Dawnstar" },
    /* 24 */ { {0}, {5460, 4212}, "Propriedade Goldenglow" },
    /* 25 */ { {0}, {4025, 3018}, "Hidromelaria Honningbrew" },
    /* 26 */ { {0}, {5656, 2556}, "Kynesgrove" },
    /* 27 */ { {0}, {4660, 1986}, "Estalagem Nightgate" },
    /* 28 */ { {0}, {1763, 2849}, "Velha Hroldan" },
    /* 29 */ { {0}, {2270, 945},  "Embaixada Thalmor" },
    /* 30 */ { {0}, {1252, 1987}, "Altar de Peryite" },
    /* 31 */ { {0}, {2030, 1161}, "Altar de Meridia" },
    /* 32 */ { {0}, {3590, 1818}, "Altar de Meruhnes Dagon" },
    /* 33 */ { {0}, {5242, 1497}, "Altar de Azura" },
    /* 34 */ { {0}, {6060, 2287}, "Sacelo de Boethiah" },
    /* 35 */ { {0}, {4900, 4237}, "Altar de Malacath" },
    /* 36 */ { {0}, {2060, 3261}, "Salto do Bardo" },
    /* 37 */ { {0}, {877, 1800},  "Travessia Dwemer" },
    /* 38 */ { {0}, {2610, 2876}, "Monumento de Gjukar" },
    /* 39 */ { {0}, {4070, 3790}, "Pedra Orfa" },
    /* 40 */ { {0}, {1330, 2876}, "Torre do Vento de Reach" },
    /* 41 */ { {0}, {4900, 4071}, "Ruinas de Bthalft" },
    /* 42 */ { {0}, {6030, 4602}, "Ruinas de Rkund" },
    /* 43 */ { {0}, {1325, 2620}, "Templo do Refugio Celestial" },
    /* 44 */ { {0}, {4343, 3450}, "Garganta do Mundo" },
    /* 45 */ { {0}, {4280, 1946}, "Pedras de Weynon" }
};

static Texture2D texMapa;
static Texture2D texDawnstar, texFalkreath, texMarkarth, texSolitude, texWinterhold;
static Texture2D texWhiterun, texWindhelm, texMorthal, texRiften;
static Texture2D texCidade, texOrquico, texPovoados, texAltares, texLandmark;

static Camera2D camera = {0};

void carregarMapa(void) {
    texMapa = LoadTexture("interface/imagens/mapa.png");
    texDawnstar = LoadTexture("interface/imagens/provincias/Dawnstar.png");
    texFalkreath = LoadTexture("interface/imagens/provincias/Falkreath.png");
    texMarkarth = LoadTexture("interface/imagens/provincias/Markarth.png");
    texSolitude = LoadTexture("interface/imagens/provincias/Solitude.png");
    texWinterhold = LoadTexture("interface/imagens/provincias/Winterhold.png");
    texWhiterun = LoadTexture("interface/imagens/provincias/Whiterun.png");
    texWindhelm = LoadTexture("interface/imagens/provincias/Windhelm.png");
    texMorthal = LoadTexture("interface/imagens/provincias/Morthal.png");
    texRiften = LoadTexture("interface/imagens/provincias/Riften.png");
    texCidade = LoadTexture("interface/imagens/cidade.png");
    texOrquico = LoadTexture("interface/imagens/orquico.png");
    texPovoados = LoadTexture("interface/imagens/assentamento.png");
    texAltares = LoadTexture("interface/imagens/altar.png");
    texLandmark = LoadTexture("interface/imagens/landmark.png");

    provincias[0].textura = texDawnstar;
    provincias[1].textura = texFalkreath;
    provincias[2].textura = texWinterhold;
    provincias[3].textura = texWindhelm;
    provincias[4].textura = texMorthal;
    provincias[5].textura = texRiften;
    provincias[6].textura = texWhiterun;
    provincias[7].textura = texSolitude;
    provincias[8].textura = texMarkarth;

    provincias[9].textura = texCidade;
    provincias[10].textura = texCidade;
    provincias[11].textura = texCidade;
    provincias[12].textura = texCidade;
    provincias[13].textura = texCidade;
    provincias[14].textura = texCidade;
    provincias[15].textura = texCidade;

    provincias[16].textura = texOrquico;
    provincias[17].textura = texOrquico;
    provincias[18].textura = texOrquico;
    provincias[19].textura = texOrquico;

    provincias[20].textura = texPovoados;
    provincias[21].textura = texPovoados;
    provincias[22].textura = texPovoados;
    provincias[23].textura = texPovoados;
    provincias[24].textura = texPovoados;
    provincias[25].textura = texPovoados;
    provincias[26].textura = texPovoados;
    provincias[27].textura = texPovoados;
    provincias[28].textura = texPovoados;
    provincias[29].textura = texPovoados;

    provincias[30].textura = texAltares;
    provincias[31].textura = texAltares;
    provincias[32].textura = texAltares;
    provincias[33].textura = texAltares;
    provincias[34].textura = texAltares;
    provincias[35].textura = texAltares;

    provincias[36].textura = texLandmark;
    provincias[37].textura = texLandmark;
    provincias[38].textura = texLandmark;
    provincias[39].textura = texLandmark;
    provincias[40].textura = texLandmark;
    provincias[41].textura = texLandmark;
    provincias[42].textura = texLandmark;
    provincias[43].textura = texLandmark;
    provincias[44].textura = texLandmark;
    provincias[45].textura = texLandmark;

    camera.target = (Vector2){ (float)texMapa.width / 2.0f, (float)texMapa.height / 2.0f };
    camera.zoom = 0.15f;
}

void descarregarMapa(void) {
    UnloadTexture(texMapa);
    UnloadTexture(texDawnstar); UnloadTexture(texFalkreath); UnloadTexture(texMarkarth);
    UnloadTexture(texSolitude); UnloadTexture(texWinterhold); UnloadTexture(texWhiterun);
    UnloadTexture(texWindhelm); UnloadTexture(texMorthal); UnloadTexture(texRiften);
    UnloadTexture(texCidade); UnloadTexture(texOrquico); UnloadTexture(texPovoados);
    UnloadTexture(texAltares); UnloadTexture(texLandmark);
}

void atualizarEDesenharMapa(ContextoApp *ctx, float larguraVisual, float alturaVisual) {
    Rectangle areaMapa = { 0, 0, larguraVisual, alturaVisual };
    bool mouseSobreMapa = CheckCollisionPointRec(GetMousePosition(), areaMapa);

    if (mouseSobreMapa) {
        float wiu = GetMouseWheelMove();
        if (wiu != 0) {
            Vector2 mAntes = GetScreenToWorld2D(GetMousePosition(), camera);
            camera.zoom += wiu * 0.05f;
            if (camera.zoom < 0.15f) camera.zoom = 0.15f;
            if (camera.zoom > 3.0f) camera.zoom = 3.0f;
            Vector2 mDepois = GetScreenToWorld2D(GetMousePosition(), camera);
            camera.target.x += mAntes.x - mDepois.x;
            camera.target.y += mAntes.y - mDepois.y;
        }
    }
    camera.offset = (Vector2){ larguraVisual / 2.0f, alturaVisual / 2.0f };

    static bool arrastando = false;
    Vector2 delta = GetMouseDelta();
    if (mouseSobreMapa && (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE) || IsMouseButtonDown(MOUSE_BUTTON_LEFT))) {
        if (delta.x != 0 || delta.y != 0) arrastando = true;
        camera.target.x -= delta.x / camera.zoom;
        camera.target.y -= delta.y / camera.zoom;
    }

    // Clique nas Provincias
    Vector2 mouseMundo = GetScreenToWorld2D(GetMousePosition(), camera);
    int provinciaHover = -1;
    if (mouseSobreMapa) {
        for (int i = 0; i < NUM_PROVINCIAS; i++) {
            Rectangle ret = { provincias[i].posicao.x, provincias[i].posicao.y, (float)provincias[i].textura.width, (float)provincias[i].textura.height };
            if (CheckCollisionPointRec(mouseMundo, ret)) { provinciaHover = i; break; }
        }
    }

    if (mouseSobreMapa && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        if (!arrastando && provinciaHover != -1) {
            if (ctx->telaAtual == TELA_PASSAGEIRO) {
                if (ctx->origemIndex == -1) ctx->origemIndex = provinciaHover;
                else if (ctx->destinoIndex == -1 && provinciaHover != ctx->origemIndex) ctx->destinoIndex = provinciaHover;
                else { ctx->origemIndex = provinciaHover; ctx->destinoIndex = -1; }
            } else {
                if (ctx->origemTrechoIndex == -1) ctx->origemTrechoIndex = provinciaHover;
                else if (ctx->destinoTrechoIndex == -1 && provinciaHover != ctx->origemTrechoIndex) ctx->destinoTrechoIndex = provinciaHover;
                else { ctx->origemTrechoIndex = provinciaHover; ctx->destinoTrechoIndex = -1; }
            }
        }
        arrastando = false;
    }

    BeginMode2D(camera);
        DrawTexture(texMapa, 0, 0, WHITE);
        for (int i = 0; i < NUM_PROVINCIAS; i++) {
            Color cor = WHITE;
            int emUso = (ctx->telaAtual == TELA_PASSAGEIRO) ? (i == ctx->origemIndex || i == ctx->destinoIndex)
                                                           : (i == ctx->origemTrechoIndex || i == ctx->destinoTrechoIndex);
            int ehOrigem = (ctx->telaAtual == TELA_PASSAGEIRO) ? (i == ctx->origemIndex) : (i == ctx->origemTrechoIndex);

            if (emUso) cor = ehOrigem ? (Color){120, 255, 120, 255} : (Color){255, 120, 120, 255};
            else if (i == provinciaHover) cor = (Color){255, 255, 255, 200};
            DrawTexture(provincias[i].textura, (int)provincias[i].posicao.x, (int)provincias[i].posicao.y, cor);
        }
    EndMode2D();
}
