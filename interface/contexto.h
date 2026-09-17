#ifndef CONTEXTO_H
#define CONTEXTO_H

#include "raylib.h"
#include "../modelo/protocolo.h"

typedef enum { TELA_LOGIN, TELA_CADASTRO, TELA_PASSAGEIRO, TELA_MOTORISTA } Tela;
typedef enum { P_BUSCAR, P_RESULTADOS, P_RESERVAS } SubTelaPassageiro;
typedef enum { M_PUBLICAR, M_CARONAS } SubTelaMotorista;

typedef struct CampoTexto {
    char texto[128];
    Rectangle caixa;
    int senha;
} CampoTexto;

// Estado centralizado da aplicação
typedef struct {
    int servidorFD;
    Tela telaAtual;
    SubTelaPassageiro subTelaPassageiro;
    SubTelaMotorista subTelaMotorista;
    char mensagemStatus[256];
   
    int autenticado;
    int souMotorista;
    char meuNome[100];
    char meuEmail[100];
   
    int origemIndex;
    int destinoIndex;
   
    int origemTrechoIndex;
    int destinoTrechoIndex;

    CampoTexto *campoAtivo;
} ContextoApp;

void atualizarCampoTexto(ContextoApp *ctx, CampoTexto *campo);
int enviarEReceber(int servidorFD, const char *acao, const char *payload, char *respAcao, char *respPayload);

#endif
