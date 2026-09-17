#ifndef CONTROLE_ITINERARIOS_H
#define CONTROLE_ITINERARIOS_H

#define MAX_ITINERARIOS_INFO 20
#define MAX_RESERVAS_INFO    20

typedef enum {
    ITIN_OK,
    ITIN_CAMPOS_INVALIDOS,
    ITIN_SELECAO_INVALIDA,
    ITIN_FALHA_SERVIDOR
} ResultadoItinerario;

typedef struct {
    ResultadoItinerario codigo;
    char mensagem[256];
} RespostaItinerario;

typedef struct {
    int idx;
    float precoTotal;
    char rota[256];
} ItinerarioInfo;

typedef struct {
    int idReserva;
    float precoTotal;
    int ativa;
    char rota[256];
} ReservaInfo;

RespostaItinerario controleBuscarItinerario(int servidorFD, const char *origemNome,
                                             const char *destinoNome, const char *data,
                                             ItinerarioInfo *saida, int *numSaida, int maxSaida);

RespostaItinerario controleReservarItinerario(int servidorFD, int indiceSelecionado);

RespostaItinerario controleListarMinhasReservas(int servidorFD, ReservaInfo *saida, int *numSaida, int maxSaida);

RespostaItinerario controleCancelarReserva(int servidorFD, const char *idReservaTexto);

#endif
