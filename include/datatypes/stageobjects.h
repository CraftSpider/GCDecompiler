#pragma once

#include <iostream>
#include <map>
#include "types.h"

namespace types {

class STAGEDEF;

struct SubfileMixin {
    
    STAGEDEF* owner;
    
    SubfileMixin(STAGEDEF* def = nullptr);
    
    virtual void read(std::iostream& input) = 0;
    virtual void write(std::iostream& output) = 0;
};

struct AnimationHeader : public SubfileMixin {
    uint num_x_rot_keyframes, x_rot_keyframes_offset, num_y_rot_keyframes, y_rot_keyframes_offset, num_z_rot_keyframes,
        z_rot_keyframes_offset, num_x_translate_keyframes, x_translate_keyframes_offset, num_y_translate_keyframes,
        y_translate_keyframes_offset, num_z_translate_keyframes, z_translate_keyframes_offset;
    
    void read(std::iostream& input) override;
    void write(std::iostream& output) override;
};

struct BackgroundAnimationHeader : public SubfileMixin {
    float anim_loop_point;
    uint num_x_rot_keyframes, x_rot_keyframes_offset, num_y_rot_keyframes, y_rot_keyframes_offset, num_z_rot_keyframes,
        z_rot_keyframes_offset, num_x_translate_keyframes, x_translate_keyframes_offset, num_y_translate_keyframes,
        y_translate_keyframes_offset, num_z_translate_keyframes, z_translate_keyframes_offset;
    
    void read(std::iostream& input) override;
    void write(std::iostream& output) override;
};

struct BackgroundAnimationHeader2 : public SubfileMixin {
    uint loop_point, num_keyframes_1, offset_keyframes_1, num_keyframes_2, offset_keyframes_2, num_x_rot_keyframes,
        offset_x_rot_keyframes, num_y_rot_keyframes, offset_y_rot_keyframes, num_z_rot_keyframes,
        offset_z_rot_keyframes, num_x_keyframes, offset_x_keyframes, num_y_keyframes, offset_y_keyframes,
        num_z_keyframes, offset_z_keyframes, num_keyframes_9, offset_keyframes_9, num_keyframes_10, offset_keyframes_10,
        num_keyframes_11, offset_keyframes_11;
    
    void read(std::iostream& input) override;
    void write(std::iostream& output) override;
};

struct EffectHeader : public SubfileMixin {
    uint num_effect1_keyframes, offset_effect1_keyframes, num_effect2_keyframes, offset_effect2_keyframes,
        offset_texture_scroll;
    
    void read(std::iostream& input) override;
    void write(std::iostream& output) override;
};

struct LevelModelPointerA : public SubfileMixin {
    uint offset_level_model;
    
    void read(std::iostream& input) override;
    void write(std::iostream& output) override;
};

struct LevelModelPointerB : public SubfileMixin {
    uint offset_pointer_a;
    
    void read(std::iostream& input) override;
    void write(std::iostream& output) override;
};

struct CollisionHeader : public SubfileMixin {
    float x_rot_center, y_rot_center, z_rot_center, x_conveyor, y_conveyor, z_conveyor, x_collision_start,
        z_collision_start, x_collision_step, z_collision_step;
    ushort x_rot, y_rot, z_rot, anim_type;
    int anim_header_offset, collision_triangle_offset, collision_grid_triangle_offset, x_collision_count,
        z_collision_count, num_goals, goals_offset;
    
    void read(std::iostream& input) override;
    void write(std::iostream& output) override;
};

struct StartPos : public SubfileMixin {
    float x_pos, y_pos, z_pos;
    ushort x_rot, y_rot, z_rot;
    
    void read(std::iostream& input) override;
    void write(std::iostream& output) override;
};

struct FalloutPlane : public SubfileMixin {
    float y_pos;
    
    void read(std::iostream& input) override;
    void write(std::iostream& output) override;
};

struct Goal : public SubfileMixin {
    float x_pos, y_pos, z_pos;
    ushort x_rot, y_rot, z_rot, type;
    
    void read(std::iostream& input) override;
    void write(std::iostream& output) override;
};

struct Bumper : public SubfileMixin {
    float x_pos, y_pos, z_pos, x_scale, y_scale, z_scale;
    ushort x_rot, y_rot, z_rot;
    
    void read(std::iostream& input) override;
    void write(std::iostream& output) override;
};

struct Jamabar : public SubfileMixin {
    float x_pos, y_pos, z_pos, x_scale, y_scale, z_scale;
    ushort x_rot, y_rot, z_rot;
    
    void read(std::iostream& input) override;
    void write(std::iostream& output) override;
};

struct Banana : public SubfileMixin {
    float x_pos, y_pos, z_pos;
    uint type;
    
    void read(std::iostream& input) override;
    void write(std::iostream& output) override;
};

struct ConeCollision : public SubfileMixin {
    float x_pos, y_pos, z_pos, bottom_radius, top_radius, height;
    ushort x_rot, y_rot, z_rot;
    
    void read(std::iostream& input) override;
    void write(std::iostream& output) override;
};

struct SphereCollision : public SubfileMixin {
    float x_pos, y_pos, z_pos, radius;
    
    void read(std::iostream& input) override;
    void write(std::iostream& output) override;
};

struct CylinderCollision : public SubfileMixin {
    float x_pos, y_pos, z_pos, radius, height;
    ushort x_rot, y_rot, z_rot;
    
    void read(std::iostream& input) override;
    void write(std::iostream& output) override;
};

struct FalloutVolume : public SubfileMixin {
    float center_x, center_y, center_z, x_size, y_size, z_size;
    ushort x_rot, y_rot, z_rot;
    
    void read(std::iostream& input) override;
    void write(std::iostream& output) override;
};

struct BackgroundModel : public SubfileMixin {
    std::string* model_name;
    BackgroundAnimationHeader* anim_header;
    BackgroundAnimationHeader2* anim_header_2;
    EffectHeader* effect_header;
    float x_pos, y_pos, z_pos, x_scale, y_scale, z_scale;
    ushort x_rot, y_rot, z_rot;
    
    BackgroundModel(STAGEDEF* def = nullptr) : SubfileMixin(def) {}
    
    void read(std::iostream& input) override;
    void write(std::iostream& output) override;
};

struct ReflectiveModel : public SubfileMixin {
    std::string* model_name;
    
    ReflectiveModel(STAGEDEF* def = nullptr) : SubfileMixin(def) {}
    
    void read(std::iostream& input) override;
    void write(std::iostream& output) override;
};

struct LevelModelInstance : public SubfileMixin {
    LevelModelPointerA* level_model_pointer;
    float x_pos, y_pos, z_pos, x_scale, y_scale, z_scale;
    short x_rot, y_rot, z_rot;
    
    LevelModelInstance(STAGEDEF* def = nullptr) : SubfileMixin(def) {}
    
    void read(std::iostream& input) override;
    void write(std::iostream& output) override;
};

struct Switch : public SubfileMixin {
    float x_pos, y_pos, z_pos;
    ushort x_rot, y_rot, z_rot, type, anim_ids;
    
    void read(std::iostream& input) override;
    void write(std::iostream& output) override;
};

struct FogAnimationHeader : public SubfileMixin {
    uint num_start_keyframes, start_keyframes_offset, num_end_keyframes, end_keyframes_offset, num_red_keyframes,
        red_keyframes_offset, num_green_keyframes, green_keyframes_offset, num_blue_keyframes, blue_keyframes_offset,
        num_unknown_keyframes, unknown_keyframes_offset;
    
    void read(std::iostream& input) override;
    void write(std::iostream& output) override;
};

struct Wormhole : public SubfileMixin {
    float x_pos, y_pos, z_pos;
    ushort x_rot, y_rot, z_rot;
    uint offset;
    Wormhole *destination;
    
    void read(std::iostream& input) override;
    void write(std::iostream& output) override;
};

struct Fog : public SubfileMixin {
    uchar type;
    float start_dist, end_dist, red, green, blue;
    
    void read(std::iostream& input) override;
    void write(std::iostream& output) override;
};

}
