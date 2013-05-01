/**
 * This file is part of PddlQi
 *
 * @copyright Nils Adermann <naderman@naderman.de>
 *
 * For the full copyright and licensing information please review the LICENSE
 * file that was distributed with this source code.
 */

#include <PddlQi/Parser/PddlGrammar.h>
#include <exception>

namespace PddlQi
{
    class ParserException : std::exception
    {
        virtual const char* what() const throw()
        {
            return "Parser Exception";
        }
    };

    class Parser
    {
        public:
            typedef PddlGrammar<std::string::const_iterator> Pddl;

            PddlDomain parse(const std::string& input);
    };
}
