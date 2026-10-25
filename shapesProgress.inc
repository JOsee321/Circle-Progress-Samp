#if defined _SHAPES_PROGRESS_INCLUDED
    #endinput
#endif
#define _SHAPES_PROGRESS_INCLUDED

/*
    ============================================================================
    * GTA:SA-MP / open.mp Perfect Shapes & Circle Progress Bar
    * 
    * Author: JOsee321
    * Features:
    *   - 16:9 Widescreen aspect ratio correction (100% round circles)
    *   - Pixel-perfect uniform stroke thickness across all angles
    *   - Complete 360-degree closure with zero gaps
    *   - Full support for Circle, Hexagon, Diamond, Square, Triangle, Parens, Arc
    *   - Auto-cleanup on player disconnect
    ============================================================================
*/

// ============================================================================
// Configuration & Constants
// ============================================================================

#if !defined CIRCLE_ASPECT_RATIO_Y
    // 4:3 canvas to 16:9 widescreen correction: (640/480)/(16/9) = 4/3 ~ 1.333333
    // Set to 1.0 if running on 4:3 monitors.
    #define CIRCLE_ASPECT_RATIO_Y       (1.333333)
#endif

#if !defined DEFAULT_CIRCLE_POLYGONS
    #define DEFAULT_CIRCLE_POLYGONS     (3.0)   // High quality 120-point circle by default
#endif

#define MAX_CIRCLES_POLYGONS            (120)
#define MAX_CIRCLES_DRAW                (10)

// ============================================================================
// Dependencies
// ============================================================================

#include <YSI_Coding\y_hooks>
#include <YSI_Data\y_iterate>

// ============================================================================
// Internal Data Structures
// ============================================================================

enum E_SHAPE_TYPE {
    SHAPE_CIRCLE = 0,
    SHAPE_HEXAGON,
    SHAPE_DIAMOND,
    SHAPE_SQUARE,
    SHAPE_TRIANGLE,
    SHAPE_PARENS,
    SHAPE_ARC
};

static enum E_CIRCLE_PROGRESS {
    E_SHAPE_TYPE:E_PROG_TYPE,
    Float:E_PROG_POS_X,
    Float:E_PROG_POS_Y,
    Float:E_PROG_SIZE,
    Float:E_PROG_THICKNESS,
    Float:E_PROG_POLYGONS,
    Float:E_PROG_MAX_VALUE,
    Float:E_PROG_VALUE,
    E_PROG_COLOR,
    E_PROG_BACKGROUND,
    E_PROG_COUNT,
    bool:E_PROG_INVERT,
    bool:E_PROG_CREATED[MAX_CIRCLES_POLYGONS],
    PlayerText:E_PROG_POINTS[MAX_CIRCLES_POLYGONS]
};

static
    Iterator:Circles[MAX_PLAYERS]<MAX_CIRCLES_DRAW>,
    e_CircleDraw[MAX_PLAYERS][MAX_CIRCLES_DRAW][E_CIRCLE_PROGRESS];

// Empty prototype for clean resets
static const e_EmptyCircleProgress[E_CIRCLE_PROGRESS];

// ============================================================================
// Internal Helpers
// ============================================================================

/*
    Calculates 2D textdraw coordinates on an ellipse/circle
    with 16:9 widescreen aspect ratio correction.
*/
stock ctd(Float:angle, Float:distance, Float:sx, Float:sy, &Float:x, &Float:y) {
    x = sx + (distance * floatsin(-angle, degrees));
    y = sy + ((distance * CIRCLE_ASPECT_RATIO_Y) * floatcos(angle, degrees));
    return true;
}

/*
    Renders or updates points for any shape in a unified, optimized pipeline.
*/
static stock _Progress_RenderPoints(playerid, id, Float:coords[][2], total_points, value) {
    if(total_points <= 0) return false;
    if(total_points > MAX_CIRCLES_POLYGONS) total_points = MAX_CIRCLES_POLYGONS;

    new maxvalue = floatround(e_CircleDraw[playerid][id][E_PROG_MAX_VALUE]);
    if(maxvalue <= 0) maxvalue = 100;

    // Clamp value
    if(value < 0) value = 0;
    if(value > maxvalue) value = maxvalue;

    new percent = (total_points * value) / maxvalue;
    new Float:thick = e_CircleDraw[playerid][id][E_PROG_THICKNESS];
    new Float:offset_y = (thick * CIRCLE_ASPECT_RATIO_Y) * 0.65;
    new Float:letter_y = thick * CIRCLE_ASPECT_RATIO_Y;
    new active_color = e_CircleDraw[playerid][id][E_PROG_COLOR];
    new bg_color = e_CircleDraw[playerid][id][E_PROG_BACKGROUND];

    for(new i = 0; i < total_points; i++) {
        new Float:px = coords[i][0];
        new Float:py = coords[i][1] - offset_y;

        if(!e_CircleDraw[playerid][id][E_PROG_CREATED][i]) {
            e_CircleDraw[playerid][id][E_PROG_POINTS][i] = CreatePlayerTextDraw(playerid, px, py, ".");
            PlayerTextDrawAlignment(playerid, e_CircleDraw[playerid][id][E_PROG_POINTS][i], 2);
            PlayerTextDrawLetterSize(playerid, e_CircleDraw[playerid][id][E_PROG_POINTS][i], thick, letter_y);
            PlayerTextDrawSetShadow(playerid, e_CircleDraw[playerid][id][E_PROG_POINTS][i], 0);
            e_CircleDraw[playerid][id][E_PROG_CREATED][i] = true;
        }

        PlayerTextDrawColor(playerid, e_CircleDraw[playerid][id][E_PROG_POINTS][i], (percent <= i) ? bg_color : active_color);
        PlayerTextDrawShow(playerid, e_CircleDraw[playerid][id][E_PROG_POINTS][i]);
    }

    e_CircleDraw[playerid][id][E_PROG_VALUE] = float(value);
    e_CircleDraw[playerid][id][E_PROG_COUNT] = total_points;
    return true;
}

// ============================================================================
// Lifecycle Hooks
// ============================================================================

hook OnPlayerDisconnect(playerid, reason) {
    DestroyPlayerCircleProgressAll(playerid);
    return Y_HOOKS_CONTINUE_RETURN_1;
}

// ============================================================================
// Destruction Natives
// ============================================================================

stock DestroyPlayerCircleProgress(playerid, id) {
    if(!Iter_Contains(Circles[playerid], id)) 
        return false;

    for(new i = 0; i < e_CircleDraw[playerid][id][E_PROG_COUNT]; i++) {
        if(e_CircleDraw[playerid][id][E_PROG_CREATED][i]) {
            PlayerTextDrawDestroy(playerid, e_CircleDraw[playerid][id][E_PROG_POINTS][i]);
            e_CircleDraw[playerid][id][E_PROG_POINTS][i] = PlayerText:INVALID_TEXT_DRAW;
            e_CircleDraw[playerid][id][E_PROG_CREATED][i] = false;
        }
    }

    Iter_Remove(Circles[playerid], id);
    e_CircleDraw[playerid][id] = e_EmptyCircleProgress;
    return true;
}

stock DestroyPlayerCircleProgressAll(playerid) {
    foreach(new i: Circles[playerid])
        DestroyPlayerCircleProgress(playerid, i);
    return true;
}

stock DestroyPlayerHexagonProgress(playerid, id)    return DestroyPlayerCircleProgress(playerid, id);
stock DestroyPlayerHexagonProgressAll(playerid)     return DestroyPlayerCircleProgressAll(playerid);

stock DestroyPlayerDiamondProgress(playerid, id)    return DestroyPlayerCircleProgress(playerid, id);
stock DestroyPlayerDiamondProgressAll(playerid)     return DestroyPlayerCircleProgressAll(playerid);

stock DestroyPlayerSquareProgress(playerid, id)     return DestroyPlayerCircleProgress(playerid, id);
stock DestroyPlayerSquareProgressAll(playerid)      return DestroyPlayerCircleProgressAll(playerid);

stock DestroyPlayerTriangleProgress(playerid, id)   return DestroyPlayerCircleProgress(playerid, id);
stock DestroyPlayerTriangleProgressAll(playerid)    return DestroyPlayerCircleProgressAll(playerid);

stock DestroyPlayerParensProgress(playerid, id)     return DestroyPlayerCircleProgress(playerid, id);
stock DestroyPlayerParensProgressAll(playerid)      return DestroyPlayerCircleProgressAll(playerid);

stock DestroyPlayerArcProgress(playerid, id)        return DestroyPlayerCircleProgress(playerid, id);
stock DestroyPlayerArcProgressAll(playerid)         return DestroyPlayerCircleProgressAll(playerid);

// ============================================================================
// Circle Progress Bar
// ============================================================================

stock CreatePlayerCircleProgress(playerid, Float:pos_x, Float:pos_y, max_value = 100, color = 0xFF0000FF, background_COLOR = 0x000000FF, Float:size = 10.0, Float:thickness = 0.2, Float:polygons = DEFAULT_CIRCLE_POLYGONS) {
    new index = Iter_Free(Circles[playerid]);
    if(index == -1) return -1;

    e_CircleDraw[playerid][index][E_PROG_TYPE] = SHAPE_CIRCLE;
    e_CircleDraw[playerid][index][E_PROG_POS_X] = pos_x;
    e_CircleDraw[playerid][index][E_PROG_POS_Y] = pos_y;
    e_CircleDraw[playerid][index][E_PROG_SIZE] = size;
    e_CircleDraw[playerid][index][E_PROG_THICKNESS] = thickness;
    e_CircleDraw[playerid][index][E_PROG_MAX_VALUE] = float(max_value);
    e_CircleDraw[playerid][index][E_PROG_VALUE] = -1.0;
    e_CircleDraw[playerid][index][E_PROG_POLYGONS] = (polygons <= 0.0) ? DEFAULT_CIRCLE_POLYGONS : polygons;
    e_CircleDraw[playerid][index][E_PROG_COLOR] = color;
    e_CircleDraw[playerid][index][E_PROG_BACKGROUND] = background_COLOR;
    e_CircleDraw[playerid][index][E_PROG_INVERT] = false;

    Iter_Add(Circles[playerid], index);
    UpdatePlayerCircleProgress(playerid, index, max_value);
    return index;
}

stock UpdatePlayerCircleProgress(playerid, id, value) {
    if(!Iter_Contains(Circles[playerid], id)) return false;
    if(floatround(e_CircleDraw[playerid][id][E_PROG_VALUE]) == value) return true;

    new Float:step = e_CircleDraw[playerid][id][E_PROG_POLYGONS];
    if(step <= 0.0) step = DEFAULT_CIRCLE_POLYGONS;

    new Float:cx = e_CircleDraw[playerid][id][E_PROG_POS_X];
    new Float:cy = e_CircleDraw[playerid][id][E_PROG_POS_Y];
    new Float:sz = e_CircleDraw[playerid][id][E_PROG_SIZE];

    new Float:coords[MAX_CIRCLES_POLYGONS][2];
    new count = 0;

    for(new Float:v = 0.0; v < 360.0 && count < MAX_CIRCLES_POLYGONS; v += step) {
        ctd(v + 180.0, sz, cx, cy, coords[count][0], coords[count][1]);
        count++;
    }

    return _Progress_RenderPoints(playerid, id, coords, count, value);
}

// ============================================================================
// Hexagon Progress Bar
// ============================================================================

stock CreatePlayerHexagonProgress(playerid, Float:pos_x, Float:pos_y, max_value = 100, color = 0xFF0000FF, background_COLOR = 0x000000FF, Float:size = 10.0, Float:thickness = 0.2, Float:polygons = DEFAULT_CIRCLE_POLYGONS) {
    #pragma unused polygons
    new index = Iter_Free(Circles[playerid]);
    if(index == -1) return -1;

    e_CircleDraw[playerid][index][E_PROG_TYPE] = SHAPE_HEXAGON;
    e_CircleDraw[playerid][index][E_PROG_POS_X] = pos_x;
    e_CircleDraw[playerid][index][E_PROG_POS_Y] = pos_y;
    e_CircleDraw[playerid][index][E_PROG_SIZE] = size;
    e_CircleDraw[playerid][index][E_PROG_THICKNESS] = thickness;
    e_CircleDraw[playerid][index][E_PROG_MAX_VALUE] = float(max_value);
    e_CircleDraw[playerid][index][E_PROG_VALUE] = -1.0;
    e_CircleDraw[playerid][index][E_PROG_COLOR] = color;
    e_CircleDraw[playerid][index][E_PROG_BACKGROUND] = background_COLOR;
    e_CircleDraw[playerid][index][E_PROG_INVERT] = false;

    Iter_Add(Circles[playerid], index);
    UpdatePlayerHexagonProgress(playerid, index, max_value);
    return index;
}

stock UpdatePlayerHexagonProgress(playerid, id, value) {
    if(!Iter_Contains(Circles[playerid], id)) return false;
    if(floatround(e_CircleDraw[playerid][id][E_PROG_VALUE]) == value) return true;

    new Float:cx = e_CircleDraw[playerid][id][E_PROG_POS_X];
    new Float:cy = e_CircleDraw[playerid][id][E_PROG_POS_Y];
    new Float:sz = e_CircleDraw[playerid][id][E_PROG_SIZE];

    new Float:coords[6][2];
    for(new i = 0; i < 6; i++) {
        ctd(i * 60.0 + 180.0, sz, cx, cy, coords[i][0], coords[i][1]);
    }

    return _Progress_RenderPoints(playerid, id, coords, 6, value);
}

// ============================================================================
// Diamond Progress Bar
// ============================================================================

stock CreatePlayerDiamondProgress(playerid, Float:pos_x, Float:pos_y, max_value = 100, color = 0xFF0000FF, background_COLOR = 0x000000FF, Float:size = 10.0, Float:thickness = 0.2, Float:polygons = DEFAULT_CIRCLE_POLYGONS) {
    #pragma unused polygons
    new index = Iter_Free(Circles[playerid]);
    if(index == -1) return -1;

    e_CircleDraw[playerid][index][E_PROG_TYPE] = SHAPE_DIAMOND;
    e_CircleDraw[playerid][index][E_PROG_POS_X] = pos_x;
    e_CircleDraw[playerid][index][E_PROG_POS_Y] = pos_y;
    e_CircleDraw[playerid][index][E_PROG_SIZE] = size;
    e_CircleDraw[playerid][index][E_PROG_THICKNESS] = thickness;
    e_CircleDraw[playerid][index][E_PROG_MAX_VALUE] = float(max_value);
    e_CircleDraw[playerid][index][E_PROG_VALUE] = -1.0;
    e_CircleDraw[playerid][index][E_PROG_COLOR] = color;
    e_CircleDraw[playerid][index][E_PROG_BACKGROUND] = background_COLOR;
    e_CircleDraw[playerid][index][E_PROG_INVERT] = false;

    Iter_Add(Circles[playerid], index);
    UpdatePlayerDiamondProgress(playerid, index, max_value);
    return index;
}

stock UpdatePlayerDiamondProgress(playerid, id, value) {
    if(!Iter_Contains(Circles[playerid], id)) return false;
    if(floatround(e_CircleDraw[playerid][id][E_PROG_VALUE]) == value) return true;

    new Float:cx = e_CircleDraw[playerid][id][E_PROG_POS_X];
    new Float:cy = e_CircleDraw[playerid][id][E_PROG_POS_Y];
    new Float:sz = e_CircleDraw[playerid][id][E_PROG_SIZE];

    new Float:coords[4][2];
    for(new i = 0; i < 4; i++) {
        ctd(i * 90.0 + 180.0, sz, cx, cy, coords[i][0], coords[i][1]);
    }

    return _Progress_RenderPoints(playerid, id, coords, 4, value);
}

// ============================================================================
// Square Progress Bar
// ============================================================================

stock CreatePlayerSquareProgress(playerid, Float:pos_x, Float:pos_y, max_value = 100, color = 0xFF0000FF, background_COLOR = 0x000000FF, Float:size = 10.0, Float:thickness = 0.2, Float:polygons = DEFAULT_CIRCLE_POLYGONS) {
    #pragma unused polygons
    new index = Iter_Free(Circles[playerid]);
    if(index == -1) return -1;

    e_CircleDraw[playerid][index][E_PROG_TYPE] = SHAPE_SQUARE;
    e_CircleDraw[playerid][index][E_PROG_POS_X] = pos_x;
    e_CircleDraw[playerid][index][E_PROG_POS_Y] = pos_y;
    e_CircleDraw[playerid][index][E_PROG_SIZE] = size;
    e_CircleDraw[playerid][index][E_PROG_THICKNESS] = thickness;
    e_CircleDraw[playerid][index][E_PROG_MAX_VALUE] = float(max_value);
    e_CircleDraw[playerid][index][E_PROG_VALUE] = -1.0;
    e_CircleDraw[playerid][index][E_PROG_COLOR] = color;
    e_CircleDraw[playerid][index][E_PROG_BACKGROUND] = background_COLOR;
    e_CircleDraw[playerid][index][E_PROG_INVERT] = false;

    Iter_Add(Circles[playerid], index);
    UpdatePlayerSquareProgress(playerid, index, max_value);
    return index;
}

stock UpdatePlayerSquareProgress(playerid, id, value) {
    if(!Iter_Contains(Circles[playerid], id)) return false;
    if(floatround(e_CircleDraw[playerid][id][E_PROG_VALUE]) == value) return true;

    new Float:cx = e_CircleDraw[playerid][id][E_PROG_POS_X];
    new Float:cy = e_CircleDraw[playerid][id][E_PROG_POS_Y];
    new Float:sz = e_CircleDraw[playerid][id][E_PROG_SIZE];
    new Float:sy_scaled = sz * CIRCLE_ASPECT_RATIO_Y;

    new Float:coords[4][2];
    coords[0][0] = cx - sz; coords[0][1] = cy - sy_scaled;
    coords[1][0] = cx + sz; coords[1][1] = cy - sy_scaled;
    coords[2][0] = cx + sz; coords[2][1] = cy + sy_scaled;
    coords[3][0] = cx - sz; coords[3][1] = cy + sy_scaled;

    return _Progress_RenderPoints(playerid, id, coords, 4, value);
}

// ============================================================================
// Triangle Progress Bar
// ============================================================================

stock CreatePlayerTriangleProgress(playerid, Float:pos_x, Float:pos_y, max_value = 100, color = 0xFF0000FF, background_COLOR = 0x000000FF, Float:size = 10.0, Float:thickness = 0.2, Float:polygons = DEFAULT_CIRCLE_POLYGONS, bool:invert = false) {
    #pragma unused polygons
    new index = Iter_Free(Circles[playerid]);
    if(index == -1) return -1;

    e_CircleDraw[playerid][index][E_PROG_TYPE] = SHAPE_TRIANGLE;
    e_CircleDraw[playerid][index][E_PROG_POS_X] = pos_x;
    e_CircleDraw[playerid][index][E_PROG_POS_Y] = pos_y;
    e_CircleDraw[playerid][index][E_PROG_SIZE] = size;
    e_CircleDraw[playerid][index][E_PROG_THICKNESS] = thickness;
    e_CircleDraw[playerid][index][E_PROG_MAX_VALUE] = float(max_value);
    e_CircleDraw[playerid][index][E_PROG_VALUE] = -1.0;
    e_CircleDraw[playerid][index][E_PROG_COLOR] = color;
    e_CircleDraw[playerid][index][E_PROG_BACKGROUND] = background_COLOR;
    e_CircleDraw[playerid][index][E_PROG_INVERT] = invert;

    Iter_Add(Circles[playerid], index);
    UpdatePlayerTriangleProgress(playerid, index, max_value);
    return index;
}

stock UpdatePlayerTriangleProgress(playerid, id, value) {
    if(!Iter_Contains(Circles[playerid], id)) return false;
    if(floatround(e_CircleDraw[playerid][id][E_PROG_VALUE]) == value) return true;

    new Float:cx = e_CircleDraw[playerid][id][E_PROG_POS_X];
    new Float:cy = e_CircleDraw[playerid][id][E_PROG_POS_Y];
    new Float:sz = e_CircleDraw[playerid][id][E_PROG_SIZE];
    new bool:inv = e_CircleDraw[playerid][id][E_PROG_INVERT];
    new Float:sy_scaled = sz * CIRCLE_ASPECT_RATIO_Y;

    new Float:coords[3][2];
    coords[0][0] = cx;
    coords[0][1] = inv ? (cy + sy_scaled) : (cy - sy_scaled);

    coords[1][0] = cx - sz;
    coords[1][1] = inv ? (cy - sy_scaled) : (cy + sy_scaled);

    coords[2][0] = cx + sz;
    coords[2][1] = inv ? (cy - sy_scaled) : (cy + sy_scaled);

    return _Progress_RenderPoints(playerid, id, coords, 3, value);
}

// ============================================================================
// Parens Progress Bar
// ============================================================================

stock CreatePlayerParensProgress(playerid, Float:pos_x, Float:pos_y, max_value = 100, color = 0xFF0000FF, background_COLOR = 0x000000FF, Float:size = 10.0, Float:thickness = 0.2, Float:polygons = DEFAULT_CIRCLE_POLYGONS, bool:invert = false) {
    #pragma unused polygons
    new index = Iter_Free(Circles[playerid]);
    if(index == -1) return -1;

    e_CircleDraw[playerid][index][E_PROG_TYPE] = SHAPE_PARENS;
    e_CircleDraw[playerid][index][E_PROG_POS_X] = pos_x;
    e_CircleDraw[playerid][index][E_PROG_POS_Y] = pos_y;
    e_CircleDraw[playerid][index][E_PROG_SIZE] = size;
    e_CircleDraw[playerid][index][E_PROG_THICKNESS] = thickness;
    e_CircleDraw[playerid][index][E_PROG_MAX_VALUE] = float(max_value);
    e_CircleDraw[playerid][index][E_PROG_VALUE] = -1.0;
    e_CircleDraw[playerid][index][E_PROG_COLOR] = color;
    e_CircleDraw[playerid][index][E_PROG_BACKGROUND] = background_COLOR;
    e_CircleDraw[playerid][index][E_PROG_INVERT] = invert;

    Iter_Add(Circles[playerid], index);
    UpdatePlayerParensProgress(playerid, index, max_value);
    return index;
}

stock UpdatePlayerParensProgress(playerid, id, value) {
    if(!Iter_Contains(Circles[playerid], id)) return false;
    if(floatround(e_CircleDraw[playerid][id][E_PROG_VALUE]) == value) return true;

    new Float:cx = e_CircleDraw[playerid][id][E_PROG_POS_X];
    new Float:cy = e_CircleDraw[playerid][id][E_PROG_POS_Y];
    new Float:sz = e_CircleDraw[playerid][id][E_PROG_SIZE];
    new bool:inv = e_CircleDraw[playerid][id][E_PROG_INVERT];

    new Float:start = inv ? 180.0 : 0.0;
    new Float:finish = inv ? 360.0 : 180.0;

    new Float:coords[MAX_CIRCLES_POLYGONS][2];
    new count = 0;

    for(new Float:v = start; v <= finish && count < MAX_CIRCLES_POLYGONS; v += 30.0) {
        ctd(v + 180.0, sz, cx, cy, coords[count][0], coords[count][1]);
        count++;
    }

    return _Progress_RenderPoints(playerid, id, coords, count, value);
}

// ============================================================================
// Arc Progress Bar
// ============================================================================

stock CreatePlayerArcProgress(playerid, Float:pos_x, Float:pos_y, max_value = 100, color = 0xFF0000FF, background_COLOR = 0x000000FF, Float:size = 10.0, Float:thickness = 0.2, Float:polygons = DEFAULT_CIRCLE_POLYGONS) {
    #pragma unused polygons
    new index = Iter_Free(Circles[playerid]);
    if(index == -1) return -1;

    e_CircleDraw[playerid][index][E_PROG_TYPE] = SHAPE_ARC;
    e_CircleDraw[playerid][index][E_PROG_POS_X] = pos_x;
    e_CircleDraw[playerid][index][E_PROG_POS_Y] = pos_y;
    e_CircleDraw[playerid][index][E_PROG_SIZE] = size;
    e_CircleDraw[playerid][index][E_PROG_THICKNESS] = thickness;
    e_CircleDraw[playerid][index][E_PROG_MAX_VALUE] = float(max_value);
    e_CircleDraw[playerid][index][E_PROG_VALUE] = -1.0;
    e_CircleDraw[playerid][index][E_PROG_COLOR] = color;
    e_CircleDraw[playerid][index][E_PROG_BACKGROUND] = background_COLOR;
    e_CircleDraw[playerid][index][E_PROG_INVERT] = false;

    Iter_Add(Circles[playerid], index);
    UpdatePlayerArcProgress(playerid, index, max_value);
    return index;
}

stock UpdatePlayerArcProgress(playerid, id, value) {
    if(!Iter_Contains(Circles[playerid], id)) return false;
    if(floatround(e_CircleDraw[playerid][id][E_PROG_VALUE]) == value) return true;

    new Float:cx = e_CircleDraw[playerid][id][E_PROG_POS_X];
    new Float:cy = e_CircleDraw[playerid][id][E_PROG_POS_Y];
    new Float:sz = e_CircleDraw[playerid][id][E_PROG_SIZE];

    new Float:coords[MAX_CIRCLES_POLYGONS][2];
    new count = 0;

    for(new Float:v = 270.0; v <= 450.0 && count < MAX_CIRCLES_POLYGONS; v += 30.0) {
        ctd(v + 180.0, sz, cx, cy, coords[count][0], coords[count][1]);
        count++;
    }

    return _Progress_RenderPoints(playerid, id, coords, count, value);
}
