/**
 * This file is part of PddlQi
 *
 * @copyright Nils Adermann <naderman@naderman.de>
 *
 * For the full copyright and licensing information please review the LICENSE
 * file that was distributed with this source code.
 */

#define BOOST_TEST_MODULE PddlQi::Parser::ParserTest
#define USE_BOOST_TEST

#include <PddlQi/Test.h>
#include <PddlQi/Parser/Parser.h>

struct ParserTestFixture
{
    ParserTestFixture() {}
    ~ParserTestFixture() {}

    PddlQi::Parser p;
    std::string domainString;
    std::string actionString;
    PddlQi::PddlDomain d;
    PddlQi::PddlAction a;
};

BOOST_FIXTURE_TEST_SUITE(ParserTests, ParserTestFixture)

BOOST_AUTO_TEST_CASE(ParserFailTrivialInvalid)
{
    BOOST_CHECK_THROW(p.parseDomain(""), PddlQi::ParserException);
    BOOST_CHECK_THROW(p.parseDomain(")"), PddlQi::ParserException);
    BOOST_CHECK_THROW(p.parseDomain("(define)"), PddlQi::ParserException);
}

BOOST_AUTO_TEST_CASE(ParserDomainName)
{
    domainString = std::string("(define (domain foo))");
    d = p.parseDomain(domainString);
    BOOST_CHECK_EQUAL(d.name, "foo");

    domainString = std::string("(define (domain 12foo))");
    BOOST_CHECK_THROW(p.parseDomain(domainString), PddlQi::ParserException);
}

BOOST_AUTO_TEST_CASE(ParserDomainWhitespace)
{
    domainString = std::string("( define\n(\t\tdomain\t foo  )  ) ");
    d = p.parseDomain(domainString);
    BOOST_CHECK_EQUAL(d.name, "foo");
}

BOOST_AUTO_TEST_CASE(ParseCompleteDomain)
{
    domainString = std::string(
            "(define (domain foo)\n"
            "(:requirements :strips :negative-preconditions)\n"
            "(:constants c1 c2 - g1 c3 c4)\n"
            "(:predicates (foo ?x) (bar ?x ?y) (baz) (typed ?x - typex ?y - typey))\n"
            ")\n");

    d = p.parseDomain(domainString);
    BOOST_CHECK_EQUAL(d.name, "foo");

    BOOST_CHECK_EQUAL(d.requirements.size(), 2);
    BOOST_CHECK_EQUAL(d.requirements.at(0), PddlQi::RequirementFlag::eStrips);
    BOOST_CHECK_EQUAL(d.requirements.at(1), PddlQi::RequirementFlag::eNegativePreconditions);

    BOOST_CHECK_EQUAL(d.constants.size(), 4);
    BOOST_CHECK_EQUAL(d.constants.at(0).name, std::string("c1"));
    BOOST_CHECK_EQUAL(d.constants.at(0).type, std::string("g1"));
    BOOST_CHECK_EQUAL(d.constants.at(1).name, std::string("c2"));
    BOOST_CHECK_EQUAL(d.constants.at(1).type, std::string("g1"));
    BOOST_CHECK_EQUAL(d.constants.at(2).name, std::string("c3"));
    BOOST_CHECK_EQUAL(d.constants.at(2).type, std::string("object"));
    BOOST_CHECK_EQUAL(d.constants.at(3).name, std::string("c4"));
    BOOST_CHECK_EQUAL(d.constants.at(3).type, std::string("object"));

    BOOST_CHECK_EQUAL(d.predicates.size(), 4);
    BOOST_CHECK_EQUAL(d.predicates.at(0).first, "foo");
    BOOST_CHECK_EQUAL(d.predicates.at(0).second.size(), 1);
    BOOST_CHECK_EQUAL(d.predicates.at(0).second.at(0).name, "x");
    BOOST_CHECK_EQUAL(d.predicates.at(0).second.at(0).type, "object");
    BOOST_CHECK_EQUAL(d.predicates.at(1).first, "bar");
    BOOST_CHECK_EQUAL(d.predicates.at(1).second.size(), 2);
    BOOST_CHECK_EQUAL(d.predicates.at(2).first, "baz");
    BOOST_CHECK_EQUAL(d.predicates.at(2).second.size(), 0);
    BOOST_CHECK_EQUAL(d.predicates.at(3).first, "typed");
    BOOST_CHECK_EQUAL(d.predicates.at(3).second.size(), 2);
    BOOST_CHECK_EQUAL(d.predicates.at(3).second.at(0).name, "x");
    BOOST_CHECK_EQUAL(d.predicates.at(3).second.at(0).type, "typex");
    BOOST_CHECK_EQUAL(d.predicates.at(3).second.at(1).name, "y");
    BOOST_CHECK_EQUAL(d.predicates.at(3).second.at(1).type, "typey");
}

BOOST_AUTO_TEST_CASE(ParseParametersAction)
{
    actionString = std::string(
            "(:action foo\n"
            ":parameters (?x ?y ?z)\n"
            ")\n");

    a = p.parseAction(actionString);
    BOOST_CHECK_EQUAL(a.name, "foo");
}

BOOST_AUTO_TEST_CASE(ParsePreconditionAction)
{
    actionString = std::string(
            "(:action foo\n"
            ":parameters (?x ?y ?z)\n"
            ":precondition\n"
            "    (or\n"
            "        (and (foo ?x) (foo ?y))\n"
            "        (not\n"
            "            (or (bar ?x ?y) (foo ?z))))\n"
            ")\n");

    try
    {
        a = p.parseAction(actionString);
    } catch (const PddlQi::ParserException& e)
    {
        std::cerr << e.what();
        throw e;
    }
    BOOST_CHECK_EQUAL(a.name, "foo");
}

BOOST_AUTO_TEST_SUITE_END()
