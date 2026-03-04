#include <stdio.h>      // Librería para usar printf (salida en consola)
#include <stdlib.h>     // Librería para convertir texto a número
#include <vector>       // Estructura dinámica tipo matriz
#include <queue>        // Cola para BFS
#include <algorithm>    // Para usar shuffle
#include <random>       // Generador de números aleatorios 
#include <chrono>       // Para medir tiempo de ejecución

using namespace std;    // Evita escribir std:: antes de cada cosa

int N;  // Variable global que guarda el tamaño del laberinto (NxN)

// Matriz que representa el laberinto
vector<vector<char>> maze;

// Movimientos para generar el laberinto
// Se salta 2 posiciones porque entre celdas hay una pared
int dx[4] = {-2, 2, 0, 0};  // Cambios posibles en X
int dy[4] = {0, 0, -2, 2};  // Cambios posibles en Y
  
// Función que verifica si una coordenada está dentro del laberinto
bool valido(int x, int y) {
    return x >= 0 && y >= 0 && x < N && y < N;
}

/*GENERACIÓN DEL LABERINTO - DFS RECURSIVO*/

// Función recursiva que crea caminos usando Backtracking
void dfs(int x, int y, mt19937 &gen) {

    // Marcamos la celda actual como camino
    maze[x][y] = '*';

    // Vector con las 4 direcciones posibles (0,1,2,3)
    vector<int> dir = {0,1,2,3};

    // Mezcla aleatoriamente las direcciones
    // Esto hace que cada ejecución sea diferente
    shuffle(dir.begin(), dir.end(), gen);

    // Recorremos cada dirección mezclada
    for(int i : dir) {

        // Calculamos la nueva posición
        int nx = x + dx[i];
        int ny = y + dy[i];

        // Si está dentro de límites y aún es pared
        if(valido(nx,ny) && maze[nx][ny] == '#') {

            // Rompemos la pared intermedia (mitad del salto)
            maze[x + dx[i]/2][y + dy[i]/2] = '*';

            // Llamada recursiva
            dfs(nx, ny, gen);
        }
    }
}

// Función que prepara el laberinto
void generar() {

    // Inicializa toda la matriz con paredes '#'
    maze.assign(N, vector<char>(N, '#'));

    // Crea generador aleatorio moderno
    static mt19937 gen(random_device{}());

    // Comienza la generación desde la esquina superior izquierda
    dfs(0,0,gen);

    // Marca inicio y fin
    maze[0][0] = 'S';
    maze[N-1][N-1] = 'E';
}

/*RESOLUCIÓN DEL LABERINTO - BFS*/

void resolver() {

    // Cola para BFS (explora por niveles)
    queue<pair<int,int>> q;

    // Matriz que indica qué celdas ya fueron visitadas
    vector<vector<bool>> vis(N, vector<bool>(N,false));

    // Guarda desde dónde vino cada celda
    // Esto permite reconstruir el camino después
    vector<vector<pair<int,int>>> parent(
        N, vector<pair<int,int>>(N,{-1,-1})
    );

    // Movimientos normales (1 paso)
    int movX[4] = {-1,1,0,0};
    int movY[4] = {0,0,-1,1};

    // Empezamos desde la entrada
    q.push({0,0});
    vis[0][0] = true;

    // Mientras haya posiciones por explorar
    while(!q.empty()) {

        // Tomamos el frente de la cola
        auto [x,y] = q.front();
        q.pop();

        // Si llegamos a la salida, detenemos búsqueda
        if(x==N-1 && y==N-1)
            break;

        // Revisamos las 4 direcciones
        for(int i=0;i<4;i++){

            int nx = x + movX[i];
            int ny = y + movY[i];

            // Si es válida, no visitada y no es pared
            if(valido(nx,ny) && !vis[nx][ny] && maze[nx][ny] != '#') {

                vis[nx][ny] = true;          // Marcar visitada
                parent[nx][ny] = {x,y};      // Guardar padre
                q.push({nx,ny});             // Agregar a la cola
            }
        }
    }

    // Reconstrucción del camino desde la salida
    int x = N-1, y = N-1;

    // Retrocedemos usando la matriz parent
    while(parent[x][y].first != -1) {

        // No sobrescribimos la salida
        if(maze[x][y] != 'E')
            maze[x][y] = '.';  // Marcar camino solución

        // Ir al padre
        tie(x,y) = parent[x][y];
    }
}

/* MOSTRAR EN CONSOLA*/

void mostrar() {

    // Recorre toda la matriz
    for(int i=0;i<N;i++){
        for(int j=0;j<N;j++)
            printf("%c ", maze[i][j]);  // Imprime cada celda
        printf("\n");                   // Salto de línea por fila
    }
}

/*FUNCIÓN PRINCIPAL*/

int main(int argc, char* argv[]) {

    // Si el usuario pasa tamaño por consola, lo usamos
    // Si no, el tamaño será 10 por defecto
    N = (argc > 1) ? atoi(argv[1]) : 10;

    // Inicio de medición de tiempo
    auto inicio = chrono::high_resolution_clock::now();

    generar();   // Genera el laberinto
    resolver();  // Lo resuelve automáticamente

    // Fin de medición
    auto fin = chrono::high_resolution_clock::now();

    // Calcula duración
    chrono::duration<double> t = fin - inicio;

    mostrar();   

    // Imprime el tiempo total
    printf("\nTiempo total: %f segundos\n", t.count());

    return 0;  
}