#include "algorithm.h"
#include <string.h>
enum CellState {
    Dead,
    Alive,
};

// TODO Cellular Automata GDExtension plugin
// custom number of states, history, rules, etc etc.
// Inherits from tilemap?
struct ConwayBoard {
    // TODO two copies of board for data-oriented?
    // And if I add a time dimension to determine the tile to use? What's more data-oriented then?
    CellState board[64][64];

    ConwayBoard(Array<v2> initialLiving) /*/: board{}*/ {
        memset(board, CellState::Dead, sizeof(CellState) * 64 * 64);

        for (usize i = 0; i < initialLiving.count; i++)
            board[(usize)initialLiving[i].x][(usize)initialLiving[i].y] = CellState::Alive;
    }

    void Update() {
        for (usize i = 1; i < 63; i++)
            for (usize j = 1; j < 63; j++) {
                usize near{};

                for (usize k = i - 1; k < i + 2; k++)
                    for (usize f = j - 1; f < j + 2; f++)
                        near += board[k][f] == CellState::Alive ? 1 : 0;

                // RULES HERE
                switch (near) {
                    case 0:
                        break;

                    case 1:
                        break;

                    case 2:
                        break;

                    case 3:
                        break;

                    default:
                        break;
                }
            }
    }

    void Draw() {}
};