#include "painelMotorista.h"
#include "tema.h"
#include "mapa.h"
#include "../controle/controle_caronas.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static CampoTexto campoDataCarona = { .caixa = {0}, .senha = 0 };
static CampoTexto campoHorarioCarona = { .caixa = {0}, .senha = 0 };
static CampoTexto campoPreco = { .caixa = {0}, .senha = 0 };
static CampoTexto campoVagas = { .caixa = {0}, .senha = 0 };
static CampoTexto campoIdCarona = { .caixa = {0}, .senha = 0 };

static TrechoPendente trechosPendentes[MAX_TRECHOS_PUBLICAR];
static int numTrechosPendentes = 0;
static char trechosResumo[MAX_TRECHOS_PUBLICAR][TAM_RESUMO_TRECHO];
static int numTrechosResumo = 0;

static CaronaInfo caronasInfo[MAX_CARONAS_INFO];
static int numCaronasInfo = 0;
static char mensagemCaronas[256] = "";

void DesenharPainelMotorista(ContextoApp *ctx, Rectangle painel, int alturaAtual) {
    float px = painel.x + 15;
    float py = 20;
    float alturaItem = 34.0f;

    DrawTextSkyrim("MOTORISTA", (int)px, (int)py, 28, WHITE);
    py += 40;

    if (desenharBotaoSkyrim((Rectangle){ px, py, 150, 32 }, "PUBLICAR", NULL, ctx->subTelaMotorista == M_PUBLICAR)) {
        ctx->subTelaMotorista = M_PUBLICAR;
    }
    if (desenharBotaoSkyrim((Rectangle){ px + 160, py, 180, 32 }, "MINHAS CARONAS", NULL, ctx->subTelaMotorista == M_CARONAS)) {
        ctx->subTelaMotorista = M_CARONAS;
        RespostaCarona resp = controleListarMinhasCaronas(ctx->servidorFD, caronasInfo, &numCaronasInfo, MAX_CARONAS_INFO);
        snprintf(mensagemCaronas, sizeof(mensagemCaronas), "%s", resp.mensagem);
    }
    py += 50;

    if (ctx->subTelaMotorista == M_PUBLICAR) {
        Rectangle rData = { px, py, 340, alturaItem };
        campoDataCarona.caixa = rData;
        atualizarCampoTexto(ctx, &campoDataCarona);
        desenharBotaoSkyrim(rData, "DATA:", campoDataCarona.texto, CheckCollisionPointRec(GetMousePosition(), rData));
        py += 42;

        Rectangle rHora = { px, py, 340, alturaItem };
        campoHorarioCarona.caixa = rHora;
        atualizarCampoTexto(ctx, &campoHorarioCarona);
        desenharBotaoSkyrim(rHora, "HORA:", campoHorarioCarona.texto, CheckCollisionPointRec(GetMousePosition(), rHora));
        py += 45;

        DrawTextSkyrim("TRECHO NO MAPA:", (int)px, (int)py, 16, GRAY);
        py += 20;
        const char *origTxt = ctx->origemTrechoIndex != -1 ? provincias[ctx->origemTrechoIndex].nome : "ORIGEM";
        const char *destTxt = ctx->destinoTrechoIndex != -1 ? provincias[ctx->destinoTrechoIndex].nome : "DESTINO";
        DrawTextSkyrim(TextFormat("%s -> %s", origTxt, destTxt), (int)px, (int)py, 18, WHITE);
        py += 35;

        Rectangle rPreco = { px, py, 165, alturaItem };
        campoPreco.caixa = rPreco;
        atualizarCampoTexto(ctx, &campoPreco);
        desenharBotaoSkyrim(rPreco, "PRECO:", campoPreco.texto, CheckCollisionPointRec(GetMousePosition(), rPreco));

        Rectangle rVagas = { px + 175, py, 165, alturaItem };
        campoVagas.caixa = rVagas;
        atualizarCampoTexto(ctx, &campoVagas);
        desenharBotaoSkyrim(rVagas, "VAGAS:", campoVagas.texto, CheckCollisionPointRec(GetMousePosition(), rVagas));
        py += 45;

        // Adiciona UM trecho manual (origem -> destino escolhidos no mapa),
        // sem checar se existe "estrada" entre eles - o motorista decide a rota dele.
        if (desenharBotaoSkyrim((Rectangle){ px, py, 340, 36 }, "ADICIONAR TRECHO", NULL, false)) {
            if (ctx->origemTrechoIndex != -1 && ctx->destinoTrechoIndex != -1) {
                float preco = (float)atof(campoPreco.texto);
                int vagas = atoi(campoVagas.texto);
                const char *origemNome = provincias[ctx->origemTrechoIndex].nome;
                const char *destinoNome = provincias[ctx->destinoTrechoIndex].nome;

                int ok = controleAdicionarTrechoManual(origemNome, destinoNome, preco, vagas,
                                                        trechosPendentes, &numTrechosPendentes, MAX_TRECHOS_PUBLICAR,
                                                        trechosResumo, &numTrechosResumo, MAX_TRECHOS_PUBLICAR);

                strcpy(ctx->mensagemStatus, ok ? "Trecho adicionado!" : "Verifique origem/destino e preco/vagas.");

                ctx->origemTrechoIndex = -1; ctx->destinoTrechoIndex = -1;
                campoPreco.texto[0] = '\0'; campoVagas.texto[0] = '\0';
            } else {
                strcpy(ctx->mensagemStatus, "Selecione origem e destino no mapa.");
            }
        }
        py += 45;

        for (int i = 0; i < numTrechosResumo && py < alturaAtual - 90; i++) {
            DrawTextSkyrim(trechosResumo[i], (int)px, (int)py, 15, GREEN);
            py += 20;
        }

        if (desenharBotaoSkyrim((Rectangle){ px, (float)(alturaAtual - 80), 340, 38 }, "PUBLICAR CARONA", NULL, false)) {
            RespostaCarona resp = controlePublicarCarona(ctx->servidorFD, campoDataCarona.texto, campoHorarioCarona.texto,
                                                          trechosPendentes, numTrechosPendentes);
            strcpy(ctx->mensagemStatus, resp.mensagem);

            if (resp.codigo == CARONA_OK) {
                numTrechosPendentes = 0;
                numTrechosResumo = 0;
            }
        }
    } else if (ctx->subTelaMotorista == M_CARONAS) {
        DrawTextSkyrim("MINHAS CARONAS:", (int)px, (int)py, 18, LIGHTGRAY);
        py += 30;

        if (numCaronasInfo == 0) {
            DrawTextSkyrim(mensagemCaronas, (int)px, (int)py, 15, GRAY);
            py += 30;
        } else {
            float limiteInferior = (float)alturaAtual - 145.0f;
            for (int i = 0; i < numCaronasInfo && py < limiteInferior; i++) {
                CaronaInfo *ci = &caronasInfo[i];
                float alturaCard = 26.0f + ci->numTrechos * 20.0f + 10.0f;
                if (py + alturaCard > limiteInferior) alturaCard = limiteInferior - py;
                if (alturaCard < 26.0f) break;

                DrawRectangle((int)px, (int)py, 340, (int)alturaCard, Fade(BLACK, 0.6f));
                DrawRectangleLines((int)px, (int)py, 340, (int)alturaCard,
                    ci->ativa ? Fade(GREEN, 0.35f) : Fade(RED, 0.35f));

                char cabecalho[64];
                snprintf(cabecalho, sizeof(cabecalho), "Carona #%d - %s %s", ci->idCarona, ci->data, ci->horario);
                DrawTextSkyrim(cabecalho, (int)px + 8, (int)py + 6, 14, WHITE);

                DrawTextSkyrim(ci->ativa ? "ATIVA" : "CANCELADA", (int)px + 255, (int)py + 6, 11,
                    ci->ativa ? GREEN : RED);

                float pyTrecho = py + 24;
                for (int t = 0; t < ci->numTrechos && pyTrecho + 18 <= py + alturaCard; t++) {
                    TrechoCaronaInfo *tr = &ci->trechos[t];
                    Color corVagas = tr->vagasDisp <= 0 ? RED : (tr->vagasDisp < tr->vagasTotal ? ORANGE : GREEN);

                    char linhaTrecho[160];
                    if (tr->numPassageiros > 0) {
                        snprintf(linhaTrecho, sizeof(linhaTrecho), "%s -> %s  R$%.2f  %d/%d vagas  (%d pax)",
                            tr->origem, tr->destino, tr->preco, tr->vagasDisp, tr->vagasTotal, tr->numPassageiros);
                    } else {
                        snprintf(linhaTrecho, sizeof(linhaTrecho), "%s -> %s  R$%.2f  %d/%d vagas",
                            tr->origem, tr->destino, tr->preco, tr->vagasDisp, tr->vagasTotal);
                    }
                    DrawTextSkyrim(linhaTrecho, (int)px + 14, (int)pyTrecho, 13, corVagas);
                    pyTrecho += 20;
                }

                py += alturaCard + 8;
            }
        }

        py += 12;
        Rectangle rId = { px, py, 340, alturaItem };
        campoIdCarona.caixa = rId;
        atualizarCampoTexto(ctx, &campoIdCarona);
        desenharBotaoSkyrim(rId, "ID DA CARONA:", campoIdCarona.texto, CheckCollisionPointRec(GetMousePosition(), rId));
        py += 48;

        if (desenharBotaoSkyrim((Rectangle){ px, py, 340, 36 }, "CANCELAR CARONA", NULL, false)) {
            RespostaCarona resp = controleCancelarCarona(ctx->servidorFD, campoIdCarona.texto);
            strcpy(ctx->mensagemStatus, resp.mensagem);
        }
    }
}
