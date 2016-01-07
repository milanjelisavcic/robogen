/*
 * @(#) JSScenario.cpp   1.0   Dec 8, 2015
 *
 * Joshua Auerbach (joshua.auerbach@epfl.ch)
 *
 * The ROBOGEN Framework
 * Copyright © 2012-2015 Joshua Auerbach
 *
 * Laboratory of Intelligent Systems, EPFL
 *
 * This file is part of the ROBOGEN Framework.
 *
 * The ROBOGEN Framework is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License (GPL)
 * as published by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * @(#) $Id$
 */

#ifdef EMSCRIPTEN

#include "scenario/JSScenario.h"
#include "config/RobogenConfig.h"
#include "config/StartPositionConfig.h"
#include "scenario/Environment.h"
#include "Robot.h"
#include "utils/JSUtils.h"

#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.

#include <sstream>


boost::uuids::random_generator generator;

namespace robogen {

std::map<std::string, JSScenario*> JSScenario::scenarios;



JSScenario::JSScenario() : Scenario(boost::shared_ptr<RobogenConfig>()),
		curTrial_(0) {
}

JSScenario::~JSScenario() {
	js::log("destroying jsScenario!");
	scenarios.erase(id_);
	js::log("removed from map");

}

boost::shared_ptr<Scenario> JSScenario::createScenario(
		boost::shared_ptr<RobogenConfig> config) {


	// super hacky -- first we generate uuid
	std::string id;
	{
		boost::uuids::uuid uuid = generator();
		std::stringstream ss;
		ss << "_myUUID_" << uuid;
		id = ss.str();
		// replace all '-' with '_' to make valid var name
		std::replace( id.begin(), id.end(), '-', '_');
	}

	js::log("using id: ");
	js::log(id);

	// now call the provided js with creating a new object at the end
	// and registering it with the given uuid
	{
		std::stringstream ss;
		ss << id << " = function () {\n";
		ss << "var UserScenario_" << id << " = Module.JSScenario.extend(\"JSScenario\",";
		ss << config->getScenario() << "\n";
		ss << ");\n" << "return new UserScenario_" << id << ";";
		ss << "}();";
		ss << id << ".setId('" << id << "');";
		emscripten_run_script(ss.str().c_str());
	}

	//finally use the uuid to get the pointer to the create object
	JSScenario *scenario = JSScenario::getScenario(id);
	if (scenario == NULL) {
		js::log("Scenario is NULL!");
	} else {
		js::log("Scenario is not NULL!");
	}
	scenario->setRobogenConfig(config);

	return boost::shared_ptr<Scenario>(scenario);
}

bool JSScenario::endSimulation() {
	curTrial_++;
	this->setStartingPosition(curTrial_);
	return endSimulationJS();
}

void JSScenario::printRobotPosition() {
	osg::Vec3 pos = this->getRobot()->getCoreComponent()->getRootPosition();
	std::stringstream ss;
	ss << "console.log(\"" << pos[0] << " " <<pos[1] << " " << pos[2] << "\")";
	emscripten_run_script(ss.str().c_str());
}

bool JSScenario::remainingTrials() {
	boost::shared_ptr<StartPositionConfig> startPos = this->getRobogenConfig()->getStartingPos();
	return curTrial_ < startPos->getStartPosition().size();
}

int JSScenario::getCurTrial() const {
	return curTrial_;
}

}

#endif