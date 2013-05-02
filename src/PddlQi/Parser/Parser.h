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
        public:
            ParserException() : exception()
            {
                message = "Unknown ParserException";
            }

            template <typename Iterator>
            ParserException(Iterator start, Iterator current, Iterator end) :
                exception()
            {
                message = "Unknown error occured here: ";
                message += std::string(current, end);
            }

            ParserException(const ParserException& src)
            {
                message = src.message;
            }

            ParserException& operator=(const ParserException& rhs)
            {
                message = rhs.message;
                return *this;
            }

            virtual ~ParserException() throw() {}

            virtual const char* what() const throw()
            {
                return message.c_str();
            }
        protected:
            std::string message;
    };

    class Parser
    {
        public:
            PddlDomain parseDomain(const std::string& input)
            {
                return parse<DomainGrammar<std::string::const_iterator>, PddlDomain>(input);
            }

            PddlAction parseAction(const std::string& input)
            {
                return parse<ActionGrammar<std::string::const_iterator>, PddlAction>(input);
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
