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

#include <boost/spirit/include/classic_parse_tree.hpp>

#include "opendavinci/odcore/opendavinci.h"
#include "opendlv/core/wrapper/parser/ASTNode.h"
#include "opendlv/core/wrapper/parser/BoostSpiritGrammar.h"
#include "opendlv/scenario/SCNGrammar.h"

namespace core { namespace wrapper { namespace parser { class ParserErrorListener; } } }
namespace core { namespace wrapper { namespace parser { class ParserTokenListener; } } }

namespace opendlv {
    namespace scenario {

        using namespace core::wrapper::parser;

        SCNGrammarTokenIdentifier::SCNGrammarTokenIdentifier(const enum IDENTIFIERS &id) :
                m_id(id) {}

        SCNGrammarTokenIdentifier::~SCNGrammarTokenIdentifier() {}

        enum SCNGrammarTokenIdentifier::IDENTIFIERS SCNGrammarTokenIdentifier::getIdentifier() const {
            return m_id;
        }

        SCNGrammar::SCNGrammar(ParserTokenListener &ptl, ParserErrorListener &pel) :
                BoostSpiritGrammar(ptl, pel),
                expect(0),
                m_guard() {}

        SCNGrammar::~SCNGrammar() {}

        ASTNode* SCNGrammar::getAST(const string &s) {
            // Reset lines counter.
            resetLines();

            // Parse the data and build an AST.
            ASTNode *root = NULL;
            tree_parse_info<> info = ast_parse(s.c_str(), *this, comment_p("/*", "*/"));

            // If the instance of the grammar could successfully parsed, build the AST.
            if (info.full == 1) {
                root = new ASTNode();
                buildAST(info.trees.begin(), root);
            }

            return root;
        }

    }
} // opendlv::scenario
