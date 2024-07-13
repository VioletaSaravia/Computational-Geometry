#pragma once
#include <random>

#include "engine.hpp"

enum TileRotation { UP = 0, RIGHT = 1, DOWN = 2, LEFT = 3 };

class TileEditor : public Scene {
   public:
    PixelCamera2D camera;

   private:
    // GUI
    void ExportBtn() {};
    void SaveBtn() {};
    void NewBtn() {};
    void ExitBtn() {
        CloseWindow();
        abort();
    };

    i32         charsetScroll = 0;
    const char *CharsetText   = "Charset";
    const char *ExportBtnText = "Export";
    const char *SaveBtnText   = "Save";
    const char *NewBtnText    = "New";
    const char *ExitBtnText   = "Exit";

    v2 anchor01 = {30, 100};
    v2 anchor02 = {anchor01.x, anchor01.y + 480};

    bool  SelectedTileEditMode = false;
    Color ColorForeValue       = {255, 255, 255, 0};
    Color ColorBackValue       = {0, 0, 0, 0};

    enum layout {
        CHARSET   = 0,
        EXPORT    = 1,
        SPINNER   = 2,
        COLORFORE = 3,
        COLORBACK = 4,
        SAVE      = 5,
        NEW       = 6,
        EXIT      = 7,
        EDITOR    = 8
    };
    Rectangle layoutRecs[9] = {
        {anchor01.x + 0, anchor01.y + 0, 128, 324},
        {anchor01.x + 0, anchor01.y + -40, 128, 24},
        {anchor01.x + 0, anchor01.y + 330, 128, 24},
        {anchor01.x + 0, anchor01.y + 370, 104, 48},
        {anchor01.x + 0, anchor01.y + 434, 104, 48},
        {anchor01.x + 0, anchor01.y + -72, 128, 24},
        {anchor02.x + 0, anchor02.y + 8, 56, 32},
        {anchor02.x + 72, anchor02.y + 8, 56, 32},
        {anchor01.x + 150, anchor01.y - 70, 800, 600},  // Editor window
    };

    // -----

    Texture tileset;
    f32     tileSize;
    u32     tilesetSize;

    i32          selected         = 0;
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
        if (vec2::IsInRectangle(GetMousePosition(), layoutRecs[CHARSET])) {
            if (GetMouseWheelMove() < 0)
                charsetScroll++;
            if (GetMouseWheelMove() > 0 && charsetScroll > 0)
                charsetScroll--;

            printf("%u\n", charsetScroll);
        }

        if (IsKeyPressed(KEY_K)) {
            selectedRotation = TileRotation((selectedRotation + 1) % 4);
        }

        for (size_t i = 0;
             // 1) Draw all tiles...
             (i < tilesetSize * tilesetSize) !=
             // 2) ...xor up to the height of the tile selector square
             ((i32)(i / (layoutRecs[CHARSET].width / tileSize - 2)) * tileSize + tileSize -
                  tileSize * charsetScroll >
              layoutRecs[CHARSET].height - 10);
             i++) {
            drawTile(
                i,
                v2{layoutRecs[CHARSET].x, layoutRecs[CHARSET].y} +
                    v2{(i % (i32)(layoutRecs[CHARSET].width / tileSize - 2)) * tileSize + tileSize,
                       (i32)(i / (layoutRecs[CHARSET].width / tileSize - 2)) * tileSize + tileSize -
                           tileSize * charsetScroll});
        }

        // Selected tile
        DrawRectangleLines(
            layoutRecs[CHARSET].x + tileSize +
                tileSize * (selected % (i32)(layoutRecs[CHARSET].width / tileSize - 2)),
            layoutRecs[CHARSET].y + -tileSize * charsetScroll + tileSize +
                tileSize * (i32)(selected / (layoutRecs[CHARSET].width / tileSize - 2)),
            tileSize,
            tileSize,
            RED);

        // Look. This is a white rectangle on top of the tiles,
        // so that the ones above the box don't render.
        // I'm just tired.
        DrawRectangle(layoutRecs[CHARSET].x,
                      layoutRecs[CHARSET].y - 500,
                      layoutRecs[CHARSET].width,
                      layoutRecs[CHARSET].height + 185,
                      RAYWHITE);

        if (IsKeyPressed(KEY_COMMA)) {
            selected = (selected - 1) % (tilesetSize * tilesetSize);
        }

        if (IsKeyPressed(KEY_PERIOD)) {
            selected = (selected + 1) % (tilesetSize * tilesetSize);
        }
    }

    void saveAsImage() {}  // TODO Not easily possible?

    void saveSerialized(const char *filename) {
        std::cout << "INFO: ENGINE: Saving asset as tilemap...\n";

        auto          outpath = std::format("../assets/testing/{}.tm", filename);
        std::ofstream outFile(outpath);
        if (!outFile) {
            std::cout << "ERROR: ENGINE: Error opening file for writing\n";
            return;
        }

        for (i32 y = 0; y < (i32)(tilemap.size / tilemap.stride); y++) {
            for (i32 x = 0; x < (i32)tilemap.stride; x++) {
                outFile << tilemap[v2i{x, y}] << ' ';
            }
            outFile << '\n';
        }

        outFile.close();
        if (outFile.fail()) {
            std::cout << "ERROR: ENGINE: Error writing to file\n";
        }

        std::cout << "INFO: ENGINE: Saved asset to " << outpath << "\n";
    }

    void saveUI() {
        static bool  saving   = false;
        static char *saveName = (char *)malloc(20);

        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_S)) {
            saving = true;
        }

        if (saving && GuiTextBox(Rectangle{200, 200, 200, 20}, saveName, 20, true)) {
            saveSerialized(saveName);
            saving = false;
        }
    }

   public:
    TileEditor(const char *uri, f32 _tileSize, u32 _tilesetSize, v2 dimensions)
        : camera(),
          tileset{LoadTexture(uri)},
          tileSize{_tileSize},
          tilesetSize{_tilesetSize},
          tilemap{Array<u32>((dimensions.y + 1) * (dimensions.x + 1), 0, nullptr)},
          tileRotation{Array<TileRotation>(
              (dimensions.y + 1) * (dimensions.x + 1), TileRotation::UP, nullptr)} {
        tilemap.stride      = (dimensions.x + 1);
        tileRotation.stride = (dimensions.x + 1);
    }

    void Draw2D() final {
        // if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        //     placeTile();
        // }

        // drawMap();
        drawSelected();

        // static bool showGrid = false;
        // if (IsKeyPressed(KEY_G)) {
        //     showGrid = !showGrid;
        // }
        // if (showGrid) {
        //     drawGrid();
        // }
    }

    void DrawUI() final {
        saveUI();

        drawTileSelector();
        GuiGroupBox(layoutRecs[CHARSET], CharsetText);

        if (GuiButton(layoutRecs[EXPORT], ExportBtnText)) {
            ExportBtn();
        }

        if (IsKeyPressed(KEY_LEFT))
            selected--;
        if (IsKeyPressed(KEY_RIGHT))
            selected++;
        if (GuiSpinner(layoutRecs[SPINNER], NULL, &selected, 0, 100, SelectedTileEditMode)) {
            SelectedTileEditMode = !SelectedTileEditMode;
        }
        GuiColorPicker(layoutRecs[COLORFORE], NULL, &ColorForeValue);
        GuiColorPicker(layoutRecs[COLORBACK], NULL, &ColorBackValue);
        if (GuiButton(layoutRecs[SAVE], SaveBtnText)) {
            SaveBtn();
        }
        if (GuiButton(layoutRecs[NEW], NewBtnText)) {
            NewBtn();
        }
        if (GuiButton(layoutRecs[EXIT], ExitBtnText)) {
            ExitBtn();
        }
        GuiPanel(layoutRecs[EDITOR], "");
    }

    void Compute() final { camera.Update(); }
};

class MainMenu : public Scene {
   private:
    void NewBtn() {};
    void OpenBtn() {};
    void ExitBtn() {
        CloseWindow();
        abort();
    };

    const char *TitleLabelText   = "Motley v0.1";
    const char *CreditsLabelText = "By Violeta Saravia - @violeta_xyz";
    const char *NewBtnText       = "New Project";
    const char *OpenBtnText      = "Open Project";
    const char *ExitBtnText      = "Exit";

    Vector2 anchor01 = {screenWidth * 0.5f - 40, screenHeight * 0.33f};

    Rectangle layoutRecs[5] = {
        {anchor01.x + -8, anchor01.y + 40, 120, 24},
        {anchor01.x + -64, anchor01.y + 240, 184, 24},
        {anchor01.x + -40, anchor01.y + 88, 120, 24},
        {anchor01.x + -40, anchor01.y + 136, 120, 24},
        {anchor01.x + -40, anchor01.y + 184, 120, 24},
    };

   public:
    MainMenu() {}

    void Draw2D() final {}

    void DrawUI() final {
        GuiLabel(layoutRecs[0], TitleLabelText);
        GuiLabel(layoutRecs[1], CreditsLabelText);
        if (GuiButton(layoutRecs[2], NewBtnText)) {
            NewBtn();
        }
        if (GuiButton(layoutRecs[3], OpenBtnText)) {
            OpenBtn();
        }
        if (GuiButton(layoutRecs[4], ExitBtnText)) {
            ExitBtn();
        }
    }
};