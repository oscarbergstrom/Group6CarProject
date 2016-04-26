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

#include <string>

#include "opendlv/threeD/NodeDescriptorComparator.h"

namespace opendlv {
    namespace threeD {

        NodeDescriptorComparator::NodeDescriptorComparator() {}

        NodeDescriptorComparator::NodeDescriptorComparator(const NodeDescriptorComparator &/*obj*/) {}

        NodeDescriptorComparator::~NodeDescriptorComparator() {}

        NodeDescriptorComparator& NodeDescriptorComparator::operator=(const NodeDescriptorComparator &/*obj*/) {
            return (*this);
        }

        bool NodeDescriptorComparator::operator()(const NodeDescriptor &nd1, const NodeDescriptor &nd2) const {
            return nd1.getName().compare(nd2.getName()) < 0;
        }

    }
} // opendlv::threeD
