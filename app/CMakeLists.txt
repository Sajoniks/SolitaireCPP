add_executable(SolitaireApp "SFML.cpp")

target_compile_features(SolitaireApp PRIVATE cxx_std_17)

find_package(SFML 2.5 COMPONENTS graphics audio REQUIRED)
target_link_libraries(SolitaireApp PUBLIC sfml-graphics sfml-audio PRIVATE SolitaireLib)