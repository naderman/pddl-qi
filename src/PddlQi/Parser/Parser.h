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
            typedef PddlGrammar<std::string::const_iterator> Pddl;

            PddlDomain parse(const std::string& input);
    };
}
