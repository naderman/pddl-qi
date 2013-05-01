/**
 * This file is part of PddlQi
 *
 * @copyright Nils Adermann <naderman@naderman.de>
 *
 * For the full copyright and licensing information please review the LICENSE
 * file that was distributed with this source code.
 */

#include "Parser.h"

using namespace PddlQi;

PddlDomain Parser::parse(const std::string& input)
{
    Pddl pddlGrammar;

    PddlDomain domain;

    std::string::const_iterator iter = input.begin();
    std::string::const_iterator end = input.end();

    bool r = phrase_parse(
        iter,
        end,
        pddlGrammar,
        boost::spirit::ascii::space,
        domain
    );

    if (!r || iter != end)
    {
        throw ParserException();
    }

    return domain;
}
