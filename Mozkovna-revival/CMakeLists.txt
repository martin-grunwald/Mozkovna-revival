# CMakeList.txt: Projekt CMake pro Mozkovna-revival, sem přidejte logiku zdrojového
# kódu a definic specifickou pro projekt.
#

# Přidejte zdrojový kód ke spustitelnému souboru tohoto projektu.
add_executable (Mozkovna-revival "Mozkovna-revival.cpp" "Mozkovna-revival.h" "Mozkovna-revival-funkce.cpp" "Mozkovna-revival-funkce.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Mozkovna-revival PROPERTY CXX_STANDARD 20)
endif()

# TODO: V případě potřeby přidejte testy a cíle instalace.
