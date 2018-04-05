cmake_minimum_required(VERSION 2.4)

if(COMMAND cmake_policy)
      cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

PROJECT(tp2)

##Indiquez le chemin vers les fichiers include d'IDS
INCLUDE_DIRECTORIES("C:/Program Files/MobileRobots/Aria/include")

##Indiquez le chemin vers les fichiers lib d'IDS
LINK_DIRECTORIES("C:/Program Files/MobileRobots/Aria/lib64")

ADD_EXECUTABLE(tp2 main.cpp)

##Decommentez et verifiez le nom du fichier lib d'IDS
TARGET_LINK_LIBRARIES(tp2 AriaVC14.lib)
