/**
 * This file is part of PddlQi
 *
 * @copyright Nils Adermann <naderman@naderman.de>
 *
 * For the full copyright and licensing information please review the LICENSE
 * file that was distributed with this source code.
 */

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

#include <string>

namespace PddlQi
{
    namespace fusion = boost::fusion;
    namespace phoenix = boost::phoenix;
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;

    struct PddlDomain
    {
        std::string name;
    };
}

BOOST_FUSION_ADAPT_STRUCT(
    PddlQi::PddlDomain,
    (std::string, name)
)

namespace PddlQi
{
    template <typename Iterator>
    struct PddlGrammar :
        qi::grammar<Iterator, PddlDomain(), ascii::space_type>
    {
        PddlGrammar() :
            PddlGrammar::base_type(pddlDomain, "PDDL Domain")
        {
            using qi::lexeme;
            using qi::lit;
            using qi::on_error;
            using qi::fail;
            using ascii::char_;
            using namespace qi::labels;

            using phoenix::construct;
            using phoenix::val;

            name %=
                lexeme[char_("a-zA-Z") >> *(char_("a-zA-Z0-9_-"))];
            name.name("name");

            pddlDomain %=
                lit('(')
                >> "define"
                >> "("
                >> "domain"
                >> name
                >> ")"
                >> ")";
            pddlDomain.name("pddlDomain");

            on_error<fail>
            (
                pddlDomain,
                std::cout
                    << val("Parse Error: Expecting ")
                    << _4
                    << val(" here: \"")
                    << construct<std::string>(_3, _2)
                    << val("\"")
                    << std::endl
            );
        }

        qi::rule<Iterator, PddlDomain(), ascii::space_type> pddlDomain;
        qi::rule<Iterator, std::string(), ascii::space_type> name;
    };
}
