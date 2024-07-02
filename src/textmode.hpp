#pragma once
#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include <random>

#include "engine.hpp"

class TileEditor : public Scene {
   private:
    Texture tileset;
    f32     tileSize;
    u32     tilesetSize;
    u32     tileSelected = 13;
    // Arena             tilemapArena;
    Array<Array<u32>> tilemap;

    void drawTile(u32 idx, v2 position) {
        DrawTextureRec(tileset,
                       Rectangle{(f32)tileSize * (idx - 1 % tilesetSize),
                                 (f32)tileSize * (idx - 1 / tilesetSize),
                                 (f32)tileSize,
                                 (f32)tileSize},
                       position,
                       WHITE);
    }

    void drawGrid() {
        static f32 thickness = 0.25f;

        for (f32 i = 0; i < screenWidth && (i / tileSize) < tilemap.size; i += tileSize) {
            DrawLineEx(v2{i, 0}, v2{i, f32(screenHeight)}, thickness, GRAY);
        }

        for (f32 i = 0; i < screenHeight && (i / tileSize) < tilemap[0].size; i += tileSize) {
            DrawLineEx(v2{0, i}, v2{f32(screenWidth), i}, thickness, GRAY);
        }
    }

    void drawSelected() {
        v2 mousePos = GetMousePosition();
        drawTile(tileSelected,
                 v2{mousePos.x - (u32)mousePos.x % (u32)tileSize,
                    mousePos.y - (u32)mousePos.y % (u32)tileSize});
    }

    void drawMap() {
        for (usize x = 0; x < tilemap.size; x++) {
            for (usize y = 0; y < tilemap[x].size; y++) {
                if (tilemap[x][y] != 0) {
                    drawTile(tilemap[x][y], v2{x * tileSize, y * tileSize});
                }
            }
        }
    }

    void placeTile() {
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            v2 mousePos = GetMousePosition();

            tilemap[mousePos.x / tileSize][mousePos.y / tileSize] = tileSelected;
            printf_s(
                "Setting %.0f %.0f to %i because mousePos is %.0f, %.0f and tilesize is %.0f\n",
                mousePos.x / tileSize,
                mousePos.y / tileSize,
                tileSelected,
                mousePos.x,
                mousePos.y,
                tileSize);
        }
    }

    void saveAsImage() {}  // TODO Not easily possible?

    void saveAsJSON() {
        // TODO Meh. Maybe just save serialized data?
        for (usize x = 0; x < tilemap.size; x++) {
            for (usize y = 0; y < tilemap[x].size; y++) {
            }
        }
    }

   public:
    TileEditor(const char* uri, f32 _tileSize, u32 _tilesetSize)
        : tileset{LoadTexture(uri)},
          tileSize{_tileSize},
          tilesetSize{_tilesetSize},
          tilemap{Array<Array<u32>>(GetScreenWidth() / tileSize + 1,
                                    Array<u32>(GetScreenHeight() / tileSize + 1, 0, nullptr),
                                    nullptr)} {
        printf_s("DIM 1: %i, DIM 2: %i\n", tilemap.count, tilemap[0].count);
        tilemap[30][30] = 4;
    }

    void Draw2D() final {}

    void DrawUI() final {
        placeTile();
        drawGrid();
        drawSelected();

        // DrawTextureV(tileset, v2{300, 100}, WHITE);
        // DrawTextureRec(tileset, Rectangle{0, 0, 8, 8}, v2{100, 100}, WHITE);
        for (u32 i = 1; i < 81; i++) {
            drawTile(i, v2{96.0f + i * tileSize, 96});
        }

        drawMap();
        saveAsImage();
    }
};