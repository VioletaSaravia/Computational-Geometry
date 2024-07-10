#pragma once
#include <random>

#include "engine.hpp"

enum TileRotation { UP = 0, RIGHT = 1, DOWN = 2, LEFT = 3 };

class TileEditor : public Scene {
   private:
    Texture tileset;
    f32     tileSize;
    u32     tilesetSize;

    u32          selected         = 0;
    TileRotation selectedRotation = UP;

    Array<u32>          tilemap;
    Array<TileRotation> tileRotation;

    void drawTile(u32 idx, v2 position, TileRotation rotation = UP) {
        v2 origin{};
        switch (rotation) {
            case UP:
                origin = {0, 0};
                break;

            case RIGHT:
                origin = {0, tileSize};
                break;

            case DOWN:
                origin = {tileSize, tileSize};
                break;

            case LEFT:
                origin = {tileSize, 0};
                break;

            default:
                break;
        }

        DrawTexturePro(tileset,
                       Rectangle{(f32)tileSize * (idx % tilesetSize),
                                 (f32)tileSize * (idx / tilesetSize),
                                 tileSize,
                                 tileSize},
                       Rectangle{position.x, position.y, (f32)tileSize, (f32)tileSize},
                       origin,
                       rotation * 90,
                       WHITE);
    }

    void drawGrid() {
        static f32 thickness = 0.1f;

        for (f32 i = 0; i < screenWidth && (i / tileSize) < tilemap.size; i += tileSize) {
            DrawLineEx(v2{i, 0}, v2{i, f32(screenHeight)}, thickness, GRAY);
        }

        for (f32 i = 0; i < screenHeight && (i / tileSize) < tilemap.stride; i += tileSize) {
            DrawLineEx(v2{0, i}, v2{f32(screenWidth), i}, thickness, GRAY);
        }
    }

    void drawSelected() {
        v2 mousePosInCanvas = camera.Reproject(GetMousePosition());
        drawTile(selected,
                 v2{(f32)((u32)mousePosInCanvas.x - (u32)mousePosInCanvas.x % (u32)tileSize),
                    (f32)((u32)mousePosInCanvas.y - (u32)mousePosInCanvas.y % (u32)tileSize)},
                 selectedRotation);
    }

    void drawMap() {
        for (i32 y = 0; y < (i32)(tilemap.size / tilemap.stride - 1); y++) {
            for (i32 x = 0; x < (i32)tilemap.stride; x++) {
                drawTile(tilemap[{x, y}], v2{x * tileSize, y * tileSize}, tileRotation[{x, y}]);
            }
        }
    }

    void placeTile() {
        v2 mousePosInCanvas = camera.Reproject(GetMousePosition());

        tilemap[v2i{(i32)(mousePosInCanvas.x / tileSize), (i32)(mousePosInCanvas.y / tileSize)}] =
            selected;
        tileRotation[v2i{(i32)(mousePosInCanvas.x / tileSize),
                         (i32)(mousePosInCanvas.y / tileSize)}] = selectedRotation;
    }

    void drawTileSelector() {
        if (IsKeyPressed(KEY_K)) {
            selectedRotation = TileRotation((selectedRotation + 1) % 4);
        }

        v2 hook{(f32)GetScreenWidth() / 2 - 130, 0.8f * GetScreenHeight()};
        GuiDrawRectangle(Rectangle{hook.x, hook.y, 260, 50}, 0, BLACK, BLACK);
        GuiGroupBox(Rectangle{hook.x, hook.y, 200, 50}, "Characters");

        for (size_t i = 0; i < tilesetSize * tilesetSize; i++) {
            drawTile(i,
                     hook + v2{(i % (i32)(200 / tileSize - 2)) * tileSize + tileSize,
                               (i32)(i / (200 / tileSize - 2)) * tileSize + tileSize});
        }

        // Selected tile
        GuiGroupBox(Rectangle{hook.x + 210, hook.y, 50, 50}, "Sel");
        drawTile(selected, v2{hook.x + 210 + 20, hook.y + 20});

        if (IsKeyPressed(KEY_COMMA)) {
            selected = (selected - 1) % (tilesetSize * tilesetSize);
        }

        if (IsKeyPressed(KEY_PERIOD)) {
            selected = (selected + 1) % (tilesetSize * tilesetSize);
        }
    }

    void saveAsImage() {}  // TODO Not easily possible?

    void saveSerialized(const char* filename) {
        std::cout << "INFO: ENGINE: Saving asset as tilemap...\n";

        auto          outpath = std::format("../assets/testing/{}.tm", filename);
        std::ofstream outFile(outpath);
        if (!outFile) {
            std::cout << "ERROR: ENGINE: Error opening file for writing\n";
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
            std::cout << "ERROR: ENGINE: Error writing to file\n";
        }

        std::cout << "INFO: ENGINE: Saved asset to " << outpath << "\n";
    }

    void saveUI() {
        static bool  saving   = false;
        static char* saveName = (char*)malloc(20);

        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_S)) {
            saving = true;
        }

        if (saving && GuiTextBox(Rectangle{200, 200, 200, 20}, saveName, 20, true)) {
            saveSerialized(saveName);
            saving = false;
        }
    }

   public:
    TileEditor(const char* uri, f32 _tileSize, u32 _tilesetSize, v2 dimensions)
        : tileset{LoadTexture(uri)},
          tileSize{_tileSize},
          tilesetSize{_tilesetSize},
          tilemap{Array<u32>((dimensions.y + 1) * (dimensions.x + 1), 0, nullptr)},
          tileRotation{Array<TileRotation>(
              (dimensions.y + 1) * (dimensions.x + 1), TileRotation::UP, nullptr)} {
        tilemap.stride      = (dimensions.x + 1);
        tileRotation.stride = (dimensions.x + 1);
    }

    void Draw2D() final {
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            placeTile();
        }

        drawMap();
        drawSelected();

        static bool showGrid = false;
        if (IsKeyPressed(KEY_G)) {
            showGrid = !showGrid;
        }
        if (showGrid) {
            drawGrid();
        }
    }

    void DrawUI() final {
        saveUI();
        drawTileSelector();
    }
};

class TestTileScene : public Scene {
   public:
    TestTileScene() {}

    void Draw2D() final {}

    void DrawUI() final {}
};