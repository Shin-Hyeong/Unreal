#pragma once

// #include "FolderName/DebugMacros.h"
#include "DrawDebugHelpers.h"

#define DRAW_SPHERE(Location) if (GetWorld()) DrawDebugSphere(GetWorld(), Location, 50.f, 48, FColor::Blue, true)
#define DRAW_LINE(StartLocation, EndLocation) if (GetWorld()) DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Yellow, true, -1.0f, 0, 1.0f)
#define DRAW_POINT(Location) if (GetWorld()) DrawDebugPoint(GetWorld(), Location, 15.f, FColor::Red, true)
#define DRAW_VECTOR(StartLocation, EndLocation)                                                      \
    if (GetWorld())                                                                                  \
    {                                                                                                \
        DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Yellow, true, -1.0f, 0, 1.0f); \
        DrawDebugPoint(GetWorld(), EndLocation, 15.f, FColor::Yellow, true);                         \
    }