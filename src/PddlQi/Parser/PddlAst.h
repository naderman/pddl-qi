/**
 * This file is part of PddlQi
 *
 * @copyright Nils Adermann <naderman@naderman.de>
 *
 * For the full copyright and licensing information please review the LICENSE
 * file that was distributed with this source code.
 */

#ifndef PDDLQI_PARSER_PDDLAST_H
#define PDDLQI_PARSER_PDDLAST_H

#include <boost/fusion/include/adapt_struct.hpp>

#include <string>
#include <vector>

namespace PddlQi
{
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

    typedef std::vector<struct Entity> TypedList;

    typedef std::vector<std::pair<std::string, TypedList> > PredicateList;

    struct Term
    {
        bool isVariable;
        std::string name;
    };

    typedef std::vector<struct Term> TermList;

    struct AtomicFormula
    {
        std::string predicateName;
        TermList args;
    };

    struct Literal
    {
        bool negate;
        AtomicFormula atomicFormula;
    };

    struct GoalDescription
    {

    };

    struct PddlAction
    {
        std::string name;
        TypedList parameters;
    };

    struct PddlDomain
    {
        std::string name;
        RequirementFlag::VectorType requirements;
        TypedList constants;
        PredicateList predicates;
    };
}

BOOST_FUSION_ADAPT_STRUCT(
    PddlQi::Entity,
    (std::string, name)
    (std::string, type)
)

BOOST_FUSION_ADAPT_STRUCT(
    PddlQi::Term,
    (bool, isVariable)
    (std::string, name)
)

BOOST_FUSION_ADAPT_STRUCT(
    PddlQi::AtomicFormula,
    (std::string, predicateName)
    (PddlQi::TermList, args)
)

BOOST_FUSION_ADAPT_STRUCT(
    PddlQi::Literal,
    (bool, negate)
    (PddlQi::AtomicFormula, atomicFormula)
)

BOOST_FUSION_ADAPT_STRUCT(
    PddlQi::PddlAction,
    (std::string, name)
    (PddlQi::TypedList, parameters)
)

BOOST_FUSION_ADAPT_STRUCT(
    PddlQi::PddlDomain,
    (std::string, name)
    (PddlQi::RequirementFlag::VectorType, requirements)
    (PddlQi::TypedList, constants)
    (PddlQi::PredicateList, predicates)
)

#endif
