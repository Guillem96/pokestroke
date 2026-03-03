#include "pkmn_sprite_sheet.h"
#include <stdlib.h>

void PkmnSpriteSheetInit(PokemonSpriteSheet *sheet, const char *filename)
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
    PkmnSpriteSheetDrawPro(sheet, pokemon, variant, pos, 1.0f, false, WHITE);
}

void PkmnSpriteSheetDrawPro(PokemonSpriteSheet *sheet, int pokemon, unsigned short variant, Vector2 pos, float scale, bool anchorToBottomCenter, Color tint)
{
    int totalSprites = (sheet->texture.width / PKMN_WIDTH) * (sheet->texture.height / PKMN_HEIGHT);

    int row = pokemon / PKMN_PER_ROW;
    int col = pokemon % PKMN_PER_ROW;
    Rectangle sourceRect = {
        col * (PKMN_WIDTH + 1) * PKMN_VARIANTS + 1 + variant * (PKMN_WIDTH + 1),
        row * (PKMN_ROW_HEIGHT_PX + 1) + PKMN_ROW_INITIAL_OFFSET_PX,
        PKMN_WIDTH, PKMN_HEIGHT};

    int newWidth = PKMN_WIDTH * scale;
    int newHeight = PKMN_HEIGHT * scale;
    int x = pos.x;
    int y = pos.y;
    if (anchorToBottomCenter)
    {
        x += (float)(PKMN_WIDTH - newWidth) / 2.f;
        y += (float)(PKMN_HEIGHT - newHeight);
    }

    Rectangle destRec = {x, y, newWidth, newHeight};
    DrawTexturePro(sheet->texture, sourceRect, destRec, (Vector2){0, 0}, 0.0f, tint);
}

void PkmnSpriteSheetUnload(PokemonSpriteSheet *sheet)
{
    UnloadTexture(sheet->texture);
}
