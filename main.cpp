#include "raylib.h"
#include <iostream>
#include <vector>

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

using namespace std;


const int SCREEN_WIDTH = 1280; // Tamanho da largura da tela 
const int SCREEN_HEIGHT = 720; // Tamanho da altura da tela

const int CELL_SIZE = 40; // Tamanho de cada "bloco" do grid
const int GRID_COLS = SCREEN_WIDTH / CELL_SIZE; // Número de colunas do grid 
const int GRID_ROWS = SCREEN_HEIGHT / CELL_SIZE; // Número de linhas do grid

enum class CellState { // Estados da FSM
    DEAD = 0, // 0 -> morto
    ALIVE = 1 // 1 => vivo
};

void updateDrawFrame();
void updateSimulation(vector<vector<CellState>>& current_grid);
CellState getNextState(CellState current, int neighbors);
int countAliveNeighbor(const vector<vector<CellState>>& grid, int row, int col);

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Conway's Game of Life"); // Inicializa a tela

    #if defined(PLATFORM_WEB) 
        emscripten_set_main_loop(updateDrawFrame, 0, 1);
    #else 
        SetTargetFPS(60); // Seta o FPS 
        while (!WindowShouldClose()) {
            updateDrawFrame();
        }
    #endif

    CloseWindow();
    return 0;
}


void updateDrawFrame() {
    /*
    @brief Função central para realizar todo o jogo. Normalmente era contida na main, no entanto com o emsdk para webAssembly, se faz necessário
           dividi-la em uma função própria
    */
    vector<vector<CellState>> grid(GRID_ROWS, vector<CellState>(GRID_COLS, CellState::DEAD)); // Matriz para armazenar as células vivas e mortas
    
    bool isRunning = false; // Flag para manter o jogo rodando ou pausado
    float updateInterval = 0.5; // Quantidade de gerações por segundo — neste caso, 2 g/s 
    float timer = 0;

    // Enquanto a tela estiver aberta
    while(!WindowShouldClose()) { 
        float deltaTime = GetFrameTime(); // Pega o tempo atual

        // Se a tecla de "ESPAÇO" for selecionado
        if (IsKeyPressed(KEY_SPACE)) { 
            isRunning = !isRunning; // Muda o estado do jogo para pausado/rodando 
        }

        // Se a tecla "ENTER" for selecionada e o jogo estiver pausado
        if (IsKeyPressed(KEY_ENTER) && !isRunning) { 
            updateSimulation(grid); // Parte para a próxima geração
        }
    
        // Se a tecla C foi selecionada
        if (IsKeyPressed(KEY_C)) {
            isRunning = false; // Pausa o jogo 
            for (int i = 0; i < GRID_ROWS; i++) { // Então, percorre por toda a malha/grade do jogo
                for (int j = 0; j < GRID_COLS; j++) {
                    grid[i][j] = CellState::DEAD; // E limpa as células, i.e., colocando todas como estado morto
                }
            }
        }

        // Verifica se o "BOTÃO ESQUERDO DO MOUSE" foi selecionado   
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            // Então pega o pixel (x, y) selecionado pelo clique do mouse  
            Vector2 mouse = GetMousePosition();  
            int col = (int)(mouse.x / CELL_SIZE); // Encontra a coluna clickada
            int row = (int)(mouse.y / CELL_SIZE); // Encontra a linha clickada
            
            // Verifica se o pixel clickado e convertido para o grid é válido
            if (col >= 0 && col < GRID_COLS && row >= 0 && row < GRID_ROWS) {
                // Então, inverte o estado da célula
                if (grid[row][col] == CellState::ALIVE) grid[row][col] = CellState::DEAD; // Se estiver vivo, vira morto com o clique
                else grid[row][col] = CellState::ALIVE; // Se estiver morto, vira vivo com o clique
            }
        }

        // Se o jogo não estiver pausado
        if (isRunning) {
            timer += deltaTime; // Incrementa o tempo de jogo 
            if (timer >= updateInterval) { // Espera o tempo de jogo atingir 0.5 segundos 
                updateSimulation(grid); // Atualiza a malha/grade do jogo
                timer = 0; // Reseta o tick de tempo para 0
            }
        }

        // Começa a desenhar 
        BeginDrawing();
            ClearBackground(RAYWHITE); // Fundo da tela
            // Percorre por todas as células do grid
            for (int row = 0; row < GRID_ROWS; row++) {
                for (int col = 0; col < GRID_COLS; col++) {
                    if (grid[row][col] == CellState::ALIVE) { // Se a célular estiver viva  
                        DrawRectangle(col * CELL_SIZE, row * CELL_SIZE, CELL_SIZE, CELL_SIZE, BLACK); // Enao pinta a célula 
                    }
                }
            }

            // Desenha as linhas do grid
            for (int x = 0; x <= SCREEN_WIDTH; x += CELL_SIZE) {
                DrawLine(x, 0, x, SCREEN_HEIGHT, LIGHTGRAY); // Desenha a linha que vai de (x, 0) e vai até (x, 1200)
            }
            // Desenha as colunas do grid
            for (int y = 0; y <= SCREEN_HEIGHT; y += CELL_SIZE) {
                DrawLine(0, y, SCREEN_WIDTH, y, LIGHTGRAY); // Desenha a linha que vai de (0, y) até (1920, y) 
            }

            // Desenhar a HUD do jogo
            DrawText("Espaço: Iniciar/Pausar", 20, 15, 20, BLACK); 
            if (isRunning) DrawText("Rodando", SCREEN_WIDTH - 100, 10, 20, BLUE);
            else DrawText("Pausado", SCREEN_WIDTH - 100, 10, 20, RED); 
            DrawText("Enter: Avançar geração (pausado)", 20, 50, 20, BLACK);
            DrawText("C: Limpar tela", 20, 85, 20, BLACK);
            
        EndDrawing(); // Termina de desenhar
    }
}


void updateSimulation(vector<vector<CellState>>& current_grid) {
    /*
    @brief Atualiza todas as células aplicando a FSM em um novo grid. A mudança não ocorre no mesmo grid, pois a mudança no mesmo pode 
           implicar um efeito cascata no estado das vizinhas. Por isso, utilizamos um novo grid que recebe o estado do grid atual.  
    @param currentGrid, A malha/grade atual do jogo   
    */

    auto next_grid = current_grid; // Gera uma cópia futura da malha/grade atual
    // Percorre por todas as células da malha
    for (int row = 0; row < GRID_ROWS; row++) {
        for (int col = 0; col < GRID_COLS; col++) {
            // Conta a quantidade de vizinhos vivos da célula atual
            int neighbors = countAliveNeighbor(current_grid, row, col);
            // Na cópia, atualiza o estado da célula para o próximo estado 
            next_grid[row][col] = getNextState(current_grid[row][col], neighbors);
        }
    }
    current_grid = next_grid; // Após todas as células terem sido avançadas para o próximo passo, o grid atual se torna o grid futuro
}


CellState getNextState(CellState current, int neighbors) {
    /*
    @brief Determina o próximo estado de uma célula com base nos seus vizinhos vivos     
    @param current, A célula cujo estado em questão está sendo verificado 
    @param neighbors, A quantidade de vizinhos viva dá célula atual (Vizinhança-Moore)
    @return CellState::ALIVE ou CellState::DEAD, Dependendo da condição de vida/morte da célula  
    */

    switch(current) { // Como as células são enum, é possível mapear por um switch
        case CellState::ALIVE: // Caso a célula esteja viva
            if (neighbors == 2 || neighbors == 3) return CellState::ALIVE; // Verifica se tem 2 ou 3 vizinhos vivos, para manter-se viva
            else return CellState::DEAD; // Caso contrário, a célula é morta

        case CellState::DEAD: // Caso a célula esteja morta
            if (neighbors == 3) return CellState::ALIVE; // Verifica se ela tem 3 vizinhos vivos, suficiente para o seu nascimento 
            else return CellState::DEAD; // Caso contrário, ela continua morta
    }
    // Se por acaso sair do switch, então a célula é considerada morta 
    return CellState::DEAD;
}


int countAliveNeighbor(const vector<vector<CellState>>& grid, int row, int col) {
    /*
    @brief Conta a quantidade de vizinhos vivos de uma célula por meio da Vizinhança-Moore (8 vizinhos)
    @param grid, A malha/grade que define as células do jogo
    @param row, A linha em que a célula está
    @param col, A coluna em que a célula está
    @retunr count, A quantidade de vizinhos da célula que estão com o estado "ALIVE" (vivo)   
    */
    int count = 0; // Contador de vizinhos 
    // Percorre pela vizinhança-Moore da célula
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i == 0 && j == 0) continue; // Se estiver na célula em questão (row, col), então pula para a próxima iteração
            // Calcula a posição dos 8 vizinhos da célula
            int x = (row + i + GRID_ROWS) % GRID_ROWS; // Caso a célula esteja em uma extremidade, ele pega o outro extremo como vizinho
            int y = (col + j + GRID_COLS) % GRID_COLS;
            // Se a célula em questão estiver viva 
            if (grid[x][y] == CellState::ALIVE) count++; // Incrementa o contador de vizinhos
        }
    }

    return count;
}