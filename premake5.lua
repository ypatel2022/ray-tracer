-- premake5.lua
workspace "ray-tracer"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "ray-tracer"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
include "Walnut/WalnutExternal.lua"

include "ray-tracer"