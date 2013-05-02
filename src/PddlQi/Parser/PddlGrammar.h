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
#include <boost/spirit/include/phoenix_bind.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

#include <string>
#include <vector>

namespace PddlQi
{
    namespace fusion = boost::fusion;
    namespace phoenix = boost::phoenix;
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;

    typedef struct RequirementFlag_ {
        typedef enum {
            eStrips,
            eNegativePreconditions
        } EnumType;
        typedef std::vector<EnumType> VectorType;
    } RequirementFlag;

    struct PddlDomain
    {
        std::string name;
        RequirementFlag::VectorType requirements;
    };
}

BOOST_FUSION_ADAPT_STRUCT(
    PddlQi::PddlDomain,
    (std::string, name)
    (std::vector<PddlQi::RequirementFlag::EnumType>, requirements)
)

namespace PddlQi
{
    struct RequirementFlagSymbols_ :
        qi::symbols<char, PddlQi::RequirementFlag::EnumType>
    {
        RequirementFlagSymbols_()
        {
            add
                (":strips", RequirementFlag::eStrips)
                (":negative-preconditions", RequirementFlag::eNegativePreconditions)
            ;
        }
    };

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
            using phoenix::at_c;
            using phoenix::if_else;

            name %=
                lexeme[char_("a-zA-Z") >> *(char_("a-zA-Z0-9_-"))];
            name.name("name");

            requireDef %=
                lit('(')
                >> lit(":requirements")
                > (+(requirementFlagSymbols))
                > lit(')');
            requireDef.name("requireDef");

            pddlDomain =
                lit('(')
                > lit("define")
                > lit('(')
                > lit("domain")
                > name[at_c<0>(_val) = _1]
                > lit(')')
                >> (-requireDef)[
                    at_c<1>(_val) = boost::phoenix::bind(
                        static_cast<const RequirementFlag::VectorType& (boost::optional<RequirementFlag::VectorType>::*) (const RequirementFlag::VectorType&) const>(
                            &boost::optional<RequirementFlag::VectorType>::get_value_or
                       ),
                       _1,
                        RequirementFlag::VectorType()
                    )]
                > lit(')');
            pddlDomain.name("pddlDomain");

            on_error<fail>
            (
                pddlDomain,
                std::cerr
                    << val("Parse Error: Expecting ")
                    << _4
                    << val(" here: \"")
                    << construct<std::string>(_3, _2)
                    << val("\"")
                    << std::endl
            );
        }

        qi::rule<Iterator, PddlDomain(), ascii::space_type> pddlDomain;
        qi::rule<Iterator, RequirementFlag::VectorType(), ascii::space_type> requireDef;
        qi::rule<Iterator, std::string(), ascii::space_type> name;

        struct RequirementFlagSymbols_ requirementFlagSymbols;
    };
}
