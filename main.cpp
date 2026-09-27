#include "raylib.h"
#include <iostream>
#include <vector>
using namespace std;

const int SCREEN_WIDTH = 1920; // Tamanho da largura da tela 
const int SCREEN_HEIGHT = 1200; // Tamanho da altura da tela

const int CELL_SIZE = 80; // Tamanho de cada "bloco" do grid
const int GRID_COLS = SCREEN_WIDTH / CELL_SIZE; // Número de colunas do grid 
const int GRID_ROWS = SCREEN_HEIGHT / CELL_SIZE; // Número de linhas do grid

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Conway's Game of Life"); // Inicializa a tela
    SetTargetFPS(240); // Seta o FPS 

    vector<vector<bool>> grid(GRID_ROWS, vector<bool>(GRID_COLS, false)); // Matriz para armazenar as células vivas e mortas
    
    Camera2D camera = {0}; // Inicializa a camera (2D)
    camera.zoom = 1.0f;  // Inicializa o zoom na tela para o padrão

    // Enquanto a tela estiver aberta
    while(!WindowShouldClose()) { 
        // Verifica se o botão esquerdo do mouse foi selecionado   
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            // Então pega o pixel (x, y) selecionado pelo clique do mouse 
            Vector2 mouseWorld = GetScreenToWorld2D(GetMousePosition(), camera);  
            int col = (int)(mouseWorld.x / CELL_SIZE); // Encontra a coluna clickada
            int row = (int)(mouseWorld.y / CELL_SIZE); // Encontra a linha clickada
            
            // Verifica se o pixel clickado e convertido para o grid é válido
            if (col >= 0 && col < GRID_COLS && row >= 0 && row < GRID_ROWS) {
                grid[row][col] = !grid[row][col]; // Então, inverte o estado da célula (false = morto, true = vivo)
            }
        }
        // Começa a desenhar 
        BeginDrawing();
            ClearBackground(RAYWHITE); // Fundo da tela
            BeginMode2D(camera); // Inicializa o modo 2D da camera
                // Percorre por todas as células do grid
                for (int row = 0; row < GRID_ROWS; row++) {
                    for (int col = 0; col < GRID_COLS; col++) {
                        if (grid[row][col]) { // Se a célular estiver viva  
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

            EndMode2D(); // Finaliza o modo 2D da camera
        EndDrawing(); // Termina de desenhar
    }

    CloseWindow(); // Fecha a janela

    return 0;
}


