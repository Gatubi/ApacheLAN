#define WIN32_LEAN_AND_MEAN // Esto le dice al sistema: "No me metas mierdas del siglo pasado"
#pragma once
#include "types.h"  // Esto le dice al header qué es Vector3
#include "ApacheConfig.h"
#include <string>

void TeleportToArmadillo();

void SendPosition(Vector3 coords);

void StartApacheClient(const ApacheConfig& config);

void StopApacheClient();

