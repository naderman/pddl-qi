/**
 * This file is part of PddlQi
 *
 * @copyright Nils Adermann <naderman@naderman.de>
 *
 * For the full copyright and licensing information please review the LICENSE
 * file that was distributed with this source code.
 */

#include <boost/spirit/home/support/info.hpp>

#include <sstream>
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

            template <typename Iterator>
            ParserException(const boost::spirit::info& expectedRule, Iterator start, Iterator end, Iterator current) :
                exception()
            {
                std::stringstream messageStream;
                messageStream << "Parse Error: Expected: ";
                messageStream << expectedRule;
                messageStream << " here: ";
                messageStream << std::string(current, end);

                message = messageStream.str();
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
}

