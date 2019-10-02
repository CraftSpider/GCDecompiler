
#include <at_utils>
#include <at_logging>
#include "filetypes/stagedef.h"
#include "datatypes/stageobjects.h"

namespace types {

static logging::Logger *logger = logging::get_logger("stageobjects");

SubfileMixin::SubfileMixin(types::STAGEDEF* def) {
    owner = def;
}

void AnimationHeader::read(std::iostream& input) {
    num_x_rot_keyframes = util::next_uint(input);
    x_rot_keyframes_offset = util::next_uint(input);
    num_y_rot_keyframes = util::next_uint(input);
    y_rot_keyframes_offset = util::next_uint(input);
    num_z_rot_keyframes = util::next_uint(input);
    z_rot_keyframes_offset = util::next_uint(input);
    num_x_translate_keyframes = util::next_uint(input);
    x_translate_keyframes_offset = util::next_uint(input);
    num_y_translate_keyframes = util::next_uint(input);
    y_translate_keyframes_offset = util::next_uint(input);
    num_z_translate_keyframes = util::next_uint(input);
    z_translate_keyframes_offset = util::next_uint(input);
    util::next_ulong(input, 16);
}

void AnimationHeader::write(std::iostream& output) {
    // TODO
}

void BackgroundAnimationHeader::read(std::iostream& input) {
    util::next_uint(input);
    anim_loop_point = util::next_float(input);
    util::next_ulong(input);
    num_x_rot_keyframes = util::next_uint(input);
    x_rot_keyframes_offset = util::next_uint(input);
    num_y_rot_keyframes = util::next_uint(input);
    y_rot_keyframes_offset = util::next_uint(input);
    num_z_rot_keyframes = util::next_uint(input);
    z_rot_keyframes_offset = util::next_uint(input);
    num_x_translate_keyframes = util::next_uint(input);
    x_translate_keyframes_offset = util::next_uint(input);
    num_y_translate_keyframes = util::next_uint(input);
    y_translate_keyframes_offset = util::next_uint(input);
    num_z_translate_keyframes = util::next_uint(input);
    z_translate_keyframes_offset = util::next_uint(input);
    util::next_ulong(input, 16);
}

void BackgroundAnimationHeader::write(std::iostream& output) {
    // TODO
}

void BackgroundAnimationHeader2::read(std::iostream& input) {
    util::next_uint(input);
    loop_point = util::next_float(input);
    num_keyframes_1 = util::next_uint(input);
    offset_keyframes_1 = util::next_uint(input);
    num_keyframes_2 = util::next_uint(input);
    offset_keyframes_2 = util::next_uint(input);
    num_x_rot_keyframes = util::next_uint(input);
    offset_x_rot_keyframes = util::next_uint(input);
    num_y_rot_keyframes = util::next_uint(input);
    offset_y_rot_keyframes = util::next_uint(input);
    num_z_rot_keyframes = util::next_uint(input);
    offset_z_rot_keyframes = util::next_uint(input);
    num_x_keyframes = util::next_uint(input);
    offset_x_keyframes = util::next_uint(input);
    num_y_keyframes = util::next_uint(input);
    offset_y_keyframes = util::next_uint(input);
    num_z_keyframes = util::next_uint(input);
    offset_z_keyframes = util::next_uint(input);
    num_keyframes_9 = util::next_uint(input);
    offset_keyframes_9 = util::next_uint(input);
    num_keyframes_10 = util::next_uint(input);
    offset_keyframes_10 = util::next_uint(input);
    num_keyframes_11 = util::next_uint(input);
    offset_keyframes_11 = util::next_uint(input);
}

void BackgroundAnimationHeader2::write(std::iostream& output) {
    // TODO
}

void EffectHeader::read(std::iostream& input) {
    num_effect1_keyframes = util::next_uint(input);
    offset_effect1_keyframes = util::next_uint(input);
    num_effect2_keyframes = util::next_uint(input);
    offset_effect2_keyframes = util::next_uint(input);
    offset_texture_scroll = util::next_uint(input);
    util::next_char(input, 28);
}

void EffectHeader::write(std::iostream& output) {
    // TODO
}

void LevelModelPointerA::read(std::iostream& input) {
    util::next_ulong(input);
    offset_level_model = util::next_uint(input);
}

void LevelModelPointerA::write(std::iostream& output) {
    // TODO
}

void LevelModelPointerB::read(std::iostream& input) {
    offset_pointer_a = util::next_uint(input);
}

void LevelModelPointerB::write(std::iostream& output) {
    // TODO
}

void CollisionHeader::read(std::iostream& input) {
    x_rot_center = util::next_float(input);
    y_rot_center = util::next_float(input);
    z_rot_center = util::next_float(input);
    x_rot = util::next_short(input);
    y_rot = util::next_short(input);
    z_rot = util::next_short(input);
    anim_type = util::next_short(input);
    anim_header_offset = util::next_uint(input);
    x_conveyor = util::next_float(input);
    y_conveyor = util::next_float(input);
    z_conveyor = util::next_float(input);
    collision_triangle_offset = util::next_uint(input);
    collision_grid_triangle_offset = util::next_uint(input);
    x_collision_start = util::next_float(input);
    z_collision_start = util::next_float(input);
    x_collision_step = util::next_float(input);
    z_collision_step = util::next_float(input);
    x_collision_count = util::next_int(input);
    z_collision_count = util::next_int(input);
    num_goals = util::next_uint(input);
    goals_offset = util::next_uint(input);
    util::next_char(input, 1104);
}

void CollisionHeader::write(std::iostream& output) {
    // TODO
}

void StartPos::read(std::iostream& input) {
    x_pos = util::next_float(input);
    y_pos = util::next_float(input);
    z_pos = util::next_float(input);
    x_rot = util::next_ushort(input);
    y_rot = util::next_ushort(input);
    z_rot = util::next_ushort(input);
    util::next_ushort(input);
}

void StartPos::write(std::iostream& output) {
    util::write_float(output, x_pos);
    util::write_float(output, y_pos);
    util::write_float(output, z_pos);
    util::write_ushort(output, x_rot);
    util::write_ushort(output, y_rot);
    util::write_ushort(output, z_rot);
    util::write_ushort(output, 0);
}

void FalloutPlane::read(std::iostream& input) {
    y_pos = util::next_float(input);
}

void FalloutPlane::write(std::iostream& output) {
    util::write_float(output, y_pos);
}

void Goal::read(std::iostream& input) {
    x_pos = util::next_float(input);
    y_pos = util::next_float(input);
    z_pos = util::next_float(input);
    x_rot = util::next_ushort(input);
    y_rot = util::next_ushort(input);
    z_rot = util::next_ushort(input);
    type = util::next_ushort(input);
}

void Goal::write(std::iostream& output) {
    util::write_float(output, x_pos);
    util::write_float(output, y_pos);
    util::write_float(output, z_pos);
    util::write_ushort(output, x_rot);
    util::write_ushort(output, y_pos);
    util::write_ushort(output, z_rot);
    util::write_ushort(output, type);
}

void Bumper::read(std::iostream& input) {
    x_pos = util::next_float(input);
    y_pos = util::next_float(input);
    z_pos = util::next_float(input);
    x_rot = util::next_ushort(input);
    y_rot = util::next_ushort(input);
    z_rot = util::next_ushort(input);
    util::next_ushort(input);
    x_scale = util::next_float(input);
    y_scale = util::next_float(input);
    z_scale = util::next_float(input);
}

void Bumper::write(std::iostream& output) {
    util::write_float(output, x_pos);
    util::write_float(output, y_pos);
    util::write_float(output, z_pos);
    util::write_ushort(output, x_rot);
    util::write_ushort(output, y_rot);
    util::write_ushort(output, z_rot);
    util::write_ushort(output, 0);
    util::write_float(output, x_scale);
    util::write_float(output, y_scale);
    util::write_float(output, z_scale);
}

void Jamabar::read(std::iostream& input) {
    x_pos = util::next_float(input);
    y_pos = util::next_float(input);
    z_pos = util::next_float(input);
    x_rot = util::next_ushort(input);
    y_rot = util::next_ushort(input);
    z_rot = util::next_ushort(input);
    util::next_ushort(input);
    x_scale = util::next_float(input);
    y_scale = util::next_float(input);
    z_scale = util::next_float(input);
}

void Jamabar::write(std::iostream& output) {
    util::write_float(output, x_pos);
    util::write_float(output, y_pos);
    util::write_float(output, z_pos);
    util::write_short(output, x_rot);
    util::write_short(output, y_rot);
    util::write_short(output, z_rot);
    util::write_short(output, 0);
    util::write_float(output, x_scale);
    util::write_float(output, y_scale);
    util::write_float(output, z_scale);
}

void Banana::read(std::iostream& input) {
    x_pos = util::next_float(input);
    y_pos = util::next_float(input);
    z_pos = util::next_float(input);
    type = util::next_uint(input);
}

void Banana::write(std::iostream& output) {
    util::write_float(output, x_pos);
    util::write_float(output, y_pos);
    util::write_float(output, z_pos);
    util::write_uint(output, type);
}

void ConeCollision::read(std::iostream& input) {
    x_pos = util::next_float(input);
    y_pos = util::next_float(input);
    z_pos = util::next_float(input);
    x_rot = util::next_ushort(input);
    y_rot = util::next_ushort(input);
    z_rot = util::next_ushort(input);
    util::next_ushort(input);
    bottom_radius = util::next_float(input);
    height = util::next_float(input);
    top_radius = util::next_float(input);
}

void ConeCollision::write(std::iostream& output) {
    util::write_float(output, x_pos);
    util::write_float(output, y_pos);
    util::write_float(output, z_pos);
    util::write_ushort(output, x_rot);
    util::write_ushort(output, y_rot);
    util::write_ushort(output, z_rot);
    util::write_ushort(output, 0);
    util::write_float(output, bottom_radius);
    util::write_float(output, height);
    util::write_float(output, top_radius);
}

void SphereCollision::read(std::iostream& input) {
    x_pos = util::next_float(input);
    y_pos = util::next_float(input);
    z_pos = util::next_float(input);
    radius = util::next_float(input);
    util::next_uint(input); // Unknown/Null
}

void SphereCollision::write(std::iostream& output) {
    util::write_float(output, x_pos);
    util::write_float(output, y_pos);
    util::write_float(output, z_pos);
    util::write_float(output, radius);
    util::write_uint(output, 0);
}

void CylinderCollision::read(std::iostream& input) {
    x_pos = util::next_float(input);
    y_pos = util::next_float(input);
    z_pos = util::next_float(input);
    radius = util::next_float(input);
    height = util::next_float(input);
    x_rot = util::next_ushort(input);
    y_rot = util::next_ushort(input);
    z_rot = util::next_ushort(input);
    util::next_ushort(input);
}

void CylinderCollision::write(std::iostream& output) {
    util::write_float(output, x_pos);
    util::write_float(output, y_pos);
    util::write_float(output, z_pos);
    util::write_float(output, radius);
    util::write_float(output, height);
    util::write_ushort(output, x_rot);
    util::write_ushort(output, y_rot);
    util::write_ushort(output, z_rot);
    util::write_ushort(output, 0);
}

void FalloutVolume::read(std::iostream& input) {
    center_x = util::next_float(input);
    center_y = util::next_float(input);
    center_z = util::next_float(input);
    x_size = util::next_float(input);
    y_size = util::next_float(input);
    z_size = util::next_float(input);
    x_rot = util::next_ushort(input);
    y_rot = util::next_ushort(input);
    z_rot = util::next_ushort(input);
    util::next_ushort(input);
}

void FalloutVolume::write(std::iostream& output) {
    util::write_float(output, center_x);
    util::write_float(output, center_y);
    util::write_float(output, center_z);
    util::write_float(output, x_size);
    util::write_float(output, y_size);
    util::write_float(output, z_size);
    util::write_ushort(output, x_rot);
    util::write_ushort(output, y_rot);
    util::write_ushort(output, z_rot);
    util::write_ushort(output, 0);
}

void BackgroundModel::read(std::iostream& input) {
    util::next_uint(input);
    uint name_offset = util::next_uint(input);
    util::next_uint(input);
    
    x_pos = util::next_float(input);
    y_pos = util::next_float(input);
    z_pos = util::next_float(input);
    x_rot = util::next_short(input);
    y_rot = util::next_short(input);
    z_rot = util::next_short(input);
    util::next_short(input);
    x_scale = util::next_float(input);
    y_scale = util::next_float(input);
    z_scale = util::next_float(input);
    
    uint anim_header_offset = util::next_uint(input);
    uint anim_header_2_offset = util::next_uint(input);
    uint effect_header_offset = util::next_uint(input);
    
    model_name = owner->_load_model_name(input, name_offset);
    anim_header = owner->_load_obj(input, anim_header_offset, owner->background_headers);
    anim_header_2 = owner->_load_obj(input, anim_header_2_offset, owner->background_headers_2);
    effect_header = owner->_load_obj(input, effect_header_offset, owner->effect_headers);
}

void BackgroundModel::write(std::iostream& output) {
    // TODO: need to handle writing offsets of sub-objects held by this object
}

void ReflectiveModel::read(std::iostream& input) {
    uint model_name_offset = util::next_uint(input);
    util::next_long(input);
    
    model_name = owner->_load_model_name(input, model_name_offset);
}

void ReflectiveModel::write(std::iostream& output) {
    // TODO: need to handle writing offsets of sub-objects held by this object
}

void LevelModelInstance::read(std::iostream& input) {
    uint level_model_pointer_offset = util::next_uint(input);
    x_pos = util::next_float(input);
    y_pos = util::next_float(input);
    z_pos = util::next_float(input);
    x_rot = util::next_ushort(input);
    y_rot = util::next_ushort(input);
    z_rot = util::next_ushort(input);
    util::next_ushort(input);
    x_scale = util::next_float(input);
    y_scale = util::next_float(input);
    z_scale = util::next_float(input);
    
    level_model_pointer = owner->_load_obj(input, level_model_pointer_offset, owner->level_pointer_As);
}

void LevelModelInstance::write(std::iostream& output) {
    // TODO: need to handle writing offsets of sub-objects held by this object
}

void Switch::read(std::iostream& input) {
    x_pos = util::next_float(input);
    y_pos = util::next_float(input);
    z_pos = util::next_float(input);
    x_rot = util::next_ushort(input);
    y_rot = util::next_ushort(input);
    z_rot = util::next_ushort(input);
    type = util::next_ushort(input);
    anim_ids = util::next_ushort(input);
    util::next_ushort(input);
}

void Switch::write(std::iostream& output) {
    // TODO
}

void FogAnimationHeader::read(std::iostream& input) {
    num_start_keyframes = util::next_int(input);
    start_keyframes_offset = util::next_int(input);
    num_end_keyframes = util::next_int(input);
    end_keyframes_offset = util::next_int(input);
    num_red_keyframes = util::next_int(input);
    red_keyframes_offset = util::next_int(input);
    num_green_keyframes = util::next_int(input);
    green_keyframes_offset = util::next_int(input);
    num_blue_keyframes = util::next_int(input);
    blue_keyframes_offset = util::next_int(input);
    num_unknown_keyframes = util::next_int(input);
    unknown_keyframes_offset = util::next_int(input);
}

void FogAnimationHeader::write(std::iostream& output) {
    // TODO
}

void Wormhole::read(std::iostream& input) {
    if (util::next_uint(input) != 1) {
        logger->warn("Malformed wormhole entry, file may be invalid");
    }
    x_pos = util::next_float(input);
    y_pos = util::next_float(input);
    z_pos = util::next_float(input);
    x_rot = util::next_ushort(input);
    y_rot = util::next_ushort(input);
    z_rot = util::next_ushort(input);
    util::next_ushort(input);
    
    destination = owner->_load_obj(input, util::next_uint(input), owner->wormholes);
}

void Wormhole::write(std::iostream& output) {
    // TODO
}

void Fog::read(std::iostream& input) {
    type = util::next_char(input);
    util::next_char(input, 3);
    start_dist = util::next_float(input);
    end_dist = util::next_float(input);
    red = util::next_float(input);
    green = util::next_float(input);
    blue = util::next_float(input);
    util::next_char(input, 12);
}

void Fog::write(std::iostream& output) {
    // TODO
}

}
