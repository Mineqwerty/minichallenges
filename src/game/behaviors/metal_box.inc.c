// metal_box.inc.c

#include "src/game/behavior_actions.h"

struct ObjectHitbox sMetalBoxHitbox = {
    /* interactType:      */ INTERACT_NONE,
    /* downOffset:        */ 0,
    /* damageOrCoinValue: */ 0,
    /* health:            */ 1,
    /* numLootCoins:      */ 0,
    /* radius:            */ 220,
    /* height:            */ 300,
    /* hurtboxRadius:     */ 220,
    /* hurtboxHeight:     */ 300,
};

struct ObjectHitbox sBigMetalBoxHitbox = {
    /* interactType:      */ INTERACT_NONE,
    /* downOffset:        */ 0,
    /* damageOrCoinValue: */ 0,
    /* health:            */ 1,
    /* numLootCoins:      */ 0,
    /* radius:            */ 660,
    /* height:            */ 900,
    /* hurtboxRadius:     */ 660,
    /* hurtboxHeight:     */ 900,
};

s32 check_if_moving_over_floor(f32 maxDist, f32 offset) {
    struct Surface *floor;
    f32 xPos = o->oPosX + sins(o->oMoveAngleYaw) * offset;
    f32 zPos = o->oPosZ + coss(o->oMoveAngleYaw) * offset;

    f32 floorHeight = find_floor(xPos, o->oPosY, zPos, &floor);

    return (absf(floorHeight - o->oPosY) < maxDist);
}

void bhv_pushable_loop(void) {
    
    o->oForwardVel = 0.0f;
    o->oFlags &= ~OBJ_FLAG_HOLDABLE;

    if (obj_check_if_collided_with_object(o, gMarioObject) && gMarioStates[0].flags & MARIO_PUSHING) {
        s16 angleToMario = obj_angle_to_object(o, gMarioObject);
        if (abs_angle_diff(angleToMario, gMarioObject->oMoveAngleYaw) > 0x4000) {
            o->oMoveAngleYaw = (s16)((gMarioObject->oMoveAngleYaw + 0x2000) & 0xc000);
            if (check_if_moving_over_floor(8.0f, 150.0f)) {
                o->oForwardVel = 18.0f;
                cur_obj_play_sound_1(SOUND_ENV_METAL_BOX_PUSH);
            }
        }
    }

    cur_obj_move_using_fvel_and_gravity();
}

void bhv_ice_block_loop(void) {
    f32 minScale = 0.25f;

    if (o->oIceScale < 0.1f) {
        o->oIceScale = 1.0f;
    }
    
    if (o->oIceScale > minScale) {
        if (o->oIceScale < 3.0) {
        obj_set_hitbox(o, &sMetalBoxHitbox);
        }
        else {
            obj_set_hitbox(o, &sBigMetalBoxHitbox);
        }
        bhv_pushable_loop();
        load_object_collision_model();
    }
    else {
        spawn_object(o, MODEL_ICE_BLOCK, bhvBreakableBoxSmall);
        obj_mark_for_deletion(o);

        
    }
    

    if (o->oIceScale > 3.0f) {
        o->oIceScale = 3.0f;
    }

    

    obj_scale(o, o->oIceScale);

    f32 dist;
    struct Object *nearFire = cur_obj_find_nearest_object_with_behavior(bhvFlame, &dist);

    if (nearFire && dist < 200.0f) {
        if (obj_has_model(nearFire, MODEL_RED_FLAME) && o->oIceScale > minScale) {
            o->oIceScale -= 0.05f;
        }
        else if (obj_has_model(nearFire, MODEL_BLUE_FLAME) && o->oIceScale < 3.0f){
            o->oIceScale += 0.05f;
        }
    }
    
    
}