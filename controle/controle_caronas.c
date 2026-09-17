#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "controle_caronas.h"
#include "../modelo/protocolo.h"

extern int enviarEReceber(int servidorFD, const char *acao, const char *payload,
                           char *respAcao, char *respPayload);

#define MAX_LINHAS_INTERNO 20
#define TAM_LINHA_INTERNO  512

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

static int contarPassageiros(const char *campo) {
    if (campo == NULL || campo[0] == '\0') return 0;
    int n = 1;
    for (const char *p = campo; *p; p++) if (*p == ',') n++;
    return n;
}

static void parseCaronaLinha(const char *raw, CaronaInfo *out) {
    memset(out, 0, sizeof(CaronaInfo));

    char buf[TAM_LINHA_INTERNO];
    strncpy(buf, raw, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';

    char status[16] = "";
    char *tok = strtok(buf, ";");
    if (tok == NULL) return;
    sscanf(tok, "%d|%19[^|]|%9[^|]|%15s", &out->idCarona, out->data, out->horario, status);
    out->ativa = (strcmp(status, "ATIVA") == 0);

    tok = strtok(NULL, ";");
    while (tok != NULL && out->numTrechos < MAX_TRECHOS_CARONA_INFO) {
        TrechoCaronaInfo *tr = &out->trechos[out->numTrechos];
        char rota[128] = "";
        char passageiros[300] = "";

        int lidos = sscanf(tok, "%d:%127[^:]:%f:%d/%d:%299[^\n]",
            &tr->idxTrecho, rota, &tr->preco, &tr->vagasDisp, &tr->vagasTotal, passageiros);

        if (lidos >= 5) {
            char *sep = strchr(rota, '-');
            if (sep != NULL) {
                *sep = '\0';
                strncpy(tr->origem, rota, sizeof(tr->origem) - 1);
                strncpy(tr->destino, sep + 1, sizeof(tr->destino) - 1);
            } else {
                strncpy(tr->origem, rota, sizeof(tr->origem) - 1);
            }
            tr->numPassageiros = contarPassageiros(passageiros);
            out->numTrechos++;
        }
        tok = strtok(NULL, ";");
    }
}

int controleAdicionarTrechoManual(const char *origemNome, const char *destinoNome, float preco, int vagas,
                                   TrechoPendente *pendentes, int *numPendentes, int maxPendentes,
                                   char resumo[][TAM_RESUMO_TRECHO], int *numResumo, int maxResumo) {
    if (campoVazio(origemNome) || campoVazio(destinoNome)) return 0;
    if (strcmp(origemNome, destinoNome) == 0) return 0;
    if (preco <= 0 || vagas <= 0) return 0;
    if (*numPendentes >= maxPendentes) return 0;

    TrechoPendente *tp = &pendentes[(*numPendentes)++];
    snprintf(tp->origem, sizeof(tp->origem), "%s", origemNome);
    snprintf(tp->destino, sizeof(tp->destino), "%s", destinoNome);
    tp->preco = preco;
    tp->vagas = vagas;

    if (*numResumo < maxResumo) {
        snprintf(resumo[*numResumo], TAM_RESUMO_TRECHO, "%s -> %s | R$%.2f | %d vagas",
                 origemNome, destinoNome, preco, vagas);
        (*numResumo)++;
    }

    return 1;
}

RespostaCarona controlePublicarCarona(int servidorFD, const char *data, const char *horario,
                                       const TrechoPendente *pendentes, int numPendentes) {
    RespostaCarona r = {0};

    if (campoVazio(data) || campoVazio(horario)) {
        r.codigo = CARONA_CAMPOS_INVALIDOS;
        snprintf(r.mensagem, sizeof(r.mensagem), "Preencha data e horario.");
        return r;
    }
    if (numPendentes <= 0) {
        r.codigo = CARONA_NENHUM_TRECHO;
        snprintf(r.mensagem, sizeof(r.mensagem), "Adicione ao menos um trecho.");
        return r;
    }

    char trechosBuf[4096] = "";
    for (int i = 0; i < numPendentes; i++) {
        char item[300];
        snprintf(item, sizeof(item), "%s,%s,%.2f,%d;",
                 pendentes[i].origem, pendentes[i].destino, pendentes[i].preco, pendentes[i].vagas);
        strncat(trechosBuf, item, sizeof(trechosBuf) - strlen(trechosBuf) - 1);
    }

    char payload[4096], respAcao[MAX_ACAO], respPayload[MAX_PAYLOAD];
    snprintf(payload, sizeof(payload), "%.20s|%.20s|%.3900s", data, horario, trechosBuf);

    if (!enviarEReceber(servidorFD, "PUBLICAR_CARONA", payload, respAcao, respPayload)) {
        r.codigo = CARONA_FALHA_SERVIDOR;
        snprintf(r.mensagem, sizeof(r.mensagem), "%s", respPayload);
        return r;
    }

    r.codigo = (strstr(respPayload, "sucesso") != NULL) ? CARONA_OK : CARONA_FALHA_SERVIDOR;
    snprintf(r.mensagem, sizeof(r.mensagem), "%s", respPayload);
    return r;
}

RespostaCarona controleListarMinhasCaronas(int servidorFD, CaronaInfo *saida, int *numSaida, int maxSaida) {
    RespostaCarona r = {0};
    char respAcao[MAX_ACAO], respPayload[MAX_PAYLOAD];
    *numSaida = 0;

    if (!enviarEReceber(servidorFD, "MINHAS_CARONAS", "", respAcao, respPayload)) {
        r.codigo = CARONA_FALHA_SERVIDOR;
        snprintf(r.mensagem, sizeof(r.mensagem), "%s", respPayload);
        return r;
    }

    char linhas[MAX_LINHAS_INTERNO][TAM_LINHA_INTERNO];
    int numLinhas = quebrarLinhasLocal(respPayload, linhas, MAX_LINHAS_INTERNO);

    if (numLinhas == 0 || (numLinhas == 1 && !linhaTemDados(linhas[0]))) {
        r.codigo = CARONA_OK;
        snprintf(r.mensagem, sizeof(r.mensagem), "%s",
                 numLinhas == 0 ? "Nenhuma carona publicada." : linhas[0]);
        return r;
    }

    for (int i = 0; i < numLinhas && *numSaida < maxSaida; i++) {
        if (!linhaTemDados(linhas[i])) continue;
        parseCaronaLinha(linhas[i], &saida[*numSaida]);
        (*numSaida)++;
    }

    r.codigo = CARONA_OK;
    r.mensagem[0] = '\0';
    return r;
}

RespostaCarona controleCancelarCarona(int servidorFD, const char *idCaronaTexto) {
    RespostaCarona r = {0};

    if (campoVazio(idCaronaTexto)) {
        r.codigo = CARONA_CAMPOS_INVALIDOS;
        snprintf(r.mensagem, sizeof(r.mensagem), "Informe o ID da carona.");
        return r;
    }

    char respAcao[MAX_ACAO], respPayload[MAX_PAYLOAD];
    if (!enviarEReceber(servidorFD, "CANCELAR_CARONA", idCaronaTexto, respAcao, respPayload)) {
        r.codigo = CARONA_FALHA_SERVIDOR;
        snprintf(r.mensagem, sizeof(r.mensagem), "%s", respPayload);
        return r;
    }

    r.codigo = (strstr(respPayload, "sucesso") != NULL) ? CARONA_OK : CARONA_FALHA_SERVIDOR;
    snprintf(r.mensagem, sizeof(r.mensagem), "%s", respPayload);
    return r;
}
