set(TARGETLIB "SolitaireLib")

#Directory of header files
set(INCLUDE_DIR "${Solitaire_SOURCE_DIR}/include/Solitaire")

#Directory of source files
set(SRC_DIR "${Solitaire_SOURCE_DIR}/src/Solitaire")

#Header files
set(HEADERS 
		"${INCLUDE_DIR}/Card.h"
		"${INCLUDE_DIR}/CardPile.h"
		"${INCLUDE_DIR}/CardSprite.h"
		"${INCLUDE_DIR}/DragDropManager.h"
		"${INCLUDE_DIR}/EventListener.h"
		"${INCLUDE_DIR}/Game.h"
		"${INCLUDE_DIR}/GameEvent.h"
		"${INCLUDE_DIR}/Logger.h"
		"${INCLUDE_DIR}/TextureManager.h"
)

#Source files
set(SRC
		"${SRC_DIR}/Card.cpp"
		"${SRC_DIR}/CardPile.cpp"
		"${SRC_DIR}/CardSprite.cpp"
		"${SRC_DIR}/DragDropManager.cpp"
		"${SRC_DIR}/EventListener.cpp"
		"${SRC_DIR}/Game.cpp"
		"${SRC_DIR}/GameEvent.cpp"
		"${SRC_DIR}/Logger.cpp"
		"${SRC_DIR}/TextureManager.cpp"
)

#Make static library
add_library(${TARGETLIB} STATIC ${SRC} ${HEADERS})

#Includes
target_include_directories(${TARGETLIB} PUBLIC ${INCLUDE_DIR})

#Set minimum standard c++17
target_compile_features(${TARGETLIB} PUBLIC cxx_std_17)


find_package(SFML 2.5 COMPONENTS graphics audio REQUIRED)
target_link_libraries(${TARGETLIB} PRIVATE sfml-graphics sfml-audio)