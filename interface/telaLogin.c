#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "telaLogin.h"
#include "tema.h"
#include "../controle/controle_autenticacao.h"

static CampoTexto campoEmail = { .caixa = {0}, .senha = 0 };
static CampoTexto campoSenha = { .caixa = {0}, .senha = 1 };
static CampoTexto campoNome = { .caixa = {0}, .senha = 0 };
static CampoTexto campoContato = { .caixa = {0}, .senha = 0 };
static int tipoSelecionado = 0;

/* Unica funcao "nao-visual" que sobra aqui: decide o que fazer na TELA
   depois que o controlador respondeu. Isso ainda e responsabilidade da
   interface (navegacao entre telas), so a parte de rede/validacao saiu. */
static void autenticarOuCadastrar(ContextoApp *ctx) {
    const char *tipo = tipoSelecionado == 1 ? "MOTORISTA" : "PASSAGEIRO";
    RespostaAutenticacao resp;

    if (ctx->telaAtual == TELA_CADASTRO) {
        resp = controleCadastro(ctx->servidorFD, tipo,
                                 campoNome.texto, campoContato.texto,
                                 campoEmail.texto, campoSenha.texto);
    } else {
        resp = controleLogin(ctx->servidorFD, tipo,
                              campoEmail.texto, campoSenha.texto);
    }

    strncpy(ctx->mensagemStatus, resp.mensagem, sizeof(ctx->mensagemStatus) - 1);
    ctx->mensagemStatus[sizeof(ctx->mensagemStatus) - 1] = '\0';

    if (resp.codigo != AUTH_OK) {
        return; /* campos vazios, credenciais invalidas ou falha de rede:
                    a mensagem ja esta na tela, so nao navega. */
    }

    if (ctx->telaAtual == TELA_LOGIN) {
        ctx->autenticado = 1;
        ctx->souMotorista = (tipoSelecionado == 1);
        ctx->telaAtual = ctx->souMotorista ? TELA_MOTORISTA : TELA_PASSAGEIRO;

        campoSenha.texto[0] = '\0';
        campoNome.texto[0] = '\0';
        campoContato.texto[0] = '\0';
    } else if (ctx->telaAtual == TELA_CADASTRO) {
        ctx->telaAtual = TELA_LOGIN;
        campoSenha.texto[0] = '\0';
        campoNome.texto[0] = '\0';
        campoContato.texto[0] = '\0';
        /* mantemos o email preenchido pra facilitar o login em seguida */
    }
}

void DesenharTelaLoginECadastro(ContextoApp *ctx, int larguraAtual, int alturaAtual) {
    float centroX = larguraAtual / 2.0f;
    float larguraElemento = 360.0f;
    float alturaElemento = 38.0f;
    float espacamento = 10.0f;

    int numItens = (ctx->telaAtual == TELA_CADASTRO) ? 8 : 5;
    float yInicial = (alturaAtual / 2.0f) - ((numItens * (alturaElemento + espacamento)) / 2.0f) + 60;

    BeginDrawing();
    ClearBackground(BLACK);

    if (texturaFundo.id != 0) {
        DrawTexturePro(texturaFundo,
            (Rectangle){0, 0, texturaFundo.width, texturaFundo.height},
            (Rectangle){0, 0, larguraAtual, alturaAtual},
            (Vector2){0, 0}, 0.0f, WHITE);
    }

    const char *titulo = "S K Y R I M";
    int tw = MedirTextoSkyrim(titulo, 60);
    DrawTextSkyrim(titulo, (int)(centroX - tw / 2), (int)(yInicial - 100), 60, WHITE);

    const char *sub = "T H E  R I D E  S C R O L L S";
    int twSub = MedirTextoSkyrim(sub, 18);
    DrawTextSkyrim(sub, (int)(centroX - twSub / 2), (int)(yInicial - 125), 18, WHITE);

    float yAtual = yInicial;
    Rectangle bounds;

    bounds = (Rectangle){ centroX - larguraElemento/2, yAtual, larguraElemento, alturaElemento };
    if (desenharBotaoSkyrim(bounds, tipoSelecionado == 0 ? "TIPO: PASSAGEIRO" : "TIPO: MOTORISTA", NULL, false)) {
        tipoSelecionado = !tipoSelecionado;
    }
    yAtual += alturaElemento + espacamento;

    if (ctx->telaAtual == TELA_CADASTRO) {
        bounds = (Rectangle){ centroX - larguraElemento/2, yAtual, larguraElemento, alturaElemento };
        campoNome.caixa = bounds;
        atualizarCampoTexto(ctx, &campoNome);
        desenharBotaoSkyrim(bounds, "NOME:", campoNome.texto, CheckCollisionPointRec(GetMousePosition(), bounds));
        yAtual += alturaElemento + espacamento;

        bounds = (Rectangle){ centroX - larguraElemento/2, yAtual, larguraElemento, alturaElemento };
        campoContato.caixa = bounds;
        atualizarCampoTexto(ctx, &campoContato);
        desenharBotaoSkyrim(bounds, "CONTATO:", campoContato.texto, CheckCollisionPointRec(GetMousePosition(), bounds));
        yAtual += alturaElemento + espacamento;
    }

    bounds = (Rectangle){ centroX - larguraElemento/2, yAtual, larguraElemento, alturaElemento };
    campoEmail.caixa = bounds;
    atualizarCampoTexto(ctx, &campoEmail);
    desenharBotaoSkyrim(bounds, "EMAIL:", campoEmail.texto, CheckCollisionPointRec(GetMousePosition(), bounds));
    yAtual += alturaElemento + espacamento;

    bounds = (Rectangle){ centroX - larguraElemento/2, yAtual, larguraElemento, alturaElemento };
    campoSenha.caixa = bounds;
    atualizarCampoTexto(ctx, &campoSenha);

    char senhaOculta[100] = {0};
    for (size_t i = 0; i < strlen(campoSenha.texto) && i < 99; i++) senhaOculta[i] = '*';
    desenharBotaoSkyrim(bounds, "SENHA:", senhaOculta, CheckCollisionPointRec(GetMousePosition(), bounds));
    yAtual += alturaElemento + espacamento;

    bounds = (Rectangle){ centroX - larguraElemento/2, yAtual, larguraElemento, alturaElemento };
    if (desenharBotaoSkyrim(bounds, ctx->telaAtual == TELA_CADASTRO ? "CADASTRAR" : "ENTRAR", NULL, false)) {
        autenticarOuCadastrar(ctx);
    }
    yAtual += alturaElemento + espacamento;

    bounds = (Rectangle){ centroX - larguraElemento/2, yAtual, larguraElemento, alturaElemento };
    if (desenharBotaoSkyrim(bounds, ctx->telaAtual == TELA_CADASTRO ? "JA TENHO CONTA" : "NOVA CONTA", NULL, false)) {
        ctx->telaAtual = (ctx->telaAtual == TELA_CADASTRO) ? TELA_LOGIN : TELA_CADASTRO;
        campoSenha.texto[0] = '\0';
    }
    yAtual += alturaElemento + espacamento;

    bounds = (Rectangle){ centroX - larguraElemento/2, yAtual, larguraElemento, alturaElemento };
    if (desenharBotaoSkyrim(bounds, "SAIR", NULL, false)) {
        exit(0);
    }

    int msgW = MedirTextoSkyrim(ctx->mensagemStatus, 18);
    DrawTextSkyrim(ctx->mensagemStatus, (int)(centroX - msgW / 2), alturaAtual - 40, 18, YELLOW);

    EndDrawing();
}
