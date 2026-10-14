#pragma once
#include "json.hpp"
namespace cs {
inline Json v6_preset(){return Json::parse(R"config({
  "simulation_version": "V6", "grid_size": 80, "world_size": 28.0,
  "tissue_rings": 1, "single_cell_start": true, "cell_radius": 0.45, "dt": 0.04,
  "photosynthesis_rate": 0.32, "light_attenuation": 0.12,
  "base_cost": 0.003, "specialization_cost": 0.002,
  "growth_rate": 0.3, "growth_efficiency": 0.65,
  "division_mass": 1.6, "division_energy": 0.02,
  "clearance_inside_fraction": 0.0, "membrane_surface_bias": 1.0,
  "connected_clearance": true,
  "homeostasis_production": 0.006, "homeostasis_threshold": 0.13,
  "homeostasis_diffusion": 0.3, "homeostasis_decay": 0.02,
  "mechanical_mobility": 1.0, "mechanical_damping": 5.0,
  "repulsion_stiffness": 30.0, "adhesion_strength": 0.7,
  "adhesion_range": 0.15, "geometry_update_interval": 5,
  "division_check_interval": 10, "max_cells": 0, "auto_expand_world": true,
  "generation_time_limit": 0.0, "division_mutation_probability": 0.0,
  "evolution_mutation_probability": 0.2, "evolution_mutation_sigma": 0.12,
  "motility_rate": 0.025, "motility_cost": 0.02,
  "signal_decay": 0.12, "signal_cost": 0.0005,
  "channel_maintenance_cost": 0.0005, "catabolism_rate": 0.01,
  "minimum_cell_mass": 0.2,
  "networks_enabled": false, "display_cell_scale": 0.65,
  "waste_channel_flow_scale": 0.0003, "channel_budget": 2.5,
  "mutation_probability": 0.0, "mutation_sigma": 0.04,
  "substances": {
    "sugar": {"contact_rate": 4.0, "membrane_rate": 0.01, "initial_cell": 0.3},
    "waste": {"contact_rate": 4.0, "membrane_rate": 3.0, "clearance_rate": 0.5,
              "diffusion_inside": 0.01}
  }
})config");}
inline Json v7_preset(){
 auto j=v6_preset();
 j["simulation_version"]="V7";
 j["fragmentation_enabled"]=true;
 j["fragment_min_cells"]=4;
 j["fragment_signal_rate"]=0.12;
 j["fragment_signal_decay"]=0.035;
 j["fragment_signal_threshold"]=0.58;
 j["fragment_detachment_strength"]=10.0;
 j["fragment_mutation_probability"]=0.2;
 j["fragment_mutation_sigma"]=0.12;
 return j;
}
inline Json v8_preset(){auto j=v7_preset();j["simulation_version"]="V8";j["fragment_min_cells"]=48;j["fragment_signal_threshold"]=.45;j["fragment_detachment_strength"]=2.5;j["bud_drift"]=.035;return j;}
}
