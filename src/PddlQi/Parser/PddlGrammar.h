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

    struct Entity
    {
        std::string name;
        std::string type;

        Entity(const std::string n, const std::string t) : name(n), type(t) {}
    };

    typedef std::vector<struct Entity> TypedNameList;

    struct PddlDomain
    {
        std::string name;
        RequirementFlag::VectorType requirements;
        TypedNameList constants;
    };
}

BOOST_FUSION_ADAPT_STRUCT(
    PddlQi::Entity,
    (std::string, name)
    (std::string, type)
)

BOOST_FUSION_ADAPT_STRUCT(
    PddlQi::PddlDomain,
    (std::string, name)
    (PddlQi::RequirementFlag::VectorType, requirements)
    (PddlQi::TypedNameList, constants)
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

    template <typename Inner>
    const Inner& optional_get_value_or(const boost::optional<Inner>& opt, const Inner& def)
    {
        return opt.get_value_or(def);
    }

    void insert_typed_name_entities(TypedNameList& entities, const std::vector<std::string>& names, const std::string& type)
    {
        std::for_each(names.begin(), names.end(), (
            phoenix::push_back(phoenix::ref(entities), phoenix::construct<struct Entity>(phoenix::arg_names::_1, phoenix::ref(type)))
        ));
    }

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
            using phoenix::bind;
            using phoenix::push_back;

            name %=
                lexeme[char_("a-zA-Z") >> *(char_("a-zA-Z0-9_-"))];
            name.name("name");

            type %= name;
            type.name("type");

            typedNameList =
                (*((+(name[push_back(_a, _1)]))
                 >> lit('-')
                 > type[bind(&insert_typed_name_entities, _val, _a, _1)]))
                >> (*(name[push_back(_val, construct<struct Entity>(_1, "object"))]))
                ;
            typedNameList.name("typedNameList");

            requireDef %=
                lit('(')
                >> lit(":requirements")
                > (+(requirementFlagSymbols))
                > lit(')');
            requireDef.name("requireDef");

            constantsDef %=
                lit('(')
                >> lit(":constants")
                > typedNameList
                > lit(')')
                ;
            constantsDef.name("constantsDef");

            pddlDomain =
                lit('(')
                > lit("define")
                > lit('(')
                > lit("domain")
                > name[at_c<0>(_val) = _1]
                > lit(')')
                >> (-requireDef)[
                    at_c<1>(_val) = bind(
                        &optional_get_value_or<RequirementFlag::VectorType>,
                        _1, RequirementFlag::VectorType()
                    )]
                >> (-constantsDef)[
                    at_c<2>(_val) = bind(
                        &optional_get_value_or<TypedNameList>,
                        _1, TypedNameList()
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
        qi::rule<Iterator, TypedNameList(), ascii::space_type> constantsDef;
        qi::rule<Iterator, TypedNameList(), qi::locals<std::vector<std::string> >, ascii::space_type> typedNameList;
        qi::rule<Iterator, std::string(), ascii::space_type> type;
        qi::rule<Iterator, std::string(), ascii::space_type> name;

        struct RequirementFlagSymbols_ requirementFlagSymbols;
    };
}
