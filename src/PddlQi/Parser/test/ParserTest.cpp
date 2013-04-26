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

BOOST_AUTO_TEST_CASE(ParserTest)
{
    PddlQi::Parser p;
    BOOST_CHECK_EQUAL(p.foo(), 1);
}
