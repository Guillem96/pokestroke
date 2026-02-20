#include "pkmn_sprite_sheet.h"
#include <stdlib.h>

void PkmnSpriteSheetLoad(PokemonSpriteSheet *sheet, const char *filename)
{
    TraceLog(LOG_INFO, "Loading Pokemon Sprite Sheet: %s", filename);
    sheet->texture = LoadTexture(filename);
    TraceLog(LOG_INFO, "Loaded Pokemon Sprite Sheet: %s (id: %d, width: %d, height: %d)", filename, sheet->texture.id, sheet->texture.width, sheet->texture.height);

    if (sheet->texture.id == 0)
    {
        TraceLog(LOG_ERROR, "Texture not loaded!");
        exit(1);
    }
}

void PkmnSpriteSheetDraw(PokemonSpriteSheet *sheet, int pokemon, unsigned short variant, Vector2 pos)
{
    PkmnSpriteSheetDrawScale(sheet, pokemon, variant, pos, 1.0f);
}

void PkmnSpriteSheetDrawScale(PokemonSpriteSheet *sheet, int pokemon, unsigned short variant, Vector2 pos, float scale)
{
    int totalSprites = (sheet->texture.width / PKMN_WIDTH) * (sheet->texture.height / PKMN_HEIGHT);

    int row = pokemon / PKMN_PER_ROW;
    int col = pokemon % PKMN_PER_ROW;

    TraceLog(LOG_DEBUG, "Drawing pokemon %d (row: %d, col: %d) variant %d from texture %dx%d", pokemon, row, col, variant, sheet->texture.width, sheet->texture.height);

    Rectangle sourceRect = {
        col * (PKMN_WIDTH + 1) * PKMN_VARIANTS + 1 + variant * (PKMN_WIDTH + 1),
        row * (PKMN_ROW_HEIGHT_PX + 1) + PKMN_ROW_INITIAL_OFFSET_PX,
        PKMN_WIDTH, PKMN_HEIGHT};

    int newWidth = PKMN_WIDTH * scale;
    int newHeight = PKMN_HEIGHT * scale;
    int x = pos.x + (float)(PKMN_WIDTH - newWidth) / 2.f;
    int y = pos.y + PKMN_HEIGHT - newHeight;

    Rectangle destRec = {x, y, newWidth, newHeight};
    DrawTexturePro(sheet->texture, sourceRect, destRec, (Vector2){0, 0}, 0.0f, WHITE);
}

void PkmnSpriteSheetUnload(PokemonSpriteSheet *sheet)
{
    UnloadTexture(sheet->texture);
}
