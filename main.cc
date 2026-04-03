#include <iostream>
#include <iomanip>
#include <format>
#include <vector>
#include <experimental/random>

using namespace std;

struct Cell {
    bool is_bomb = false;
    bool is_revealed = false;
    int count = -1;
};

const int grid_size = 9;
const int num_bombs = 10;

void print_mod(const vector<vector<Cell>> &grid) {
    // 1. Offset for row headers (6 spaces to match " 10 | ")
    cout << "      "; 
    for (int x = 0; x < grid_size; x++) {
        // Force every column header to be exactly 3 characters wide ("1  ", "10 ")
        cout << left << setw(3) << x;
    }
    cout << right << "\n"; // Reset to right-alignment for the row numbers

    // 2. Print top border. Since each column is 3 chars wide, the border needs to be longer.
    cout << "    +" << string(grid_size * 3 + 1, '-') << "\n";

    // 3. Print the rows
    for (int y = 0; y < grid_size; y++) {
        cout << setw(3) << y << " | "; 
        
        for (int x = 0; x < grid_size; x++) {
            char c;
            if (grid[y][x].is_bomb) {
                c = '*';
            }
            else {
                c = '0' + grid[y][x].count;
            }
            
            // CRITICAL: Since headers are 3 chars wide, the cells must be too!
            // We print the character, plus TWO spaces.
            cout << c << "  "; 
        }
        cout << "|\n"; 
    }
    
    // 4. Print bottom border
    cout << "    +" << string(grid_size * 3 + 1, '-') << "\n";
}

void print_user(const vector<vector<Cell>> &grid) {
    // 1. Offset for row headers (6 spaces to match " 10 | ")
    cout << "      "; 
    for (int x = 0; x < grid_size; x++) {
        // Force every column header to be exactly 3 characters wide ("1  ", "10 ")
        cout << left << setw(3) << x;
    }
    cout << right << "\n"; // Reset to right-alignment for the row numbers

    // 2. Print top border. Since each column is 3 chars wide, the border needs to be longer.
    cout << "    +" << string(grid_size * 3 + 1, '-') << "\n";

    // 3. Print the rows
    for (int y = 0; y < grid_size; y++) {
        cout << setw(3) << y << " | "; 
        
        for (int x = 0; x < grid_size; x++) {
            char c;
            string color = "\033[0m"; // Default terminal color (Reset)

            if (grid[y][x].is_revealed && grid[y][x].is_bomb) {
                c = 'B';
                color = "\033[1;31m"; // Bold Red for Bombs
            }
            else if (grid[y][x].is_revealed && !grid[y][x].is_bomb) {
                c = '0' + grid[y][x].count;
                
                // Classic Minesweeper Number Colors
                if (c == '0')      color = "\033[90m"; // Dark Grey (fades into background)
                else if (c == '1') color = "\033[94m"; // Light Blue
                else if (c == '2') color = "\033[92m"; // Light Green
                else if (c == '3') color = "\033[91m"; // Light Red
                else if (c == '4') color = "\033[95m"; // Light Purple
                else if (c >= '5') color = "\033[93m"; // Light Yellow for high numbers
            }
            else {
                c = 'X';
                color = "\033[37m"; // Standard White/Light Grey for unrevealed
            }
            
            // Print the color, the character, reset the color, and THEN print the two spaces.
            // This ensures our formatting never accidentally bleeds into the borders.
            cout << color << c << "\033[0m" << "  "; 
        }
        cout << "|\n"; 
    }
    
    // 4. Print bottom border
    cout << "    +" << string(grid_size * 3 + 1, '-') << "\n";
}

struct Tuple {
    int first;
    int second;
    Tuple(int f, int s) : first(f), second(s) {}
};

void flood_fill(vector<vector<Cell>> &grid, int x, int y, int &rvld_cnt, vector<Tuple> &flood_coords) {
    if (x >= 0 && x < grid_size && y >= 0 && y < grid_size && grid[y][x].count == 0 && !grid[y][x].is_revealed) {
        // Reveal the cell
        grid[y][x].is_revealed = true;
        rvld_cnt++;

        // Push the coords in the list in order to get their neighbors after the dfs ends
        flood_coords.push_back(Tuple(x, y));

        // DFS
        flood_fill(grid, x - 1, y, rvld_cnt, flood_coords);
        flood_fill(grid, x + 1, y, rvld_cnt, flood_coords);
        flood_fill(grid, x, y - 1, rvld_cnt, flood_coords);
        flood_fill(grid, x, y + 1, rvld_cnt, flood_coords);
    }
}

int main() {
    vector<vector<Cell>> grid(grid_size, vector<Cell>(grid_size));

    // Place the bombs
    for (int i = 0; i < num_bombs; i++) {
        while (1) {
            int x = experimental::randint(0, grid_size - 1);
            int y = experimental::randint(0, grid_size - 1);
            if (grid[y][x].is_bomb == false) {
                grid[y][x].is_bomb = true;
                break;
            }
        }
    }

    //// Place the bombs manually
    //vector<vector<bool>> bomb_placements = {
        //{0, 0, 0, 0, 0, 1, 0, 0},
        //{1, 1, 0, 0, 0, 1, 0, 0},
        //{0, 0, 0, 0, 0, 0, 1, 0},
        //{0, 0, 1, 0, 0, 0, 0, 0},
        //{0, 0, 0, 0, 1, 0, 0, 1},
        //{0, 0, 0, 1, 0, 0, 0, 0},
        //{0, 0, 0, 0, 0, 0, 0, 0},
        //{1, 0, 0, 0, 0, 0, 0, 0},
    //};
    //for (int y = 0; y < grid_size; y++) {
        //for (int x = 0; x < grid_size; x++) {
            //grid[y][x].is_bomb = bomb_placements[y][x];
        //}
    //}

    // Count the bomb neighbors for each cell
    for (int y = 0; y < grid_size; y++) {
        for (int x = 0; x < grid_size; x++) {
            int cnt = 0;
            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    if (dx == 0 && dy == 0) continue;
                    if (x + dx >= 0 && x + dx < grid_size && y + dy >= 0 && y + dy < grid_size) {
                        if (grid[y+dy][x+dx].is_bomb) cnt++;
                    }
                }
            }
            grid[y][x].count = cnt;
        }
    }

    // Start Playing!
    int p_x, p_y;
    int revealed_cnt = 0;
    string msg;
    bool have_msg = false;
    while (1) {
        system("clear");

        //print_mod(grid);
        print_user(grid);
        cout << "\n" << "You have revealed " << revealed_cnt << " out of the " << grid_size*grid_size << " cells!\n" << endl;

        // User input
        if (have_msg) {
            cout << msg << endl;
            have_msg = false;
        }
        cout << "Play (x, y): ";
        if (!(cin >> p_x >> p_y)) {
            // Clear the error flag so we can use cin again
            cin.clear();
            // Discard everything currently in the input buffer
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        if (p_x < 0 || p_x >= grid_size || p_y < 0 || p_y >= grid_size) {
            msg = std::format("The number should be in the range [0 - {}]", grid_size - 1);
            have_msg = true;
            continue;
        }
        if (grid[p_y][p_x].is_revealed) {
            msg = "The chosen cell should not be already revealed";
            have_msg = true;
            continue;
        }
        else if (grid[p_y][p_x].is_bomb) {
            for (int y = 0; y < grid_size; y++) {
                for (int x = 0; x < grid_size; x++) {
                    grid[y][x].is_revealed = true;
                }
            }
            system("clear");
            print_user(grid);

            cout << "\nBOOM! You clicked on a bomb!" << endl;
            break;
        }
        else if (grid[p_y][p_x].count > 0) {
            grid[p_y][p_x].is_revealed = true;
            revealed_cnt++;
        }
        else {
            // flood-filling algorithm
            vector<Tuple> flood_coords;
            flood_fill(grid, p_x, p_y, revealed_cnt, flood_coords);
            for (const auto [f_x, f_y] : flood_coords) {
                // Reveal the neighbors of the cell and the cell itself
                for (int dy = -1; dy <= 1; dy++) {
                    for (int dx = -1; dx <= 1; dx++) {
                        if (f_x + dx >= 0 && f_x + dx < grid_size && f_y + dy >= 0 && f_y + dy < grid_size) {
                            if (!grid[f_y+dy][f_x+dx].is_revealed) revealed_cnt++;
                            grid[f_y+dy][f_x+dx].is_revealed = true;
                        }
                    }
                }
            }
        }

        // Check for win
        if (grid_size * grid_size - revealed_cnt == num_bombs) {
            for (int y = 0; y < grid_size; y++) {
                for (int x = 0; x < grid_size; x++) {
                    grid[y][x].is_revealed = true;
                }
            }
            system("clear");
            print_user(grid);
            cout << "YOU WON! CONGRATULATIONS!" << endl;
            break;
        }

    }

    return 0;
}
