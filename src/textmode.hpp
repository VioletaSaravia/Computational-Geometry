#pragma once
#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include <random>

#include "engine.hpp"

class TileEditor : public Scene {
   private:
    Texture    tileset;
    f32        tileSize;
    u32        tilesetSize;
    u32        tileSelected = 0;
    Array<u32> tilemap;

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

        for (f32 i = 0; i < screenHeight && (i / tileSize) < tilemap.stride; i += tileSize) {
            DrawLineEx(v2{0, i}, v2{f32(screenWidth), i}, thickness, GRAY);
        }
    }

    void drawSelected() {
        v2i mousePos{GetScreenWidth() < GetMouseX() ? GetScreenWidth() - 1 : GetMouseX(),
                     GetScreenHeight() < GetMouseY() ? GetScreenHeight() - 1 : GetMouseY()};
        drawTile(tileSelected,
                 v2{(f32)mousePos.x - (u32)mousePos.x % (u32)tileSize,
                    (f32)mousePos.y - (u32)mousePos.y % (u32)tileSize});
    }

    void drawMap() {
        for (i32 y = 0; y < (i32)(tilemap.size / tilemap.stride - 1); y++) {
            for (i32 x = 0; x < (i32)tilemap.stride; x++) {
                drawTile(tilemap[{x, y}], v2{x * tileSize, y * tileSize});
            }
        }
    }

    void placeTile() {
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            v2i mousePos{GetScreenWidth() < GetMouseX() ? GetScreenWidth() - 1 : GetMouseX(),
                         GetScreenHeight() < GetMouseY() ? GetScreenHeight() - 1 : GetMouseY()};

            tilemap[v2i{(i32)(mousePos.x / tileSize), (i32)(mousePos.y / tileSize)}] = tileSelected;
        }
    }

    void drawTileSelector() {
        static v2 hook{(f32)GetScreenWidth() / 2 - 130, 0.8f * GetScreenHeight()};
        GuiGroupBox(Rectangle{hook.x, hook.y, 200, 50}, "Characters");
        GuiGroupBox(Rectangle{hook.x + 210, hook.y, 50, 50}, "Sel");

        for (size_t i = 0; i < tilesetSize; i++) {
            drawTile(i,
                     hook + v2{(i % (i32)(200 / tileSize - 2)) * tileSize + tileSize,
                               (i32)(i / (200 / tileSize - 2)) * tileSize + tileSize});
        }

        drawTile(tileSelected, v2{hook.x + 210 + 20, hook.y + 20});

        if (IsKeyPressed(KEY_LEFT)) {
            tileSelected = (tileSelected - 1) % tilesetSize;
        }

        if (IsKeyPressed(KEY_RIGHT)) {
            tileSelected = (tileSelected + 1) % tilesetSize;
        }
    }

    void saveAsImage() {}  // TODO Not easily possible?

    void saveSerialized() {
        std::ofstream outFile("../test.tm");
        if (!outFile) {
            std::cerr << "Error opening file for writing\n";
            return;
        }

        for (i32 y = 0; y < (i32)(tilemap.size / tilemap.stride); y++) {
            for (i32 x = 0; x < (i32)tilemap.stride; x++) {
                outFile << tilemap[v2i{x, y}] << " ";
            }
            outFile << "\n";
        }

        outFile.close();
        if (outFile.fail()) {
            std::cerr << "Error writing to file\n";
        }
    }

   public:
    TileEditor(const char* uri, f32 _tileSize, u32 _tilesetSize)
        : tileset{LoadTexture(uri)},
          tileSize{_tileSize},
          tilesetSize{_tilesetSize},
          tilemap{Array<u32>(
              (GetScreenHeight() / tileSize + 1) * (GetScreenWidth() / tileSize + 1), 0, nullptr)} {
        tilemap.stride = (GetScreenWidth() / tileSize + 1);

        tilemap[v2i{0, 30}]  = 255;
        tilemap[v2i{10, 20}] = 244;
        printf_s("DIM 1: %i, DIM 2: %i\n", tilemap.count / tilemap.stride, tilemap.stride);
    }

    void Draw2D() final {}

    void DrawUI() final {
        placeTile();

        // DrawTextureV(tileset, v2{300, 100}, WHITE);
        // DrawTextureRec(tileset, Rectangle{0, 0, 8, 8}, v2{100, 100}, WHITE);
        for (u32 i = 1; i < 81; i++) {
            drawTile(i, v2{96.0f + i * tileSize, 96});
        }

        drawMap();
        drawSelected();

        drawGrid();

        drawTileSelector();
        saveAsImage();
    }
};