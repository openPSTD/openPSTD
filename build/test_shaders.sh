#!/usr/bin/env bash

SHADER_DIRECTORY=GUI/GPU/

echo ===============Debug2D===============
glslangValidator ${SHADER_DIRECTORY}Debug2D.frag ${SHADER_DIRECTORY}Debug2D.vert

echo ===============Grid===============
glslangValidator ${SHADER_DIRECTORY}Grid.frag ${SHADER_DIRECTORY}Line.geom ${SHADER_DIRECTORY}Grid.vert

echo ===============Interactive===============
glslangValidator ${SHADER_DIRECTORY}Interactive.frag ${SHADER_DIRECTORY}Interactive.vert

echo ===============Scene2D===============
glslangValidator ${SHADER_DIRECTORY}Scene2D.frag ${SHADER_DIRECTORY}Line.geom ${SHADER_DIRECTORY}Scene2D.vert

echo ===============Simulation2D===============
glslangValidator ${SHADER_DIRECTORY}Simulation2D.frag ${SHADER_DIRECTORY}Simulation2D.vert

echo ===============Symbol===============
glslangValidator ${SHADER_DIRECTORY}Symbol.frag ${SHADER_DIRECTORY}Symbol.geom ${SHADER_DIRECTORY}Symbol.vert

echo ===============SymbolInternal===============
glslangValidator ${SHADER_DIRECTORY}Symbol.frag ${SHADER_DIRECTORY}SymbolInternal.geom ${SHADER_DIRECTORY}Symbol.vert

echo ===============Text===============
glslangValidator ${SHADER_DIRECTORY}Text.frag ${SHADER_DIRECTORY}Text.vert
