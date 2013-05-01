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

BOOST_AUTO_TEST_CASE(ParserFailTrivialInvalid)
{
    PddlQi::Parser p;

    BOOST_CHECK_THROW(p.parse(""), PddlQi::ParserException);
    BOOST_CHECK_THROW(p.parse(")"), PddlQi::ParserException);
    BOOST_CHECK_THROW(p.parse("(define)"), PddlQi::ParserException);
}

BOOST_AUTO_TEST_CASE(ParserDomainName)
{
    PddlQi::Parser p;
    std::string domainString;
    PddlQi::PddlDomain d;

    domainString = std::string("(define (domain foo))");
    d = p.parse(domainString);
    BOOST_CHECK_EQUAL(d.name, "foo");

    domainString = std::string("(define (domain 12foo))");
    BOOST_CHECK_THROW(p.parse(domainString), PddlQi::ParserException);
}

