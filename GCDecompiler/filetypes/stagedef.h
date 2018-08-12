//
// Created by Rune Tynan on 8/11/2018.
//

#pragma once

#include <string>
#include "types.h"
#include "utils.h"

struct StartPos {
    float x_pos, y_pos, z_pos;
    ushort x_rot, y_rot, z_rot;
};

struct FalloutPlane {
    float y_pos;
};

struct Goal {
    float x_pos, y_pos, z_pos;
    ushort x_rot, y_rot, z_rot, type;
};

struct Bumper {
    float x_pos, y_pos, z_pos, x_scale, y_scale, z_scale;
    ushort x_rot, y_rot, z_rot;
};

struct Jamabar {
    float x_pos, y_pos, z_pos, x_scale, y_scale, z_scale;
    ushort x_rot, y_rot, z_rot;
};

struct Banana {
    float x_pos, y_pos, z_pos;
    uint type;
};

struct FalloutVolume {
    float center_x, center_y, center_z, x_size, y_size, z_size;
    ushort x_rot, y_rot, z_rot;
};

struct Switch {
    float x_pos, y_pos, z_pos;
    ushort x_rot, y_rot, z_rot, type, anim_ids;
};

struct Wormhole {
    float x_pos, y_pos, z_pos;
    ushort x_rot, y_rot, z_rot;
    uint offset;
    Wormhole *destination;
};

struct ConeCollision {
    float x_pos, y_pos, z_pos, radius, height;
    ushort x_rot, y_rot, z_rot;
};

struct SphereCollision {
    float x_pos, y_pos, z_pos, radius;
};

struct CylinderCollision {
    float x_pos, y_pos, z_pos, radius, height;
    ushort x_rot, y_rot, z_rot;
};

namespace types {

class STAGEDEF {
    
    uint num_collision, collision_offset, start_pos_offset, fallout_plane_offset, num_goals, goals_offset, num_bumpers,
    bumper_offset, num_jamabars, jamabar_offset, num_bananas, banana_offset, num_cones, cone_offset, num_spheres,
    sphere_offset, num_cylinders, cylinder_offset, num_fallouts, fallouts_offset, num_backgrounds, background_offset,
    num_unkown8, unknown8_offset, num_reflective, reflective_offset, num_model_instances, model_offset,
    num_models_a, models_a_offset, num_models_b, models_b_offset, num_switches, switch_offset,
    fog_offset, fog_anim_offset, num_wormholes, wormhole_offset, unknown3_offset;
    
    StartPos *starts;
    FalloutPlane fallout_plane;
    Goal *goals;
    Bumper *bumpers;
    Jamabar *jamabars;
    Banana *bananas;
    FalloutVolume *fallout_volumes;
    Switch *switches;
    Wormhole *wormholes;
    ConeCollision *cones;
    SphereCollision *spheres;
    CylinderCollision *cylinders;
    
    STAGEDEF(const std::string &filename);
    
    void save_smb1(const std::string &output);
    
    void save_smb2(const std::string &output);
    
};

}
