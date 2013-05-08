/**
 * This file is part of PddlQi
 *
 * @copyright Nils Adermann <naderman@naderman.de>
 *
 * For the full copyright and licensing information please review the LICENSE
 * file that was distributed with this source code.
 */

#include <PddlQi/Parser/PddlAst.h>
#include <PddlQi/Parser/ParserException.h>

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/spirit/include/phoenix_bind.hpp>

namespace PddlQi
{
    namespace Grammar
    {
        namespace fusion = boost::fusion;
        namespace phoenix = boost::phoenix;
        namespace qi = boost::spirit::qi;
        namespace ascii = boost::spirit::ascii;

        using qi::lexeme;
        using qi::lit;
        using qi::lazy;
        using qi::on_error;
        using qi::fail;
        using ascii::char_;
        using namespace qi::labels;

        using phoenix::construct;
        using phoenix::val;
        using phoenix::ref;
        using phoenix::at_c;
        using phoenix::if_else;
        using phoenix::bind;
        using phoenix::push_back;
        using phoenix::insert;
        using phoenix::clear;
        using phoenix::begin;
        using phoenix::end;

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

        void insert_typed_name_entities(TypedList& entities, const std::vector<std::string>& names, const std::string& type)
        {
            std::for_each(names.begin(), names.end(), (
                phoenix::push_back(phoenix::ref(entities), phoenix::construct<struct Entity>(phoenix::arg_names::_1, phoenix::ref(type)))
            ));
        }

        template <typename Iterator>
        struct BaseGrammar
        {
            BaseGrammar()
            {
                name %= lexeme[char_("a-zA-Z") >> *(char_("a-zA-Z0-9_-"))];
                name.name("name");

                variable %= lit('?') > name;
                variable.name("variable");

                type %= name;
                type.name("type");

                typedListExplicitType = (+(lazy(_r1)[push_back(_a, _1)]))
                     >> lit('-')
                     > type[bind(&insert_typed_name_entities, _val, _a, _1)];
                typedListExplicitType.name("typedListExplicitType");

                typedList =
                    (*(typedListExplicitType(_r1)[insert(_val, end(_val), begin(_1), end(_1))]))
                    >> (*(lazy(_r1)[push_back(_val, construct<struct Entity>(_1, "object"))]))
                    ;
                typedList.name("typedList");

                term = name | variable;
                term.name("term");

                atomicFormula = lit('(') >> name >> *term >> lit(')');
                atomicFormula.name("atomicFormula");

                literal = atomicFormula |
                    (lit('(') >> lit("not") > atomicFormula > lit(')'));
                literal.name("literal");

                goalDescription =
                    (lit('(') >> lit("and") >> (*goalDescription) >> lit(')')) |
                    (lit('(') >> lit("or") >> (*goalDescription) >> lit(')')) |
                    (lit('(') >> lit("not") >> goalDescription >> lit(')')) |
                    (lit('(') >> lit("imply") >> goalDescription >> goalDescription >> lit(')')) |
                    (lit('(') >> lit("exists") > lit('(') >> typedList(ref(variable)) >> lit(')') >> goalDescription >> lit(')')) |
                    (lit('(') >> lit("forall") > lit('(') >> typedList(ref(variable)) >> lit(')') >> goalDescription >> lit(')')) |
                    literal |
                    atomicFormula;
                goalDescription.name("goalDescription");

                effect =
                    (lit('(') >> lit("and") >> (*effect) >> lit(')')) |
                    (lit('(') >> lit("not") >> atomicFormula >> lit(')')) |
                    atomicFormula |
                    (lit('(') >> lit("forall") > lit('(') >> *variable >> lit(')') >> effect >> lit(')')) |
                    (lit('(') >> lit("when") > goalDescription >> effect >> lit(')'));
                effect.name("effect");
            }

            typedef qi::rule<Iterator, std::string(), ascii::space_type> StringRule;

            qi::rule<Iterator, void(), ascii::space_type> effect;
            qi::rule<Iterator, void(), ascii::space_type> goalDescription;
            qi::rule<Iterator, void(), ascii::space_type> literal;
            qi::rule<Iterator, void(), ascii::space_type> atomicFormula;
            qi::rule<Iterator, void(), ascii::space_type> term;
            qi::rule<Iterator, TypedList(StringRule), ascii::space_type> typedList;
            qi::rule<Iterator, TypedList(StringRule), qi::locals<std::vector<std::string> >, ascii::space_type> typedListExplicitType;
            StringRule type;
            StringRule name;
            StringRule variable;
        };

        template <typename Iterator>
        struct Action :
            qi::grammar<Iterator, PddlAction(), ascii::space_type>, BaseGrammar<Iterator>
        {
            typedef BaseGrammar<Iterator> base;

            Action() :
                Action::base_type(pddlAction, "PDDL Action"), BaseGrammar<Iterator>()
            {
                pddlAction =
                    lit('(')
                    > lit(":action")
                    > base::name[at_c<0>(_val) = _1]
                    > lit(":parameters")
                    > lit("(")
                    > base::typedList(ref(base::variable))[at_c<1>(_val) = _1]
                    > lit(")")
                    >> -(
                        lit(":precondition")
                        >> base::goalDescription
                    )
                    >> -(
                        lit(":effect")
                        >> base::effect
                    )
                    > lit(')');
                pddlAction.name("pddlAction");

                on_error<fail>
                (
                    pddlAction,
                    construct<ParserException>(_4, _1, _2, _3)
                );
            }

            qi::rule<Iterator, PddlAction(), ascii::space_type> pddlAction;
        };

        template <typename Iterator>
        struct Domain :
            qi::grammar<Iterator, PddlDomain(), ascii::space_type>, BaseGrammar<Iterator>
        {
            typedef BaseGrammar<Iterator> base;

            Domain() :
                Domain::base_type(pddlDomain, "PDDL Domain"), BaseGrammar<Iterator>()
            {
                requireDef %= -(
                    lit('(')
                    >> lit(":requirements")
                    > (+(requirementFlagSymbols))
                    > lit(')')
                    );
                requireDef.name("requireDef");

                constantsDef %= -(
                    lit('(')
                    >> lit(":constants")
                    > base::typedList(ref(base::name))
                    > lit(')')
                    );
                constantsDef.name("constantsDef");

                predicatesDef = -(
                    lit('(')
                    >> lit(":predicates")
                    > (+(lit('(')
                        > base::name[_a = _1]
                        >> base::typedList(ref(base::variable))[_b = _1]
                        > lit(')'))[push_back(_val, construct<std::pair<std::string, TypedList> >(_a, _b))]
                    )
                    > lit(')')
                    );
                predicatesDef.name("predicatesDef");

                pddlDomain =
                    lit('(')
                    > lit("define")
                    > lit('(')
                    > lit("domain")
                    > base::name[at_c<0>(_val) = _1]
                    > lit(')')
                    >> requireDef[at_c<1>(_val) = _1]
                    >> constantsDef[at_c<2>(_val) = _1]
                    >> predicatesDef[at_c<3>(_val) = _1]
                    > lit(')');
                pddlDomain.name("pddlDomain");

                on_error<fail>
                (
                    pddlDomain,
                    construct<ParserException>(_4, _1, _2, _3)
                );
            }

            qi::rule<Iterator, PddlDomain(), ascii::space_type> pddlDomain;
            qi::rule<Iterator, RequirementFlag::VectorType(), ascii::space_type> requireDef;
            qi::rule<Iterator, TypedList(), ascii::space_type> constantsDef;
            qi::rule<Iterator, PredicateList(), qi::locals<std::string, TypedList>, ascii::space_type> predicatesDef;

            struct RequirementFlagSymbols_ requirementFlagSymbols;
        };
    }
}
