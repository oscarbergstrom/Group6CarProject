/**
 * OpenDLV - Simulation environment
 * Copyright (C) 2008 - 2015 Christian Berger, Bernhard Rumpe
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <iostream>

#include "opendavinci/odcore/base/Lock.h"
#include "opendavinci/odcore/base/Mutex.h"
#include "opendlv/data/scenario/Scenario.h"
#include "opendlv/scenario/SCNXArchive.h"
#include "opendlv/scenegraph/SceneNode.h"
#include "opendlv/scenegraph/transformation/ScenarioTransformation.h"
#include "opendlv/scenegraph/transformation/SceneGraphFactory.h"

namespace opendlv {
    namespace scenegraph {
        namespace transformation {

            using namespace std;
            using namespace odcore::base;
            using namespace opendlv::data::environment;
            using namespace data::scenario;
            using namespace scenario;

            // Initialize singleton instance.
            Mutex SceneGraphFactory::m_singletonMutex;
            SceneGraphFactory* SceneGraphFactory::m_singleton = NULL;

            SceneGraphFactory::SceneGraphFactory() {}

            SceneGraphFactory::~SceneGraphFactory() {
                SceneGraphFactory::m_singleton = NULL;
            }

            SceneGraphFactory& SceneGraphFactory::getInstance() {
                {
                    Lock l(SceneGraphFactory::m_singletonMutex);
                    if (SceneGraphFactory::m_singleton == NULL) {
                        SceneGraphFactory::m_singleton = new SceneGraphFactory();
                    }
                }

                return (*SceneGraphFactory::m_singleton);
            }

            SceneNode* SceneGraphFactory::transform(scenario::SCNXArchive &scnxArchive) {
                return transform(scnxArchive, true);
            }

            SceneNode* SceneGraphFactory::transform(scenario::SCNXArchive &scnxArchive, const bool &showLaneConnectors) {
                SceneNode *sn = new SceneNode();

                if (scnxArchive.getAerialImage() != NULL) {
                    clog << "SceneGraphFactory: Found AerialImage but currently no transformation available." << endl;
                }

                ScenarioTransformation scenarioTransformer(showLaneConnectors);

                Scenario &scenario = scnxArchive.getScenario();
                scenario.accept(scenarioTransformer);

                sn->addChild(scenarioTransformer.getRoot());

                return sn;
            }

        }
    }
} // opendlv::scenegraph::transformation
