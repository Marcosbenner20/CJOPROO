#include <vector>
#include <raylib.h>
#include <iostream>
#include <deque> // Biblioteca que cria estrutura dinãmica
#include <raymath.h> // Operações matemáticas da biblioteca raylib

using namespace std;

// Cores personalizadas para o jogo
Color blue = {173, 216, 230, 255};      // Azul claro (fundo do jogo)
Color darkBlue = {43, 24, 51, 255};     // Azul escuro (utilizado na cobra e comida)

// Configurações da grade
int tamanhoCelulas = 30;      // Tamanho de cada célula no grid
int contagemCelulas = 25;     // Número de células em linhas e colunas (grid seria quadrado)

// Variáveis globais para controle do jogo
double controleTempo = 0; // Usado para controlar o tempo entre os movimentos da cobra
int pontuacao = 0;        // Pontuação atual do jogador

// Classe que representa a Cobra
class Cobra {
public:
    // Corpo da cobra representado como uma lista de segmentos
    deque<Vector2> corpo = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};
    // Direção atual da cobra (1, 0) significa que está indo para a direita
    Vector2 direcao{1, 0};
    
    // Desenha a cobra na tela
    void desenhar() {
        for (unsigned int i = 0; i < corpo.size(); i++) {
            float x = corpo[i].x;
            float y = corpo[i].y;
            Rectangle pedaco = Rectangle{x * tamanhoCelulas, y * tamanhoCelulas, (float)tamanhoCelulas, (float)tamanhoCelulas};
            DrawRectangleRounded(pedaco, 0.5, 6, darkBlue); // Desenha segmentos arredondados
        }
    }

    // Move a cobra na direção atual
    void movimenta() {
        corpo.pop_back(); // Remove o último segmento (cauda)
        corpo.push_front(Vector2Add(corpo[0], direcao)); // Adiciona um novo segmento na cabeça
    }

    // Faz a cobra crescer adicionando um segmento na cauda
    void crescer() {
        corpo.push_back(corpo.back()); // Duplica o último segmento para simular crescimento
    }
};

// Classe que representa a Comida
class Comida {
public:
    Vector2 position; // Posição da comida no grid

    // Construtor que posiciona a comida aleatoriamente
    Comida() {
        position = aleatorio();
    }

    // Desenha a comida na tela
    void desenhar() {
        DrawRectangle(position.x * tamanhoCelulas, position.y * tamanhoCelulas, tamanhoCelulas, tamanhoCelulas, darkBlue);
    }

    // Gera uma nova posição aleatória para a comida
    Vector2 aleatorio() {
        float x = GetRandomValue(0, contagemCelulas - 1);
        float y = GetRandomValue(0, contagemCelulas - 1);
        return Vector2{x, y};
    }
};

// Função que controla a velocidade da cobra
bool controleCobra(double intervalo) {
    double tempoCorrido = GetTime(); // Obtém o tempo desde o início do jogo
    if (tempoCorrido - controleTempo >= intervalo) { // Verifica se o intervalo foi atingido
        controleTempo = tempoCorrido; // Atualiza o controle de tempo
        return true; // Permite que a cobra se mova
    }
    return false; // Caso contrário, espera mais tempo
}

// Classe principal do jogo
class Jogo {
public:
    Cobra cobra = Cobra();  // Instância da cobra
    Comida comida = Comida();  // Instância da comida

    // Desenha todos os elementos do jogo
    void desenhar() {
        comida.desenhar();    // Desenha a comida
        cobra.desenhar();     // Desenha a cobra
        desenhaPontuacao();   // Desenha a pontuação na tela
    }

    // Movimenta a cobra
    void movimentar() {
        cobra.movimenta();
    }

    // Verifica colisões entre a cobra e a comida
    void verificaColisao() {
        if (CheckCollisionRecs(
                Rectangle{cobra.corpo[0].x * tamanhoCelulas, cobra.corpo[0].y * tamanhoCelulas, tamanhoCelulas, tamanhoCelulas},
                Rectangle{comida.position.x * tamanhoCelulas, comida.position.y * tamanhoCelulas, tamanhoCelulas, tamanhoCelulas})) {
            comida.position = comida.aleatorio(); // Reposiciona a comida
            cobra.crescer();                     // Faz a cobra crescer
            pontuacao++;                         // Incrementa a pontuação
        }
    }

    // Verifica se a cobra colidiu com as bordas do jogo
    void verificaColisaoBorda() {
        if (cobra.corpo[0].x < 0 || cobra.corpo[0].x >= contagemCelulas ||
            cobra.corpo[0].y < 0 || cobra.corpo[0].y >= contagemCelulas) {
            CloseWindow(); // Encerra o jogo em caso de colisão
        }
    }

    // Verifica se a cobra colidiu consigo mesma
    void verificaColisaoCorpo() {
        for (size_t i = 1; i < cobra.corpo.size(); i++) {
            if (cobra.corpo[0].x == cobra.corpo[i].x && cobra.corpo[0].y == cobra.corpo[i].y) {
                CloseWindow(); // Encerra o jogo em caso de colisão
            }
        }
    }

    // Exibe a pontuação atual do jogador
    void desenhaPontuacao() {
        DrawText(TextFormat("Pontuacao: %d", pontuacao), 10, 10, 20, DARKGRAY);
    }
};

// Função principal que inicia o jogo
int main() {
    cout << "\n";
    cout << "## Jogo da Cobrinha ##\n\n";

    Jogo jogo = Jogo(); // Instancia o jogo

    // Define o tamanho da janela baseado no tamanho da grade
    int tam = tamanhoCelulas * contagemCelulas;

    InitWindow(tam, tam, "Meu Primeiro Jogo em C++");
    SetTargetFPS(60); // Define a taxa de quadros para 60 FPS

    while (!WindowShouldClose()) { // Loop principal do jogo
        BeginDrawing();
        ClearBackground(blue); // Limpa o fundo da tela com a cor azul

        // Controla a velocidade da cobra com base na pontuação
        double intervalo = max(0.2 - pontuacao * 0.01, 0.05); // Aumenta a velocidade conforme a pontuação
        if (controleCobra(intervalo)) {
            jogo.movimentar();          // Move a cobra
            jogo.verificaColisao();     // Verifica colisão com a comida
            jogo.verificaColisaoBorda(); // Verifica colisão com as bordas
            jogo.verificaColisaoCorpo(); // Verifica colisão com o próprio corpo
        }

        // Controla o movimento com as teclas de direção
        if (IsKeyPressed(KEY_UP) && jogo.cobra.direcao.y != 1) {
            jogo.cobra.direcao = {0, -1};
        }
        if (IsKeyPressed(KEY_DOWN) && jogo.cobra.direcao.y != -1) {
            jogo.cobra.direcao = {0, 1};
        }
        if (IsKeyPressed(KEY_LEFT) && jogo.cobra.direcao.x != 1) {
            jogo.cobra.direcao = {-1, 0};
        }
        if (IsKeyPressed(KEY_RIGHT) && jogo.cobra.direcao.x != -1) {
            jogo.cobra.direcao = {1, 0};
        }

        // Desenha os elementos do jogo
        jogo.desenhar();

        EndDrawing();
    }

    CloseWindow(); // Encerra a janela ao final do jogo
    return 0;
}
