#ifndef CONTROLE_AUTENTICACAO_H
#define CONTROLE_AUTENTICACAO_H

typedef enum {
    AUTH_OK,
    AUTH_CAMPOS_VAZIOS,
    AUTH_CREDENCIAIS_INVALIDAS,
    AUTH_FALHA_SERVIDOR
} ResultadoAutenticacao;

typedef struct {
    ResultadoAutenticacao codigo;
    char mensagem[256]; 
} RespostaAutenticacao;

RespostaAutenticacao controleLogin(int servidorFD, const char *tipo,
                                    const char *email, const char *senha);

RespostaAutenticacao controleCadastro(int servidorFD, const char *tipo,
                                       const char *nome, const char *contato,
                                       const char *email, const char *senha);

#endif
