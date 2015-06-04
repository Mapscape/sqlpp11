/*
 * column_spec.h
 *
 * Copyright (c) 2015 Mapscape B.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 *   Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 *   Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef SQLPP11_COLUMN_SPEC_H_
#define SQLPP11_COLUMN_SPEC_H_
#include <sqlpp11/alias_provider.h>
#include <sqlpp11/make_traits_tuple.h>
#include <boost/preprocessor/seq/enum.hpp>
#include <boost/preprocessor/seq/transform.hpp>

// Used in BOOST_PP_SEQ_TRANSFORM
// to translate an expression into the non-const type of that expression.
#define SQLPP_REM_CONST_DECLTYPE( ignore, ignored_, arg) typename std::remove_const<decltype( sqlpp::tag_values::arg)>::type

// Used in BOOST_PP_SEQ_TRANSFORM.
// Prefix input argument 'arg' with the namespace of tag values.
#define SQLPP_PREFIX_VALUE( ignore, ignored_, arg) sqlpp::tag_values::arg

// translate a sequence of tag expressions into a comma-separated list of tag types.
#define SQLPP_UNWRAP_TAGS( prefix, tag_sequence)                                                 \
    BOOST_PP_SEQ_ENUM( BOOST_PP_SEQ_TRANSFORM( SQLPP_REM_CONST_DECLTYPE, prefix, tag_sequence))  \
    /**/

// Translate a sequence of tag expressions into a comma-separated list of those expressions.
#define SQLPP_UNWRAP_VALUES( prefix, tag_sequence)                                          \
    BOOST_PP_SEQ_ENUM( BOOST_PP_SEQ_TRANSFORM( SQLPP_PREFIX_VALUE, prefix, tag_sequence))   \
    /**/

// generate code for a column specification, to be used in a table definition.
#define SQLPP_COLUMN_SPEC( name, type, tag_sequence)        \
    struct name                                             \
    {                                                       \
        SQLPP_ALIAS_STRUCT( name);                          \
        static constexpr auto traits_values                 \
            = sqlpp::make_traits_tuple(                     \
                    SQLPP_UNWRAP_VALUES( BOOST_PP_EMPTY(), tag_sequence));\
        using _traits = sqlpp::make_traits< sqlpp:: type,   \
            SQLPP_UNWRAP_TAGS( decltype, tag_sequence)      \
            >;                                              \
    }                                                       \
    /**/

/// generate column specification code from a sequence of column specification options
/// the first two elements of the sequence should be the column name and type respectively.
#define SQLPP_COLUMN_SPEC_FROM_SEQ( seq)\
    SQLPP_COLUMN_SPEC_FROM_SEQ2( BOOST_PP_SEQ_HEAD( seq), BOOST_PP_SEQ_TAIL( seq))

// utility macro for SQLPP_COLUMN_SPEC_FROM_SEQ that extracts the type information from the sequence.
#define SQLPP_COLUMN_SPEC_FROM_SEQ2( name, seq)\
    SQLPP_COLUMN_SPEC( name, BOOST_PP_SEQ_HEAD(seq), BOOST_PP_SEQ_TAIL(seq))

#endif /* SQLPP11_COLUMN_SPEC_H_ */
