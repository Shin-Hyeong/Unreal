#pragma once

// #include "FolderName/DebugMacros.h"
#include "DrawDebugHelpers.h"

#define DRAW_SPHERE(Location) if (GetWorld()) DrawDebugSphere(GetWorld(), Location, 75.f, 24, FColor::Blue, true)
#define DRAW_LINE(StartLocation, EndLocation) if (GetWorld()) DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Yellow, true, -1.0f, 0, 1.0f)
#define DRAW_POINT(Location) if (GetWorld()) DrawDebugPoint(GetWorld(), Location, 15.f, FColor::Red, true)
#define DRAW_VECTOR(StartLocation, EndLocation)                                                      \
    if (GetWorld())                                                                                  \
    {                                                                                                \
        DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Yellow, true, -1.0f, 0, 1.0f); \
        DrawDebugPoint(GetWorld(), EndLocation, 15.f, FColor::Yellow, true);                         \
    }

// 1프레임만 유지하는 Sphere
#define DRAW_SPHERE_SingleFrame(Location) if (GetWorld()) DrawDebugSphere(GetWorld(), Location, 75.f, 24, FColor::Green, false, -1.f)
// 1프레임만 유지하는 Line
#define DRAW_LINE_SingleFrame(StartLocation, EndLocation) if (GetWorld()) DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Green, false, -1.0f, 0, 1.0f)
// 1프레이만 유지하는 Point
#define DRAW_POINT_SingleFrame(Location) if (GetWorld()) DrawDebugPoint(GetWorld(), Location, 15.f, FColor::Green, false, -1.f)
// 1프레이만 유지하는 Vector
#define DRAW_VECTOR_SingleFrame(StartLocation, EndLocation)                                                      \
    if (GetWorld())                                                                                  \
    {                                                                                                \
        DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Green, false, -1.0f, 0, 1.0f); \
        DrawDebugPoint(GetWorld(), EndLocation, 15.f, FColor::Green, false, -1.f);                         \
    }