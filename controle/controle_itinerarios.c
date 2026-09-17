#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "controle_itinerarios.h"
#include "../modelo/protocolo.h"

extern int enviarEReceber(int servidorFD, const char *acao, const char *payload,
                           char *respAcao, char *respPayload);

#define MAX_LINHAS_INTERNO 20
#define TAM_LINHA_INTERNO  2048

static int campoVazio(const char *texto) {
    if (texto == NULL) return 1;
    for (const char *p = texto; *p != '\0'; p++) {
        if (*p != ' ') return 0;
    }
    return 1;
}

static int quebrarLinhasLocal(const char *payload, char linhas[][TAM_LINHA_INTERNO], int maxLinhas) {
    int n = 0;
    const char *inicio = payload;
    while (*inicio && n < maxLinhas) {
        const char *fim = strchr(inicio, '\n');
        int tamanho = fim ? (int)(fim - inicio) : (int)strlen(inicio);
        if (tamanho > 0) {
            int copiar = tamanho < TAM_LINHA_INTERNO - 1 ? tamanho : TAM_LINHA_INTERNO - 1;
            memcpy(linhas[n], inicio, copiar);
            linhas[n][copiar] = '\0';
            n++;
        }
        if (!fim) break;
        inicio = fim + 1;
    }
    return n;
}

static int linhaTemDados(const char *raw) {
    return strchr(raw, '|') != NULL;
}

static void parseItinerarioLinha(const char *raw, ItinerarioInfo *out) {
    memset(out, 0, sizeof(ItinerarioInfo));

    char buf[TAM_LINHA_INTERNO];
    strncpy(buf, raw, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';

    char *tok = strtok(buf, ";");
    if (tok == NULL) return;
    sscanf(tok, "%d|%f", &out->idx, &out->precoTotal);

    char rotaMontada[256] = "";
    int primeiro = 1;

    tok = strtok(NULL, ";");
    while (tok != NULL) {
        int idCarona = 0, idxTrecho = 0;
        char trecho[128] = "";

        if (sscanf(tok, "%d:%d:%127[^\n]", &idCarona, &idxTrecho, trecho) == 3) {
            char origemT[64] = "", destinoT[64] = "";
            char *sep = strchr(trecho, '-');
            if (sep != NULL) {
                *sep = '\0';
                snprintf(origemT, sizeof(origemT), "%s", trecho);
                snprintf(destinoT, sizeof(destinoT), "%s", sep + 1);
            }

            if (primeiro) {
                strncat(rotaMontada, origemT, sizeof(rotaMontada) - strlen(rotaMontada) - 1);
                primeiro = 0;
            }
            strncat(rotaMontada, " -> ", sizeof(rotaMontada) - strlen(rotaMontada) - 1);
            strncat(rotaMontada, destinoT, sizeof(rotaMontada) - strlen(rotaMontada) - 1);
        }
        tok = strtok(NULL, ";");
    }
    snprintf(out->rota, sizeof(out->rota), "%s", rotaMontada);
}

RespostaItinerario controleBuscarItinerario(int servidorFD, const char *origemNome,
                                             const char *destinoNome, const char *data,
                                             ItinerarioInfo *saida, int *numSaida, int maxSaida) {
    RespostaItinerario r = {0};
    *numSaida = 0;

    if (campoVazio(origemNome) || campoVazio(destinoNome) || campoVazio(data)) {
        r.codigo = ITIN_CAMPOS_INVALIDOS;
        snprintf(r.mensagem, sizeof(r.mensagem), "Selecione origem, destino e informe a data.");
        return r;
    }

    char payload[300], respAcao[MAX_ACAO], respPayload[MAX_PAYLOAD];
    snprintf(payload, sizeof(payload), "%.49s|%.49s|%.19s", origemNome, destinoNome, data);

    if (!enviarEReceber(servidorFD, "BUSCAR_ITINERARIO", payload, respAcao, respPayload)) {
        r.codigo = ITIN_FALHA_SERVIDOR;
        snprintf(r.mensagem, sizeof(r.mensagem), "%s", respPayload);
        return r;
    }

    char linhas[MAX_LINHAS_INTERNO][TAM_LINHA_INTERNO];
    int numLinhas = quebrarLinhasLocal(respPayload, linhas, MAX_LINHAS_INTERNO);

    if (numLinhas == 0 || (numLinhas == 1 && !linhaTemDados(linhas[0]))) {
        r.codigo = ITIN_OK;
        snprintf(r.mensagem, sizeof(r.mensagem), "%s",
                 numLinhas == 0 ? "Nenhum itinerario encontrado." : linhas[0]);
        return r;
    }

    for (int i = 0; i < numLinhas && *numSaida < maxSaida; i++) {
        if (!linhaTemDados(linhas[i])) continue;
        parseItinerarioLinha(linhas[i], &saida[*numSaida]);
        (*numSaida)++;
    }

    r.codigo = ITIN_OK;
    r.mensagem[0] = '\0';
    return r;
}

RespostaItinerario controleReservarItinerario(int servidorFD, int indiceSelecionado) {
    RespostaItinerario r = {0};

    if (indiceSelecionado < 0) {
        r.codigo = ITIN_SELECAO_INVALIDA;
        snprintf(r.mensagem, sizeof(r.mensagem), "Selecione um itinerario antes de reservar.");
        return r;
    }

    char payload[16], respAcao[MAX_ACAO], respPayload[MAX_PAYLOAD];
    snprintf(payload, sizeof(payload), "%d", indiceSelecionado);

    if (!enviarEReceber(servidorFD, "RESERVAR_ITINERARIO", payload, respAcao, respPayload)) {
        r.codigo = ITIN_FALHA_SERVIDOR;
        snprintf(r.mensagem, sizeof(r.mensagem), "%s", respPayload);
        return r;
    }

    r.codigo = (strstr(respPayload, "sucesso") != NULL) ? ITIN_OK : ITIN_FALHA_SERVIDOR;
    snprintf(r.mensagem, sizeof(r.mensagem), "%s", respPayload);
    return r;
}

RespostaItinerario controleListarMinhasReservas(int servidorFD, ReservaInfo *saida, int *numSaida, int maxSaida) {
    RespostaItinerario r = {0};
    char respAcao[MAX_ACAO], respPayload[MAX_PAYLOAD];
    *numSaida = 0;

    if (!enviarEReceber(servidorFD, "MINHAS_RESERVAS", "", respAcao, respPayload)) {
        r.codigo = ITIN_FALHA_SERVIDOR;
        snprintf(r.mensagem, sizeof(r.mensagem), "%s", respPayload);
        return r;
    }

    char linhas[MAX_LINHAS_INTERNO][TAM_LINHA_INTERNO];
    int numLinhas = quebrarLinhasLocal(respPayload, linhas, MAX_LINHAS_INTERNO);

    if (numLinhas == 0 || (numLinhas == 1 && !linhaTemDados(linhas[0]))) {
        r.codigo = ITIN_OK;
        snprintf(r.mensagem, sizeof(r.mensagem), "%s",
                 numLinhas == 0 ? "Nenhuma reserva encontrada." : linhas[0]);
        return r;
    }

    for (int i = 0; i < numLinhas && *numSaida < maxSaida; i++) {
        if (!linhaTemDados(linhas[i])) continue;

        ReservaInfo *ri = &saida[*numSaida];
        memset(ri, 0, sizeof(ReservaInfo));

        char buf[TAM_LINHA_INTERNO];
        strncpy(buf, linhas[i], sizeof(buf) - 1);
        buf[sizeof(buf) - 1] = '\0';

        char status[16] = "";
        char *tok = strtok(buf, ";");
        if (tok == NULL) continue;
        sscanf(tok, "%d|%f|%15s", &ri->idReserva, &ri->precoTotal, status);
        ri->ativa = (strcmp(status, "ATIVA") == 0);

        char rotaMontada[256] = "";
        tok = strtok(NULL, ";");
        while (tok != NULL) {
            if (rotaMontada[0] != '\0') strncat(rotaMontada, " | ", sizeof(rotaMontada) - strlen(rotaMontada) - 1);
            strncat(rotaMontada, tok, sizeof(rotaMontada) - strlen(rotaMontada) - 1);
            tok = strtok(NULL, ";");
        }
        snprintf(ri->rota, sizeof(ri->rota), "%s", rotaMontada);

        (*numSaida)++;
    }

    r.codigo = ITIN_OK;
    r.mensagem[0] = '\0';
    return r;
}

RespostaItinerario controleCancelarReserva(int servidorFD, const char *idReservaTexto) {
    RespostaItinerario r = {0};

    if (campoVazio(idReservaTexto)) {
        r.codigo = ITIN_CAMPOS_INVALIDOS;
        snprintf(r.mensagem, sizeof(r.mensagem), "Informe o ID da reserva.");
        return r;
    }

    char respAcao[MAX_ACAO], respPayload[MAX_PAYLOAD];
    if (!enviarEReceber(servidorFD, "CANCELAR_RESERVA", idReservaTexto, respAcao, respPayload)) {
        r.codigo = ITIN_FALHA_SERVIDOR;
        snprintf(r.mensagem, sizeof(r.mensagem), "%s", respPayload);
        return r;
    }

    r.codigo = (strstr(respPayload, "sucesso") != NULL) ? ITIN_OK : ITIN_FALHA_SERVIDOR;
    snprintf(r.mensagem, sizeof(r.mensagem), "%s", respPayload);
    return r;
}
