/**
 * odsupercomponent - Configuration and monitoring component for
 *                    distributed software systems
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

#ifndef SUPERCOMPONENT_CONNECTEDMODULES_H_
#define SUPERCOMPONENT_CONNECTEDMODULES_H_

#include <map>
#include <string>
#include <vector>

#include "opendavinci/odcore/opendavinci.h"
#include "opendavinci/odcore/base/Mutex.h"

namespace odcore { namespace data { namespace dmcp { class ModuleDescriptor; } } }
namespace odcore { namespace data { namespace dmcp { class PulseMessage; } } }
namespace odcore { namespace data { class Container; } }

namespace odsupercomponent {

class ConnectedModule;

    using namespace std;

    class ConnectedModules {
        public:
            ConnectedModules();
            virtual ~ConnectedModules();

            void addModule(const odcore::data::dmcp::ModuleDescriptor& md, ConnectedModule* module);
            ConnectedModule* getModule(const odcore::data::dmcp::ModuleDescriptor& md);
            void removeModule(const odcore::data::dmcp::ModuleDescriptor& md);
            bool hasModule(const odcore::data::dmcp::ModuleDescriptor& md);

            /**
             * This method sends a pulse to all connected modules.
             *
             * @param pm Pulse to be sent.
             */
            void pulse(const odcore::data::dmcp::PulseMessage &pm);

            /**
             * This method sends a pulse to all connected modules but shifts
             * the alignment interval by shift microseconds for each connected
             * module.
             *
             * @param pm Pulse to be sent.
             * @shift Increment for each newly connected module.
             */
            void pulseShift(const odcore::data::dmcp::PulseMessage &pm, const uint32_t &shift);

            /**
             * This method sends a pulse to all connected modules and
             * requires an ACK confirmation sent from the respective,
             * dependent module that the PULSE has been processed.
             *
             * @param pm Pulse to be sent.
             * @param timeout Timeout in milliseconds to wait for an ACK from the dependent module.
             * @param yield Time to wait in microseconds before sending the pulse to the next module in the list.
             * @param modulesToIgnore Modules that are skipped when sending the pulse signal.
             */
            void pulse_ack(const odcore::data::dmcp::PulseMessage &pm, const uint32_t &timeout, const uint32_t &yield, const vector<string> &modulesToIgnore);

            /**
             * This method sends a pulse to all connected modules and
             * requires an ACK confirmation sent from the respective,
             * dependent module that the PULSE has been processed.
             *
             * @param pm Pulse to be sent.
             * @param timeout Timeout in milliseconds to wait for an ACK from the dependent module.
             * @param yield Time to wait in microseconds before sending the pulse to the next module in the list.
             * @param modulesToIgnore Modules that are skipped when sending the pulse signal.
             * @return Containers to be transferred to supercomponent.
             */
            vector<odcore::data::Container> pulse_ack_containers(const odcore::data::dmcp::PulseMessage &pm, const uint32_t &timeout, const uint32_t &yield, const vector<string> &modulesToIgnore);

            void deleteAllModules();

        protected:
            odcore::base::Mutex m_modulesMutex;
            map<string, ConnectedModule*> m_modules;

        private:
            ConnectedModules(const ConnectedModule &);
            ConnectedModules& operator=(const ConnectedModule &);
    };
}

#endif /*SUPERCOMPONENT_CONNECTEDMODULES_H_*/
