/*
 * PROJETO INTEGRADOR UVV & BASE27
 * Sistema Digital de Controle de Horas Extras
 * * Protótipo Funcional em Linguagem C
 * Foco: Lógica de Programação, Estrutura de Dados e Manipulação de Arquivos.
 * [cite: 11, 52, 61]
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- DEFINIÇÃO DOS ARQUIVOS (Banco de Dados Simulado) ---
#define ARQUIVO_USUARIOS "usuarios.dat"
#define ARQUIVO_HORAS "horas.dat"

// --- DEFINIÇÃO DAS ESTRUTURAS (Structs) ---

// Enum para Status do Registro
typedef enum {
    PENDENTE,
    APROVADO,
    REPROVADO
} StatusAprovacao;

// Enum para Cargo do Usuário
typedef enum {
    FUNCIONARIO,
    GESTOR
} Cargo;

// Estrutura para o Usuário
typedef struct {
    int id;
    char username[50];
    char senha[50];
    Cargo cargo;
} Usuario;

// Estrutura para o Registro de Hora Extra
typedef struct {
    int idRegistro;
    int idFuncionario; // Chave para ligar ao usuário
    char data[11]; // "dd/mm/aaaa"
    double horas;
    char justificativa[200];
    StatusAprovacao status;
} RegistroHoraExtra;

// --- PROTÓTIPOS DAS FUNÇÕES (Menu) ---
void menuFuncionario(Usuario usuario);
void menuGestor(Usuario usuario);
Usuario* login(); // Função de login

// --- PROTÓTIPOS DAS FUNÇÕES (Funcionário) ---
void solicitarHoras(int idFuncionario);
void consultarMinhasHoras(int idFuncionario);

// --- PROTÓTIPOS DAS FUNÇÕES (Gestor) ---
void aprovarHoras();
void gerarRelatorio();

// --- PROTÓTIPOS DAS FUNÇÕES (Auxiliares) ---
int obterProximoIdRegistro();
const char* statusParaString(StatusAprovacao status);
const char* cargoParaString(Cargo cargo);
void limparTela();
void pausar();


// --- FUNÇÃO PRINCIPAL (MAIN) ---
int main() {
    Usuario* usuarioLogado;

    while (1) {
        limparTela();
        printf("====================================================\n");
        printf("  Sistema de Controle de Horas Extras - Base27/UVV\n");
        printf("          (Protótipo Lógico em Linguagem C)\n");
        printf("====================================================\n\n");
        
        usuarioLogado = login();

        if (usuarioLogado != NULL) {
            printf("\nLogin bem-sucedido! Bem-vindo, %s.\n", usuarioLogado->username);
            pausar();

            if (usuarioLogado->cargo == FUNCIONARIO) {
                menuFuncionario(*usuarioLogado);
            } else if (usuarioLogado->cargo == GESTOR) {
                menuGestor(*usuarioLogado);
            }
            
            free(usuarioLogado); // Libera a memória alocada para o usuário
        } else {
            printf("\nUsername ou senha incorretos. Tente novamente.\n");
            pausar();
        }
    }

    return 0;
}

// --- IMPLEMENTAÇÃO DAS FUNÇÕES ---

/*
 * Limpa o console (compatível com Windows e Linux/Mac)
 */
void limparTela() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

/*
 * Pausa a execução esperando o usuário pressionar ENTER
 */
void pausar() {
    printf("\nPressione ENTER para continuar...");
    // Limpa o buffer de entrada antes de esperar
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    getchar(); 
}

/*
 * Converte o ENUM de Status para uma string legível
 */
const char* statusParaString(StatusAprovacao status) {
    switch (status) {
        case PENDENTE: return "PENDENTE";
        case APROVADO: return "APROVADO";
        case REPROVADO: return "REPROVADO";
        default: return "DESCONHECIDO";
    }
}

/*
 * Converte o ENUM de Cargo para uma string legível
 */
const char* cargoParaString(Cargo cargo) {
    switch (cargo) {
        case FUNCIONARIO: return "FUNCIONARIO";
        case GESTOR: return "GESTOR";
        default: return "DESCONHECIDO";
    }
}

/*
 * Função de Login: Lê o arquivo usuarios.dat
 * Retorna um ponteiro para um Usuário (alocado dinamicamente) se o login for válido,
 * ou NULL se for inválido.
 */
Usuario* login() {
    char username[50];
    char senha[50];
    
    printf("--- Login ---\n");
    printf("Username: ");
    scanf("%s", username);
    printf("Senha: ");
    scanf("%s", senha);

    FILE* f = fopen(ARQUIVO_USUARIOS, "r");
    if (f == NULL) {
        printf("ERRO: Nao foi possivel abrir o arquivo %s\n", ARQUIVO_USUARIOS);
        return NULL;
    }

    int id;
    char user[50], pass[50], cargoStr[20];
    
    // Lê o arquivo linha por linha
    // Formato: id;username;senha;CARGO
    while (fscanf(f, "%d;%49[^;];%49[^;];%19[^\n]\n", &id, user, pass, cargoStr) == 4) {
        
        if (strcmp(username, user) == 0 && strcmp(senha, pass) == 0) {
            // Aloca memória dinamicamente para o usuário
            Usuario* usuarioEncontrado = (Usuario*) malloc(sizeof(Usuario));
            
            usuarioEncontrado->id = id;
            strcpy(usuarioEncontrado->username, user);
            strcpy(usuarioEncontrado->senha, pass);
            
            if (strcmp(cargoStr, "GESTOR") == 0) {
                usuarioEncontrado->cargo = GESTOR;
            } else {
                usuarioEncontrado->cargo = FUNCIONARIO;
            }
            
            fclose(f);
            return usuarioEncontrado; // Sucesso
        }
    }

    fclose(f);
    return NULL; // Falha
}

// --- Funções do Menu ---

void menuFuncionario(Usuario usuario) {
    int opcao = 0;
    while (opcao != 3) {
        limparTela();
        printf("====================================================\n");
        printf("  Painel do Funcionario: %s (ID: %d)\n", usuario.username, usuario.id);
        printf("====================================================\n\n");
        printf("1. Solicitar Horas Extras\n");
        printf("2. Consultar Minhas Solicitacoes\n");
        printf("3. Sair (Logout)\n");
        printf("\nEscolha uma opcao: ");
        
        scanf("%d", &opcao);

        switch (opcao) {
            case 1:
                solicitarHoras(usuario.id);
                break;
            case 2:
                consultarMinhasHoras(usuario.id);
                break;
            case 3:
                printf("\nFazendo logout...\n");
                break;
            default:
                printf("\nOpcao invalida!\n");
                break;
        }
        if (opcao != 3) pausar();
    }
}

void menuGestor(Usuario usuario) {
    int opcao = 0;
    while (opcao != 3) {
        limparTela();
        printf("====================================================\n");
        printf("  Painel do Gestor: %s (ID: %d)\n", usuario.username, usuario.id);
        printf("====================================================\n\n");
        printf("1. Aprovar/Reprovar Horas (Fluxo de Validacao) [cite: 37]\n");
        printf("2. Gerar Relatorio de Horas Aprovadas \n");
        printf("3. Sair (Logout)\n");
        printf("\nEscolha uma opcao: ");
        
        scanf("%d", &opcao);

        switch (opcao) {
            case 1:
                aprovarHoras();
                break;
            case 2:
                gerarRelatorio();
                break;
            case 3:
                printf("\nFazendo logout...\n");
                break;
            default:
                printf("\nOpcao invalida!\n");
                break;
        }
        if (opcao != 3) pausar();
    }
}

/*
 * Obtém o próximo ID para um novo registro, lendo o arquivo.
 * Em um sistema real, isso seria um AUTO_INCREMENT do banco.
 */
int obterProximoIdRegistro() {
    FILE* f = fopen(ARQUIVO_HORAS, "r");
    if (f == NULL) return 1; // Arquivo não existe, primeiro ID é 1

    RegistroHoraExtra reg;
    int maxId = 0;
    
    // Formato: idRegistro;idFunc;data;horas;justificativa;status(int)
    while (fscanf(f, "%d;%d;%10[^;];%lf;%199[^;];%d\n", 
           &reg.idRegistro, &reg.idFuncionario, reg.data, 
           &reg.horas, reg.justificativa, (int*)&reg.status) == 6) 
    {
        if (reg.idRegistro > maxId) {
            maxId = reg.idRegistro;
        }
    }
    fclose(f);
    return maxId + 1;
}

// --- Funções de Lógica (Core do Sistema) ---

/*
 * FUNÇÃO FUNCIONÁRIO: Solicitar Horas
 * Adiciona um novo registro no arquivo horas.dat com status PENDENTE.
 * 
 */
void solicitarHoras(int idFuncionario) {
    RegistroHoraExtra novoRegistro;
    
    novoRegistro.idRegistro = obterProximoIdRegistro();
    novoRegistro.idFuncionario = idFuncionario;
    novoRegistro.status = PENDENTE;
    
    limparTela();
    printf("--- Solicitar Horas Extras ---\n");
    
    printf("Data (dd/mm/aaaa): ");
    scanf("%s", novoRegistro.data);
    
    printf("Quantidade de Horas (ex: 2.5): ");
    scanf("%lf", &novoRegistro.horas);
    
    // Limpa o buffer do \n deixado pelo scanf
    int c;
    while ((c = getchar()) != '\n' && c != EOF);

    printf("Justificativa: ");
    fgets(novoRegistro.justificativa, 199, stdin);
    // Remove o \n pego pelo fgets
    novoRegistro.justificativa[strcspn(novoRegistro.justificativa, "\n")] = 0;

    // Abre o arquivo em modo "append" (adicionar ao final)
    FILE* f = fopen(ARQUIVO_HORAS, "a");
    if (f == NULL) {
        printf("ERRO: Nao foi possivel salvar o registro!\n");
        return;
    }
    
    // Formato: idRegistro;idFunc;data;horas;justificativa;status(int)
    fprintf(f, "%d;%d;%s;%.2f;%s;%d\n",
            novoRegistro.idRegistro,
            novoRegistro.idFuncionario,
            novoRegistro.data,
            novoRegistro.horas,
            novoRegistro.justificativa,
            novoRegistro.status);
            
    fclose(f);
    
    printf("\nSolicitacao (ID %d) enviada para aprovacao com sucesso!\n", novoRegistro.idRegistro);
}

/*
 * FUNÇÃO FUNCIONÁRIO: Consultar Minhas Horas
 * Lê o arquivo horas.dat e exibe apenas os registros do usuário logado.
 */
void consultarMinhasHoras(int idFuncionario) {
    limparTela();
    printf("--- Minhas Solicitacoes ---\n");
    
    FILE* f = fopen(ARQUIVO_HORAS, "r");
    if (f == NULL) {
        printf("Nenhum registro encontrado.\n");
        return;
    }
    
    RegistroHoraExtra reg;
    int encontrou = 0;
    
    while (fscanf(f, "%d;%d;%10[^;];%lf;%199[^;];%d\n", 
           &reg.idRegistro, &reg.idFuncionario, reg.data, 
           &reg.horas, reg.justificativa, (int*)&reg.status) == 6) 
    {
        if (reg.idFuncionario == idFuncionario) {
            printf("\n----------------------------------------\n");
            printf("ID Registro: %d\n", reg.idRegistro);
            printf("Data: %s\n", reg.data);
            printf("Horas: %.2f\n", reg.horas);
            printf("Status: %s\n", statusParaString(reg.status));
            printf("Justificativa: %s\n", reg.justificativa);
            encontrou = 1;
        }
    }
    
    if (!encontrou) {
        printf("\nVoce ainda nao possui solicitacoes.\n");
    }
    
    printf("\n----------------------------------------\n");
    fclose(f);
}

/*
 * FUNÇÃO GESTOR: Aprovar Horas
 * Lê TODOS os registros, permite ao gestor modificar o status
 * e REESCREVE o arquivo horas.dat com os dados atualizados.
 * Este é o "núcleo lógico" do protótipo.
 * [cite: 37]
 */
void aprovarHoras() {
    limparTela();
    printf("--- Aprovar Solicitacoes Pendentes ---\n");

    FILE* f = fopen(ARQUIVO_HORAS, "r");
    if (f == NULL) {
        printf("Nenhum registro encontrado.\n");
        return;
    }
    
    // 1. Carregar TODOS os registros para a memória (um array)
    RegistroHoraExtra registros[1000]; // Limite de 1000 registros para este protótipo
    int numRegistros = 0;
    int pendentes = 0;

    while (fscanf(f, "%d;%d;%10[^;];%lf;%199[^;];%d\n", 
           &registros[numRegistros].idRegistro, 
           &registros[numRegistros].idFuncionario, 
           registros[numRegistros].data, 
           &registros[numRegistros].horas, 
           registros[numRegistros].justificativa, 
           (int*)&registros[numRegistros].status) == 6) 
    {
        // Imprime os pendentes na tela
        if (registros[numRegistros].status == PENDENTE) {
            printf("\n----------------------------------------\n");
            printf("ID Registro: %d\n", registros[numRegistros].idRegistro);
            printf("ID Funcionario: %d\n", registros[numRegistros].idFuncionario);
            printf("Data: %s\n", registros[numRegistros].data);
            printf("Horas: %.2f\n", registros[numRegistros].horas);
            printf("Justificativa: %s\n", registros[numRegistros].justificativa);
            pendentes++;
        }
        numRegistros++;
    }
    fclose(f);
    
    if (pendentes == 0) {
        printf("\nNenhuma solicitacao pendente encontrada.\n");
        return;
    }
    
    printf("\n----------------------------------------\n");
    
    // 2. Solicitar ação do gestor
    int idParaAprovar;
    printf("\nDigite o ID do Registro que deseja processar: ");
    scanf("%d", &idParaAprovar);
    
    int opcao = 0;
    printf("1. Aprovar\n");
    printf("2. Reprovar\n");
    printf("Escolha (1 ou 2): ");
    scanf("%d", &opcao);
    
    StatusAprovacao novoStatus = (opcao == 1) ? APROVADO : REPROVADO;
    
    // 3. Modificar o registro na memória
    int encontrado = 0;
    for (int i = 0; i < numRegistros; i++) {
        if (registros[i].idRegistro == idParaAprovar) {
            if (registros[i].status == PENDENTE) {
                registros[i].status = novoStatus;
                encontrado = 1;
                break;
            }
        }
    }
    
    if (!encontrado) {
        printf("\nID nao encontrado ou ja processado.\n");
        return;
    }
    
    // 4. Reescrever o arquivo INTEIRO com os dados atualizados
    // (Modo "w" apaga o conteúdo anterior)
    f = fopen(ARQUIVO_HORAS, "w");
    if (f == NULL) {
        printf("ERRO FATAL: Nao foi possivel atualizar o banco de dados!\n");
        return;
    }
    
    for (int i = 0; i < numRegistros; i++) {
        fprintf(f, "%d;%d;%s;%.2f;%s;%d\n",
                registros[i].idRegistro,
                registros[i].idFuncionario,
                registros[i].data,
                registros[i].horas,
                registros[i].justificativa,
                registros[i].status);
    }
    fclose(f);
    
    printf("\nRegistro %d atualizado para %s com sucesso!\n", 
           idParaAprovar, statusParaString(novoStatus));
}

/*
 * FUNÇÃO GESTOR: Gerar Relatório
 * Lê o arquivo e calcula/exibe o total de horas APROVADAS.
 * 
 */
void gerarRelatorio() {
    limparTela();
    printf("--- Relatorio de Horas Aprovadas ---\n");
    
    FILE* f = fopen(ARQUIVO_HORAS, "r");
    if (f == NULL) {
        printf("Nenhum registro encontrado.\n");
        return;
    }
    
    RegistroHoraExtra reg;
    double totalHorasAprovadas = 0.0;
    
    printf("\nRegistros Aprovados:\n");
    printf("----------------------------------------\n");
    printf("| ID Reg | ID Func | Data       | Horas |\n");
    printf("----------------------------------------\n");
    
    while (fscanf(f, "%d;%d;%10[^;];%lf;%199[^;];%d\n", 
           &reg.idRegistro, &reg.idFuncionario, reg.data, 
           &reg.horas, reg.justificativa, (int*)&reg.status) == 6) 
    {
        if (reg.status == APROVADO) {
            printf("| %-6d | %-7d | %-10s | %-5.2f |\n",
                   reg.idRegistro, reg.idFuncionario, reg.data, reg.horas);
            totalHorasAprovadas += reg.horas;
        }
    }
    
    printf("----------------------------------------\n");
    printf("\nTOTAL DE HORAS APROVADAS: %.2f horas\n", totalHorasAprovadas);
    
    fclose(f);
}