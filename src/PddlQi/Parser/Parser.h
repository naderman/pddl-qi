/**
 * This file is part of PddlQi
 *
 * @copyright Nils Adermann <naderman@naderman.de>
 *
 * For the full copyright and licensing information please review the LICENSE
 * file that was distributed with this source code.
 */

#include <PddlQi/Parser/PddlGrammar.h>

namespace PddlQi
{
    class Parser
    {
        public:
            PddlDomain parseDomain(const std::string& input)
            {
                return parse<Grammar::Domain<std::string::const_iterator>, PddlDomain>(input);
            }

            PddlAction parseAction(const std::string& input)
            {
                return parse<Grammar::Action<std::string::const_iterator>, PddlAction>(input);
            }

            template <typename Grammar, typename Attribute>
            Attribute parse(const std::string& input)
            {
                Grammar grammar;

                Attribute data;

                std::string::const_iterator iter = input.begin();
                std::string::const_iterator end = input.end();

                bool r = phrase_parse(
                    iter,
                    end,
                    grammar,
                    boost::spirit::ascii::space,
                    data
                );

                if (!r || iter != end)
                {
                    throw ParserException(input.begin(), iter, end);
                }

                return data;
            }
    };
}
