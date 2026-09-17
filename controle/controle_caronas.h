#ifndef CONTROLE_CARONAS_H
#define CONTROLE_CARONAS_H

#define MAX_TRECHOS_PUBLICAR     64
#define MAX_TRECHOS_CARONA_INFO  16
#define MAX_CARONAS_INFO         20
#define TAM_RESUMO_TRECHO        128

typedef enum {
    CARONA_OK,
    CARONA_CAMPOS_INVALIDOS,
    CARONA_NENHUM_TRECHO,
    CARONA_FALHA_SERVIDOR
} ResultadoCarona;

typedef struct {
    ResultadoCarona codigo;
    char mensagem[256];
} RespostaCarona;

typedef struct {
    char origem[64];
    char destino[64];
    float preco;
    int vagas;
} TrechoPendente;

typedef struct {
    int idxTrecho;
    char origem[64];
    char destino[64];
    float preco;
    int vagasDisp;
    int vagasTotal;
    int numPassageiros;
} TrechoCaronaInfo;

typedef struct {
    int idCarona;
    char data[20];
    char horario[10];
    int ativa;
    TrechoCaronaInfo trechos[MAX_TRECHOS_CARONA_INFO];
    int numTrechos;
} CaronaInfo;

int controleAdicionarTrechoManual(const char *origemNome, const char *destinoNome, float preco, int vagas,
                                   TrechoPendente *pendentes, int *numPendentes, int maxPendentes,
                                   char resumo[][TAM_RESUMO_TRECHO], int *numResumo, int maxResumo);

RespostaCarona controlePublicarCarona(int servidorFD, const char *data, const char *horario,
                                       const TrechoPendente *pendentes, int numPendentes);

RespostaCarona controleListarMinhasCaronas(int servidorFD, CaronaInfo *saida, int *numSaida, int maxSaida);

RespostaCarona controleCancelarCarona(int servidorFD, const char *idCaronaTexto);

#endif
