/*
 * robogen_js.cpp
 *
 *  Created on: Aug 11, 2015
 *      Author: guillaume3
 */
#include <iostream>
#include <emscripten/bind.h>

/***** WARNING ******
 * WE ARE INCLUDING .cpp files !!
 * This file is "merging" cpp files and expose functions to the javascript world
 ********************/

#include <Evolver.cpp>
#include <viewer/FileViewer.cpp>

using namespace robogen;

int main(int argc, char** argv) {
	std::cout << "A robogenJS worker has started, he is waiting for any task" << std::endl;
	return 0;
}

EMSCRIPTEN_BINDINGS(my_module) {
	emscripten::function("simulationViewer", &simulationViewer);
	emscripten::function("runEvolution", &robogen::runEvolution);
}

