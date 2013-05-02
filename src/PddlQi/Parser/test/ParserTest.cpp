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
    PddlQi::PddlDomain d;
};

BOOST_FIXTURE_TEST_SUITE(ParserTests, ParserTestFixture)

BOOST_AUTO_TEST_CASE(ParserFailTrivialInvalid)
{
    BOOST_CHECK_THROW(p.parse(""), PddlQi::ParserException);
    BOOST_CHECK_THROW(p.parse(")"), PddlQi::ParserException);
    BOOST_CHECK_THROW(p.parse("(define)"), PddlQi::ParserException);
}

BOOST_AUTO_TEST_CASE(ParserDomainName)
{
    domainString = std::string("(define (domain foo))");
    d = p.parse(domainString);
    BOOST_CHECK_EQUAL(d.name, "foo");

    domainString = std::string("(define (domain 12foo))");
    BOOST_CHECK_THROW(p.parse(domainString), PddlQi::ParserException);
}

BOOST_AUTO_TEST_CASE(ParserDomainWhitespace)
{
    domainString = std::string("( define\n(\t\tdomain\t foo  )  ) ");
    d = p.parse(domainString);
    BOOST_CHECK_EQUAL(d.name, "foo");
}

BOOST_AUTO_TEST_CASE(ParseCompleteDomain)
{
    domainString = std::string(
            "(define (domain foo) (:requirements :strips :negative-preconditions)"
            "(:constants c1 c2 - g1 c3 c4))"
            );
    d = p.parse(domainString);
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
}

BOOST_AUTO_TEST_SUITE_END()
