#include <stdio.h>
#include <string.h>

#include "painelPassageiro.h"
#include "tema.h"
#include "mapa.h"
#include "../controle/controle_itinerarios.h"

static CampoTexto campoData = { .caixa = {0}, .senha = 0 };
static CampoTexto campoIdReserva = { .caixa = {0}, .senha = 0 };

static ItinerarioInfo itinerariosInfo[MAX_ITINERARIOS_INFO];
static int numItinerariosInfo = 0;
static char mensagemItinerarios[256] = "";
static int itinerarioSelecionado = -1;

static ReservaInfo reservasInfo[MAX_RESERVAS_INFO];
static int numReservasInfo = 0;
static char mensagemReservas[256] = "";

void DesenharPainelPassageiro(ContextoApp *ctx, Rectangle painel) {
    float px = painel.x + 15;
    float py = 20;
    float larguraBotaoAba = 108.0f;
    float alturaItem = 34.0f;

    DrawTextSkyrim("PASSAGEIRO", (int)px, (int)py, 28, WHITE);
    py += 40;

    if (desenharBotaoSkyrim((Rectangle){ px, py, larguraBotaoAba, 30 }, "BUSCAR", NULL, ctx->subTelaPassageiro == P_BUSCAR)) {
        ctx->subTelaPassageiro = P_BUSCAR;
    }
    if (desenharBotaoSkyrim((Rectangle){ px + 116, py, larguraBotaoAba, 30 }, "ROTAS", NULL, ctx->subTelaPassageiro == P_RESULTADOS)) {
        ctx->subTelaPassageiro = P_RESULTADOS;
    }
    if (desenharBotaoSkyrim((Rectangle){ px + 232, py, larguraBotaoAba, 30 }, "RESERVAS", NULL, ctx->subTelaPassageiro == P_RESERVAS)) {
        ctx->subTelaPassageiro = P_RESERVAS;
        RespostaItinerario resp = controleListarMinhasReservas(ctx->servidorFD, reservasInfo, &numReservasInfo, MAX_RESERVAS_INFO);
        snprintf(mensagemReservas, sizeof(mensagemReservas), "%s", resp.mensagem);
    }
    py += 50;

    if (ctx->subTelaPassageiro == P_BUSCAR) {
        Rectangle rData = { px, py, 340, alturaItem };
        campoData.caixa = rData;
        atualizarCampoTexto(ctx, &campoData);
        desenharBotaoSkyrim(rData, "DATA:", campoData.texto, CheckCollisionPointRec(GetMousePosition(), rData));
        py += 50;

        const char *origemNome = ctx->origemIndex != -1 ? provincias[ctx->origemIndex].nome : "SELECIONE NO MAPA";
        const char *destinoNome = ctx->destinoIndex != -1 ? provincias[ctx->destinoIndex].nome : "SELECIONE NO MAPA";

        DrawTextSkyrim("ORIGEM:", (int)px, (int)py, 18, GRAY);
        DrawTextSkyrim(origemNome, (int)px + 80, (int)py, 18, WHITE);
        py += 25;
        DrawTextSkyrim("DESTINO:", (int)px, (int)py, 18, GRAY);
        DrawTextSkyrim(destinoNome, (int)px + 80, (int)py, 18, WHITE);
        py += 40;

        if (desenharBotaoSkyrim((Rectangle){ px, py, 340, 38 }, "BUSCAR ITINERARIO", NULL, false)) {
            const char *origemReal = ctx->origemIndex != -1 ? provincias[ctx->origemIndex].nome : "";
            const char *destinoReal = ctx->destinoIndex != -1 ? provincias[ctx->destinoIndex].nome : "";

            RespostaItinerario resp = controleBuscarItinerario(ctx->servidorFD, origemReal, destinoReal, campoData.texto,
                                                                 itinerariosInfo, &numItinerariosInfo, MAX_ITINERARIOS_INFO);
            snprintf(mensagemItinerarios, sizeof(mensagemItinerarios), "%s", resp.mensagem);
            itinerarioSelecionado = -1;

            if (resp.codigo != ITIN_CAMPOS_INVALIDOS) {
                ctx->subTelaPassageiro = P_RESULTADOS;
            }
            strcpy(ctx->mensagemStatus, resp.mensagem);
        }
        py += 48;

        if (desenharBotaoSkyrim((Rectangle){ px, py, 340, 32 }, "LIMPAR SELECAO", NULL, false)) {
            ctx->origemIndex = -1;
            ctx->destinoIndex = -1;
        }

    } else if (ctx->subTelaPassageiro == P_RESULTADOS) {
        DrawTextSkyrim("ITINERARIOS ENCONTRADOS:", (int)px, (int)py, 18, LIGHTGRAY);
        py += 30;

        if (numItinerariosInfo == 0) {
            DrawTextSkyrim(mensagemItinerarios, (int)px, (int)py, 15, GRAY);
            py += 30;
        } else {
            for (int i = 0; i < numItinerariosInfo; i++) {
                ItinerarioInfo *ii = &itinerariosInfo[i];
                char titulo[64];
                snprintf(titulo, sizeof(titulo), "Itinerario %d - R$ %.2f", i + 1, ii->precoTotal);

                Rectangle rLinha = { px, py, 340, 36 };

                if (desenharBotaoSkyrim(rLinha, titulo, NULL, itinerarioSelecionado == i)) {
                    itinerarioSelecionado = i;
                }

                DrawTextSkyrim(ii->rota, (int)px + 10, (int)py + 40, 14, LIGHTGRAY);

                py += 65;
            }
        }

        py += 10;
        if (desenharBotaoSkyrim((Rectangle){ px, py, 340, 38 }, "RESERVAR SELECIONADO", NULL, false)) {
            RespostaItinerario resp = controleReservarItinerario(ctx->servidorFD, itinerarioSelecionado);
            strcpy(ctx->mensagemStatus, resp.mensagem);
        }

    } else if (ctx->subTelaPassageiro == P_RESERVAS) {
        DrawTextSkyrim("MINHAS RESERVAS:", (int)px, (int)py, 18, LIGHTGRAY);
        py += 30;

        if (numReservasInfo == 0) {
            DrawTextSkyrim(mensagemReservas, (int)px, (int)py, 15, GRAY);
            py += 30;
        } else {
            for (int i = 0; i < numReservasInfo; i++) {
                ReservaInfo *ri = &reservasInfo[i];

                DrawRectangle((int)px, (int)py, 340, 46, Fade(BLACK, 0.6f));
                DrawRectangleLines((int)px, (int)py, 340, 46,
                    ri->ativa ? Fade(GREEN, 0.35f) : Fade(RED, 0.35f));

                char titulo[64];
                snprintf(titulo, sizeof(titulo), "Reserva #%d - R$ %.2f", ri->idReserva, ri->precoTotal);
                DrawTextSkyrim(titulo, (int)px + 8, (int)py + 6, 14, WHITE);
                DrawTextSkyrim(ri->ativa ? "ATIVA" : "CANCELADA", (int)px + 255, (int)py + 6, 11,
                    ri->ativa ? GREEN : RED);
                DrawTextSkyrim(ri->rota, (int)px + 8, (int)py + 26, 13, LIGHTGRAY);

                py += 50;
            }
        }

        py += 12;
        Rectangle rId = { px, py, 340, alturaItem };
        campoIdReserva.caixa = rId;
        atualizarCampoTexto(ctx, &campoIdReserva);
        desenharBotaoSkyrim(rId, "ID DA RESERVA:", campoIdReserva.texto, CheckCollisionPointRec(GetMousePosition(), rId));
        py += 48;

        if (desenharBotaoSkyrim((Rectangle){ px, py, 340, 36 }, "CANCELAR RESERVA", NULL, false)) {
            RespostaItinerario resp = controleCancelarReserva(ctx->servidorFD, campoIdReserva.texto);
            strcpy(ctx->mensagemStatus, resp.mensagem);
        }
    }
}
