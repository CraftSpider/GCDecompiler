//
// Created by Rune Tynan on 8/11/2018.
//

#pragma once

#include <string>
#include <map>
#include "types.h"
#include "datatypes/stageobjects.h"

namespace types {

class STAGEDEF {

private:

public:
    
    uint num_collision, collision_offset, start_pos_offset, fallout_plane_offset, num_bumpers, bumper_offset,
    num_jamabars, jamabar_offset, num_bananas, banana_offset, num_cones, cone_offset, num_spheres, sphere_offset,
    num_cylinders, cylinder_offset, num_fallouts, fallouts_offset, num_backgrounds, background_offset, num_unkown8,
    unknown8_offset, num_reflective, reflective_offset, num_model_instances, model_instances_offset, num_models_a,
    models_a_offset, num_models_b, models_b_offset, num_switches, switch_offset, fog_offset, fog_anim_offset,
    num_wormholes, wormhole_offset, unknown3_offset;
    
    // TODO: make these vectors, auto destructed and allows editing easily
    StartPos start;
    FalloutPlane fallout_plane;
    FogAnimationHeader* fog_anim_header;
    Fog* fog;
    std::map<uint, Goal*> goals;
    std::map<uint, Bumper*> bumpers;
    std::map<uint, Jamabar*> jamabars;
    std::map<uint, Banana*> bananas;
    std::map<uint, FalloutVolume*> fallout_volumes;
    std::map<uint, Switch*> switches;
    std::map<uint, Wormhole*> wormholes;
    std::map<uint, ConeCollision*> cones;
    std::map<uint, SphereCollision*> spheres;
    std::map<uint, CylinderCollision*> cylinders;
    std::map<uint, BackgroundModel*> backgrounds;
    std::map<uint, ReflectiveModel*> reflective_models;
    std::map<uint, LevelModelInstance*> model_instances;
    
    std::vector<std::string> model_names;
    std::map<uint, BackgroundAnimationHeader*> background_headers;
    std::map<uint, BackgroundAnimationHeader2*> background_headers_2;
    std::map<uint, EffectHeader*> effect_headers;
    std::map<uint, LevelModelPointerA*> level_pointer_As;
    std::map<uint, LevelModelPointerB*> level_pointer_Bs;
    
    STAGEDEF(const std::string &filename);
    
    std::string* _load_model_name(std::iostream& input, uint offset);
    
    template<typename T>
    T* _load_obj(std::iostream& input, ulong offset, std::map<uint, T*>& cache);
    
    template<typename T>
    T* _load_obj(std::iostream& input, std::map<uint, T*>& cache);
    
    void save_smb1(const std::string &output);
    
    void save_smb2(const std::string &output);
    
};

template<typename T>
T* STAGEDEF::_load_obj(std::iostream& input, ulong offset, std::map<uint, T*>& cache) {
    if (!cache.count(offset)) {
        size_t pos = input.tellg();
        input.seekg(offset);
        T* obj = new T {};
        obj->read(input);
        cache[offset] = obj;
        input.seekg(pos + sizeof(T));
    }
    
    return cache[offset];
}

template<typename T>
T* STAGEDEF::_load_obj(std::iostream& input, std::map<uint, T*>& cache) {
    return this->_load_obj(input, input.tellg(), cache);
}

}
